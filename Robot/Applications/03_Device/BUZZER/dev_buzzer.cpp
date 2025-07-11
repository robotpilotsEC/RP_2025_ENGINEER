/******************************************************************************
 * @brief        
 * 
 * @file         dev_buzzer.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-29
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "buzzer/dev_buzzer.hpp"

namespace my_engineer {

CDevBuzzer *pDevBuzzer_test;
CMusic Music_Startup;
CMusic Music_Shutdown;
CMusic Music_Init;
CMusic Music_Error;
CMusic Music_Up;
CMusic Music_Down;

EAppStatus CDevBuzzer::InitDevice(const SDevInitParam_Base *pStructInitParam){
	
	// 检查param是否正确
	if (pStructInitParam == nullptr) return APP_ERROR;
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换
	auto &buzzerParam = *static_cast<const SDevInitParam_Buzzer *>(pStructInitParam);
	deviceID = buzzerParam.deviceID;
	halTimerHandle_ = buzzerParam.halTimerHandle;
	channel_ = buzzerParam.channel;
	base_frequency_ = buzzerParam.base_frequency;

	RegisterDevice_();
	InitAllMusic();
	SelectMusic(MusicType::STARTUP);

	pDevBuzzer_test = this;

	deviceStatus = APP_OK;

	return APP_OK;
}

void CDevBuzzer::SelectMusic(MusicType music_type){
	switch (music_type)
	{
		case MusicType::NONE:
			pcurrent_music = nullptr;
			break;
		case MusicType::STARTUP:
			pcurrent_music = &Music_Startup;
			break;
		case MusicType::SHUTDOWN:
			pcurrent_music = &Music_Shutdown;
			break;
		case MusicType::INIT:
			pcurrent_music = &Music_Init;
			break;
		case MusicType::ERROR:
			pcurrent_music = &Music_Error;
			break;
		case MusicType::UP:
			pcurrent_music = &Music_Up;
			break;
		case MusicType::DOWN:
			pcurrent_music = &Music_Down;
			break;
		default:
			break;
	}

	if (pcurrent_music != nullptr)
	{
		pcurrent_music->StopMusic();
	}

}

void CDevBuzzer::update_pwm(double frequency, uint8_t duty){

	if (frequency == 0)
	{
		HAL_TIM_PWM_Stop(halTimerHandle_, channel_);
		return;
	}
	
	uint32_t period = static_cast<uint32_t>(static_cast<double>(base_frequency_) / frequency);
	uint32_t pulse = period * duty / 100 - 1;

	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pulse;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	__HAL_TIM_SET_AUTORELOAD(halTimerHandle_, period);
	HAL_TIM_PWM_ConfigChannel(halTimerHandle_, &sConfigOC, channel_);
	HAL_TIM_PWM_Start(halTimerHandle_, channel_);

}

/**
 * @brief 更新处理
 * 
 */
void CDevBuzzer::UpdateHandler_(){

	static double last_frequency = 0;
	static double frequency = 0;
	
	if (deviceStatus == APP_RESET) return;

	if (pcurrent_music != nullptr)
	{
		uint32_t current_time = HAL_GetTick();
		frequency = pcurrent_music->PlayMusic(current_time);
		if (frequency != last_frequency)
		{
			last_frequency = frequency;
			update_pwm(frequency, pcurrent_music->GetVolume());
		}
	}

}

/**
 * @brief 心跳处理
 * 
 */
void CDevBuzzer::HeartbeatHandler_(){

}

void CDevBuzzer::InitAllMusic(){
	CMusic::SMusicInitParam Music_Startup_param;
	Music_Startup_param.tempo = 40;
	Music_Startup_param.volume = 50;
	Music_Startup_param.repeat = false;
	Music_Startup_param.notes = {
		{CMusic::ETONE::C4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::E4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::G4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::C5, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::C5, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
	};
	Music_Startup.InitMusic(Music_Startup_param);

	CMusic::SMusicInitParam Music_Shutdown_param;
	Music_Shutdown_param.tempo = 40;
	Music_Shutdown_param.volume = 50;
	Music_Shutdown_param.repeat = false;
	Music_Shutdown_param.notes = {
		{CMusic::ETONE::C5, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::G4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::E4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::C4, CMusic::EMUSIC_DURATION::DURATION_1_2},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
	};
	Music_Shutdown.InitMusic(Music_Shutdown_param);

	CMusic::SMusicInitParam Music_Init_param;
	Music_Init_param.tempo = 60;
	Music_Init_param.volume = 50;
	Music_Init_param.repeat = true;
	Music_Init_param.notes = {
		{CMusic::ETONE::A4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::A4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
	};
	Music_Init.InitMusic(Music_Init_param);

	CMusic::SMusicInitParam Music_Error_param;
	Music_Error_param.tempo = 60;
	Music_Error_param.volume = 50;
	Music_Error_param.repeat = true;
	Music_Error_param.notes = {
		{CMusic::ETONE::A4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::A4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::A4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
	};
	Music_Error.InitMusic(Music_Error_param);

	CMusic::SMusicInitParam Music_Up_param;
	Music_Up_param.tempo = 60;
	Music_Up_param.volume = 50;
	Music_Up_param.repeat = true;
	Music_Up_param.notes = {
		{CMusic::ETONE::C4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::D4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::E4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
	};
	Music_Up.InitMusic(Music_Up_param);

	CMusic::SMusicInitParam Music_Down_param;
	Music_Down_param.tempo = 60;
	Music_Down_param.volume = 50;
	Music_Down_param.repeat = true;
	Music_Down_param.notes = {
		{CMusic::ETONE::E4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::D4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_8},
		{CMusic::ETONE::C4, CMusic::EMUSIC_DURATION::DURATION_1_4},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
		{CMusic::ETONE::SILENT, CMusic::EMUSIC_DURATION::DURATION_1_1},
	};
	Music_Down.InitMusic(Music_Down_param);

}

} // namespace my_engineer
