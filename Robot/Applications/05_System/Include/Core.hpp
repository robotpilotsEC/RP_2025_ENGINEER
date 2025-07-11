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
#include "algo_other.hpp"

#define I_AM_CONTROLLER 0 // 当前板子是控制器

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
        DOGHOLE,
        GROUND_ORE,
        GOLD_ORE,
        EXCHANGE,
        PUSH_ORE,
        POP_ORE,
        TURNOVER,
    } currentAutoCtrlProcess_ = EAutoCtrlProcess::NONE;

    EVarStatus use_Controller_ = false; ///< 是否使用控制器

    // 初始化系统核心
    EAppStatus InitSystemCore();

private:
    // 定义系统核心的状态
    EAppStatus coreStatus = APP_RESET;

    // 定义系统核心响应频率
    const float_t freq = 1000.f;

    // 模块指针
    CModChassis *pchassis_ = nullptr;
    CModGimbal *pgimbal_ = nullptr;
    // CModGantry *pgantry_ = nullptr;
    // CModClimber *pclimber_ = nullptr;
    CModSubGantry *psubgantry_ = nullptr;
    CModArm *parm_ = nullptr;

    TaskHandle_t autoCtrlTaskHandle_ = nullptr;

    // 定义系统核心的更新处理
    void UpdateHandler_();

    // 定义系统核心的心跳处理
    void HeartbeatHandler_();

    // 系统操作方式
    void ControlFromRemote_();
    void ControlFromKeyboard_();
    void ControlFromController_();

    // 自动操作(启动与停止)
    EAppStatus StartAutoCtrlTask_(EAutoCtrlProcess process);
    EAppStatus StopAutoCtrlTask_();


    void StartRobot(bool if_remote_control, bool I_dont_have_a_remote = false);

    // 软件复位
    void RESET_SYSTEM();

    // 声明自动操作的任务函数
    static void StartReturnOriginTask(void *arg);

    static void StartReturnDriveTask(void *arg);

    static void StartDogHoleTask(void *arg);

    static void StartGroundOreTask(void *arg);

    static void StartSilverOreTask(void *arg);

    static void StartGoldOreTask(void *arg);

    static void StartExchangeTask(void *arg);

    static void StartPopOreTask(void *arg);

    static void StartPushOreTask(void *arg);

    static void StartVisionExchangeTask(void *arg);

    static void StartTurnoverTask(void *arg);
    
};

void JointAngleToEulerAngle(const float_t *jointAngle, float_t *eulerAngle);

extern CSystemCore SystemCore;

} // namespace my_engineer

#endif // CORE_HPP
