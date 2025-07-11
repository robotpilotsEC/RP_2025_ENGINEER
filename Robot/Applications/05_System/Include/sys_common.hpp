/**
 * @file sys_common.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义系统基类
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SYS_COMMON_HPP
#define SYS_COMMON_HPP

#include "Configuration.hpp"

namespace my_engineer {

/**
 * @brief 系统基类
 * 
 */
class CSystemBase{
    // 声明两个友元函数
    // 这个和别的层中的StartUpdateTask不一样,因为它要求频率不那么高
    friend void StartSystemUpdateTask(void *argument); 
    friend void StartUpdateTask(void *argument);
    friend void StartHeartbeatTask(void *argument);

protected:

    // 系统任务句柄
    TaskHandle_t systemTaskHandle = nullptr;

    // 声明更新函数
    virtual void UpdateHandler_() {}

    // 声明心跳函数
    virtual void HeartbeatHandler_() {}

    // 注册系统
    EAppStatus RegisterSystem_();

    // 注销系统
    EAppStatus UnregisterSystem_();

public:

    // 系统初始化参数结构体
    struct SSystemInitParam_Base{
        ESystemID systemID = ESystemID::SYS_NULL; ///< 系统ID
    };

    // 系统ID
    ESystemID systemID = ESystemID::SYS_NULL;

    // 系统状态
    EAppStatus systemStatus = APP_RESET;

    CSystemBase() = default;
    virtual ~CSystemBase() { UnregisterSystem_(); }
    virtual EAppStatus InitSystem(SSystemInitParam_Base *pStruct) = 0;

};

extern std::map<ESystemID, CSystemBase*> SystemIDMap;


}   // namespace my_engineer

#endif // SYS_COMMON_HPP
