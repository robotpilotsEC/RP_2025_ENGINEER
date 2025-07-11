/******************************************************************************
 * @brief        
 * 
 * @file         mod_arm.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-05-04
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/
#ifndef MOD_ARM_HPP
#define MOD_ARM_HPP

#define ARM_YAW_PHYSICAL_RANGE_MIN -90.0f
#define ARM_YAW_PHYSICAL_RANGE_MAX 90.0f
#define ARM_PITCH1_PHYSICAL_RANGE_MIN -90.0f
#define ARM_PITCH1_PHYSICAL_RANGE_MAX 90.0f
#define ARM_PITCH2_PHYSICAL_RANGE_MIN -90.0f
#define ARM_PITCH2_PHYSICAL_RANGE_MAX 90.0f
#define ARM_ROLL_PHYSICAL_RANGE_MIN -180.0f
#define ARM_ROLL_PHYSICAL_RANGE_MAX 180.0f
#define ARM_END_PITCH_PHYSICAL_RANGE_MIN -90.0f
#define ARM_END_PITCH_PHYSICAL_RANGE_MAX 90.0f
#define ARM_PITCH1_MOTOR_RANGE 65535
#define ARM_PITCH2_MOTOR_RANGE 65535
#define ARM_ROLL_MOTOR_RANGE 65535
#define ARM_END_PITCH_MOTOR_RANGE 100000
#define ARM_PITCH1_MOTOR_RATIO (ARM_PITCH1_MOTOR_RANGE / (ARM_PITCH1_PHYSICAL_RANGE_MAX - ARM_PITCH1_PHYSICAL_RANGE_MIN))
#define ARM_PITCH2_MOTOR_RATIO (ARM_PITCH2_MOTOR_RANGE / (ARM_PITCH2_PHYSICAL_RANGE_MAX - ARM_PITCH2_PHYSICAL_RANGE_MIN))
#define ARM_ROLL_MOTOR_RATIO (ARM_ROLL_MOTOR_RANGE / (ARM_ROLL_PHYSICAL_RANGE_MAX - ARM_ROLL_PHYSICAL_RANGE_MIN))
#define ARM_END_PITCH_MOTOR_RATIO (ARM_END_PITCH_MOTOR_RANGE / (ARM_END_PITCH_PHYSICAL_RANGE_MAX - ARM_END_PITCH_PHYSICAL_RANGE_MIN))
#define ARM_END_ROLL_MOTOR_RATIO 2469.4f
// 这里的offset都是物理的零点相对电机的零点的偏移值，电机的零点在物理的最小值
#define ARM_YAW_MOTOR_OFFSET 30.0f // 但这个比较特殊，测量这个就是从电机0位置到物理0位置总共的角度
#define ARM_PITCH1_MOTOR_OFFSET -ARM_PITCH1_PHYSICAL_RANGE_MIN * ARM_PITCH1_MOTOR_RATIO
#define ARM_PITCH2_MOTOR_OFFSET -ARM_PITCH2_PHYSICAL_RANGE_MIN * ARM_PITCH2_MOTOR_RATIO
#define ARM_ROLL_MOTOR_OFFSET -ARM_ROLL_PHYSICAL_RANGE_MIN * ARM_ROLL_MOTOR_RATIO
#define ARM_END_PITCH_MOTOR_OFFSET -ARM_END_PITCH_PHYSICAL_RANGE_MIN * ARM_END_PITCH_MOTOR_RATIO

#define ARM_YAW_MOTOR_DIR 1
#define ARM_PITCH1_MOTOR_DIR 1
#define ARM_PITCH2_MOTOR_DIR 1
#define ARM_ROLL_MOTOR_DIR 1
#define ARM_END_PITCH_MOTOR_L_DIR -1
#define ARM_END_PITCH_MOTOR_R_DIR 1
#define ARM_END_ROLL_MOTOR_L_DIR -1
#define ARM_END_ROLL_MOTOR_R_DIR -1

#define ARM_YAW_INIT_ANGLE 0.0f
#define ARM_PITCH1_INIT_ANGLE 0.0f
#define ARM_PITCH2_INIT_ANGLE 0.0f
#define ARM_ROLL_INIT_ANGLE 0.0f
#define ARM_END_PITCH_INIT_ANGLE 0.0f
#define ARM_END_ROLL_INIT_ANGLE 0.0f

#define deg2rad(x) ((x) * 0.017453292519943295769236907684886)
#define rad2deg(x) ((x) * 57.295779513082320876798154814105)

#include "mod_common.hpp"

namespace my_engineer {

/**
 * @brief 机械臂模块类
 * 
 */
