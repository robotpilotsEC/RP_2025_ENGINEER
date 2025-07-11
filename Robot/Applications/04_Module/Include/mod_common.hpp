/**
 * @file mod_common.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义模块基类
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MOD_COMMON_HPP
#define MOD_COMMON_HPP

#include "Configuration.hpp"
#include "algo_pid.hpp"
#include "Interface.hpp"
#include "Device.hpp"
#include "conf_CanTxNode.hpp"

namespace my_engineer {

/**
 * @brief 模块基类
 * 
 */
class CModBase{
// 声明两个友元函数
friend void StartUpdateTask(void *argument);
friend void StartHeartbeatTask(void *argument);
protected:
    /**
     * @brief 定义一个用于包含模块初始化参数的结构体基类
     * 
     */
    struct SModInitParam_Base{
        EModuleID moduleID = EModuleID::MOD_NULL; ///< 模块ID
    };

    // 模块FSM状态
    FSM_Flag Module_FSMFlag_ = FSM_RESET;

    // 更新处理
    virtual void UpdateHandler_() {}

    // 心跳处理
    virtual void HeartbeatHandler_() {}

    // 创建模块任务(任务在子类定义中声明，在proc_xxx.cpp中定义)
    virtual EAppStatus CreateModuleTask_() { return APP_ERROR; }

    // 注册模块
    EAppStatus RegisterModule_();

    // 注销模块
    EAppStatus UnregisterModule_();

public:

    // 定义组件基类
    class CComponentBase{
    protected:
        // 组件FSM状态
        FSM_Flag Component_FSMFlag_ = FSM_RESET;
    public:

        EAppStatus componentStatus = APP_RESET; ///< 组件状态   

        CComponentBase() = default;
        virtual ~CComponentBase() = default;

        // 组件初始化
        virtual EAppStatus InitComponent(SModInitParam_Base &param) = 0;

        // 组件启动
        virtual EAppStatus StartComponent();

        // 组件停止
        virtual EAppStatus StopComponent();

        // 组件更新
        virtual EAppStatus UpdateComponent() { return APP_ERROR; }

        // 组件心跳
        virtual EAppStatus HeartbeatComponent() { return APP_ERROR; }
    };

    // 模块ID
    EModuleID moduleID = EModuleID::MOD_NULL;

    // 模块状态
    EAppStatus moduleStatus = APP_RESET;

    // 模块任务句柄
    TaskHandle_t moduleTaskHandle = nullptr;

    // 模块构造函数
    CModBase() = default;

    // 模块析构函数
    virtual ~CModBase() {UnregisterModule_();};

    // 模块初始化
    virtual EAppStatus InitModule(SModInitParam_Base &param) = 0;

    // 模块启动
    virtual EAppStatus StartModule();

    // 模块停止
    virtual EAppStatus StopModule();

};

extern std::map<EModuleID, CModBase*> ModuleIDMap;

} // namespace my_engineer

#endif // MOD_COMMON_HPP
