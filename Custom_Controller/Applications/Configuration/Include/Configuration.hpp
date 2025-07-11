/**
 * @file Configuration.hpp
 * @author Zoe
 * @brief My Engineer Robot 应用程序的总配置头文件。
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 在此文件中定义应用于各个配置的枚举类型。
 */


#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "conf_common.hpp"
#include "conf_process.hpp"
#include "conf_interface.hpp"
#include "conf_module.hpp"
#include "conf_device.hpp"

namespace my_engineer {

/**
 * @brief 接口ID枚举类型
 * 
 */
enum class EInterfaceID{
    INF_NULL = -1,  ///< 空接口
    INF_TEST = 0,   ///< 测试接口
    INF_DBUS,       ///< DBUS接口(遥控器，使用USART5)
    INF_UART1,      ///< 串口1(用于裁判系统)
    INF_UART7,      ///< 串口7(用于视觉)
    INF_UART10,     ///< 串口10(用于调试)
    INF_CAN1,       ///< CAN1(用于底盘电机)
    INF_CAN2,       ///< CAN2(用于龙门架电机)
    INF_CAN3,       ///< CAN3(用于机械臂电机)
    INF_SPI2,       ///< SPI2(用于BMI-088 MEMS传感器)
    INF_SPI6,       ///< SPI6(用于WS2312 LED)
    INF_ADC1,       ///< ADC1(用于摇杆)
    INF_USB_CDC, ///< USB CDC(用于调试)
};


/**
 * @brief 设备ID枚举类型
 * 
 */
enum class EDeviceID{
    DEV_NULL = -1,          ///< 空设备
    DEV_TEST = 0,           ///< 测试设备
    DEV_RC_DR16,            ///< 遥控接收器（DR16）
    DEV_RM_REFEREE,         ///< RoboMaster裁判系统设备
    DEV_RP_VISION,          ///< RoboPilots视觉系统设备
    DEV_BOARD_LINK,         ///< 板间通信设备
    DEV_CONTROLLER_LINK,   ///< 控制器通信设备
    DEV_ROCKER,             ///< 摇杆设备
    DEV_BUZZER,             ///< 蜂鸣器设备
    DEV_LASER_L,            ///< 激光雷达左
    DEV_LASER_R,             ///< 激光雷达右
    DEV_MEMS_BMI088,        ///< 6轴MEMS（BMI-088）
    DEV_CHAS_MTR_LF,        ///< 底盘电机前左（M3508）
    DEV_CHAS_MTR_RF,        ///< 底盘电机前右（M3508）
    DEV_CHAS_MTR_LB,        ///< 底盘电机后左（M3508）
    DEV_CHAS_MTR_RB,        ///< 底盘电机后右（M3508）
    DEV_GIMBAL_MTR_LIFT_L,  ///< 云台电机升降左（M2006）
    DEV_GIMBAL_MTR_LIFT_R,  ///< 云台电机升降右（M2006）
    DEV_MANTIS_MTR_FORELEG_L,   ///< 螳螂臂前肢电机左（KT-4010）
    DEV_MANTIS_MTR_FORELEG_R,   ///< 螳螂臂前肢电机右（KT-4010）
    DEV_MANTIS_MTR_MIDLEG,      ///< 螳螂臂中肢电机（M2006）
    DEV_MANTIS_MTR_HINDLEG_L,   ///< 螳螂臂后肢电机左（KT-4010）
    DEV_MANTIS_MTR_HINDLEG_R,   ///< 螳螂臂后肢电机右（KT-4010）
    DEV_GANTRY_MTR_LIFT_L,      ///< 龙门架升降电机左（M3508）
    DEV_GANTRY_MTR_LIFT_R,      ///< 龙门架升降电机右（M3508)
    DEV_GANTRY_MTR_STRETCH,     ///< 龙门架伸缩电机（M2006）
    DEV_GANTRY_MTR_TRAVERSE,    ///< 龙门架横移电机（M2006）
    DEV_GANTRY_MTR_JOINT_YAW,   ///< 龙门架关节yaw电机（DM4310）
    DEV_GANTRY_MTR_JOINT_ROLL,  ///< 龙门架关节roll电机（DM4310）
    DEV_GANTRY_MTR_END_L,   ///< 龙门架末端电机左（M2006）
    DEV_GANTRY_MTR_END_R,   ///< 龙门架末端电机右（M2006）
    DEV_CONTROLLER_MTR_TRAVERSE,    ///< 控制器横移电机（M2006）
    DEV_CONTROLLER_MTR_STRETCH,     ///< 控制器前伸电机（M2006）
    DEV_CONTROLLER_MTR_YAW,         ///< 控制器Yaw电机（M2006）
    DEV_CONTROLLER_MTR_ROLL,        ///< 控制器滚轴电机（M2006）


};

/**
 * @brief 模块ID枚举类型
 * 
 */
enum class EModuleID{
    MOD_NULL = -1,          ///< 空模块
    MOD_TEST = 0,           ///< 测试模块
    MOD_CHASSIS,            ///< 底盘模块
    MOD_MANTIS,             ///< 螳螂臂模块
    MOD_GIMBAL,             ///< 云台模块
    MOD_GANTRY,             ///< 龙门模块
    MOD_SUBGANTRY,          ///< 子龙门模块
    MOD_CONTROLLER,         ///< 自定义控制器模块
};

/**
 * @brief 系统ID枚举类型
 * 
 */
enum class ESystemID{
    SYS_NULL = -1,          ///< 空系统
    SYS_TEST = 0,           ///< 测试系统
    SYS_REMOTE,             ///< 遥控系统
    SYS_REFEREE,            ///< RoboMaster裁判系统
    SYS_VISION,             ///< RoboPilots视觉系统
    SYS_BOARDLINK,          ///< 板间通信系统
    SYS_CONTROLLERLINK,     ///< 控制器通信系统
};

/**
 * @brief Application的入口函数
 * @return None
 */
void ApplicationEntryPoint();

} // namespace my_engineer

#endif // CONFIGURATION_HPP
