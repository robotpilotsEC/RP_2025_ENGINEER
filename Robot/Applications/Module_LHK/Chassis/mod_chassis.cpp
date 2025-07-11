/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2024-12-20
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-05-10   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 */

#include "mod_chassis.hpp"

namespace my_engineer {

C_ModChassis *Chassis_test = nullptr;

/**
 * @brief
 * @param param
 * @return
 */
EAppStatus C_ModChassis::InitModule(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  /* Initialize Module */
  auto chassisParam = static_cast<S_ModInitParam_Chassis &>(param);
  moduleId = chassisParam.moduleId;

  /* Initialize Components */
  comWheelset_.InitComponent(param);

  /*Creat Task and Register*/
  CreateModuleTask_();
  RegisterModule_();

  // test
  Chassis_test = this;

  /* Set Component Flags */
  Module_FSMFlag_ = FSM_RESET;
  moduleState = APP_OK;

  return APP_OK;
}


/**
 * @brief Update Chassis Module 
 */
void C_ModChassis::UpdateHandler_() {

  if (moduleState == COM_RESET) return;

  /* Update Component */
  comWheelset_.UpdateComponent();

  /* Send Motor Output */
  C_MtrDji::FillCanTxBuffer(comWheelset_.motor[C_ComWheelset::LF],
                            comWheelset_.mtrCanTxNode[C_ComWheelset::LF]->dataBuffer,
                            comWheelset_.mtrOutputBuffer[C_ComWheelset::LF]);
  C_MtrDji::FillCanTxBuffer(comWheelset_.motor[C_ComWheelset::RF],
                            comWheelset_.mtrCanTxNode[C_ComWheelset::RF]->dataBuffer,
                            comWheelset_.mtrOutputBuffer[C_ComWheelset::RF]);
  C_MtrDji::FillCanTxBuffer(comWheelset_.motor[C_ComWheelset::LB],
                            comWheelset_.mtrCanTxNode[C_ComWheelset::LB]->dataBuffer,
                            comWheelset_.mtrOutputBuffer[C_ComWheelset::LB]);
  C_MtrDji::FillCanTxBuffer(comWheelset_.motor[C_ComWheelset::RB],
                            comWheelset_.mtrCanTxNode[C_ComWheelset::RB]->dataBuffer,
                            comWheelset_.mtrOutputBuffer[C_ComWheelset::RB]);
}


/**
 * @brief
 */
void C_ModChassis::HeartbeatHandler_() {}


EAppStatus C_ModChassis::CreateModuleTask_() {

  if (moduleTaskHandle != nullptr) vTaskDelete(moduleTaskHandle);
  xTaskCreate(StartChassisModuleTask, "Chassis Module Task",
              512, this, proc_ModuleTaskPriority,
              &moduleTaskHandle);

  return APP_OK;
}


/**
 * @brief Limit the size of chassis module control commands
 * @return
 */
EAppStatus C_ModChassis::RestrictChassisCommand_() {

  if (moduleState == COM_RESET) {
    chassisCmd = S_ChassisCmd();
    return APP_ERROR;
  }

  chassisCmd.speed_X = std::clamp(chassisCmd.speed_X, -100.0f, 100.0f);
  chassisCmd.speed_Y = std::clamp(chassisCmd.speed_Y, -100.0f, 100.0f);
  chassisCmd.speed_W = std::clamp(chassisCmd.speed_W, -100.0f, 100.0f);

  return APP_OK;
}

} // namespace robotpilots
