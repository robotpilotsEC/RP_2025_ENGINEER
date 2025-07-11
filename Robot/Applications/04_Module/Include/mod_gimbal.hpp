/**
 * @file mod_gimbal.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-01-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef MOD_GIMBAL_HPP
#define MOD_GIMBAL_HPP

#include "mod_common.hpp"

#define GIMBAL_LIFT_PHYSICAL_RANGE 280.0f
#define GIMBAL_LIFT_MOTOR_RANGE 832500
#define GIMBAL_LIFT_MOTOR_RATIO (GIMBAL_LIFT_MOTOR_RANGE / GIMBAL_LIFT_PHYSICAL_RANGE)
#define GIMBAL_LIFT_MOTOR_DIR 1


namespace my_engineer {

class CModGimbal final: public CModBase{
public:

	// 定义云台模块初始化参数结构体
	struct SModInitParam_Gimbal: public SModInitParam_Base{
		EDeviceID liftMotorID = EDeviceID::DEV_NULL;
		CInfCAN::CCanTxNode *liftMotorTxNode;
		CAlgoPid::SAlgoInitParam_Pid liftPosPidParam;
		CAlgoPid::SAlgoInitParam_Pid liftSpdPidParam;
	};

	// 定义云台信息结构体并实例化
	struct SGimbalInfo{
		EVarStatus isModuleAvailable = false;
		float_t posit_lift = 0.0f;
		bool isPositArrived_Lift = false;
	} gimbalInfo;

	// 定义云台控制命令结构体并实例化
	struct SGimbalCmd{
		EVarStatus isAutoCtrl = false;
		float_t set_posit_lift = 0.0f;
	} gimbalCmd;

	CModGimbal() = default;

	explicit CModGimbal(SModInitParam_Gimbal &param) { InitModule(param); }

	// 模块析构函数
	~CModGimbal() final { UnregisterModule_(); }

	// 初始化模块
	EAppStatus InitModule(SModInitParam_Base &param) final;

private:
	// 定义云台升降组件类并实例化
	class CComLift: public CComponentBase{
	public:

		const int32_t rangeLimit = GIMBAL_LIFT_MOTOR_RANGE;

		struct SLiftInfo{
			int32_t posit = 0;
			bool isPositArrived = false;
		} liftInfo;

		struct SLiftCmd{
			int32_t setPosit = 0;
		} liftCmd;

		CDevMtr *motor = nullptr;

		CAlgoPid pidPosCtrl;
		CAlgoPid pidSpdCtrl;

		std::array<int16_t, 1> mtrOutputBuffer = {0};

		EAppStatus InitComponent(SModInitParam_Base &param) final;

		EAppStatus UpdateComponent() final;

		static int32_t PhyPositToMtrPosit(float_t phyPosit);

		static float_t MtrPositToPhyPosit(int32_t mtrPosit);

		EAppStatus _UpdateOutput(float_t posit);

		CInfCAN::CCanTxNode* mtrCanTxNode;

	} comLift_;

	// 重写基类函数
    void UpdateHandler_() final;
    void HeartbeatHandler_() final;
    EAppStatus CreateModuleTask_() final;

    static void StartGimbalModuleTask(void *argument);

    // 声明控制量限制函数(负责对控制量进行限幅，在上面那个任务中进行调用)
    EAppStatus RestrictGimbalCommand_();

};

} // namespace my_engineer

#endif // MOD_GIMBAL_HPP
