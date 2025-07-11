/**
 * @file mtr_common.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义电机设备基类
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MTR_COMMON_HPP
#define MTR_COMMON_HPP

#include "dev_common.hpp"

namespace my_engineer {

/**
 * @brief 电机设备基类
 * 
 */
class CDevMtr : public CDevBase
{
protected:
    /**
     * @brief 定义包含电机初始化参数的结构体
     * 
     */
    struct SMtrInitParam_Base : public SDevInitParam_Base{
        EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 电机CAN接口ID
    };

    // 注册与注销电机设备
    EAppStatus RegisterMotor_();
    EAppStatus UnregisterMotor_();

public:

    /**
     * @brief 电机类型枚举变量
     * 
     */
    enum class EMotorType{
        MTR_UNDEF = -1, ///< 未定义电机
        MTR_DM4310,     ///< DM4310电机
        MTR_M2006,      ///< M2006电机
        MTR_M3508,      ///< M3508电机
        MTR_RM6020,     ///< RM6020电机
        MTR_KT4010,     ///< KT4010电机
    }motorType = EMotorType::MTR_UNDEF;

    /**
     * @brief 电机状态枚举变量
     * 
     */
    enum class EMotorStatus{
        RESET = -1, ///< 电机复位
        OFFLINE,    ///< 电机离线
        STOP,       ///< 电机停止
        RUNNING,    ///< 电机运行
        STALL,      ///< 电机堵转
        ERROR,      ///< 电机错误
    } motorStatus = EMotorStatus::RESET;

    /**
     * @brief 电机物理数据的类型枚举变量
     * 
     */
    enum EMotorDataType {
        DATA_UNDEF = -1, ///< 未定义数据
        DATA_SPEED = 0, ///< 电机速度
        DATA_ANGLE,    ///< 电机角度
        DATA_POSIT,   ///< 电机位置
        DATA_TORQUE, ///< 电机转矩
        DATA_VOLTAGE, ///< 电机电压
        DATA_CURRENT, ///< 电机电流
        DATA_TEMP,   ///< 电机温度
        DATA_ERR, ///< 电机错误
        DATA_ID, ///< 电机ID
        DATA_COUNT_, ///< 数据类型数量
    };

    // 存放电机物理数据的数组
    std::array<int32_t, DATA_COUNT_> motorData = {0};

    /**
     * @brief 电机设备类的构造函数
     * 
     */
    CDevMtr() { deviceType = EDevType::DEV_MTR; }

    /**
     * @brief 电机设备类的析构函数
     * 
     */
    ~CDevMtr() override { UnregisterMotor_(); }

    virtual EAppStatus SetMotorOutput(float_t output) { return APP_ERROR; }

};

extern std::map<EDeviceID, CDevMtr *> MotorIDMap; ///< 电机实例映射表

} // namespace my_engineer

#endif // MTR_COMMON_HPP
