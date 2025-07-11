/******************************************************************************
 * @brief        
 * 
 * @file         com_stretch.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-30
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_controller.hpp"

namespace my_engineer {

/******************************************************************************
 * @brief    初始化自定义控制器前伸电机模块
 ******************************************************************************/
EAppStatus CModController::CComStretch::InitComponent(SModInitParam_Base &param){
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto controllerParam = static_cast<SModInitParam_Controller &>(param);

	// 保存电机指针
	motor[0] = MotorIDMap.at(controllerParam.stretch_id);

	// 设置发送节点
	mtrCanTxNode_[0] = controllerParam.stretchTxNode;

	// 初始化PID控制器
	controllerParam.stretchPosPidParam.threadNum = 1;
	pidPosCtrl.InitPID(&controllerParam.stretchPosPidParam);

	controllerParam.stretchSpdPidParam.threadNum = 1;
	pidSpdCtrl.InitPID(&controllerParam.stretchSpdPidParam);

	// 初始化电机数据输出缓冲区
	mtrOutputBuffer.fill(0);	

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新组件
 ******************************************************************************/
EAppStatus CModController::CComStretch::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	// 更新组件信息
	stretchInfo.posit = motor[0]->motorData[CDevMtr::DATA_POSIT] * CONTROLLER_STRETCH_MOTOR_DIR;
	stretchInfo.isPositArrived = (abs(stretchCmd.setPosit - stretchInfo.posit) < 8192 * 0.15);

	switch (Component_FSMFlag_){
		case FSM_RESET: {
			StopComponent();
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			componentStatus = APP_OK;
			return APP_OK;
		}

		case FSM_PREINIT: {
			// 预初始化状态下，把电机位置设定值设为1.2倍的范围限制
			stretchCmd.setPosit = static_cast<int32_t>(rangeLimit * 1.2);
			motor[0]->motorData[CDevMtr::DATA_POSIT] = stretchCmd.setPosit * CONTROLLER_STRETCH_MOTOR_DIR;
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			// 电机堵转,说明初始化完成
			if (motor[0]->motorStatus == CDevMtr::EMotorStatus::STALL) {
				stretchCmd = SStretchCmd();
				// 补偿超出限位的值
				motor[0]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>(0.2 * 8192) * CONTROLLER_STRETCH_MOTOR_DIR;
				pidPosCtrl.ResetPidController();
				pidSpdCtrl.ResetPidController();
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
				return APP_OK;
			}
			stretchCmd.setPosit -= 400;
			return _UpdateOutput(static_cast<float_t>(stretchCmd.setPosit));
		}

		case FSM_CTRL: {
			if (stretchCmd.isFree) {
				#ifdef CONTROLLER_ASSIST_ENABLE
					static int32_t lastPosit = stretchInfo.posit;  // 上次位置
					static int8_t sign_record = 0; // 记录上次位置的符号
					static uint32_t sign_record_cnt = 0; // 同一符号连续出现的次数
					static uint32_t sign_record_judge = 5; // 判断方向变化的次数阈值
					static int8_t sign = 0; // 当前方向
					static float_t judge = 1.0f; // 判断位置变化的编码器阈值
					static float_t output_quiet = -700; // 静止时的输出
					static float_t output_move = -250; // 移动时的输出
					static uint32_t output_cnt = 0; // quiet输出计数器
					static uint32_t output_move_judge = 20; // quiet输出时长


					// 获取当前符号
					int8_t current_sign;
					if (abs(lastPosit - stretchInfo.posit) >= judge) {
						current_sign = (stretchInfo.posit - lastPosit) > 0 ? 1 : -1;
					} else {
						current_sign = 0;
					}
					
					// 判断符号变化
					if (current_sign != sign_record) {
						sign_record = current_sign;
						sign_record_cnt = 0;
					}
					else {
						sign_record_cnt++;
						// 确定方向发生变化，重置输出方向及计数器
						if (sign_record_cnt >= sign_record_judge && current_sign != sign) {
							sign = current_sign;
							output_cnt = 0;
						}
					}

					// 根据计数器判断输出
					if (output_cnt < output_move_judge) {
						mtrOutputBuffer[0] = static_cast<int16_t>(output_quiet * sign);
					} else {
						mtrOutputBuffer[0] = static_cast<int16_t>(output_move * sign);
					}

					// 进行更新
					output_cnt++;
					lastPosit = stretchInfo.posit;

					return APP_OK;
				#else
					// 电机数据输出缓冲区始终为0
					mtrOutputBuffer.fill(0);
					return APP_OK;
				#endif
			}
			// 限制位置
			stretchCmd.setPosit = std::clamp(stretchCmd.setPosit, static_cast<int32_t>(0), rangeLimit);
			return _UpdateOutput(static_cast<float_t>(stretchCmd.setPosit));
		}

		default: {
			StopComponent();
			mtrOutputBuffer.fill(0);
			pidPosCtrl.ResetPidController();
			pidSpdCtrl.ResetPidController();
			componentStatus = APP_ERROR;
			return APP_ERROR;
		}
	}
}

/******************************************************************************
 * @brief    物理位置转换为电机位置
 * 
 * @param    phyPosit 
 * @return   int32_t 
 ******************************************************************************/
int32_t CModController::CComStretch::PhyPositToMtrPosit(float_t phyPosit){
	const int32_t zeroOffset = 0;
	const float_t scale = CONTROLLER_STRETCH_MOTOR_RATIO;

	return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/******************************************************************************
 * @brief    电机位置转换为物理位置
 * 
 * @param    mtrPosit 
 * @return   float_t 
 ******************************************************************************/
float_t CModController::CComStretch::MtrPositToPhyPosit(int32_t mtrPosit){
	const int32_t zeroOffset = 0;
	const float_t scale = CONTROLLER_STRETCH_MOTOR_RATIO;

	return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/******************************************************************************
 * @brief    输出更新函数
 ******************************************************************************/
EAppStatus CModController::CComStretch::_UpdateOutput(float_t posit){

	// 位置环
	DataBuffer<float_t> stretchPos = {
		static_cast<float_t>(posit) * CONTROLLER_STRETCH_MOTOR_DIR,
	};

	DataBuffer<float_t> stretchPosMeasure = {
		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_POSIT]),
	};

	auto stretchSpd = 
		pidPosCtrl.UpdatePidController(stretchPos, stretchPosMeasure);

	// 速度环
	DataBuffer<float_t> stretchSpdMeasure = {
		static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_SPEED]),
	};

	auto output = 
		pidSpdCtrl.UpdatePidController(stretchSpd, stretchSpdMeasure);

	mtrOutputBuffer = {
		static_cast<int16_t>(output[0]),
	};

	return APP_OK;
}

} // namespace my_engineer
