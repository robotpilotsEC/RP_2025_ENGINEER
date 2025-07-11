/******************************************************************************
 * @brief        
 * 
 * @file         proc_gimbal.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-03
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_gimbal.hpp"

namespace my_engineer {

void C_ModGimbal::StartGimbalModuleTask(void *arg) {

  if (arg == nullptr) proc_return();

  /* Get Gimbal Module Handle */
  auto &gimbal = *static_cast<C_ModGimbal *>(arg);


  /* Gimbal Module Task */
  while (true) {

    /* Gimbal Module Control FSM */
    switch (gimbal.Module_FSMFlag_) {

      case FSM_RESET: {

        gimbal.gimbalInfo.isModuleAvailable = false;
        gimbal.comPitch_.StopComponent();

        proc_waitMs(20);  // 50Hz
        break;
      }

      case FSM_INIT: {

        gimbal.comPitch_.StartComponent();

        gimbal.gimbalCmd = S_GimbalCmd();
        gimbal.gimbalInfo.isModuleAvailable = true;
        gimbal.Module_FSMFlag_ = FSM_CTRL;
        gimbal.moduleState = APP_OK;
        gimbal.gimbalCmd.setStep_Pitch = 1;   // up
        break;
      }

      case FSM_CTRL: {

        gimbal.RestrictGimbalCommand_();

        gimbal.comPitch_.pitchCmd.setStep = gimbal.gimbalCmd.setStep_Pitch;
        proc_waitMs(1);   // 1000Hz
        break;
      }

      default: { gimbal.StopModule(); }
    }
  }

  /* Task Exit */
  gimbal.moduleTaskHandle = nullptr;
  proc_return();
}

}
