/**
 * @file mod_subgantry.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义子龙门模块
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MOD_SUBGANTRY_HPP
#define MOD_SUBGANTRY_HPP

#include "mod_common.hpp"

#define SUB_GANTRY_LIFT_PHYSICAL_RANGE_L 77.5f
#define SUB_GANTRY_LIFT_PHYSICAL_RANGE_R 77.5f
#define SUB_GANTRY_STRETCH_PHYSICAL_RANGE_L 261.1f
#define SUB_GANTRY_STRETCH_PHYSICAL_RANGE_R 371.1f
#define SUB_GANTRY_LIFT_MOTOR_RANGE_L 221500
#define SUB_GANTRY_LIFT_MOTOR_RANGE_R 221500
#define SUB_GANTRY_STRETCH_MOTOR_RANGE_L 745000
#define SUB_GANTRY_STRETCH_MOTOR_RANGE_R 1112000
#define SUB_GANTRY_LIFT_MOTOR_RATIO_L (SUB_GANTRY_LIFT_MOTOR_RANGE_L / SUB_GANTRY_LIFT_PHYSICAL_RANGE_L)
#define SUB_GANTRY_LIFT_MOTOR_RATIO_R (SUB_GANTRY_LIFT_MOTOR_RANGE_R / SUB_GANTRY_LIFT_PHYSICAL_RANGE_R)
#define SUB_GANTRY_STRETCH_MOTOR_RATIO_L (SUB_GANTRY_STRETCH_MOTOR_RANGE_L / SUB_GANTRY_STRETCH_PHYSICAL_RANGE_L)
#define SUB_GANTRY_STRETCH_MOTOR_RATIO_R (SUB_GANTRY_STRETCH_MOTOR_RANGE_R / SUB_GANTRY_STRETCH_PHYSICAL_RANGE_R)
// 当物理位置从0增大时，电机位置的变化方向
#define SUB_GANTRY_LIFT_MOTOR_DIR_L -1
#define SUB_GANTRY_LIFT_MOTOR_DIR_R -1
#define SUB_GANTRY_STRETCH_MOTOR_DIR_L -1
#define SUB_GANTRY_STRETCH_MOTOR_DIR_R 1

#define MOS_ON_GPIO_STATE GPIO_PIN_SET ///< MOS管开关状态
#define MOS_OFF_GPIO_STATE GPIO_PIN_RESET ///< MOS管开关状态

namespace my_engineer {

/**
 * @brief 子龙门模块类
 * 
 */
class CModSubGantry final: public CModBase{
public:

    // 定义子龙门模块初始化参数结构体
    struct SModInitParam_SubGantry: public SModInitParam_Base{
        GPIO_TypeDef *LeftPumpPort = nullptr; ///< 左气泵GPIO端口
        uint16_t LeftPumpPin = 0; ///< 左气泵GPIO引脚
        GPIO_TypeDef *RightPumpPort = nullptr; ///< 右气泵GPIO端
        uint16_t RightPumpPin = 0; ///< 右气泵GPIO引脚
        GPIO_TypeDef *ArmPumpPort = nullptr; ///< 机械臂气泵GPIO端口
        uint16_t ArmPumpPin = 0; ///< 机械臂气泵GPIO引脚
        EDeviceID liftMotorID_L = EDeviceID::DEV_NULL; ///< 左升降电机ID
		EDeviceID stretchMotorID_L = EDeviceID::DEV_NULL; ///< 左伸缩电机ID
		EDeviceID liftMotorID_R = EDeviceID::DEV_NULL; ///< 右升降电机ID
		EDeviceID stretchMotorID_R = EDeviceID::DEV_NULL; ///< 右伸缩电机ID
		CInfCAN::CCanTxNode *liftMotorTxNode_L = nullptr; ///< 左升降电机CAN发送节点
		CInfCAN::CCanTxNode *stretchMotorTxNode_L = nullptr; ///< 左伸缩电机CAN发送节点
		CInfCAN::CCanTxNode *liftMotorTxNode_R = nullptr; ///< 右升降电机CAN发送节点
		CInfCAN::CCanTxNode *stretchMotorTxNode_R = nullptr; ///< 右伸缩电机CAN发送节点
		CAlgoPid::SAlgoInitParam_Pid liftPosPidParam; ///< 升降电机PID参数
		CAlgoPid::SAlgoInitParam_Pid liftSpdPidParam; ///< 升降电机速度PID参数
		CAlgoPid::SAlgoInitParam_Pid stretchPosPidParam; ///< 伸缩电机PID参数
		CAlgoPid::SAlgoInitParam_Pid stretchSpdPidParam; ///< 伸缩电机速度PID参数
    };

