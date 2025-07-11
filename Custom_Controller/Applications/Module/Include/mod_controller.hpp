/******************************************************************************
 * @brief        
 * 
 * @file         mod_controller.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-30
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef MOD_CONTROLLER_HPP
#define MOD_CONTROLLER_HPP

#include "mod_common.hpp"

// 直接把机器人的物理位置给自定义控制器，懒得转换了
#define CONTROLLER_TRAVERSE_PHYSICAL_RANGE 140.1f
#define CONTROLLER_STRETCH_PHYSICAL_RANGE 328.6f
#define CONTROLLER_YAW_PHYSICAL_RANGE_MIN -90.0f
#define CONTROLLER_YAW_PHYSICAL_RANGE_MAX 90.0f
#define CONTROLLER_TRAVERSE_MOTOR_RANGE 406000
#define CONTROLLER_STRETCH_MOTOR_RANGE 392000
#define CONTROLLER_YAW_MOTOR_RANGE 1959
#define CONTROLLER_TRAVERSE_MOTOR_RATIO (CONTROLLER_TRAVERSE_MOTOR_RANGE / CONTROLLER_TRAVERSE_PHYSICAL_RANGE)
#define CONTROLLER_STRETCH_MOTOR_RATIO (CONTROLLER_STRETCH_MOTOR_RANGE / CONTROLLER_STRETCH_PHYSICAL_RANGE)
#define CONTROLLER_YAW_MOTOR_RATIO (CONTROLLER_YAW_MOTOR_RANGE / (CONTROLLER_YAW_PHYSICAL_RANGE_MAX - CONTROLLER_YAW_PHYSICAL_RANGE_MIN))
#define CONTROLLER_YAW_MOTOR_OFFSET -CONTROLLER_YAW_MOTOR_RATIO * CONTROLLER_YAW_PHYSICAL_RANGE_MIN
// 当物理位置从0增大时，电机位置的变化方向
#define CONTROLLER_TRAVERSE_MOTOR_DIR 1
#define CONTROLLER_STRETCH_MOTOR_DIR -1
#define CONTROLLER_ROLL_MOTOR_DIR 1
#define CONTROLLER_YAW_MOTOR_DIR -1

// 与自定义控制器模块相关的宏定义
#define CONTROLLER_ROCKER_DEAD_ZONE 10000 // 摇杆死区
#define CONTROLLER_ROCKER_RANGE 65500 // 摇杆范围
#define CONTROLLER_ROCKER_LOW 5000
#define CONTROLLER_ROCKER_HIGH 65530
#define CONTROLLER_ROCKER_KEY_LONG_PRESS_DURATION 2000
#define CONTROLLER_ROLL_SPEED_MAX 200.0f // 大Roll轴电机最大有效速度


// 前伸横移辅助移动
#define CONTROLLER_ASSIST_ENABLE 1 // 是否启用辅助移动

namespace my_engineer {

/**
 * @brief 控制器模块类
 * 
 */
class CModController final: public CModBase{
public:

	// 定义控制器模块初始化参数结构体
	struct SModInitParam_Controller: public SModInitParam_Base{
		EDeviceID rocker_id = EDeviceID::DEV_NULL; ///< 摇杆设备ID
		EDeviceID buzzer_id = EDeviceID::DEV_NULL; ///< 蜂鸣器设备ID
		EDeviceID traverse_id = EDeviceID::DEV_NULL; ///< 横移电机设备ID
		EDeviceID stretch_id = EDeviceID::DEV_NULL; ///< 前伸电机设备ID
		EDeviceID yaw_id = EDeviceID::DEV_NULL; ///< Yaw电机设备ID
		EDeviceID roll_id = EDeviceID::DEV_NULL; ///< 大Roll轴电机设备ID
		CInfCAN::CCanTxNode *traverseTxNode;
		CInfCAN::CCanTxNode *stretchTxNode;
		CInfCAN::CCanTxNode *rollTxNode;
		CInfCAN::CCanTxNode *yawTxNode;
		CAlgoPid::SAlgoInitParam_Pid traversePosPidParam;
		CAlgoPid::SAlgoInitParam_Pid traverseSpdPidParam;
		CAlgoPid::SAlgoInitParam_Pid stretchPosPidParam;
		CAlgoPid::SAlgoInitParam_Pid stretchSpdPidParam;
		CAlgoPid::SAlgoInitParam_Pid rollPosPidParam;
		CAlgoPid::SAlgoInitParam_Pid rollSpdPidParam;
		CAlgoPid::SAlgoInitParam_Pid yawPosPidParam;
		CAlgoPid::SAlgoInitParam_Pid yawSpdPidParam;
	};

