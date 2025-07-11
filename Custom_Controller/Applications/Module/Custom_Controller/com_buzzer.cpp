/******************************************************************************
 * @brief        
 * 
 * @file         com_buzzer.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-30
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mod_controller.hpp"

namespace my_engineer {

/******************************************************************************
 * @brief    初始化自定义控制器蜂鸣器模块
 ******************************************************************************/
EAppStatus CModController::CComBuzzer::InitComponent(SModInitParam_Base &param) {
	
	// 检查param是否正确
	if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

	// 类型转换
	auto controllerParam = static_cast<SModInitParam_Controller &>(param);

	// 保存蜂鸣器指针
	buzzer = static_cast<CDevBuzzer *>(DeviceIDMap.at(controllerParam.buzzer_id));

	Component_FSMFlag_ = FSM_RESET;
	componentStatus = APP_OK;

	return APP_OK;
}

/******************************************************************************
 * @brief    更新组件
 ******************************************************************************/
EAppStatus CModController::CComBuzzer::UpdateComponent() {
	// 检查组件状态
	if (componentStatus == APP_RESET) return APP_ERROR;

	componentStatus = APP_OK;

	// 更新组件信息
	// 当play_ready为false时，模块层才可操作cmd去选择音乐
	buzzerInfo.play_ready = buzzer->pcurrent_music->if_PlayReady();

	if (current_music != buzzerCmd.musicType) {
		buzzer->SelectMusic(buzzerCmd.musicType);
		current_music = buzzerCmd.musicType;
	}

	return APP_OK;

}

} // namespace my_engineer
