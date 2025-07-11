/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-04-02
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-03-30   <td>1.0         <td>qianchen        <td>First Create.
 * </table>
 */
#include "mod_gimbal.hpp"

namespace my_engineer {

EAppStatus C_ModGimbal::C_ComPitch::InitComponent(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  /* Initialize Component */
  auto &gimbalParam = reinterpret_cast<S_ModGimbalInitParam &>(param);
  halTimHandle      = gimbalParam.gimbalPitchHalTimHandle;
  timChannel        = gimbalParam.gimbalPitchTimChannel;

  /* Set Component Flags */
  Component_FSMFlag_ = FSM_RESET;
  ComponentState = APP_OK;

  return APP_OK;
}


EAppStatus C_ModGimbal::C_ComPitch::UpdateComponent() {

  if (ComponentState == COM_RESET) return APP_ERROR;

  switch (Component_FSMFlag_) {
    case FSM_RESET: {
      HAL_TIM_PWM_Stop(halTimHandle, timChannel);
      return APP_OK;
    }
    case FSM_PREINIT:{
      HAL_TIM_PWM_Start(halTimHandle, timChannel);
      ComponentState = APP_OK;
      Component_FSMFlag_ = FSM_CTRL;
      return APP_OK;
    }
    case FSM_INIT: {
      HAL_TIM_PWM_Start(halTimHandle, timChannel);
      ComponentState = APP_OK;
      Component_FSMFlag_ = FSM_CTRL;
      return APP_OK;
    }
    case FSM_CTRL: {
      if (pitchCmd.setStep == 0)        // Middle
        __HAL_TIM_SET_COMPARE(halTimHandle, timChannel, pitchCmd.setMid);
      else if (pitchCmd.setStep == 1)   // Up
        __HAL_TIM_SET_COMPARE(halTimHandle, timChannel, pitchCmd.setUp);
      else if (pitchCmd.setStep == 2)   // Down
        __HAL_TIM_SET_COMPARE(halTimHandle, timChannel, pitchCmd.setDown);
      else if (pitchCmd.setStep == 3)   // Vision
        __HAL_TIM_SET_COMPARE(halTimHandle, timChannel, pitchCmd.setVision);
	  HAL_TIM_PWM_Start(halTimHandle, timChannel);
      return APP_OK;
    }
    default: {
      StopComponent();
      ComponentState = APP_ERROR;
      return APP_ERROR;
    }
  }

}

}
