/**
 * @file mems_common.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 基于Device基类的MEMS基类的定义
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MEMS_COMMON_HPP
#define MEMS_COMMON_HPP

#include "dev_common.hpp"

namespace my_engineer {

/**
 * @brief MEMS基类
 * 
 */
class CMemsBase : public CDevBase{
protected:
    /**
     * @brief 定义一个派生结构体，用于包含MEMS的初始化参数
     * 
     */
    struct SMemsInitParam_Base : public SDevInitParam_Base{
        EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 通信接口ID
    };

    /**
     * @brief 注册MEMS设备
     * 
     */
    EAppStatus RegisterMems_();

    /**
     * @brief 注销MEMS设备
     * 
     */
    EAppStatus UnregisterMems_();

    // 延时
    static void DelayMs_(uint32_t ms);
    static void DelayUs_(uint32_t us);

public:
    
    // MEMS类型
    enum class EMemsType {
        MEMS_UNDEF = -1,
        MEMS_BMI088,
        MEMS_BMI08x,
        MEMS_BMI270,
        MEMS_MPU6050,
        MEMS_MPU6500,
    } memsType = EMemsType::MEMS_UNDEF;

    // MEMS状态
    enum class EMemsStatus {
        RESET = -1,
        INIT,
        NORMAL,
        ERROR,
    } memsStatus = EMemsStatus::RESET;

    // MEMS数据类型
    enum EMemsDataType {
        DATA_UNDEF = -1,
        DATA_ACC_X, ///< 加速度X轴(单位：m/s^2)
        DATA_ACC_Y, ///< 加速度Y轴(单位：m/s^2)
        DATA_ACC_Z, ///< 加速度Z轴(单位：m/s^2)
        DATA_GYRO_X, ///< 陀螺仪X轴(单位：rad/s)
        DATA_GYRO_Y, ///< 陀螺仪Y轴(单位：rad/s)
        DATA_GYRO_Z, ///< 陀螺仪Z轴(单位：rad/s)
        DATA_TEMP, ///< 温度(单位：摄氏度)
        DATA_COUNT_, ///< 数据类型数量
    };

    std::array<float, DATA_COUNT_> memsData = {0.0f}; ///< MEMS数据

    CMemsBase() {deviceType = EDevType::DEV_MEMS;} ///< 构造函数
    ~CMemsBase() override { UnregisterDevice_(); } ///< 析构函数

};

extern std::map<EDeviceID, CMemsBase *> MemsIDMap; ///< MEMS实例映射表

} // namespace my_engineer

#endif // MEMS_COMMON_HPP
