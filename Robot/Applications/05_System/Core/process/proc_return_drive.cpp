/******************************************************************************
 * @brief        
 * 
 * @file         proc_return_drive.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-23
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "Core.hpp"

namespace my_engineer {

/******************************************************************************
 * @brief    返回任务
 ******************************************************************************/
void CSystemCore::StartReturnDriveTask(void *arg) {

	if (arg == nullptr) proc_return();

	/* Get System Core Handle */
	auto &core = *reinterpret_cast<CSystemCore *>(arg);
	const auto timeout = 60000 / 5;    // unit: ms
	auto cnt = 0;

/* Process Exit */
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