    // 定义子龙门信息结构体并实例化
    struct SSubGantryInfo{
        EVarStatus isModuleAvailable  = false; ///< 模块是否可用
        bool isPumpOn_Left = false; ///< 左气泵开关状态
        bool isPumpOn_Right = false; ///< 右气泵开关状态
        bool isPumpOn_Arm = false; ///< 机械臂气泵开关状态
		float_t liftPosit_L = 0.0f; ///< 左升降电机位置
		float_t liftPosit_R = 0.0f; ///< 右升降电机位置
		float_t stretchPosit_L = 0.0f; ///< 左伸缩电机位置
		float_t stretchPosit_R = 0.0f; ///< 右伸缩电机位置
        bool isPositArrived_Lift_L = false; ///< 左升降电机位置到达标定位置
		bool isPositArrived_Lift_R = false; ///< 右升降电机位置到达标定位置
		bool isPositArrived_Stretch_L = false; ///< 左伸缩电机位置到达标定位置
		bool isPositArrived_Stretch_R = false; ///< 右伸缩电机位置到达标定位置
    } subGantryInfo;

    // 定义子龙门控制命令结构体并实例化
    struct SSubGantryCmd {
        bool isAutoCtrl = false; ///< 是否自动控制
        bool setPumpOn_Left = false; ///< 设置左气泵开关状态
        bool setPumpOn_Right = false; ///< 设置右气泵开关状态
        bool setPumpOn_Arm = false; ///< 设置机械臂气泵开关状态
		float_t setLiftPosit_L = 0.0f; ///< 左升降电机目标位置
		float_t setLiftPosit_R = 0.0f; ///< 右升降电机目标位置
		float_t setStretchPosit_L = 0.0f; ///< 左伸缩电机目标位置
		float_t setStretchPosit_R = 0.0f; ///< 右伸缩电机目标位置
    } subGantryCmd;

    CModSubGantry() = default;

    // 定义带参数的模块构造函数，创建模块时自动调用初始化函数
    explicit CModSubGantry(SModInitParam_SubGantry &param) { InitModule(param); }

    // 模块析构函数,好像重不重定义无所谓，和基类一样
    ~CModSubGantry() final { UnregisterModule_(); }

    // 初始化模块
    EAppStatus InitModule(SModInitParam_Base &param) final;

private:

    // 定义气泵组件类并实例化
    class CComPump: public CComponentBase{
    public:
        // 定义气泵信息结构体并实例化
        struct SPumpInfo {
            bool isPumpOn_Left = false; ///< 左气泵开关状态
            bool isPumpOn_Right = false; ///< 右气泵开关状态
            bool isPumpOn_Arm = false; ///< 机械臂气泵开关状态
        } pumpInfo;

        // 定义气泵控制命令结构体并实例化
        struct SPumpCmd {
            bool setPumpOn_Left = false; ///< 设置左气泵开关状态
			bool setPumpOn_Right = false; ///< 设置右气泵开关状态
			bool setPumpOn_Arm = false; ///< 设置机械臂气泵开关状态
        } pumpCmd;

        // 气泵GPIO端口
        GPIO_TypeDef *LeftPump_GpioPort = nullptr; ///< 左气泵GPIO端口
        GPIO_TypeDef *RightPump_GpioPort = nullptr; ///< 右气泵GPIO端口
        GPIO_TypeDef *ArmPump_GpioPort = nullptr; ///< 机械臂气泵GPIO端口

        // 气泵GPIO引脚
        uint16_t LeftPump_GpioPin = 0; ///< 左气泵GPIO引脚
		uint16_t RightPump_GpioPin = 0; ///< 右气泵GPIO引脚
		uint16_t ArmPump_GpioPin = 0; ///< 机械臂气泵GPIO引脚

        // 初始化组件
        EAppStatus InitComponent(SModInitParam_Base &param) final;

        // 更新组件
        EAppStatus UpdateComponent() final;

    } comPump_;
    
    // 定义子龙门升降组件类并实例化
    class CComLift_sub: public CComponentBase{
    public:

        const int32_t rangeLimit_L = SUB_GANTRY_LIFT_MOTOR_RANGE_L; ///< 电机位置范围限制
		const int32_t rangeLimit_R = SUB_GANTRY_LIFT_MOTOR_RANGE_R; ///< 电机位置范围限制

