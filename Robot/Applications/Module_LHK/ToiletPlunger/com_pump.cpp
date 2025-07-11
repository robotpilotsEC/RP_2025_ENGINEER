/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2024-12-20
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-12-20   <td>1.0         <td>Sassinak        <td>First Create.
 * </table>
 */

#include "mod_toiletplunger.hpp"

namespace my_engineer {

  EAppStatus C_ModToiletPlunger::C_ComPump::InitComponent(S_ModInitParam &param) {

    if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

    /* Initialize Component */
    auto toiletplungerparam = static_cast<S_ModInitParam_ToiletPlunger &>(param);
    /*Set Pump GPIO*/
    pumpGpioPort_L = toiletplungerparam.pumpGpioPort_L;
    pumpGpioPin_L = toiletplungerparam.pumpGpioPin_L;
    pumpGpioPort_R = toiletplungerparam.pumpGpioPort_R;
    pumpGpioPin_R = toiletplungerparam.pumpGpioPin_R;
    pumpGpioPort_ARM = toiletplungerparam.pumpGpioPort_ARM;
    pumpGpioPin_ARM = toiletplungerparam.pumpGpioPin_ARM;
    /*Set Solenoid Valve GPIO*/
    svGpioPort_L = toiletplungerparam.svGpioPort_L;
    svGpioPin_L = toiletplungerparam.svGpioPin_L;
    svGpioPort_R = toiletplungerparam.svGpioPort_R;
    svGpioPin_R = toiletplungerparam.svGpioPin_R;
    svGpioPort_M = toiletplungerparam.svGpioPort_M;
    svGpioPin_M = toiletplungerparam.svGpioPin_M;
    /* Set Component Flags */
    Component_FSMFlag_ = FSM_RESET;
    ComponentState = APP_OK;

    return APP_OK;
  }
  
  EAppStatus C_ModToiletPlunger::C_ComPump::UpdateComponent() {

    if (ComponentState == COM_RESET) return APP_ERROR;

    /* Update Info */
    PumpInfo.isPumpOn_L  = HAL_GPIO_ReadPin(pumpGpioPort_L, pumpGpioPin_L);
    PumpInfo.isPumpOn_R  = HAL_GPIO_ReadPin(pumpGpioPort_R, pumpGpioPin_R);
    PumpInfo.isPumpOn_ARM = HAL_GPIO_ReadPin(pumpGpioPort_ARM, pumpGpioPin_ARM);
    PumpInfo.isSVOn_L     = HAL_GPIO_ReadPin(svGpioPort_L, svGpioPin_L);
    PumpInfo.isSVOn_R     = HAL_GPIO_ReadPin(svGpioPort_R, svGpioPin_R);
    PumpInfo.isSVOn_M     = HAL_GPIO_ReadPin(svGpioPort_M, svGpioPin_M);
    switch(Component_FSMFlag_) {
      case FSM_RESET: {
        HAL_GPIO_WritePin(pumpGpioPort_L, pumpGpioPin_L, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pumpGpioPort_R, pumpGpioPin_R, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pumpGpioPort_ARM, pumpGpioPin_ARM, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(svGpioPort_L, svGpioPin_L, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(svGpioPort_R, svGpioPin_R, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(svGpioPort_M, svGpioPin_M, GPIO_PIN_RESET);
        return APP_OK;
      }
      case FSM_PREINIT:{
        HAL_GPIO_WritePin(pumpGpioPort_L, pumpGpioPin_L, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pumpGpioPort_R, pumpGpioPin_R, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pumpGpioPort_ARM, pumpGpioPin_ARM, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(svGpioPort_L, svGpioPin_L, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(svGpioPort_R, svGpioPin_R, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(svGpioPort_M, svGpioPin_M, GPIO_PIN_RESET);
        Component_FSMFlag_ = FSM_INIT;
        return APP_OK;
      }
      case FSM_INIT:{

        PumpCmd = S_PumpCmd();
        ComponentState = APP_OK;
        Component_FSMFlag_ = FSM_CTRL;
        return APP_OK;
      }
      case FSM_CTRL:{

        if (PumpCmd.setPumpOn_L != PumpInfo.isPumpOn_L) 
          HAL_GPIO_WritePin(pumpGpioPort_L, pumpGpioPin_L, PumpCmd.setPumpOn_L ? GPIO_PIN_SET : GPIO_PIN_RESET);

        if (PumpCmd.setPumpOn_R != PumpInfo.isPumpOn_R) 
          HAL_GPIO_WritePin(pumpGpioPort_R, pumpGpioPin_R, PumpCmd.setPumpOn_R ? GPIO_PIN_SET : GPIO_PIN_RESET);

        if (PumpCmd.setPumpOn_ARM != PumpInfo.isPumpOn_ARM)
          HAL_GPIO_WritePin(pumpGpioPort_ARM, pumpGpioPin_ARM, PumpCmd.setPumpOn_ARM ? GPIO_PIN_SET : GPIO_PIN_RESET);

        if (PumpCmd.setSVOn_L != PumpInfo.isSVOn_L)
          HAL_GPIO_WritePin(svGpioPort_L, svGpioPin_L, PumpCmd.setSVOn_L ? GPIO_PIN_SET : GPIO_PIN_RESET);
        if (PumpCmd.setSVOn_R != PumpInfo.isSVOn_R)
          HAL_GPIO_WritePin(svGpioPort_R, svGpioPin_R, PumpCmd.setSVOn_R ? GPIO_PIN_SET : GPIO_PIN_RESET);
        if (PumpCmd.setSVOn_M != PumpInfo.isSVOn_M)
          HAL_GPIO_WritePin(svGpioPort_M, svGpioPin_M, PumpCmd.setSVOn_M ? GPIO_PIN_SET : GPIO_PIN_RESET);
        return APP_OK;
      }

      default:{
        StopComponent();
        ComponentState = APP_ERROR;
        return APP_ERROR;
      }     
    }
  }



}
