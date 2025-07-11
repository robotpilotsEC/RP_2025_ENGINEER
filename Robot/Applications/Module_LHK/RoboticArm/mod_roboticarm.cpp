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
#include "mod_toiletplunger.hpp"

namespace my_engineer {
using my_engineer::MesgCenter;

/*The debug pointer*/
C_ModRoboticArm *P_ModRobticArm_test = nullptr;

  EAppStatus C_ModRoboticArm::InitModule(S_ModInitParam &param) {
    if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

    auto paramRoboticArm = static_cast<S_ModInitParam_RoboticArm &>(param);
    moduleId = paramRoboticArm.moduleId;

    Comjoint_.InitComponent(param);
    Comend_.InitComponent(param);

    /* Register Module */
    CreateModuleTask_();
    RegisterModule_();

    P_ModRobticArm_test = this;

    TotalMesgCenter.subscribe<C_ModToiletPlunger::S_ToiletPlungerInfo>("topic_ToiletPlungerInfo",
      [this](const C_ModToiletPlunger::S_ToiletPlungerInfo &info) {
        // if (moduleState == COM_RESET) return;
        // /*If toilet plunger is not available, robotic arm is not available*/
        // roboticArmInfo.isModuleAvailable = info.isModuleAvailable;
        // if (!roboticArmInfo.isModuleAvailable) {
        //   moduleState = COM_RESET;
        //   Module_FSMFlag_ = FSM_RESET;
        //   return;
        // }
      });

    Module_FSMFlag_ = FSM_RESET;
    moduleState = APP_OK;

    return APP_OK;
  }