        // 定义子龙门升降信息结构体并实例化
        struct SLiftInfo {
            int32_t posit_L = 0;    ///< Lift Position Left
			int32_t posit_R = 0;    ///< Lift Position Right
            bool isPositArrived_L = false; ///< Lift Position Arrived Left
			bool isPositArrived_R = false; ///< Lift Position Arrived Right
        } liftInfo;

        // 定义子龙门升降控制命令结构体并实例化
        struct SLiftCmd {
            int32_t setPosit_L = 0;    ///< Lift Position Set Left
			int32_t setPosit_R = 0;    ///< Lift Position Set Right
        } liftCmd;

        // 电机实例指针数组
        std::array<CDevMtr*, 2> motor = {nullptr}; 

        // 定义子龙门升降PID控制器
        CAlgoPid pidPosCtrl;
        CAlgoPid pidSpdCtrl;

        // 电机数据输出缓冲区
        std::array<int16_t, 2> mtrOutputBuffer = {0};

        // 初始化组件
        EAppStatus InitComponent(SModInitParam_Base &param) final;

        // 更新组件
        EAppStatus UpdateComponent() final;

        // 物理位置转换为电机位置
        static int32_t PhyPositToMtrPosit_L(float_t phyPosit);
		static int32_t PhyPositToMtrPosit_R(float_t phyPosit);	

        // 电机位置转换为物理位置
        static float_t MtrPositToPhyPosit_L(int32_t mtrPosit);
		static float_t MtrPositToPhyPosit_R(int32_t mtrPosit);

        // 输出更新函数
        EAppStatus _UpdateOutput(float_t posit_L, float_t posit_R);
    
        // 电机can发送节点
        std::array<CInfCAN::CCanTxNode*, 2> mtrCanTxNode;
    } comLift_;

    // 定义子龙门伸缩组件类并实例化
    class CComStretch_sub: public CComponentBase{
    public:

        const int32_t rangeLimit_L = SUB_GANTRY_STRETCH_MOTOR_RANGE_L; ///< 电机位置范围限制
		const int32_t rangeLimit_R = SUB_GANTRY_STRETCH_MOTOR_RANGE_R; ///< 电机位置范围限制

        // 定义子龙门伸缩信息结构体并实例化
        struct SStretchInfo {
            int32_t posit_L = 0;    ///< Stretch Position Left
			int32_t posit_R = 0;    ///< Stretch Position Right
			bool isPositArrived_L = false; ///< Stretch Position Arrived Left
			bool isPositArrived_R = false; ///< Stretch Position Arrived Right
        } stretchInfo;

        // 定义子龙门伸缩控制命令结构体并实例化
        struct SStretchCmd {
            int32_t setPosit_L = 0;    ///< Stretch Position Set Left
			int32_t setPosit_R = 0;    ///< Stretch Position Set Right
        } stretchCmd;

        // 电机实例指针数组
        std::array<CDevMtr*, 2> motor = {nullptr}; 

        // 定义子龙门伸缩PID控制器
        CAlgoPid pidPosCtrl;
        CAlgoPid pidSpdCtrl;

        // 电机数据输出缓冲区
        std::array<int16_t, 2> mtrOutputBuffer = {0};

        // 初始化组件
        EAppStatus InitComponent(SModInitParam_Base &param) final;

        // 更新组件
        EAppStatus UpdateComponent() final;

        // 物理位置转换为电机位置
        static int32_t PhyPositToMtrPosit_L(float_t phyPosit);
		static int32_t PhyPositToMtrPosit_R(float_t phyPosit);

        // 电机位置转换为物理位置
        static float_t MtrPositToPhyPosit_L(int32_t mtrPosit);
		static float_t MtrPositToPhyPosit_R(int32_t mtrPosit);	

        // 输出更新函数
        EAppStatus _UpdateOutput(float_t posit_L, float_t posit_R);
    
        // 电机can发送节点
        std::array<CInfCAN::CCanTxNode*, 2> mtrCanTxNode;
    } comStretch_;

    // 重写基类函数
    void UpdateHandler_() final;
    void HeartbeatHandler_() final;
    EAppStatus CreateModuleTask_() final;

    // 声明子龙门模块任务函数
    static void StartSubGantryModuleTask(void *argument);

    // 控制量限制函数
    EAppStatus RestrictSubGantryCommand_();

};

} // namespace my_engineer

#endif // MOD_SUBGANTRY_HPP
