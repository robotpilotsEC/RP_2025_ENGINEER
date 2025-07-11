/**
 * @file proc_exchange.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 
 * @version 1.0
 * @date 2025-01-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Core.hpp"

#define deg2rad (3.1415926f / 180.0f)

#define sucker_down 3.0f	// 吸盘吸矿下垂补偿


namespace my_engineer {

void CSystemCore::StartExchangeTask(void *arg) {
	
	if (arg == nullptr) proc_return();

	// 获取SystemCore句柄
	auto &core = *reinterpret_cast<CSystemCore *>(arg);
	auto &keyboard = SysRemote.remoteInfo.keyboard;
	auto cnt = 0;
	const auto timeout = 60000 / 5; // unit: ms

	while (keyboard.key_Ctrl) {

		if (keyboard.mouse_L) {

			goto proc_exit;
		}

		if (keyboard.mouse_R) {

			goto proc_exit;
		}

		proc_waitMs(5);

	}

// 退出
proc_exit:
	core.parm_->armCmd.isAutoCtrl = false;
	core.psubgantry_->subGantryCmd.isAutoCtrl = false;
	core.pchassis_->chassisCmd.isAutoCtrl = false;
	core.pgimbal_->gimbalCmd.isAutoCtrl = false;
	core.autoCtrlTaskHandle_ = nullptr;
	core.currentAutoCtrlProcess_ = EAutoCtrlProcess::NONE;
	proc_return();

}

} // namespace my_engineer


/*****************************************************************************

 * 				
 * 				    Z
 * 				 ↗
 * 				|————————————————————→ X
 * 				|          相机系（操作手视角）
 *   			|          
 * 				|          
 * 				|          
 * 				↓
 * 				Y
 */