  void C_ModRoboticArm::UpdateHandler_() {
    
    if (moduleState == COM_RESET) return;

    Comjoint_.UpdateComponent();
    Comend_.UpdateComponent();

    /*Basic angle update*/
    roboticArmInfo.Angle_Joint1_yaw   = Comjoint_.MtrPositToPhyPosit_Yaw(Comjoint_.jointInfo.angle_joint1_yaw);
    roboticArmInfo.Angle_Joint2_pitch = Comjoint_.MtrPositToPhyPosit_Pitch_1(Comjoint_.jointInfo.angle_joint2_pitch);
    roboticArmInfo.Angle_Joint3_pitch = Comjoint_.MtrPositToPhyPosit_Pitch_2(Comjoint_.jointInfo.angle_joint3_pitch);
    roboticArmInfo.Angle_Joint4_roll  = Comjoint_.MtrPositToPhyPosit_Roll(Comjoint_.jointInfo.angle_joint4_roll);
    roboticArmInfo.Angle_Joint5_pitch = Comend_.MtrPositToPhyPosit_Pitch(Comend_.endInfo.angle_pitch_end);
    roboticArmInfo.Angle_Joint6_roll  = Comend_.MtrPositToPhyPosit_Roll(Comend_.endInfo.angle_roll_end);
    /*is angle reach the goal*/
    roboticArmInfo.isAngleArrived_joint1_yaw   = Comjoint_.jointInfo.isAngleArrived_joint1_yaw;
    roboticArmInfo.isAngleArrived_joint2_pitch = Comjoint_.jointInfo.isAngleArrived_joint2_pitch;
    roboticArmInfo.isAngleArrived_joint3_pitch = Comjoint_.jointInfo.isAngleArrived_joint3_pitch;
    roboticArmInfo.isAngleArrived_joint4_roll  = Comjoint_.jointInfo.isAngleArrived_joint4_roll;
    roboticArmInfo.isAngleArrived_joint5_pitch = Comend_.endInfo.isAngleArrived_pitch_end;
    roboticArmInfo.isAngleArrived_joint6_roll  = Comend_.endInfo.isAngleArrived_roll_end;

    C_MtrKT::FillCanTxBuffer(Comjoint_.motor[C_ComJoint::Y],
                             Comjoint_.mtrCanTxNode[C_ComJoint::Y]->dataBuffer,
                             Comjoint_.mtrOutputBuffer[C_ComJoint::Y]);
    C_MtrKT::FillCanTxBuffer(Comjoint_.motor[C_ComJoint::P1],
                             Comjoint_.mtrCanTxNode[C_ComJoint::P1]->dataBuffer,
                             Comjoint_.mtrOutputBuffer[C_ComJoint::P1]);
    C_MtrKT::FillCanTxBuffer(Comjoint_.motor[C_ComJoint::P2],
                             Comjoint_.mtrCanTxNode[C_ComJoint::P2]->dataBuffer,
                             Comjoint_.mtrOutputBuffer[C_ComJoint::P2]);
    C_MtrKT::FillCanTxBuffer(Comjoint_.motor[C_ComJoint::R],
                              Comjoint_.mtrCanTxNode[C_ComJoint::R]->dataBuffer,
                              Comjoint_.mtrOutputBuffer[C_ComJoint::R]);
    C_MtrDji::FillCanTxBuffer(Comend_.motor[C_ComEnd::R],
                              Comend_.mtrCanTxNode[C_ComEnd::R]->dataBuffer,
                              Comend_.mtrOutputBuffer[C_ComEnd::R]);
    C_MtrDji::FillCanTxBuffer(Comend_.motor[C_ComEnd::P],
                              Comend_.mtrCanTxNode[C_ComEnd::P]->dataBuffer,
                              Comend_.mtrOutputBuffer[C_ComEnd::P]);
  }

void C_ModRoboticArm::HeartbeatHandler_(){if(moduleState == COM_RESET) return;}


EAppStatus C_ModRoboticArm::CreateModuleTask_() {
  
  /*If task is existed,delete the taskhandle*/
  if(moduleTaskHandle != nullptr)vTaskDelete(moduleTaskHandle);
  xTaskCreate(StartRoboticArmModuleTask, "RoboticArmTask", 
              1024, this, proc_ModuleTaskPriority, 
              &moduleTaskHandle);
  
  return APP_OK;
}



EAppStatus C_ModRoboticArm::RestrictRoboticArmCommand_() {
  
  if(moduleState == COM_RESET){
    roboticArmCmd = S_RoboticArmCmd();
    return APP_ERROR;
  }

  if(roboticArmCmd.isAutoCtrl)return COM_BUSY;
  /*Basical joint angle setting*/
  roboticArmCmd.setAngle_joint1_yaw = std::clamp(roboticArmCmd.setAngle_joint1_yaw, -25.0f, 25.0f);
  roboticArmCmd.setAngle_joint2_pitch = std::clamp(roboticArmCmd.setAngle_joint2_pitch, 0.0f, 96.0f);
  if(roboticArmCmd.setAngle_joint2_pitch <= 25.0f) 
    roboticArmCmd.setAngle_joint3_pitch = std::clamp(roboticArmCmd.setAngle_joint3_pitch, 4.8f, 27.0f);
  else if(roboticArmCmd.setAngle_joint2_pitch >= 25.0f) 
    roboticArmCmd.setAngle_joint3_pitch = std::clamp(roboticArmCmd.setAngle_joint3_pitch, 18.8f, 118.0f);
  roboticArmCmd.setAngle_joint4_roll = std::clamp(roboticArmCmd.setAngle_joint4_roll, -90.0f, 90.0f);
  roboticArmCmd.setAngle_joint5_pitch = std::clamp(roboticArmCmd.setAngle_joint5_pitch, -114.8f, 60.0f);      
  roboticArmCmd.setAngle_joint6_roll = std::clamp(roboticArmCmd.setAngle_joint6_roll, -720.0f, 720.0f);   

  /*Spatial pose and its Euler angle setting*/
  roboticArmCmd.setEuler_roll = std::clamp(roboticArmCmd.setEuler_roll, 0.0f, 0.0f);
  roboticArmCmd.setEuler_pitch = std::clamp(roboticArmCmd.setEuler_pitch, 0.0f, 0.0f);
  roboticArmCmd.setEuler_yaw = std::clamp(roboticArmCmd.setEuler_yaw, 0.0f, 0.0f);
  roboticArmCmd.setPosit_X = std::clamp(roboticArmCmd.setPosit_X, int32_t(0), int32_t(0));
  roboticArmCmd.setPosit_Y = std::clamp(roboticArmCmd.setPosit_Y, int32_t(0), int32_t(0));
  roboticArmCmd.setPosit_Z = std::clamp(roboticArmCmd.setPosit_Z, int32_t(0), int32_t(0));

  return APP_OK;
}


} // namespace my_engineer
