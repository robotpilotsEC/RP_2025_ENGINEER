/**
 * @file com_stretch.cpp
 * @author Fish_Joe
 * @brief 子龙门伸缩模块
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_subgantry.hpp"

namespace my_engineer {

/**
 * @brief 初始化子龙门伸缩模块
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModSubGantry::CComStretch_sub::InitComponent(SModInitParam_Base &param) {
  // 检查param是否正确
  if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

  // 类型转换
  auto subGantryParam = static_cast<SModInitParam_SubGantry &>(param);

  // 保存电机指针
  motor[0] = MotorIDMap.at(subGantryParam.stretchMotorID_L);
  motor[1] = MotorIDMap.at(subGantryParam.stretchMotorID_R);

  // 设置发送节点
  mtrCanTxNode[0] = subGantryParam.stretchMotorTxNode_L;
  mtrCanTxNode[1] = subGantryParam.stretchMotorTxNode_R;

  // 初始化PID控制器
  subGantryParam.stretchPosPidParam.threadNum = 2;
  pidPosCtrl.InitPID(&subGantryParam.stretchPosPidParam);

  subGantryParam.stretchSpdPidParam.threadNum = 2;
  pidSpdCtrl.InitPID(&subGantryParam.stretchSpdPidParam);

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
EAppStatus CModSubGantry::CComStretch_sub::UpdateComponent() {
  // 检查组件状态
  if (componentStatus == APP_RESET) return APP_ERROR;

  // 更新组件信息
  stretchInfo.posit_L = motor[0]->motorData[CDevMtr::DATA_POSIT] * SUB_GANTRY_STRETCH_MOTOR_DIR_L;
  stretchInfo.posit_R = motor[1]->motorData[CDevMtr::DATA_POSIT] * SUB_GANTRY_STRETCH_MOTOR_DIR_R;
  stretchInfo.isPositArrived_L = (abs(stretchCmd.setPosit_L - stretchInfo.posit_L) < 8192);
  stretchInfo.isPositArrived_R = (abs(stretchCmd.setPosit_R - stretchInfo.posit_R) < 8192);

  switch (Component_FSMFlag_) {
    case FSM_RESET: {
      // 重置状态下，电机数据输出缓冲区始终为0
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      return APP_OK;
    }

    case FSM_PREINIT: {
      // 预初始化状态下，把电机位置设定值设为1.2倍的范围限制
      stretchCmd.setPosit_L = static_cast<int32_t>(rangeLimit_L * 1.2);
      stretchCmd.setPosit_R = static_cast<int32_t>(rangeLimit_R * 1.2);
      motor[0]->motorData[CDevMtr::DATA_POSIT] = stretchCmd.setPosit_L * SUB_GANTRY_STRETCH_MOTOR_DIR_L;
      motor[1]->motorData[CDevMtr::DATA_POSIT] = stretchCmd.setPosit_R * SUB_GANTRY_STRETCH_MOTOR_DIR_R;
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
        stretchCmd = SStretchCmd();
        // 补偿超出限位的值
        motor[0]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>(8192 * 0.1) * SUB_GANTRY_STRETCH_MOTOR_DIR_L;
        motor[1]->motorData[CDevMtr::DATA_POSIT] = -static_cast<int32_t>(8192 * 0.1) * SUB_GANTRY_STRETCH_MOTOR_DIR_R;
        pidPosCtrl.ResetPidController();
        pidSpdCtrl.ResetPidController();
        Component_FSMFlag_ = FSM_CTRL;
        componentStatus = APP_OK;
        return APP_OK;
      }
      stretchCmd.setPosit_L -= 400;
      stretchCmd.setPosit_R -= 400;
      return _UpdateOutput(static_cast<float_t>(stretchCmd.setPosit_L),
                           static_cast<float_t>(stretchCmd.setPosit_R));
    }

    case FSM_CTRL: {
      stretchCmd.setPosit_L = std::clamp(stretchCmd.setPosit_L, static_cast<int32_t>(0), rangeLimit_L);
      stretchCmd.setPosit_R = std::clamp(stretchCmd.setPosit_R, static_cast<int32_t>(0), rangeLimit_R);
      return _UpdateOutput(static_cast<float_t>(stretchCmd.setPosit_L),
                           static_cast<float_t>(stretchCmd.setPosit_R));
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
int32_t CModSubGantry::CComStretch_sub::PhyPositToMtrPosit_L(float_t phyPosit) {
  const int32_t zeroOffset = 0;
  const float_t scale      = SUB_GANTRY_STRETCH_MOTOR_RATIO_L;

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

int32_t CModSubGantry::CComStretch_sub::PhyPositToMtrPosit_R(float_t phyPosit) {
  const int32_t zeroOffset = 0;
  const float_t scale      = SUB_GANTRY_STRETCH_MOTOR_RATIO_R;

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/**
 * @brief 电机位置转换为物理位置
 * 
 * @param mtrPosit 
 * @return float_t 
 */
float_t CModSubGantry::CComStretch_sub::MtrPositToPhyPosit_L(int32_t mtrPosit) {
  const int32_t zeroOffset = 0;
  const float_t scale      = SUB_GANTRY_STRETCH_MOTOR_RATIO_L;

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

float_t CModSubGantry::CComStretch_sub::MtrPositToPhyPosit_R(int32_t mtrPosit) {
  const int32_t zeroOffset = 0;
  const float_t scale      = SUB_GANTRY_STRETCH_MOTOR_RATIO_R;

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/**
 * @brief 更新输出
 * 
 */
EAppStatus CModSubGantry::CComStretch_sub::_UpdateOutput(float_t posit_L, float_t posit_R) {
  // 位置环
  DataBuffer<float_t> stretchPos = {
    static_cast<float_t>(posit_L * SUB_GANTRY_STRETCH_MOTOR_DIR_L),
    static_cast<float_t>(posit_R * SUB_GANTRY_STRETCH_MOTOR_DIR_R),
  };

  DataBuffer<float_t> stretchPosMeasure = {
    static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_POSIT]),
    static_cast<float_t>(motor[1]->motorData[CDevMtr::DATA_POSIT]),
  };

  auto stretchSpd = pidPosCtrl.UpdatePidController(stretchPos, stretchPosMeasure);

  // 速度环
  DataBuffer<float_t> stretchSpdMeasure = {
    static_cast<float_t>(motor[0]->motorData[CDevMtr::DATA_SPEED]),
    static_cast<float_t>(motor[1]->motorData[CDevMtr::DATA_SPEED]),
  };

  auto output = pidSpdCtrl.UpdatePidController(stretchSpd, stretchSpdMeasure);

  mtrOutputBuffer = {
    static_cast<int16_t>(output[0]),
    static_cast<int16_t>(output[1]),
  };

  return APP_OK;
}

} // namespace my_engineer
