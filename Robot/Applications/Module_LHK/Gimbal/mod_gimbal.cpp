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


C_ModGimbal *Gimbal_test = nullptr;  

EAppStatus C_ModGimbal::InitModule(S_ModInitParam &param){

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  auto &gimbalParam = static_cast<S_ModGimbalInitParam &>(param);
  moduleId = gimbalParam.moduleId;

  comPitch_.InitComponent(param);

  /* Register Module */
  CreateModuleTask_();
  RegisterModule_();

  Gimbal_test = this;

  /* Set Component Flags */
  Module_FSMFlag_ = FSM_RESET;
  moduleState = APP_OK;

  return APP_OK;
}  

void C_ModGimbal::UpdateHandler_(){

  if (moduleState == COM_RESET) return;

  comPitch_.UpdateComponent();

  /* Update Gimbal Info */
  gimbalInfo.step_pitch = comPitch_.pitchCmd.setStep;
}

void C_ModGimbal::HeartbeatHandler_() {

  if (moduleState == COM_RESET) return;

}

EAppStatus C_ModGimbal::CreateModuleTask_() {

  if (moduleTaskHandle != nullptr) vTaskDelete(moduleTaskHandle);
  xTaskCreate(StartGimbalModuleTask, "Gimbal Module Task",
              512, this, 5,
              &moduleTaskHandle);

  return APP_OK;
}

EAppStatus C_ModGimbal::RestrictGimbalCommand_() {

  if (moduleState == COM_RESET) {
    gimbalCmd = S_GimbalCmd();
    return APP_ERROR;
  }

  if (gimbalCmd.isAutoCtrl)
    return COM_BUSY;

  gimbalCmd.setStep_Pitch =
    std::clamp(gimbalCmd.setStep_Pitch, 0.0f, 3.0f);

  return APP_OK;
}


}
