/**
 * @file mod_chassis.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义底盘模块
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MOD_CHASSIS_HPP
#define MOD_CHASSIS_HPP

#include "mod_common.hpp"

#define LASER_ZERO_OFFSET_L 0
#define LASER_ZERO_OFFSET_R 0

namespace my_engineer {

/**
 * @brief 底盘模块类
 * 
 */
class CModChassis final: public CModBase{
public:
    // 定义底盘模块初始化参数结构体
    struct SModInitParam_Chassis: public SModInitParam_Base{
        EDeviceID memsDevID = EDeviceID::DEV_NULL;
        EDeviceID wheelsetMotorID_LF = EDeviceID::DEV_NULL;
        EDeviceID wheelsetMotorID_RF = EDeviceID::DEV_NULL;
        EDeviceID wheelsetMotorID_LB = EDeviceID::DEV_NULL;
        EDeviceID wheelsetMotorID_RB = EDeviceID::DEV_NULL;
        CInfCAN::CCanTxNode *wheelsetMotorTxNode_LF;
        CInfCAN::CCanTxNode *wheelsetMotorTxNode_RF;
        CInfCAN::CCanTxNode *wheelsetMotorTxNode_LB;
        CInfCAN::CCanTxNode *wheelsetMotorTxNode_RB;
        CAlgoPid::SAlgoInitParam_Pid wheelsetSpdPidParam;
        CAlgoPid::SAlgoInitParam_Pid lineCorrectionPidParam;
        CAlgoPid::SAlgoInitParam_Pid yawCorrectionPidParam;
    };

    // 定义底盘信息结构体并实例化
    struct SChassisInfo{
        EVarStatus isModuleAvailable  = false; ///< 模块是否可用

        // 下面这三个变量由于没有传感器可以直接读取，所以并不会更新
        float_t speed_X = 0.0f; ///< 底盘X轴速度
        float_t speed_Y = 0.0f; ///< 底盘Y轴速度
        float_t speed_W = 0.0f; ///< 底盘角速度
        int16_t laser_distance_L = 0; ///< 激光传感器左距离
        int16_t laser_distance_R = 0; ///< 激光传感器右距离
    } chassisInfo;

    // 定义底盘控制命令结构体并实例化
    struct SChassisCmd {
        bool isAutoCtrl = false; ///< 是否自动控制
        float_t speed_X = 0;    ///< 底盘X轴速度(范围-100％~100％)
        float_t speed_Y = 0;    ///< 底盘Y轴速度(范围-100％~100％)
        float_t speed_W = 0;    ///< 底盘角速度(范围-100％~100％)
    } chassisCmd;

    CModChassis() = default;

    // 定义带参数的模块构造函数，创建模块时自动调用初始化函数
    explicit CModChassis(SModInitParam_Base &param) { InitModule(param); }

    // 模块析构函数,好像重不重定义无所谓，和基类一样
    ~CModChassis() final { UnregisterModule_();}

    // 初始化模块
    EAppStatus InitModule(SModInitParam_Base &param) final;

private:

    // 定义底盘轮组组件类并实例化
    class CComWheelset: public CComponentBase{
    public:
        enum {LF = 0, RF = 1, LB = 2, RB = 3};

        // 定义底盘轮组信息结构体并实例化
        struct SWheelsetInfo {
            float_t speed_LF = 0.0f;    ///< Chassis Speed LF (Unit: rpm)
            float_t speed_RF = 0.0f;    ///< Chassis Speed RF (Unit: rpm)
            float_t speed_LB = 0.0f;    ///< Chassis Speed LB (Unit: rpm)
            float_t speed_RB = 0.0f;    ///< Chassis Speed RB (Unit: rpm)
        } wheelsetInfo;

        // 定义底盘轮组控制命令结构体并实例化
        struct SWheelsetCommand {
            float_t speed_X = 0.0f;    ///< Chassis Speed X (Range: -100% ~ 100%)
            float_t speed_Y = 0.0f;    ///< Chassis Speed Y (Range: -100% ~ 100%)
            float_t speed_W = 0.0f;    ///< Chassis Speed W (Range: -100% ~ 100%)
        } wheelsetCmd;

        // 传感器实例指针
        CMemsBase *mems = nullptr;

        // 电机实例指针数组
        CDevMtr *motor[4] = {nullptr};

        // 定义底盘PID控制器
        CAlgoPid pidYawCtrl;
        CAlgoPid pidLineCorrectionCtrl;
        CAlgoPid pidSpdCtrl;

        // 电机数据输出缓冲区
        std::array<int16_t, 4> mtrOutputBuffer = {0};

        // 初始化组件
        EAppStatus InitComponent(SModInitParam_Base &param) final;

        // 重写组件更新函数
        EAppStatus UpdateComponent() final;

        // 声明组件输出更新函数(负责根据控制量进行解算，以及进行PID运算，最后得到输出值)
        EAppStatus _UpdateOutput(float speed_X, float speed_Y, float speed_W);
    
        // 电机can发送节点
        std::array<CInfCAN::CCanTxNode*, 4> mtrCanTxNode;
    } comWheelset_;

    // 重写基类函数
    void UpdateHandler_() final;
    void HeartbeatHandler_() final;
    EAppStatus CreateModuleTask_() final;

    // 声明底盘模块任务函数(在proc_chassis.cpp中定义)
    static void StartChassisModuleTask(void *argument);

    // 声明控制量限制函数(负责对控制量进行限幅，在上面那个任务中进行调用)
    EAppStatus RestrictChassisCommand_();


};

} // namespace my_engineer

#endif // MOD_CHASSIS_HPP   
