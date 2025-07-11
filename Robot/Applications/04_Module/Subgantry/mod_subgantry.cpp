/**
 * @file mod_subgantry.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 子龙门模块
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_subgantry.hpp"

namespace my_engineer {

CModSubGantry *pSubGantry_test = nullptr;

/**
 * @brief 初始化子龙门模块
 * 
 * @param param
 * @return EAppStatus 
 */
EAppStatus CModSubGantry::InitModule(SModInitParam_Base &param){
    // 检查param是否正确
    if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

    // 类型转换
    auto subGantryParam = static_cast<SModInitParam_SubGantry &>(param);
    moduleID = subGantryParam.moduleID;

    // 初始化气泵组件
    comPump_.InitComponent(param);

    // 初始化子龙门升降组件
    comLift_.InitComponent(param);

    // 初始化子龙门伸缩组件
    comStretch_.InitComponent(param);

    // 创建任务并注册模块
    CreateModuleTask_();
    RegisterModule_();

    // test
    pSubGantry_test = this;

    Module_FSMFlag_ = FSM_RESET;
    moduleStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 更新处理
 * 
 * @return EAppStatus 
 */
void CModSubGantry::UpdateHandler_(){
    // 检查模块状态
    if (moduleStatus == APP_RESET) return;

    // 更新所有组件
    comPump_.UpdateComponent();
    comLift_.UpdateComponent();
    comStretch_.UpdateComponent();

    // 更新模块信息
    subGantryInfo.isPumpOn_Left = comPump_.pumpInfo.isPumpOn_Left;
    subGantryInfo.isPumpOn_Right = comPump_.pumpInfo.isPumpOn_Right;
    subGantryInfo.isPumpOn_Arm = comPump_.pumpInfo.isPumpOn_Arm;
    subGantryInfo.liftPosit_L = 
        comLift_.MtrPositToPhyPosit_L(comLift_.liftInfo.posit_L);
    subGantryInfo.liftPosit_R = 
        comLift_.MtrPositToPhyPosit_R(comLift_.liftInfo.posit_R);
    subGantryInfo.stretchPosit_L =
        comStretch_.MtrPositToPhyPosit_L(comStretch_.stretchInfo.posit_L);
    subGantryInfo.stretchPosit_R =
        comStretch_.MtrPositToPhyPosit_R(comStretch_.stretchInfo.posit_R);
    subGantryInfo.isPositArrived_Lift_L = comLift_.liftInfo.isPositArrived_L;
    subGantryInfo.isPositArrived_Lift_R = comLift_.liftInfo.isPositArrived_R;
    subGantryInfo.isPositArrived_Stretch_L = comStretch_.stretchInfo.isPositArrived_L;
    subGantryInfo.isPositArrived_Stretch_R = comStretch_.stretchInfo.isPositArrived_R;

    // 填充电机发送缓冲区
    CDevMtrDJI::FillCanTxBuffer(comLift_.motor[0],
                                comLift_.mtrCanTxNode[0]->dataBuffer,
                                comLift_.mtrOutputBuffer[0]);
    CDevMtrDJI::FillCanTxBuffer(comLift_.motor[1],
                                comLift_.mtrCanTxNode[1]->dataBuffer,
                                comLift_.mtrOutputBuffer[1]);
    CDevMtrDJI::FillCanTxBuffer(comStretch_.motor[0],
                                comStretch_.mtrCanTxNode[0]->dataBuffer,
                                comStretch_.mtrOutputBuffer[0]);
    CDevMtrDJI::FillCanTxBuffer(comStretch_.motor[1],
                                comStretch_.mtrCanTxNode[1]->dataBuffer,
                                comStretch_.mtrOutputBuffer[1]);
}


/**
 * @brief 心跳处理
 * 
 */
void CModSubGantry::HeartbeatHandler_(){

}

/**
 * @brief 创建模块任务
 * 
 * @return EAppStatus 
 */
EAppStatus CModSubGantry::CreateModuleTask_(){
    
    // 任务已存在，删除任务
    if(moduleTaskHandle != nullptr) vTaskDelete(moduleTaskHandle);

    // 创建任务
    xTaskCreate(StartSubGantryModuleTask, "SubGantry Module Task", 
                512, this, proc_ModuleTaskPriority, 
                &moduleTaskHandle);

    return APP_OK;
}

/**
 * @brief 限制子龙门模块的控制命令大小
 * 
 * @details 会在StartSubGantryModuleTask中被调用
 */
EAppStatus CModSubGantry::RestrictSubGantryCommand_() {

    // 检查模块状态
    if (moduleStatus == APP_RESET) {
        subGantryCmd = SSubGantryCmd();
        return APP_ERROR;
    }

    // 限制子龙门各模块的控制命令大小
    subGantryCmd.setLiftPosit_L = 
        std::clamp(subGantryCmd.setLiftPosit_L, 0.0f, SUB_GANTRY_LIFT_PHYSICAL_RANGE_L);
    subGantryCmd.setLiftPosit_R =
        std::clamp(subGantryCmd.setLiftPosit_R, 0.0f, SUB_GANTRY_LIFT_PHYSICAL_RANGE_R);
    subGantryCmd.setStretchPosit_L =
        std::clamp(subGantryCmd.setStretchPosit_L, 0.0f, SUB_GANTRY_STRETCH_PHYSICAL_RANGE_L);
    subGantryCmd.setStretchPosit_R =
        std::clamp(subGantryCmd.setStretchPosit_R, 0.0f, SUB_GANTRY_STRETCH_PHYSICAL_RANGE_R);

    // 自动控制启用，则不继续做限制
    if (subGantryCmd.isAutoCtrl) return APP_OK;

    return APP_OK;

}

} // namespace my_engineer
