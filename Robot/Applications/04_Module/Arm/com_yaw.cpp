/******************************************************************************
 * @brief        
 * 
 * @file         com_yaw.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-07-09
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_arm.hpp"

namespace my_engineer {

/**
 * @brief 初始化机械臂关节Yaw组件
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComYaw::InitComponent(SModInitParam_Base &param) {
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto armParam = static_cast<SModInitParam_Arm &>(param);

	// 保存电机指针
	motor = MotorIDMap.at(armParam.MotorID_Yaw);

	// 初始化PID控制器
	mitCtrl.kp = armParam.MIT_Yaw_kp;
	mitCtrl.kd = armParam.MIT_Yaw_kd;

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/**
 * @brief 更新Yaw关节组件
 * 
 * @return EAppStatus 
 */
EAppStatus CModArm::CComYaw::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	CDevMtrDM_MIT *pMtr = static_cast<CDevMtrDM_MIT *>(motor);

	// 更新组件信息
	yawInfo.angle = rad2deg(pMtr->motorPhyAngle) * ARM_YAW_MOTOR_DIR;
	yawInfo.isAngleArrived = (fabs(yawInfo.angle - yawCmd.setAngle) < 0.2f);

	// 缓慢移动控制逻辑
	static float_t next_angle = 0.0f;
	static float_t gradual_kp = 0.005f;
	static float_t gradual_min = 0.03f;

	next_angle += (yawCmd.setAngle - next_angle) * gradual_kp;
	if (fabs(next_angle - yawCmd.setAngle) < gradual_min) {
		next_angle = yawCmd.setAngle;
	}

	switch (Component_FSMFlag_) {
		case FSM_RESET: {
			pMtr->Control_MIT(0.0f, 0.0f, deg2rad(0.0f) * ARM_YAW_MOTOR_DIR, 0.0f, 0.0f);
			yawCmd = SYawCmd();
			return APP_OK;
		}

		case FSM_PREINIT: {
			yawCmd.setAngle = deg2rad(ARM_YAW_INIT_ANGLE);
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

		case FSM_INIT: {
			if (yawInfo.isAngleArrived) {
				Component_FSMFlag_ = FSM_CTRL;
				componentStatus = APP_OK;
			}
			pMtr->Control_MIT(5.0f, mitCtrl.kd, deg2rad(next_angle) * ARM_YAW_MOTOR_DIR, 0.0f, 0.0f);
			return APP_OK;
		}

		case FSM_CTRL: {
			pMtr->Control_MIT(mitCtrl.kp, mitCtrl.kd, deg2rad(next_angle) * ARM_YAW_MOTOR_DIR, 0.0f, 0.0f);
			return APP_OK;
		}

		default: {
			componentStatus = APP_ERROR;
			return APP_ERROR;
		}
	}

	return APP_OK;
}

} // namespace my_engineer

