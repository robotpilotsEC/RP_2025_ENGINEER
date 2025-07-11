/******************************************************************************
 * @brief        
 * 
 * @file         proc_controller.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-01
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_controller.hpp"

namespace my_engineer {

/******************************************************************************
 * @brief    创建控制器模块任务
 ******************************************************************************/
void CModController::StartControllerModuleTask(void *argument) {

	// 要求参数为CModController类的实例，如果传入为空则删除任务并返回
	if (argument == nullptr) proc_return();

	// 类型转换
	auto &controller = *static_cast<CModController *>(argument);

	// 任务循环
	while (true) {

		switch (controller.Module_FSMFlag_) {

			case FSM_RESET: {
				
				controller.ControllerInfo.isModuleAvailable = false;
				controller.comTraverse_.StopComponent();
				controller.comStretch_.StopComponent();
				controller.comYaw_.StopComponent();
				controller.comRoll_.StopComponent();
				controller.comRocker_.StopComponent();
				controller.comBuzzer_.StopComponent();

				proc_waitMs(20);
				continue; // 跳过下面的代码，直接进入下一次循环
			}

			case FSM_INIT: {

				proc_waitMs(250); // 等待系统稳定

				controller.comRocker_.StartComponent();
				controller.comBuzzer_.StartComponent();

				controller.comTraverse_.StartComponent();
				controller.comStretch_.StartComponent();
				controller.comYaw_.StartComponent();
				proc_waitUntil(controller.comTraverse_.componentStatus == APP_OK
					&& controller.comStretch_.componentStatus == APP_OK
					&& controller.comYaw_.componentStatus == APP_OK);
				// proc_waitUntil(controller.comTraverse_.componentStatus == APP_OK);
				// proc_waitUntil(controller.comStretch_.componentStatus == APP_OK);
				// proc_waitUntil(controller.comRoll_.componentStatus == APP_OK);

				controller.comBuzzer_.buzzerCmd.musicType = CDevBuzzer::MusicType::STARTUP;

				controller.ControllerCmd = SControllerCmd();
				controller.ControllerCmd.isFree = true;
				controller.ControllerInfo.isModuleAvailable = true;
				controller.Module_FSMFlag_ = FSM_CTRL;
				controller.moduleStatus = APP_OK;
				
				continue;
			}

			case FSM_CTRL: {

				// 限制控制量
				controller.RestrictControllerCommand_();

				static bool last_StartControl = false;

				// // 开始进行控制
				// if (controller.ControllerCmd.StartControl &&
				// 	!last_StartControl)
				// {
				// 	controller.ControllerInfo.isReturnSuccess = false;
				// 	controller.ControllerCmd.isFree = false;
				// }
				// last_StartControl = controller.ControllerCmd.StartControl;

				// 将控制量传递给组件
				controller.comTraverse_.traverseCmd.isFree = controller.ControllerCmd.isFree;
				controller.comStretch_.stretchCmd.isFree = controller.ControllerCmd.isFree;
				controller.comYaw_.yawCmd.isFree = controller.ControllerCmd.isFree;


				
				if(!controller.ControllerCmd.isFree) {

					controller.comTraverse_.traverseCmd.setPosit = controller.ControllerCmd.cmd_traverse;
					controller.comStretch_.stretchCmd.setPosit = controller.ControllerCmd.cmd_stretch;
					controller.comYaw_.yawCmd.setPosit = CModController::CComYaw::PhyPositToMtrPosit(controller.ControllerCmd.cmd_yaw);
				}

				// 检查是否归位完成
				if (controller.comTraverse_.traverseInfo.isPositArrived &&
					controller.comStretch_.stretchInfo.isPositArrived &&
					controller.comYaw_.yawInfo.isPositArrived)
				{
					controller.ControllerInfo.isReturnSuccess = true;
					controller.ControllerCmd.isFree = true;
				}

				proc_waitMs(1);

				continue;
			}

			default: { controller.StopModule(); }
		}
	}

	// 任务退出
	controller.moduleTaskHandle = nullptr;
	proc_return();
}

} // namespace my_engineer
