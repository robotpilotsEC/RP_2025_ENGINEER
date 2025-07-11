/**
 * @file Core.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 系统核心头文件
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_HPP
#define CORE_HPP

#include "Interface.hpp"
#include "Device.hpp"
#include "Module.hpp"
#include "System.hpp"

namespace my_engineer {

/**
 * @brief 定义系统核心类
 * 
 */
class CSystemCore final {
    // 友元函数
    friend void StartUpdateTask(void *argument);
    friend void StartHeartbeatTask(void *argument);

public:
    // 定义自动操作的任务类型并实例化表示当前任务类型
    enum class EAutoCtrlProcess {
        NONE,
        RETURN_ORIGIN,
        RETURN_DRIVE,
        GROUND_ORE,
        SILVER_ORE,
        GOLD_ORE,
        EXCHANGE,
        PUSH_ORE,
        POP_ORE,
    } currentAutoCtrlProcess_ = EAutoCtrlProcess::NONE;

    // 初始化系统核心
    EAppStatus InitSystemCore();

private:
    // 定义系统核心的状态
    EAppStatus coreStatus = APP_RESET;

    // 定义系统核心响应频率
    const float_t freq = 1000.f; //??为什么是float_t

    // 模块指针
    // CModChassis *pchassis_ = nullptr;
    // // CModGimbal *pgimbal_ = nullptr;
    // // CModGantry *pgantry_ = nullptr;
    // // CModSubGantry *psubgantry_ = nullptr;
    // CModMantis *pmantis_ = nullptr;
    CModController *pcontroller_ = nullptr;

    // 定义系统核心的更新处理
    void UpdateHandler_();

    // 定义系统核心的心跳处理
    void HeartbeatHandler_();


    // 自动操作(启动与停止)
    EAppStatus StartAutoCtrlTask_(EAutoCtrlProcess process);
    EAppStatus StopAutoCtrlTask_();

    // 软件复位
    void RESET_SYSTEM();

    // 声明自动操作的任务函数
    /*
    static void StartReturnOriginTask(void *arg);

    static void StartReturnDriveTask(void *arg);

    static void StartGroundOreTask(void *arg);

    static void StartSilverOreTask(void *arg);

    static void StartGoldOreTask(void *arg);

    static void StartExchangeTask(void *arg);

    static void StartPopOreTask(void *arg);

    static void StartPushOreTask(void *arg);
    */
    
};

extern CSystemCore SystemCore;

} // namespace my_engineer

#endif // CORE_HPP
