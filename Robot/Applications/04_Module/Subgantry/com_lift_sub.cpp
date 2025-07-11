/**
 * @file com_lift.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 子龙门升降模块
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_subgantry.hpp"

namespace my_engineer {

/**
 * @brief 初始化子龙门升降模块
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModSubGantry::CComLift_sub::InitComponent(SModInitParam_Base &param){
    // 检查param是否正确
    if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

    // 类型转换
    auto subGantryParam = static_cast<SModInitParam_SubGantry &>(param);

    // 保存电机指针
    motor[0] = MotorIDMap.at(subGantryParam.liftMotorID_L);
    motor[1] = MotorIDMap.at(subGantryParam.liftMotorID_R);

    // 设置发送节点
    mtrCanTxNode[0] = subGantryParam.liftMotorTxNode_L;
    mtrCanTxNode[1] = subGantryParam.liftMotorTxNode_R;

    // 初始化PID控制器
    subGantryParam.liftPosPidParam.threadNum = 2;
    pidPosCtrl.InitPID(&subGantryParam.liftPosPidParam);

    subGantryParam.liftSpdPidParam.threadNum = 2;
    pidSpdCtrl.InitPID(&subGantryParam.liftSpdPidParam);

    // 初始化电机数据输出缓冲区
    mtrOutputBuffer.fill(0);

    Component_FSMFlag_ = FSM_RESET;
    componentStatus = APP_OK;

    return APP_OK;
}


/**
 * @brief 更新组件
 * 
 */
EAppStatus CModSubGantry::CComLift_sub::UpdateComponent() {
    // 检查组件状态
    if (componentStatus == APP_RESET) return APP_ERROR;

    // 更新组件信息
    liftInfo.posit_L = motor[0]->motorData[CDevMtr::DATA_POSIT] * SUB_GANTRY_LIFT_MOTOR_DIR_L;
    liftInfo.posit_R = motor[1]->motorData[CDevMtr::DATA_POSIT] * SUB_GANTRY_LIFT_MOTOR_DIR_R;
    liftInfo.isPositArrived_L  = (abs(liftCmd.setPosit_L - liftInfo.posit_L) < 8192);
    liftInfo.isPositArrived_R  = (abs(liftCmd.setPosit_R - liftInfo.posit_R) < 8192);

    switch (Component_FSMFlag_){
        case FSM_RESET: {
            // 重置状态下，电机数据输出缓冲区始终为0
            mtrOutputBuffer.fill(0);
            pidPosCtrl.ResetPidController();
            pidSpdCtrl.ResetPidController();
            return APP_OK;
        }

        case FSM_PREINIT: {
            // 预初始化状态下，把电机位置设定值设为1.2倍的范围限制
            liftCmd.setPosit_L = static_cast<int32_t>(rangeLimit_L * 1.2);
            liftCmd.setPosit_R = static_cast<int32_t>(rangeLimit_R * 1.2);
            motor[0]->motorData[CDevMtr::DATA_POSIT] = liftCmd.setPosit_L * SUB_GANTRY_LIFT_MOTOR_DIR_L;
            motor[1]->motorData[CDevMtr::DATA_POSIT] = liftCmd.setPosit_R * SUB_GANTRY_LIFT_MOTOR_DIR_R;
            mtrOutputBuffer.fill(0);
            pidPosCtrl.ResetPidController();
            pidSpdCtrl.ResetPidController();
            Component_FSMFlag_ = FSM_INIT;
            return APP_OK;
        }

        case FSM_INIT: {
            // 电机堵转,说明初始化完成
            if (motor[0]->motorStatus == CDevMtr::EMotorStatus::STALL
                && motor[1]->motorStatus == CDevMtr::EMotorStatus::STALL) {
                liftCmd = SLiftCmd();
                // 补偿超出限位的值
                motor[0]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>( 8192 * 0.1) * SUB_GANTRY_LIFT_MOTOR_DIR_L;
                motor[1]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>( 8192 * 0.1) * SUB_GANTRY_LIFT_MOTOR_DIR_R;
                pidPosCtrl.ResetPidController();
                pidSpdCtrl.ResetPidController();
                Component_FSMFlag_ = FSM_CTRL;
                componentStatus = APP_OK;
                return APP_OK;
            }
            liftCmd.setPosit_L -= 400;
            liftCmd.setPosit_R -= 400;
            return _UpdateOutput(static_cast<float_t>(liftCmd.setPosit_L),
                                 static_cast<float_t>(liftCmd.setPosit_R));
        }

        case FSM_CTRL: {
            liftCmd.setPosit_L = std::clamp(liftCmd.setPosit_L, static_cast<int32_t>(0), rangeLimit_L);
            liftCmd.setPosit_R = std::clamp(liftCmd.setPosit_R, static_cast<int32_t>(0), rangeLimit_R);
            return _UpdateOutput(static_cast<float_t>(liftCmd.setPosit_L),
                                 static_cast<float_t>(liftCmd.setPosit_R));
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

/**
 * @brief 物理位置转换为电机位置
 * 
 * @param phyPosit 
 * @return int32_t 
 */
int32_t CModSubGantry::CComLift_sub::PhyPositToMtrPosit_L(float_t phyPosit){
    
    const int32_t zeroOffset = 0;
    const float_t scale      = SUB_GANTRY_LIFT_MOTOR_RATIO_L;

    return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

int32_t CModSubGantry::CComLift_sub::PhyPositToMtrPosit_R(float_t phyPosit){
    
    const int32_t zeroOffset = 0;
    const float_t scale      = SUB_GANTRY_LIFT_MOTOR_RATIO_R;

    return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/**
 * @brief 电机位置转换为物理位置
 * 
 * @param mtrPosit 
 * @return float_t 
 */
float_t CModSubGantry::CComLift_sub::MtrPositToPhyPosit_L(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = SUB_GANTRY_LIFT_MOTOR_RATIO_L;

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

float_t CModSubGantry::CComLift_sub::MtrPositToPhyPosit_R(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = SUB_GANTRY_LIFT_MOTOR_RATIO_R;

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/**
 * @brief 更新输出
 * 
 */
EAppStatus CModSubGantry::CComLift_sub::_UpdateOutput(float_t posit_L, float_t posit_R) {

    // 位置环
    DataBuffer<float_t> liftPos = {
      static_cast<float_t>(posit_L * SUB_GANTRY_LIFT_MOTOR_DIR_L),
      static_cast<float_t>(posit_R * SUB_GANTRY_LIFT_MOTOR_DIR_R),
    };

    DataBuffer<float_t> liftPosMeasure = {
      static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_POSIT]),
      static_cast<float_t>(motor[1]->motorData[CDevMtr::DATA_POSIT]),
    };

    auto liftSpd =
      pidPosCtrl.UpdatePidController(liftPos, liftPosMeasure);

    // 速度环
    DataBuffer<float_t> liftSpdMeasure = {
      static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_SPEED]),
      static_cast<float_t>(motor[1]->motorData[CDevMtr::DATA_SPEED]),
    };

    auto output =
      pidSpdCtrl.UpdatePidController(liftSpd, liftSpdMeasure);

    mtrOutputBuffer = {
      static_cast<int16_t>(output[0]),
      static_cast<int16_t>(output[1]),
    };

    return APP_OK;
}

} // namespace my_engineer
