/******************************************************************************
 * @brief        
 * 
 * @file         proc_arm.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-05-04
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_arm.hpp"

namespace my_engineer {

/**
 * @brief 创建机械臂任务
 * 
 * @param argument 
 */
void CModArm::StartArmModuleTask(void *argument) {

	// 要求参数为CModArm类的实例，如果传入为空则删除任务并返回
	if (argument == nullptr) proc_return();

	// 类型转换
	auto &arm = *static_cast<CModArm *>(argument);

	// 任务循环
	while (true) {

		// FSM
		switch (arm.Module_FSMFlag_) {

			case FSM_RESET: {

				arm.armInfo.isModuleAvailable = false;
				arm.comYaw_.StopComponent();
				arm.comPitch1_.StopComponent();
				arm.comPitch2_.StopComponent();
				arm.comRoll_.StopComponent();
				arm.comEnd_.StopComponent();

				proc_waitMs(20);
				continue; // 跳过下面的代码，直接进入下一次循环
			}

			case FSM_INIT: {

				proc_waitMs(250); // 等待系统稳定
				
				arm.comYaw_.StartComponent();
				proc_waitUntil(arm.comYaw_.componentStatus == APP_OK);
				

				arm.comEnd_.StartComponent();
				arm.comPitch1_.StartComponent();
				arm.comPitch2_.StartComponent();
				arm.comRoll_.StartComponent();
				proc_waitUntil(arm.comEnd_.componentStatus == APP_OK &&
							   arm.comPitch1_.componentStatus == APP_OK &&
							   arm.comPitch2_.componentStatus == APP_OK &&
							   arm.comRoll_.componentStatus == APP_OK);

				arm.armCmd = SArmCmd();
				arm.armCmd.set_angle_Yaw = ARM_YAW_INIT_ANGLE;
				arm.armCmd.set_angle_Pitch1 = ARM_PITCH1_INIT_ANGLE;
				arm.armCmd.set_angle_Pitch2 = ARM_PITCH2_INIT_ANGLE;
				arm.armCmd.set_angle_Roll = ARM_ROLL_INIT_ANGLE;
				arm.armCmd.set_angle_end_pitch = ARM_END_PITCH_INIT_ANGLE;
				arm.armInfo.isModuleAvailable = true;
				arm.Module_FSMFlag_ = FSM_CTRL;
				arm.moduleStatus = APP_OK;

				break;
			}

			case FSM_CTRL: {

				arm.RestrictArmCommand_();

				arm.comYaw_.yawCmd.setAngle = CComYaw::PhyAngleToMtrAngle(arm.armCmd.set_angle_Yaw);
				arm.comPitch1_.pitch1Cmd.setPosit =
					CComPitch1::PhyPositToMtrPosit(arm.armCmd.set_angle_Pitch1);
				arm.comPitch2_.pitch2Cmd.setPosit =
					CComPitch2::PhyPositToMtrPosit(arm.armCmd.set_angle_Pitch2);
				arm.comRoll_.rollCmd.setPosit = 
					CComRoll::PhyPositToMtrPosit(arm.armCmd.set_angle_Roll);
				arm.comEnd_.endCmd.setPosit_Pitch =
					CComEnd::PhyPositToMtrPosit_Pitch(arm.armCmd.set_angle_end_pitch);
				arm.comEnd_.endCmd.setPosit_Roll =
					CComEnd::PhyPositToMtrPosit_Roll(arm.armCmd.set_angle_end_roll);

				proc_waitMs(1); // 1000Hz
				break;
			}

			default: { arm.StopModule(); }
		}
	}

	// 任务退出
	arm.moduleTaskHandle = nullptr;
	proc_return();
}

} // namespace my_engineer
