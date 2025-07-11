/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-01-11
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-12-16   <td>1.0         <td>Sassinak  <td>First Create.
 * </table>
 */

#include "mod_roboticarm.hpp"

namespace my_engineer{

  void C_ModRoboticArm::StartRoboticArmModuleTask(void *arg) {
    
    if(arg == nullptr) proc_return();
    auto &robotic_arm = *static_cast<C_ModRoboticArm *>(arg);

    while (true) {
      switch (robotic_arm.Module_FSMFlag_) {
        case FSM_RESET: {
          robotic_arm.roboticArmInfo.isModuleAvailable = false;
          robotic_arm.Comjoint_.StopComponent();
          robotic_arm.Comend_.StopComponent();
          proc_waitMs(20);
          continue;
        }
        case FSM_INIT: {
          proc_waitMs(250);
          robotic_arm.Comjoint_.StartComponent();
          robotic_arm.Comend_.StartComponent();
          proc_waitUntil(robotic_arm.Comjoint_.ComponentState == APP_OK
                          && robotic_arm.Comend_.ComponentState == APP_OK);
          robotic_arm.roboticArmCmd = S_RoboticArmCmd();
          robotic_arm.roboticArmInfo.isModuleAvailable = true;
          robotic_arm.Module_FSMFlag_ = FSM_CTRL;
          continue;
        }
        case FSM_CTRL: {
          robotic_arm.RestrictRoboticArmCommand_();
          robotic_arm.Comjoint_.jointCmd.setAngle_joint1_yaw = robotic_arm.Comjoint_.PhyPositToMtrPosit_Yaw(robotic_arm.roboticArmCmd.setAngle_joint1_yaw);
          robotic_arm.Comjoint_.jointCmd.setAngle_joint2_pitch = robotic_arm.Comjoint_.PhyPositToMtrPosit_Pitch_1(robotic_arm.roboticArmCmd.setAngle_joint2_pitch);
          robotic_arm.Comjoint_.jointCmd.setAngle_joint3_pitch = robotic_arm.Comjoint_.PhyPositToMtrPosit_Pitch_2(robotic_arm.roboticArmCmd.setAngle_joint3_pitch);
          robotic_arm.Comjoint_.jointCmd.setAngle_joint4_roll = robotic_arm.Comjoint_.PhyPositToMtrPosit_Roll(robotic_arm.roboticArmCmd.setAngle_joint4_roll);
          robotic_arm.Comend_.endCmd.setAngle_roll_end = robotic_arm.Comend_.PhyPositToMtrPosit_Roll(robotic_arm.roboticArmCmd.setAngle_joint6_roll);
          robotic_arm.Comend_.endCmd.setAngle_pitch_end = robotic_arm.Comend_.PhyPositToMtrPosit_Pitch(robotic_arm.roboticArmCmd.setAngle_joint5_pitch);
          proc_waitMs(1);
          continue;
        } 
        default: {robotic_arm.StopModule();}
      }
    }
    robotic_arm.moduleTaskHandle = nullptr;
    proc_return();
  }
}
