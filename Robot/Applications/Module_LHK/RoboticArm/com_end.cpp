/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-01-10
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-12-16   <td>1.0         <td>Sassinak  <td>First Create.
 * </table>
 */

#include "mod_roboticarm.hpp"

namespace my_engineer {

/**
 * @brief
 * @param param
 * @return
 */
EAppStatus C_ModRoboticArm::C_ComEnd::InitComponent(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;  

  auto paramEnd = static_cast<S_ModInitParam_RoboticArm &>(param);
  /*Attendtion! if you don't register motor in motorMap, you may enter an endless loop*/
  motor[R]  = MotorMap.at(paramEnd.endMotor_RollID);
  motor[P]  = MotorMap.at(paramEnd.endMotor_PitchID);

  mtrCanTxNode[R]   = paramEnd.endMotorTxNode_Roll;
  mtrCanTxNode[P] =  paramEnd.endMotorTxNode_Pitch;
  /*Initialize PID*/
  paramEnd.endPosPidParam.threadNum = 2;
  pidPosCtrl.InitPID(&paramEnd.endPosPidParam);

  paramEnd.endSpdPidParam.threadNum = 2;
  pidSpdCtrl.InitPID(&paramEnd.endSpdPidParam);

  mtrOutputBuffer.fill(0);

  Component_FSMFlag_ = FSM_RESET;
  ComponentState = APP_OK;

  return APP_OK;

}

EAppStatus C_ModRoboticArm::C_ComEnd::UpdateComponent(){
  
  if (ComponentState == COM_RESET) return APP_ERROR;
  /*In the Component the Data always raw data,The information at the module level is usually processed*/
  endInfo.angle_roll_end  = (motor[R]->motorData[C_MtrInstance::DATA_POSIT]+motor[P]->motorData[C_MtrInstance::DATA_POSIT])/2.0;
  endInfo.angle_pitch_end = ((motor[P]->motorData[C_MtrInstance::DATA_POSIT]-endInfo.angle_roll_end)-(motor[R]->motorData[C_MtrInstance::DATA_POSIT]-endInfo.angle_roll_end))/2.0;

  endInfo.isAngleArrived_roll_end  = (abs(endCmd.setAngle_roll_end  - endInfo.angle_roll_end)  < 8192 * 3);
  endInfo.isAngleArrived_pitch_end = (abs(endCmd.setAngle_pitch_end - endInfo.angle_pitch_end) < 8192 * 3);

  switch (Component_FSMFlag_) {
    case FSM_RESET: {
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      return APP_OK;
    }

    case FSM_PREINIT: {
      endCmd.setAngle_pitch_end = 0;
      motor[R]->motorData[C_MtrInstance::DATA_POSIT] = 0;
      motor[P]->motorData[C_MtrInstance::DATA_POSIT] = 0;
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      Component_FSMFlag_ = FSM_INIT;
      return APP_OK;
    }

    case FSM_INIT: {
      if (motor[R]->motorState == C_MtrInstance::E_MotorStatus::STALL || motor[P]->motorState == C_MtrInstance::E_MotorStatus::STALL) {
        endCmd.setAngle_pitch_end = 308213.0f;            
        endCmd.setAngle_roll_end  = 0;            
        motor[R]->motorData[C_MtrInstance::DATA_POSIT]  = -RAmod_LIMIT_end;
        motor[P]->motorData[C_MtrInstance::DATA_POSIT]  =  RAmod_LIMIT_end;
        pidPosCtrl.ResetPidController();
        pidSpdCtrl.ResetPidController();
        ComponentState = APP_OK;
        Component_FSMFlag_ = FSM_CTRL;
        return APP_OK;
      }
      endCmd.setAngle_pitch_end += 200;    // Reset Speed
      return _UpdateOutput(static_cast<float_t>(endCmd.setAngle_pitch_end), 0);
    }

    case FSM_CTRL: {
      // endCmd.setAngle_pitch_end = std::clamp(endCmd.setAngle_pitch_end, static_cast<int32_t>(-10732), rangeLimit_pitch);
      // endCmd.setAngle_roll_end  = std::clamp(endCmd.setAngle_roll_end,  static_cast<int32_t>(0), rangeLimit_roll);
      return _UpdateOutput(static_cast<float_t>(endCmd.setAngle_pitch_end), static_cast<float_t>(endCmd.setAngle_roll_end));
    }

    default: {
      StopComponent();
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      ComponentState = APP_ERROR;
      return APP_ERROR;
    }
  }
}

/**
 * @brief
 * @param phyPosit
 * @return
 */
int32_t C_ModRoboticArm::C_ComEnd::PhyPositToMtrPosit_Pitch(float_t phyPosit) {

  const float_t scale = 1638.52f;
  const int32_t zeroOffset = RAmod_INIT_mtrEND;

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/**
 * @brief
 * @param phyPosit
 * @return
 */
int32_t C_ModRoboticArm::C_ComEnd::PhyPositToMtrPosit_Roll(float_t phyPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale = 3524.07f;

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}

/**
 * @brief
 * @param mtrPosit
 * @return
 */

float_t C_ModRoboticArm::C_ComEnd::MtrPositToPhyPosit_Pitch(int32_t mtrPosit) {

  const float_t scale = 1638.52f;
  const int32_t zeroOffset = RAmod_INIT_mtrEND;

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);

}


/**
 * @brief
 * @param mtrPosit
 * @return
 */
float_t C_ModRoboticArm::C_ComEnd::MtrPositToPhyPosit_Roll(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale = 3524.07f;

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}


/**
 * @brief
 * @param posit
 * @return
 */
EAppStatus C_ModRoboticArm::C_ComEnd::_UpdateOutput(float_t posit_Pitch, float_t posit_Roll) {

  DataBuffer<float_t> endPos = {
    posit_Roll - posit_Pitch,
    posit_Roll + posit_Pitch,
  };

  DataBuffer<float_t> endPosMeasure = {
    static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_POSIT]),
    static_cast<float_t>(motor[P]->motorData[C_MtrInstance::DATA_POSIT]),
  };

  auto endSpd =
    pidPosCtrl.UpdatePidController(endPos, endPosMeasure);

  DataBuffer<float_t> liftSpdMeasure = {
    static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_SPEED]),
    static_cast<float_t>(motor[P]->motorData[C_MtrInstance::DATA_SPEED]),
  };

  auto output =
    pidSpdCtrl.UpdatePidController(endSpd, liftSpdMeasure);

  mtrOutputBuffer = {
    static_cast<int16_t>(output[R]),
    static_cast<int16_t>(output[P]),
  };

  return APP_OK;
}

}