	enum KEY_STATUS  {RELEASE = 0, PRESS = 1, LONG_PRESS = 2,};

	// 定义控制器信息结构体并实例化
	struct SControllerInfo{
		EVarStatus isModuleAvailable = false; ///< 模块是否可用
		EVarStatus isReturnSuccess = false; ///< 归位是否成功
		int8_t rocker_X = 0; ///< 摇杆X轴值 -100 - 100
		int8_t rocker_Y = 0; ///< 摇杆Y轴值 -100 - 100
		KEY_STATUS rocker_Key = KEY_STATUS::RELEASE; ///< 摇杆按键状态
		float_t posit_traverse = 0; ///< 横移电机位置
		float_t posit_stretch = 0; ///< 前伸电机位置
		int8_t speed_roll = 0; ///< 大Roll轴电机速度 -100 - 100
		float_t angle_yaw = 0; ///< Yaw轴电机位置
	} ControllerInfo = { };

	// 定义控制器命令结构体并实例化
	struct SControllerCmd{
		EVarStatus StartControl = false; ///< 控制器开始控制信号
		EVarStatus isFree = false; ///< 控制器是否可自由控制
		float_t cmd_traverse = 0; ///< 横移电机命令
		float_t cmd_stretch = 0; ///< 前伸电机命令
		float_t cmd_yaw = 0; ///< Yaw轴角度命令
	} ControllerCmd = { };

	CModController() = default;

	// 定义带参数的模块构造函数，创建模块时自动调用初始化函数
	explicit CModController(SModInitParam_Controller &param) { InitModule(param); }

	// 模块析构函数
	~CModController() final { UnregisterModule_(); }

	// 模块初始化
	EAppStatus InitModule(SModInitParam_Base &param) final;

	int16_t get_rocker_x() {return comRocker_.rockerInfo.X;};
	int16_t get_rocker_y() {return comRocker_.rockerInfo.Y;};

private:

	// 定义横移组件类并实例化
	class CComTraverse: public CComponentBase{
	public:

		const int32_t rangeLimit = CONTROLLER_TRAVERSE_MOTOR_RANGE; ///< 电机位置范围限制

		// 定义横移信息结构体并实例化
		struct STraverseInfo {
			int32_t posit = 0;    ///< Traverse Position
			bool isPositArrived = false; ///< Traverse Position Arrived
		} traverseInfo;

		// 定义横移控制命令结构体并实例化
		struct STraverseCmd {
			bool isFree = false;	 ///< Traverse Free
			int32_t setPosit = 0;    ///< Traverse Position Set
		} traverseCmd;

		// 电机实例指针
		CDevMtr *motor[1] = {nullptr};

		// 定义横移PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 1> mtrOutputBuffer = {0};

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 物理位置转换为电机位置
		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		// 电机位置转换为物理位置
		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		// 输出更新函数
		EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		std::array<CInfCAN::CCanTxNode*, 1> mtrCanTxNode_;

	} comTraverse_;

	// 定义前伸组件类并实例化
	class CComStretch: public CComponentBase{
	public:

		const int32_t rangeLimit = CONTROLLER_STRETCH_MOTOR_RANGE; ///< 电机位置范围限制

		// 定义前伸信息结构体并实例化
		struct SStretchInfo {
			int32_t posit = 0;    ///< Stretch Position
			bool isPositArrived = false; ///< Stretch Position Arrived
		} stretchInfo;