class CModArm final: public CModBase {
public:

	// 定义机械臂模块初始化参数结构体
	struct SModInitParam_Arm: public SModInitParam_Base {
		EDeviceID MotorID_Yaw = EDeviceID::DEV_NULL;
		EDeviceID MotorID_Pitch1 = EDeviceID::DEV_NULL;
		EDeviceID MotorID_Pitch2 = EDeviceID::DEV_NULL;
		EDeviceID MotorID_Roll = EDeviceID::DEV_NULL;
		EDeviceID MotorID_End_L = EDeviceID::DEV_NULL;
		EDeviceID MotorID_End_R = EDeviceID::DEV_NULL;
		CInfCAN::CCanTxNode *MotorTxNode_Pitch1; ///< 机械臂关节Pitch电机1发送节点
		CInfCAN::CCanTxNode *MotorTxNode_Pitch2; ///< 机械臂关节Pitch电机2发送节点
		CInfCAN::CCanTxNode *MotorTxNode_Roll; ///< 机械臂关节Roll电机发送节点
		CInfCAN::CCanTxNode *MotorTxNode_End_L;
		CInfCAN::CCanTxNode *MotorTxNode_End_R;
		float_t MIT_Yaw_kp = 0.0f; ///< MIT控制器比例系数
		float_t MIT_Yaw_kd = 0.0f; ///< MIT控制器微分系数
		CAlgoPid::SAlgoInitParam_Pid Pitch1PosPidParam;
		CAlgoPid::SAlgoInitParam_Pid Pitch1SpdPidParam;
		CAlgoPid::SAlgoInitParam_Pid Pitch2PosPidParam;
		CAlgoPid::SAlgoInitParam_Pid Pitch2SpdPidParam;
		CAlgoPid::SAlgoInitParam_Pid RollPosPidParam;
		CAlgoPid::SAlgoInitParam_Pid RollSpdPidParam;
		CAlgoPid::SAlgoInitParam_Pid endPosPidParam;
		CAlgoPid::SAlgoInitParam_Pid endSpdPidParam;
	};

	// 定义机械臂信息结构体并实例化
	struct SArmInfo {
		EVarStatus isModuleAvailable = false; ///< 模块是否可用
		float_t angle_Yaw = 0.0f; ///< 机械臂关节Yaw角度
		float_t angle_Pitch1 = 0.0f; ///< 机械臂关节Pitch1角度
		float_t angle_Pitch2 = 0.0f; ///< 机械臂关节Pitch2角度
		float_t angle_Roll = 0.0f; ///< 机械臂关节Roll角度
		float_t angle_end_pitch = 0.0f; ///< 机械臂末端Pitch角度
		float_t angle_end_roll = 0.0f; ///< 机械臂末端Roll角度
		bool isAngleArrived_Yaw = false; ///< 机械臂关节Yaw角度是否到达
		bool isAngleArrived_Pitch1 = false; ///< 机械臂关节Pitch1角度是否到达
		bool isAngleArrived_Pitch2 = false; ///< 机械臂关节Pitch2角度是否到达
		bool isAngleArrived_Roll = false; ///< 机械臂关节Roll角度是否到达
		bool isAngleArrived_End_Pitch = false; ///< 机械臂末端Pitch角度是否到达
		bool isAngleArrived_End_Roll = false; ///< 机械臂末端Roll角度是否到达
	} armInfo;

