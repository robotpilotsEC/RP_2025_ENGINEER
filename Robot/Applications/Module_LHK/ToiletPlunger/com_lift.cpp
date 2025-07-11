/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2024-12-20
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-05-15   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 */

#include "mod_toiletplunger.hpp"

namespace my_engineer {

/**
 * @brief
 * @param param
 * @return
 */
EAppStatus C_ModToiletPlunger::C_ComLift::InitComponent(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  /* Initialize Component */
  auto toiletplungerparam = static_cast<S_ModInitParam_ToiletPlunger &>(param);
  /*Attention! if you don't register motor in motorMap, you may enter an endless loop*/
  motor[L]             = MotorMap.at(toiletplungerparam.liftMotorID_L);
  motor[R]             = MotorMap.at(toiletplungerparam.liftMotorID_R);
	
  /*set Can TxNode*/
  mtrCanTxNode[L] 	   = toiletplungerparam.liftmotorTxNode_L;
  mtrCanTxNode[R] 	   = toiletplungerparam.liftmotorTxNode_R;
  
  /* Initialize PID Controllers */
  toiletplungerparam.liftPosPidParam.threadNum = 2;
  pidPosCtrl.InitPID(&toiletplungerparam.liftPosPidParam);

  toiletplungerparam.liftSpdPidParam.threadNum = 2;
  pidSpdCtrl.InitPID(&toiletplungerparam.liftSpdPidParam);

  /* Clear Motor Output Buffer */
  mtrOutputBuffer.fill(0);

  /* Set Component Flags */
  Component_FSMFlag_ = FSM_RESET;
  ComponentState = APP_OK;

  return APP_OK;
}


/**
 * @brief
 */
EAppStatus C_ModToiletPlunger::C_ComLift::UpdateComponent() {

  if (ComponentState == COM_RESET) return APP_ERROR;

  /* Update Info */
  liftInfo.posit_L = motor[L]->motorData[C_MtrInstance::DATA_POSIT];
  liftInfo.posit_R = motor[R]->motorData[C_MtrInstance::DATA_POSIT];
  liftInfo.isPositArrived_L = (abs(liftCmd.setPosit_L - liftInfo.posit_L) < 8192 );
  liftInfo.isPositArrived_R = (abs(liftCmd.setPosit_R - liftInfo.posit_R) < 8192 );

  /* Gantry Lift Component FSM */
  switch (Component_FSMFlag_) {

    case FSM_RESET: {
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      return APP_OK;
    }

    case FSM_PREINIT: {
      liftCmd.setPosit_L                            = static_cast<int32_t>(rangeLimit_L * 1.2);
      liftCmd.setPosit_R                            = static_cast<int32_t>(rangeLimit_R * 1.2);
      motor[L]->motorData[C_MtrInstance::DATA_POSIT] = liftCmd.setPosit_L;
      motor[R]->motorData[C_MtrInstance::DATA_POSIT] = liftCmd.setPosit_R;
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      Component_FSMFlag_ = FSM_INIT;
      return APP_OK;
    }

    case FSM_INIT: {
      if (motor[L]->motorState == C_MtrInstance::E_MotorStatus::STALL && motor[R]->motorState == C_MtrInstance::E_MotorStatus::STALL) {
        liftCmd = S_LiftCmd();
        motor[L]->motorData[C_MtrInstance::DATA_POSIT] = -static_cast<int32_t>( 8192 * 0.5);
        motor[R]->motorData[C_MtrInstance::DATA_POSIT] = static_cast<int32_t>( 8192 * 0.5);
        pidPosCtrl.ResetPidController();
        pidSpdCtrl.ResetPidController();
        ComponentState = APP_OK;
        Component_FSMFlag_ = FSM_CTRL;
        return APP_OK;
      }
      liftCmd.setPosit_L -= 150;      // Reset Speed
      liftCmd.setPosit_R += 150;      // Reset Speed
      return _UpdateOutput(static_cast<float_t>(liftCmd.setPosit_L),
                           static_cast<float_t>(liftCmd.setPosit_R));
    }

    case FSM_CTRL: {
      liftCmd.setPosit_L = std::clamp(liftCmd.setPosit_L, static_cast<int32_t>(0), rangeLimit_L);
      liftCmd.setPosit_R = std::clamp(liftCmd.setPosit_R,rangeLimit_R, static_cast<int32_t>(0));
      return _UpdateOutput(static_cast<float_t>(liftCmd.setPosit_L),
                           static_cast<float_t>(liftCmd.setPosit_R));
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
 * @brief Convert physical position to motor position
 * @param phyPosit
 * @return
 */
int32_t C_ModToiletPlunger::C_ComLift::PhyPositToMtrPosit_L(float_t phyPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = (TPmod_LIMIT_mtrLIFT_L/TPmod_LIMIT_phyLIFT_L);            // (Encoder total value / Physical total value)

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}


/**
 * @brief Convert motor position to physical position
 * @param mtrPosit
 * @return
 */
float_t C_ModToiletPlunger::C_ComLift::MtrPositToPhyPosit_L(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = (TPmod_LIMIT_mtrLIFT_L/TPmod_LIMIT_phyLIFT_L);

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/**
 * @brief Convert physical position to motor position
 * @param phyPosit
 * @return
 */
int32_t C_ModToiletPlunger::C_ComLift::PhyPositToMtrPosit_R(float_t phyPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = -(TPmod_LIMIT_mtrLIFT_R/TPmod_LIMIT_phyLIFT_R);            // (Encoder total value / Physical total value)

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}


/**
 * @brief Convert motor position to physical position
 * @param mtrPosit
 * @return
 */
float_t C_ModToiletPlunger::C_ComLift::MtrPositToPhyPosit_R(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = -(TPmod_LIMIT_mtrLIFT_R/TPmod_LIMIT_phyLIFT_R);

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}


/**
 * @brief
 * @param posit_L
 * @param posit_R
 * @return
 */
EAppStatus C_ModToiletPlunger::C_ComLift::_UpdateOutput(float_t posit_L, float_t posit_R) {

    DataBuffer<float_t> liftPos = {
      static_cast<float_t>(posit_L),
      static_cast<float_t>(posit_R),
    };

    DataBuffer<float_t> liftPosMeasure = {
      static_cast<float_t>(motor[L]->motorData[C_MtrInstance::DATA_POSIT]),
      static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_POSIT]),
    };

    auto liftSpd = pidPosCtrl.UpdatePidController(liftPos, liftPosMeasure);

    DataBuffer<float_t> liftSpdMeasure = {
      static_cast<float_t>(motor[L]->motorData[C_MtrInstance::DATA_SPEED]),
      static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_SPEED]),
    };

    auto output = pidSpdCtrl.UpdatePidController(liftSpd, liftSpdMeasure);

    mtrOutputBuffer = {
      static_cast<int16_t>(output[L]),
      static_cast<int16_t>(output[R]),
    };

    return APP_OK;
}

} // namespace robotpilots
