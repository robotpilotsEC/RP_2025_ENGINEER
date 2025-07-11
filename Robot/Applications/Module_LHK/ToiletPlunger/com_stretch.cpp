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
EAppStatus C_ModToiletPlunger::C_ComStretch::InitComponent(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  /* Initialize Component */
  auto toiletplungerparam = static_cast<S_ModInitParam_ToiletPlunger &>(param);
  /*Attention! if you don't register motor in motorMap, you may enter an endless loop*/
  motor[L]             = MotorMap.at(toiletplungerparam.stretchMotorID_L);
  motor[R]             = MotorMap.at(toiletplungerparam.stretchMotorID_R);

  /*set Can TxNode*/
  mtrCanTxNode[L] = toiletplungerparam.stretchmotorTxNode_L;
  mtrCanTxNode[R] = toiletplungerparam.stretchmotorTxNode_R;

  /* Initialize PID Controllers */
  toiletplungerparam.stretchPosPidParam.threadNum = 2;
  pidPosCtrl.InitPID(&toiletplungerparam.stretchPosPidParam);

  toiletplungerparam.stretchSpdPidParam.threadNum = 2;
  pidSpdCtrl.InitPID(&toiletplungerparam.stretchSpdPidParam);

  /* Clear Motor Output Buffer */
  mtrOutputBuffer.fill(0);

  /* Set Component Flags */
  Component_FSMFlag_   = FSM_RESET;
  ComponentState = APP_OK;

  return APP_OK;
}


/**
 * @brief
 */
EAppStatus C_ModToiletPlunger::C_ComStretch::UpdateComponent() {

  if (ComponentState == COM_RESET) return APP_ERROR;

  /* Update Info */
  stretchInfo.posit_L =  motor[L]->motorData[C_MtrInstance::DATA_POSIT];
  stretchInfo.posit_R =  motor[R]->motorData[C_MtrInstance::DATA_POSIT];
  stretchInfo.isPositArrived_L = (abs(stretchCmd.setPosit_L - stretchInfo.posit_L) < 8192 );
  stretchInfo.isPositArrived_R = (abs(stretchCmd.setPosit_R - stretchInfo.posit_R) < 8192 );

  /* Gantry Lift Component FSM */
  switch (Component_FSMFlag_) {

    case FSM_RESET: {
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      return APP_OK;
    }

    case FSM_PREINIT: {
      stretchCmd.setPosit_L                            = static_cast<int32_t>(rangeLimit_L * 1.2);
      stretchCmd.setPosit_R                            = static_cast<int32_t>(rangeLimit_R * 1.2);
      motor[L]->motorData[C_MtrInstance::DATA_POSIT] =  stretchCmd.setPosit_L;
      motor[R]->motorData[C_MtrInstance::DATA_POSIT] =  stretchCmd.setPosit_R;
      mtrOutputBuffer.fill(0);
      pidPosCtrl.ResetPidController();
      pidSpdCtrl.ResetPidController();
      Component_FSMFlag_ = FSM_INIT;
      return APP_OK;
    }

    case FSM_INIT: {
      if (motor[L]->motorState == C_MtrInstance::E_MotorStatus::STALL && motor[R]->motorState == C_MtrInstance::E_MotorStatus::STALL) {
        stretchCmd = S_StretchCmd();
        motor[L]->motorData[C_MtrInstance::DATA_POSIT] =  static_cast<int32_t>(8192 * 0.5);
        motor[R]->motorData[C_MtrInstance::DATA_POSIT] =  -static_cast<int32_t>(8192 * 0.5);
        pidPosCtrl.ResetPidController();
        pidSpdCtrl.ResetPidController();
        ComponentState = APP_OK;
        Component_FSMFlag_ = FSM_CTRL;
        return APP_OK;
      }
      stretchCmd.setPosit_L += 200;       // Reset Speed
      stretchCmd.setPosit_R -= 200;       // Reset Speed
      return _UpdateOutput(static_cast<float_t>(stretchCmd.setPosit_L),
                           static_cast<float_t>(stretchCmd.setPosit_R));
    }

    case FSM_CTRL: {
      stretchCmd.setPosit_L = std::clamp(stretchCmd.setPosit_L, rangeLimit_L, static_cast<int32_t>(0));
      stretchCmd.setPosit_R = std::clamp(stretchCmd.setPosit_R, static_cast<int32_t>(0), rangeLimit_R);
      return _UpdateOutput(static_cast<float_t>(stretchCmd.setPosit_L),
                           static_cast<float_t>(stretchCmd.setPosit_R));
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
 * because the motors' position is smaller than the position when it move back,the scale should add minus sign
 * besides, the posit_limit of two motors should exchange the position of the two param in the function std::clamp()
 */

/**
 * @brief Convert physical position to motor position
 * @param phyPosit
 * @return
 */
int32_t C_ModToiletPlunger::C_ComStretch::PhyPositToMtrPosit_L(float_t phyPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = -(TPmod_LIMIT_mtrSTRECH_L/TPmod_LIMIT_phySTRECH_L);

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}


/**
 * @brief Convert motor position to physical position
 * @param mtrPosit
 * @return
 */
float_t C_ModToiletPlunger::C_ComStretch::MtrPositToPhyPosit_L(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = -(TPmod_LIMIT_mtrSTRECH_L/TPmod_LIMIT_phySTRECH_L);

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}


/**
 * @brief Convert physical position to motor position
 * @param phyPosit
 * @return
 */
int32_t C_ModToiletPlunger::C_ComStretch::PhyPositToMtrPosit_R(float_t phyPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = (TPmod_LIMIT_mtrSTRECH_R/TPmod_LIMIT_phySTRECH_R);

  return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
}


/**
 * @brief Convert motor position to physical position
 * @param mtrPosit
 * @return
 */
float_t C_ModToiletPlunger::C_ComStretch::MtrPositToPhyPosit_R(int32_t mtrPosit) {

  const int32_t zeroOffset = 0;
  const float_t scale      = (TPmod_LIMIT_mtrSTRECH_R/TPmod_LIMIT_phySTRECH_R);

  return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
}

/**
 * @brief
 * @param posit_L
 * @param posit_R
 * @return
 */
EAppStatus C_ModToiletPlunger::C_ComStretch::_UpdateOutput(float_t posit_L, float_t posit_R) {

  DataBuffer<float_t> stretchPos = {
    static_cast<float_t>( posit_L),
    static_cast<float_t>( posit_R),
  };

  DataBuffer<float_t> stretchPosMeasure = {
    static_cast<float_t>(motor[L]->motorData[C_MtrInstance::DATA_POSIT]),
    static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_POSIT]),
  };

  auto stretchSpd =
    pidPosCtrl.UpdatePidController(stretchPos, stretchPosMeasure);

  DataBuffer<float_t> stretchSpdMeasure = {
    static_cast<float_t>(motor[L]->motorData[C_MtrInstance::DATA_SPEED]),
    static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_SPEED]),
  };

  auto output =
    pidSpdCtrl.UpdatePidController(stretchSpd, stretchSpdMeasure);

  mtrOutputBuffer = {
    static_cast<int16_t>(output[L]),
    static_cast<int16_t>(output[R]),
  };

  return APP_OK;
}

} // namespace robotpilots
