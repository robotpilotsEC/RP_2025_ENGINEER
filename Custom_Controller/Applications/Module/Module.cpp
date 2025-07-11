/**
 * @file Module.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 模块层的汇总源文件
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 * @details
 * 定义模块的map容器，用于存储所有模块的实例。
 * 在这里重写模块启动和停止方法
 * 重写模块注册和注销方法(因为容器被定义在这里)
 * 重写组件启动和停止方法
 */

#include "Module.hpp"

namespace my_engineer {

std::map<EModuleID, CModBase*> ModuleIDMap;

/**
 * @brief 启动模块
 * 
 * @return EAppStatus 
 */
EAppStatus CModBase::StartModule() {
    
    if (moduleStatus == APP_RESET) return APP_ERROR;

    Module_FSMFlag_ = FSM_INIT; // 开始初始化模块

    moduleStatus = APP_BUSY;

    return APP_OK;
}

/**
 * @brief 停止模块
 * 
 * @return EAppStatus 
 */
EAppStatus CModBase::StopModule() {
    
    if (moduleStatus == APP_RESET) return APP_ERROR;

    Module_FSMFlag_ = FSM_RESET; // 将模块设置为重置状态
    CreateModuleTask_(); // 创建模块任务

    moduleStatus = APP_OK;

    return APP_OK;

}

/**
 * @brief 注册模块
 * 
 * @return EAppStatus 
 */
EAppStatus CModBase::RegisterModule_() {
    
    if (moduleID == EModuleID::MOD_NULL) return APP_ERROR;

    ModuleIDMap.insert(std::make_pair(moduleID, this));
    return APP_OK;
}

/**
 * @brief 注销模块
 * 
 * @return EAppStatus 
 */
EAppStatus CModBase::UnregisterModule_() {
    
    if (moduleID == EModuleID::MOD_NULL) return APP_ERROR;

    ModuleIDMap.erase(moduleID);
    return APP_OK;
}

/**
 * @brief 启动组件
 * 
 * @return EAppStatus 
 */
EAppStatus CModBase::CComponentBase::StartComponent() {
    
    if (componentStatus == APP_RESET) return APP_ERROR;

    Component_FSMFlag_ = FSM_PREINIT; // 开始初始化组件
    componentStatus = APP_BUSY;

    return APP_OK;
}

/**
 * @brief 停止组件
 * 
 * @return EAppStatus 
 */
EAppStatus CModBase::CComponentBase::StopComponent() {
    
    if (componentStatus == APP_RESET) return APP_ERROR;

    Component_FSMFlag_ = FSM_RESET; // 将组件设置为重置状态
    componentStatus = APP_OK;

    return APP_OK;
}

} // namespace my_engineer
