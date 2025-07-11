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

namespace my_engineer {

/**
 * @brief Start ToiletPlunger Module
 * @param arg
 */
void C_ModToiletPlunger::StartToiletPlungerModuleTask(void *arg) {

  /*if the address is null, return*/
  if (arg == nullptr) proc_return();

  /* Get toiletplunger Module Handle */
  auto &toiletplunger = *static_cast<C_ModToiletPlunger *>(arg);

  /* toiletplunger Module Task */
  while (true) {
    /* toiletplunger Module Control FSM */
    switch (toiletplunger.Module_FSMFlag_) {

      case FSM_RESET: {

        toiletplunger.ToiletPlungerInfo.isModuleAvailable = false;
        toiletplunger.comLift_.StopComponent();
        toiletplunger.comStretch_.StopComponent();
        toiletplunger.comPump_.StopComponent();
        proc_waitMs(20);  // 50Hz
        continue;
      }

      case FSM_INIT: {

        proc_waitMs(250);

        toiletplunger.comLift_.StartComponent();
        toiletplunger.comStretch_.StartComponent();
        toiletplunger.comPump_.StartComponent();
        proc_waitUntil(toiletplunger.comLift_.ComponentState == APP_OK
                        && toiletplunger.comStretch_.ComponentState == APP_OK
                        && toiletplunger.comPump_.ComponentState == APP_OK);     
        toiletplunger.ToiletPlungerCmd = S_ToiletPlungerCmd();  
        toiletplunger.ToiletPlungerInfo.isModuleAvailable = true;
        toiletplunger.Module_FSMFlag_ = FSM_CTRL;
        continue;
      }

      case FSM_CTRL: {

        toiletplunger.RestrictToiletPlungerCommand_();

        toiletplunger.comPump_.PumpCmd.setPumpOn_L = toiletplunger.ToiletPlungerCmd.setPumpOn_L;
        toiletplunger.comPump_.PumpCmd.setPumpOn_R = toiletplunger.ToiletPlungerCmd.setPumpOn_R;
        toiletplunger.comPump_.PumpCmd.setPumpOn_ARM = toiletplunger.ToiletPlungerCmd.setPumpOn_ARM;
        toiletplunger.comPump_.PumpCmd.setSVOn_L = toiletplunger.ToiletPlungerCmd.setSVOn_L;
        toiletplunger.comPump_.PumpCmd.setSVOn_R = toiletplunger.ToiletPlungerCmd.setSVOn_R;
        toiletplunger.comPump_.PumpCmd.setSVOn_M = toiletplunger.ToiletPlungerCmd.setSVOn_M;
        
        toiletplunger.comLift_.liftCmd.setPosit_L = C_ComLift::PhyPositToMtrPosit_L(toiletplunger.ToiletPlungerCmd.setPosit_Lift_L);
        toiletplunger.comLift_.liftCmd.setPosit_R = C_ComLift::PhyPositToMtrPosit_R(toiletplunger.ToiletPlungerCmd.setPosit_Lift_R);
        toiletplunger.comStretch_.stretchCmd.setPosit_L = C_ComStretch::PhyPositToMtrPosit_L(toiletplunger.ToiletPlungerCmd.setPosit_Stretch_L);
        toiletplunger.comStretch_.stretchCmd.setPosit_R = C_ComStretch::PhyPositToMtrPosit_R(toiletplunger.ToiletPlungerCmd.setPosit_Stretch_R);
        proc_waitMs(1);  // 1000Hz
        continue;
      }

      default: { toiletplunger.StopModule(); }
    }
  }

  /* Task Exit */
  toiletplunger.moduleTaskHandle = nullptr;
  proc_return();
}

} // namespace robotpilots
