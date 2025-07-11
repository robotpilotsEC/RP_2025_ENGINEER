/**
 * @file        com_wheelset.cpp
 * @version     1.0
 * @date        2024-12-20
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-04-05   <td>1.0         <td>Morthine Xiang  <td>First Create.
 * <tr><td>2024-05-10   <td>1.1         <td>Morthine Xiang  <td>Rewrite Component Structure.
 * </table>
 */

#include "mod_chassis.hpp"

namespace my_engineer {

/**
 * @brief Initialize Chassis WheelSet Component
 * @param param
 * @return
 */
EAppStatus C_ModChassis::C_ComWheelset::InitComponent(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  /* Initialize Component */
  auto chassisParam = static_cast<S_ModInitParam_Chassis &>(param);
  mems = MemsMap.at(chassisParam.memsDevID);
  /*Attention! if you don't register motor in motorMap, you may enter an endless loop*/
  motor[LF] = MotorMap.at(chassisParam.wheelsetMotorID_LF);
  motor[RF] = MotorMap.at(chassisParam.wheelsetMotorID_RF);
  motor[LB] = MotorMap.at(chassisParam.wheelsetMotorID_LB);
  motor[RB] = MotorMap.at(chassisParam.wheelsetMotorID_RB);

  /*Set CAN sending Node*/
  mtrCanTxNode[LF] = chassisParam.wheelsetMotorTxNode_LF;
  mtrCanTxNode[RF] = chassisParam.wheelsetMotorTxNode_RF;
  mtrCanTxNode[LB] = chassisParam.wheelsetMotorTxNode_LB;
  mtrCanTxNode[RB] = chassisParam.wheelsetMotorTxNode_RB;

  /* Initialize PID Controllers */
  chassisParam.wheelsetSpeedPidParam.threadNum = 4;
  pidSpdCtrl.InitPID(&chassisParam.wheelsetSpeedPidParam);

  chassisParam.yawCorrectionPidParam.threadNum = 1;
  pidYawCtrl.InitPID(&chassisParam.yawCorrectionPidParam);

  /* Clear Motor Output Buffer */
  mtrOutputBuffer = {0, 0, 0, 0};

  /* Set Component Flags */
  Component_FSMFlag_ = FSM_RESET;
  ComponentState = APP_OK;

  return APP_OK;
}


/**
 * @brief Update Chassis WheelSet Component
 * @return
 */
EAppStatus C_ModChassis::C_ComWheelset::UpdateComponent() {

  if (ComponentState == COM_RESET) return APP_ERROR;

  /* FSM */
  switch (Component_FSMFlag_) {

    case FSM_RESET: {   // WheelSet Reset
      mtrOutputBuffer.fill(0);    // RESET status buffer always 0
      return APP_OK;
    }

    case FSM_PREINIT: {   // WheelSet Pre-Init
      motor[LF]->motorData[C_MtrInstance::DATA_POSIT] = 0;
      motor[RF]->motorData[C_MtrInstance::DATA_POSIT] = 0;
      motor[LB]->motorData[C_MtrInstance::DATA_POSIT] = 0;
      motor[RB]->motorData[C_MtrInstance::DATA_POSIT] = 0;
      mtrOutputBuffer = {0, 0, 0, 0};
      pidYawCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      Component_FSMFlag_ = FSM_INIT;
    }

    case FSM_INIT: {   // WheelSet Init
      if (mems->memsState == C_MemsInstance::E_MemsStatus::NORMAL) {
        wheelsetCmd = S_WheelsetCommand();
        Component_FSMFlag_ = FSM_CTRL;
        ComponentState = APP_OK;
        return APP_OK;
      }
      return _UpdateOutput(wheelsetCmd.speed_X, wheelsetCmd.speed_Y, wheelsetCmd.speed_W);
    }

    case FSM_CTRL: {   // WheelSet Control
      DataBuffer<float_t> yawSpd = { wheelsetCmd.speed_W};
      DataBuffer<float_t> yawSpdMeasure = {mems->memsData[C_MemsInstance::DATA_GYRO_Z] };

      auto output = pidYawCtrl.UpdatePidController(yawSpd, yawSpdMeasure);

      return _UpdateOutput(wheelsetCmd.speed_X, wheelsetCmd.speed_Y, output[0]);
    }

    default: {
      Component_FSMFlag_ = FSM_RESET;
      return APP_ERROR;
    }
  }
}


/**
 * @brief
 * @param speed_X
 * @param speed_Y
 * @param speed_W
 * @return
 */
EAppStatus C_ModChassis::C_ComWheelset::_UpdateOutput(float_t speed_X,float_t speed_Y,float_t speed_W) {

  DataBuffer<float_t> wheelSpd = {
      speed_Y + speed_X + speed_W,
    - speed_Y + speed_X + speed_W,
      speed_Y - speed_X + speed_W,
    - speed_Y - speed_X + speed_W,
  };

  DataBuffer<float_t> wheelSpdMeasure = {
    static_cast<float_t>(motor[LF]->motorData[C_MtrInstance::DATA_SPEED]),
    static_cast<float_t>(motor[RF]->motorData[C_MtrInstance::DATA_SPEED]),
    static_cast<float_t>(motor[LB]->motorData[C_MtrInstance::DATA_SPEED]),
    static_cast<float_t>(motor[RB]->motorData[C_MtrInstance::DATA_SPEED]),
  };

  auto output
    = pidSpdCtrl.UpdatePidController(wheelSpd, wheelSpdMeasure);

  mtrOutputBuffer = {
    static_cast<int16_t>(output[LF]),
    static_cast<int16_t>(output[RF]),
    static_cast<int16_t>(output[LB]),
    static_cast<int16_t>(output[RB]),
  };

  return APP_OK;
}

} // namespace robotpilots