		// 定义前伸控制命令结构体并实例化
		struct SStretchCmd {
			bool isFree = false;	 ///< Stretch Free
			int32_t setPosit = 0;    ///< Stretch Position Set
		} stretchCmd;

		// 电机实例指针
		CDevMtr *motor[1] = {nullptr};

		// 定义前伸PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 1> mtrOutputBuffer = {0};

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 物理位置转换为电机位置
		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		// 电机位置转换为物理位置
		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		// 输出更新函数
		EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		std::array<CInfCAN::CCanTxNode*, 1> mtrCanTxNode_;

	} comStretch_;

	// 定义大Roll轴组件类并实例化
	class CComRoll: public CComponentBase{
	public:

		// 定义大Roll轴信息结构体并实例化
		struct SRollInfo {
			int16_t speed = 0;    ///< Roll Speed
		} rollInfo;

		// // 定义大Roll轴控制命令结构体并实例化
		// struct SRollCmd {
		// 	bool isEnd = false;	 ///< Roll Arrived End
		// } rollCmd;

		// 电机实例指针
		CDevMtr *motor[1] = {nullptr};

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 输出更新函数
		// EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		// std::array<CInfCAN::CCanTxNode*, 1> mtrCanTxNode_;

	} comRoll_;

	// 定义Yaw轴组件类并实例化
	class CComYaw: public CComponentBase{
	public:

		const int32_t rangeLimit = CONTROLLER_YAW_MOTOR_RANGE; ///< 电机位置范围限制

		// 定义Yaw轴信息结构体并实例化
		struct SYawInfo {
			int32_t posit = 0;    ///< Yaw Position
			bool isPositArrived = false; ///< Yaw Position Arrived
		} yawInfo;

		// 定义Yaw轴控制命令结构体并实例化
		struct SYawCmd {
			bool isFree = false;	 ///< Yaw Free
			int32_t setPosit = 0;    ///< Yaw Position Set
		} yawCmd;

		// 电机实例指针
		CDevMtr *motor[1] = {nullptr};

		// 定义Yaw轴PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 1> mtrOutputBuffer = {0};

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		std::array<CInfCAN::CCanTxNode*, 1> mtrCanTxNode_;

	} comYaw_;

	// 定义摇杆组件类并实例化
	class CComRocker: public CComponentBase{
	public:

		CDevRocker *rocker = nullptr; ///< 摇杆设备指针

		using KEY_STATUS = CModController::KEY_STATUS;

		// 定义摇杆信息结构体并实例化
		struct SRockerInfo {
			int32_t X = 0; ///< X轴值
			int32_t Y = 0; ///< Y轴值
			KEY_STATUS Key_status = KEY_STATUS::RELEASE; ///< 按键状态
		} rockerInfo;

		uint32_t last_time_stamp = 0; ///< 上次时间戳
		uint32_t key_duration = 0; ///< 按键持续时间

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

	} comRocker_;

	// 定义蜂鸣器组件类并实例化
	class CComBuzzer: public CComponentBase{
	public:

		using MusicType = CDevBuzzer::MusicType; ///< 音乐类型

		CDevBuzzer *buzzer = nullptr; ///< 蜂鸣器设备指针

		MusicType current_music = MusicType::NONE; ///< 当前音乐类型

		// 定义蜂鸣器信息结构体并实例化
		struct SBuzzerInfo {
			bool play_ready = false; ///< 可播放
		} buzzerInfo;

		// 定义蜂鸣器控制命令结构体并实例化
		struct SBuzzerCmd {
			MusicType musicType = MusicType::NONE; ///< 音乐类型
		} buzzerCmd;

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

	} comBuzzer_;


	// 重写基类的方法
	void UpdateHandler_() final;
	void HeartbeatHandler_() final;
	EAppStatus CreateModuleTask_() final;

	// 声明模块任务函数
	static void StartControllerModuleTask(void *argument);

	// 控制量限制函数
	EAppStatus RestrictControllerCommand_();

};

} // namespace my_engineer

#endif // MOD_CONTROLLER_HPP
		