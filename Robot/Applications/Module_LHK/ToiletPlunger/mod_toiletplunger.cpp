/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-01-08
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-05-10   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 */

#include "mod_toiletplunger.hpp"
#include "mod_roboticarm.hpp"

namespace my_engineer {

/*The debug pointer*/
C_ModToiletPlunger *P_ModToiletPlunger_test = nullptr;

/**
 * @brief
 * @param param
 * @return
 */
EAppStatus C_ModToiletPlunger::InitModule(S_ModInitParam &param) {

  if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  /* Initialize Module */
  auto toiletplungerParam = static_cast<S_ModInitParam_ToiletPlunger &>(param);
  moduleId = toiletplungerParam.moduleId;

  /* Initialize Components */
  comLift_.InitComponent(param);
  comPump_.InitComponent(param);
  comStretch_.InitComponent(param);

  /* Register Module */
  CreateModuleTask_();
  RegisterModule_();

  P_ModToiletPlunger_test = this;

  /* Set Component Flags */
  Module_FSMFlag_ = FSM_RESET;
  moduleState = APP_OK;
  return APP_OK;
}


/**
 * @brief Update ToiletPlunger Module
 */
void C_ModToiletPlunger::UpdateHandler_() {

  if (moduleState == COM_RESET) return;

  /* Update Component */
  comLift_.UpdateComponent();
  comPump_.UpdateComponent();
  comStretch_.UpdateComponent();

  /* Update Lift Component Info */
  ToiletPlungerInfo.posit_Lift_L =
    C_ComLift::MtrPositToPhyPosit_L(comLift_.liftInfo.posit_L);
  ToiletPlungerInfo.posit_Lift_R =
    C_ComLift::MtrPositToPhyPosit_R(comLift_.liftInfo.posit_R);
  /* Update Strech Component Info */
  ToiletPlungerInfo.posit_Stretch_L =
    C_ComStretch::MtrPositToPhyPosit_L(comStretch_.stretchInfo.posit_L);
  ToiletPlungerInfo.posit_Stretch_R =
    C_ComStretch::MtrPositToPhyPosit_R(comStretch_.stretchInfo.posit_R);
  /* Update Pump Component Info */

  /*Update total info*/
  ToiletPlungerInfo.isPumpOn_L = comPump_.PumpInfo.isPumpOn_L;
  ToiletPlungerInfo.isPumpOn_R = comPump_.PumpInfo.isPumpOn_R;
  ToiletPlungerInfo.isPumpOn_ARM = comPump_.PumpInfo.isPumpOn_ARM;
  ToiletPlungerInfo.isSVOn_L = comPump_.PumpInfo.isSVOn_L;
  ToiletPlungerInfo.isSVOn_R = comPump_.PumpInfo.isSVOn_R;
  ToiletPlungerInfo.isSVOn_M = comPump_.PumpInfo.isSVOn_M;
  ToiletPlungerInfo.isPositArrived_Lift_L = comLift_.liftInfo.isPositArrived_L;
  ToiletPlungerInfo.isPositArrived_Lift_R = comLift_.liftInfo.isPositArrived_R;
  ToiletPlungerInfo.isPositArrived_Stretch_L = comStretch_.stretchInfo.isPositArrived_L;
  ToiletPlungerInfo.isPositArrived_Stretch_R = comStretch_.stretchInfo.isPositArrived_R;

  /* Send Motor Output */
  C_MtrDji::FillCanTxBuffer(comLift_.motor[C_ComLift::L],
                           comLift_.mtrCanTxNode[C_ComLift::L]->dataBuffer,
                           comLift_.mtrOutputBuffer[C_ComLift::L]);
  C_MtrDji::FillCanTxBuffer(comLift_.motor[C_ComLift::R],
                           comLift_.mtrCanTxNode[C_ComLift::R]->dataBuffer,
                           comLift_.mtrOutputBuffer[C_ComLift::R]);
  C_MtrDji::FillCanTxBuffer(comStretch_.motor[C_ComStretch::L],
                            comStretch_.mtrCanTxNode[C_ComStretch::L]->dataBuffer,
                           comStretch_.mtrOutputBuffer[C_ComStretch::L]);
  C_MtrDji::FillCanTxBuffer(comStretch_.motor[C_ComStretch::R],
                           comStretch_.mtrCanTxNode[C_ComStretch::R]->dataBuffer,
                           comStretch_.mtrOutputBuffer[C_ComStretch::R]);
}


/**
 * @brief
 */
void C_ModToiletPlunger::HeartbeatHandler_() {

}


/**
 * @brief
 * @return
 */
EAppStatus C_ModToiletPlunger::CreateModuleTask_() {

  if (moduleTaskHandle != nullptr) vTaskDelete(moduleTaskHandle);
  xTaskCreate(StartToiletPlungerModuleTask, "Toilet Plunger Module Task",
              512, this, proc_ModuleTaskPriority,
              &moduleTaskHandle);

  return APP_OK;
}


/**
 * @brief Limit the size of toilet plunger module control commands
 * @return
 */
EAppStatus C_ModToiletPlunger::RestrictToiletPlungerCommand_() {

    if (moduleState == COM_RESET) {
      ToiletPlungerCmd = S_ToiletPlungerCmd();
      return APP_ERROR;
    }

    // auto it = ModuleMap.find(E_ModuleID::MOD_ROBOTIC_ARM);
    // C_ModRoboticArm* roboticArm_ = nullptr;
    // if (it != ModuleMap.end()) {
    //   roboticArm_ = reinterpret_cast<C_ModRoboticArm *>(it->second);
    // }

    if (ToiletPlungerCmd.isAutoCtrl) return COM_BUSY;

    /* Mechanical Limit */
    ToiletPlungerCmd.setPosit_Lift_L = std::clamp(ToiletPlungerCmd.setPosit_Lift_L, 0.0f, TPmod_LIMIT_phyLIFT_L);
    ToiletPlungerCmd.setPosit_Lift_R = std::clamp(ToiletPlungerCmd.setPosit_Lift_R, 0.0f, TPmod_LIMIT_phyLIFT_R);
    ToiletPlungerCmd.setPosit_Stretch_L = std::clamp(ToiletPlungerCmd.setPosit_Stretch_L,0.0f,TPmod_LIMIT_phySTRECH_L);
    ToiletPlungerCmd.setPosit_Stretch_R = std::clamp(ToiletPlungerCmd.setPosit_Stretch_R,0.0f, TPmod_LIMIT_phySTRECH_R);

    // static float_t L0 = 400.0f;                                                            ///< The length of the J2 (400mm)
    // static float_t alpha = 15.0f;                                                           ///< The angle between the J2 and the Left-toiletplunger
    // static float_t beta = roboticArm_->roboticArmInfo.Angle_Joint1_yaw;                                                            ///< The angle between the J2 and the Right-toiletplunger
    // /*In some special pose,you need to reset the range of joints*/
    //  if(roboticArm_ != nullptr){
    //    if(beta >= 0.0f && roboticArm_->roboticArmInfo.Angle_Joint2_pitch <= 25.0f){
    //       float_t detaA = ((L0-(TPmod_LIMIT_phySTRECH_L/tanf(alpha*PI/180.0f)))*sinf(beta*PI/180.0f))/sinf(PI-(beta+alpha)*PI/180.0f);
    //       ToiletPlungerCmd.setPosit_Stretch_L = std::clamp(ToiletPlungerCmd.setPosit_Stretch_L,abs(detaA),TPmod_LIMIT_phySTRECH_L);
    //    }
    //  }

    return APP_OK;
}

}
