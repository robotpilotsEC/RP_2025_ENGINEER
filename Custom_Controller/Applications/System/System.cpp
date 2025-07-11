/**
 * @file System.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 系统层源文件
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * @details 注册注销系统以及定义一个映射MAP
 */

#include "System.hpp"
#include "Module.hpp"

namespace my_engineer {

std::map<ESystemID, CSystemBase*> SystemIDMap;

/**
 * @brief 注册系统
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemBase::RegisterSystem_() {

    if (systemID == ESystemID::SYS_NULL) return APP_ERROR;
    
    SystemIDMap.insert(std::make_pair(systemID, this));
    return APP_OK;
}

/**
 * @brief 注销系统
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemBase::UnregisterSystem_() {

    if (systemID == ESystemID::SYS_NULL) return APP_ERROR;

    SystemIDMap.erase(systemID);
    return APP_OK;
}

}   // namespace my_engineer
