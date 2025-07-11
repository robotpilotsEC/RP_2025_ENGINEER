/**
 * @file com_pump.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 气泵模块
 * @version 1.0
 * @date 2024-11-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_subgantry.hpp"

namespace my_engineer {

/**
 * @brief 初始化气泵模块
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CModSubGantry::CComPump::InitComponent(SModInitParam_Base &param){
    // 检查param是否正确
    if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

    // 类型转换
    auto subGantryParam = static_cast<SModInitParam_SubGantry &>(param);

    // 保存GPIO端口和引脚
    LeftPump_GpioPort = subGantryParam.LeftPumpPort;
    LeftPump_GpioPin = subGantryParam.LeftPumpPin;
    RightPump_GpioPort = subGantryParam.RightPumpPort;
    RightPump_GpioPin = subGantryParam.RightPumpPin;
    ArmPump_GpioPort = subGantryParam.ArmPumpPort;
    ArmPump_GpioPin = subGantryParam.ArmPumpPin;

    Component_FSMFlag_ = FSM_RESET;
    componentStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 更新组件
 * 
 */
EAppStatus CModSubGantry::CComPump::UpdateComponent() {
    // 检查组件状态
    if (componentStatus == APP_RESET) return APP_ERROR;

    // 更新组件信息
    pumpInfo.isPumpOn_Left = HAL_GPIO_ReadPin(LeftPump_GpioPort, LeftPump_GpioPin) == MOS_ON_GPIO_STATE;
    pumpInfo.isPumpOn_Right = HAL_GPIO_ReadPin(RightPump_GpioPort, RightPump_GpioPin) == MOS_ON_GPIO_STATE;
    pumpInfo.isPumpOn_Arm = HAL_GPIO_ReadPin(ArmPump_GpioPort, ArmPump_GpioPin) == MOS_ON_GPIO_STATE;


    switch (Component_FSMFlag_){

        case FSM_RESET: {

            HAL_GPIO_WritePin(LeftPump_GpioPort, LeftPump_GpioPin, MOS_OFF_GPIO_STATE);
            HAL_GPIO_WritePin(RightPump_GpioPort, RightPump_GpioPin, MOS_OFF_GPIO_STATE);
            HAL_GPIO_WritePin(ArmPump_GpioPort, ArmPump_GpioPin, MOS_OFF_GPIO_STATE);
            return APP_OK;

        }

        case FSM_PREINIT: {
			Component_FSMFlag_ = FSM_INIT;
			return APP_OK;
		}

        case FSM_INIT: {

            pumpCmd = SPumpCmd();
            componentStatus = APP_OK;
            Component_FSMFlag_ = FSM_CTRL;
            return APP_OK;

        }

        case FSM_CTRL: {

            if(pumpCmd.setPumpOn_Left != pumpInfo.isPumpOn_Left)
                HAL_GPIO_WritePin(LeftPump_GpioPort, LeftPump_GpioPin, 
                    pumpCmd.setPumpOn_Left ? MOS_ON_GPIO_STATE : MOS_OFF_GPIO_STATE);
            if(pumpCmd.setPumpOn_Right != pumpInfo.isPumpOn_Right)
                HAL_GPIO_WritePin(RightPump_GpioPort, RightPump_GpioPin, 
                    pumpCmd.setPumpOn_Right ? MOS_ON_GPIO_STATE : MOS_OFF_GPIO_STATE);
            if(pumpCmd.setPumpOn_Arm != pumpInfo.isPumpOn_Arm)
                HAL_GPIO_WritePin(ArmPump_GpioPort, ArmPump_GpioPin, 
                    pumpCmd.setPumpOn_Arm ? MOS_ON_GPIO_STATE : MOS_OFF_GPIO_STATE);
            return APP_OK;
        }

        default: {
            StopComponent();
            componentStatus = APP_ERROR;
            return APP_ERROR;
        }
    }
}

} // namespace my_engineer
