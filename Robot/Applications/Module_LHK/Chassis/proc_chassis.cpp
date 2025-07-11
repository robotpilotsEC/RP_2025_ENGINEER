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

/**
 * @brief
 * @param arg
 */
void C_ModChassis::StartChassisModuleTask(void *arg) {

  if (arg == nullptr) proc_return();

  /* Get Chassis Module Handle */
  auto &chassis = *static_cast<C_ModChassis *>(arg);

  /* Chassis Module Task */
  while (true) {

    /* FSM */
    switch (chassis.Module_FSMFlag_) {

      case FSM_RESET: {

        chassis.chassisInfo.isModuleAvailable = false;
        chassis.comWheelset_.StopComponent();

        proc_waitMs(20);  // 50Hz
        continue;
      }

      case FSM_INIT: {

        chassis.comWheelset_.StartComponent();

        chassis.chassisCmd = S_ChassisCmd();
        chassis.chassisInfo.isModuleAvailable = true;
        chassis.Module_FSMFlag_ = FSM_CTRL;    // Enter Control Mode
        continue;
      }

      case FSM_CTRL: {

        chassis.RestrictChassisCommand_();

        chassis.comWheelset_.wheelsetCmd.speed_Y = chassis.chassisCmd.speed_Y * 80;
        chassis.comWheelset_.wheelsetCmd.speed_X = chassis.chassisCmd.speed_X * 80;
        chassis.comWheelset_.wheelsetCmd.speed_W = chassis.chassisCmd.speed_W * 40;
        proc_waitMs(1);   // 1000Hz
        continue;
      }

      default: { chassis.StopModule(); }
    }
  }

  /* Task Exit */
  chassis.moduleTaskHandle = nullptr;
  proc_return();
}

} // namespace robotpilots