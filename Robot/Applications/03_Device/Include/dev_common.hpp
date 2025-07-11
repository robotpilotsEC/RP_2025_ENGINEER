/**
 * @file dev_common.hpp
 * @author Zoe
 * @brief 定义各种设备的基类
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 定义设备基类
 * 定义设备的注册、注销、初始化、启动、停止、更新、心跳方法。
 * 定义一个包含设备类型的枚举类型。
 */

#ifndef DEV_COMMON_HPP
#define DEV_COMMON_HPP

#include "Configuration.hpp"

namespace my_engineer {

class CDevBase{
    friend void StartUpdateTask(void *argument);
    friend void StartHeartbeatTask(void *argument);
protected:
    /**
     * @brief 定义一个用于包含设备初始化参数的结构体基类
     * 
     */
    struct SDevInitParam_Base{
        EDeviceID deviceID = EDeviceID::DEV_NULL; ///< 设备ID
    };

    // 上一次心跳时间
    uint32_t lastHeartbeatTime_ = 0;

    /**
     * @brief 设备更新处理
     * 
     */
    virtual void UpdateHandler_() { }

    /**
     * @brief 设备心跳处理
     * 
     */
    virtual void HeartbeatHandler_() { }

    /**
     * @brief 设备注册
     * 
     */
    EAppStatus RegisterDevice_();

    /**
     * @brief 设备注销
     * 
     */
    EAppStatus UnregisterDevice_();

public:
    /**
     * @brief 定义一个包含设备类型的枚举类型。
     * 并定义一个记录设备类型的变量。
     * 
     */
    enum class EDevType{
        DEV_UNDEF = -1, ///< 未定义
        DEV_RC,         ///< 遥控器
        DEV_BUZZER,     ///< 蜂鸣器
        DEV_MTR,        ///< 电机
        DEV_SERVO,      ///< 舵机
        DEV_MEMS,       ///< MEMS传感器
        DEV_REFEREE,    ///< 裁判系统
        DEV_VISION,     ///< 视觉系统
        DEV_LED,        ///< LED灯
        DEV_BOARD_LINK, ///< 板间通信设备
        DEV_CONTROLLER_LINK, ///< 控制器通信设备
        DEV_OTHER,      ///< 其他设备
    } deviceType = EDevType::DEV_UNDEF;    ///< 设备类型

    EDeviceID deviceID = EDeviceID::DEV_NULL; ///< 设备ID

    EAppStatus deviceStatus = APP_RESET; ///< 设备状态

    CDevBase() = default; ///< 构造函数

    virtual ~CDevBase() {UnregisterDevice_();} ///< 析构函数

    /**
     * @brief 初始化设备
     * @param pStructInitParam - 参数结构体指针
     * 
     * @note 纯虚函数，必须由派生类实现
     */
    virtual EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) = 0;

    /**
     * @brief 启动设备
     * 
     */
    virtual EAppStatus StartDevice() { return APP_ERROR; }

    /**
     * @brief 停止设备
     * 
     */
    virtual EAppStatus StopDevice() { return APP_ERROR; }
};

extern std::map<EDeviceID, CDevBase *> DeviceIDMap; ///< 设备实例映射表


} // namespace my_engineer

#endif // DEV_COMMON_HPP