	// 定义机械臂控制命令结构体并实例化
	struct SArmCmd {
		bool isAutoCtrl = false; ///< 是否自动控制
		float_t set_angle_Yaw = 0.0f; ///< 机械臂关节Yaw角度设定
		float_t set_angle_Pitch1 = 0.0f; ///< 机械臂关节Pitch1角度设定
		float_t set_angle_Pitch2 = 0.0f; ///< 机械臂关节Pitch2角度设定
		float_t set_angle_Roll = 0.0f; ///< 机械臂关节Roll角度设定
		float_t set_angle_end_pitch = 0.0f; ///< 机械臂末端Pitch角度设定
		float_t set_angle_end_roll = 0.0f; ///< 机械臂末端Roll角度设定
	} armCmd;

	CModArm() = default;

	// 定义带参数的模块构造函数，创建模块时自动调用初始化函数
	explicit CModArm(SModInitParam_Arm &param) { InitModule(param); }

	// 模块析构函数
	~CModArm() final { UnregisterModule_(); }

	// 初始化模块
	EAppStatus InitModule(SModInitParam_Base &param) final;

private:

	// 定义机械臂Yaw关节组件类并实例化
	class CComYaw: public CComponentBase {
	public:
		// 定义Yaw关节信息结构体
		struct SYawInfo {
			float_t angle = 0.0f;           ///< Yaw关节当前角度
			bool isAngleArrived = false;    ///< Yaw角度是否到达目标
		} yawInfo;

		// 定义Yaw关节控制命令结构体
		struct SYawCmd {
			float_t setAngle = 0.0f;        ///< Yaw关节目标角度
		} yawCmd;

		// MIT控制结构体
		struct SMitCtrl {
			float_t kp = 0.0f;
			float_t kd = 0.0f;
			float_t q = 0.0f;
			float_t dq = 0.0f;
			float_t tau = 0.0f;
		} mitCtrl;

		// 电机实例指针
		CDevMtr* motor = nullptr;

		static float_t MtrAngleToPhyAngle(float_t angle) {
			// 将电机角度转换为物理角度
			return angle + ARM_YAW_MOTOR_OFFSET;
		}

		static float_t PhyAngleToMtrAngle(float_t angle) {
			// 将物理角度转换为电机角度
			return angle - ARM_YAW_MOTOR_OFFSET;
		}

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;
	} comYaw_;

	// 定义机械臂Pitch1关节组件类并实例化
	class CComPitch1: public CComponentBase {
	public:
		const int32_t rangeLimit = ARM_PITCH1_MOTOR_RANGE; ///< Pitch1关节电机范围限制
		
		// 定义Pitch1关节信息结构体
		struct SPitch1Info {
			int32_t posit = 0.0f;           ///< Pitch1关节当前位置
			bool isPositArrived = false;    ///< Pitch1位置是否到达目标
		} pitch1Info;

		// 定义Pitch1关节控制命令结构体
		struct SPitch1Cmd {
			int32_t setPosit = 0.0f;        ///< Pitch1关节目标位置
		} pitch1Cmd;

		// PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 1> mtrOutputBuffer = {0};

		// 电机实例指针
		CDevMtr* motor = nullptr;

		// 物理位置转换为电机位置
		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		// 电机位置转换为物理位置
		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 输出更新函数
		EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		CInfCAN::CCanTxNode* mtrCanTxNode = nullptr;

	} comPitch1_;

	// 定义机械臂Pitch2关节组件类并实例化
	class CComPitch2: public CComponentBase {
	public:
		const int32_t rangeLimit = ARM_PITCH2_MOTOR_RANGE; ///< Pitch2关节电机范围限制

		// 定义Pitch2关节信息结构体
		struct SPitch1Info {
			int32_t posit = 0.0f;           ///< Pitch2关节当前位置
			bool isPositArrived = false;    ///< Pitch2位置是否到达目标
		} pitch2Info;

		// 定义Pitch2关节控制命令结构体
		struct SPitch2Cmd {
			int32_t setPosit = 0.0f;        ///< Pitch2关节目标位置
		} pitch2Cmd;

		// PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 1> mtrOutputBuffer = {0};

		// 电机实例指针
		CDevMtr* motor = nullptr;


		// 物理位置转换为电机位置
		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		// 电机位置转换为物理位置
		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 输出更新函数
		EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		CInfCAN::CCanTxNode* mtrCanTxNode = nullptr;

	} comPitch2_;


	// 定义机械臂Roll关节组件类并实例化
	class CComRoll: public CComponentBase {
	public:
		const int32_t rangeLimit = ARM_ROLL_MOTOR_RANGE; ///< Roll关节电机范围限制
		
		// 定义Roll关节信息结构体
		struct SRollInfo {
			int32_t posit = 0.0f;           ///< Roll关节当前位置
			bool isPositArrived = false;    ///< Roll位置是否到达目标
		} rollInfo;

		// 定义Roll关节控制命令结构体
		struct SRollCmd {
			int32_t setPosit = 0.0f;        ///< Roll关节目标位置
		} rollCmd;

		// PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 1> mtrOutputBuffer = {0};

		// 电机实例指针
		CDevMtr* motor = nullptr;

		// 物理位置转换为电机位置
		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		// 电机位置转换为物理位置
		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 输出更新函数
		EAppStatus _UpdateOutput(float_t posit);

		// 电机can发送节点
		CInfCAN::CCanTxNode* mtrCanTxNode = nullptr;

	} comRoll_;


	// 定义机械臂末端组件类并实例化
	class CComEnd: public CComponentBase {
	public:

		enum { L = 0, R = 1 };

		const int32_t rangeLimit_Pitch = ARM_END_PITCH_MOTOR_RANGE; ///< 电机位置范围限制

		// 定义机械臂末端信息结构体并实例化
		struct SEndInfo {
			int32_t posit_Pitch = 0;    ///< End Posit Pitch
			int32_t posit_Roll = 0;    ///< End Posit Roll
			bool isPositArrived_Pitch = false; ///< End Posit Arrived Pitch
			bool isPositArrived_Roll = false; ///< End Posit Arrived Roll
		} endInfo;

		// 定义机械臂末端控制命令结构体并实例化
		struct SEndCmd {
			int32_t setPosit_Pitch = 0;    ///< End Posit Set Pitch
			int32_t setPosit_Roll = 0;    ///< End Posit Set Roll
		} endCmd;

		// 电机实例指针数组
		std::array<CDevMtr*, 2> motor = {nullptr};

		// 定义机械臂末端PID控制器
		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		// 电机数据输出缓冲区
		std::array<int16_t, 2> mtrOutputBuffer = {0};

		// 初始化组件
		EAppStatus InitComponent(SModInitParam_Base &param) final;

		// 更新组件
		EAppStatus UpdateComponent() final;

		// 物理位置转换为电机位置: Pitch
		static int32_t PhyPositToMtrPosit_Pitch(float_t phyPosit);

		// 电机位置转换为物理位置: Pitch
		static float_t MtrPositToPhyPosit_Pitch(int32_t mtrPosit);

		// 物理位置转换为电机位置: Roll
		static int32_t PhyPositToMtrPosit_Roll(float_t phyPosit);

		// 电机位置转换为物理位置: Roll
		static float_t MtrPositToPhyPosit_Roll(int32_t mtrPosit);

		// 输出更新函数
		EAppStatus _UpdateOutput(float_t posit_Pitch, float_t posit_Roll);

		// 电机can发送节点
		std::array<CInfCAN::CCanTxNode*, 2> mtrCanTxNode;

	} comEnd_;

	// 重写基类函数
	void UpdateHandler_() final;
	void HeartbeatHandler_() final;
	EAppStatus CreateModuleTask_() final;

	// 声明机械臂模块任务函数
	static void StartArmModuleTask(void *argument);

	// 控制量限制函数
	EAppStatus RestrictArmCommand_();

};

} // namespace my_engineer

#endif // MOD_ARM_HPP

