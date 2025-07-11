/******************************************************************************
 * @brief        
 * 
 * @file         dev_buzzer.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-29
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef DEV_BUZZER_HPP
#define DEV_BUZZER_HPP

#include "dev_common.hpp"
#include "music_lib.hpp"

namespace my_engineer {

/**
 * @brief 蜂鸣器设备类
 * 
 */
class CDevBuzzer final: public CDevBase{
public:

	enum class MusicType {
		NONE = 0,
		STARTUP,
		SHUTDOWN,
		INIT,
		ERROR,
		UP,
		DOWN,
	};

	// 定义蜂鸣器设备初始化参数结构体
	struct SDevInitParam_Buzzer: public SDevInitParam_Base{
		TIM_HandleTypeDef *halTimerHandle = nullptr; ///< 定时器句柄
		uint32_t channel = 0; ///< 通道
		uint32_t base_frequency = 0; ///< 频率
	};

	CDevBuzzer() {deviceType = EDevType::DEV_BUZZER; }

	void InitAllMusic();
	void SelectMusic(MusicType music_type);

	CMusic *pcurrent_music;

	EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

private:

	TIM_HandleTypeDef *halTimerHandle_ = nullptr; ///< 定时器句柄
	uint32_t channel_ = 0; ///< 通道
	uint32_t base_frequency_ = 0; ///< 给蜂鸣器的定时器基频率

	void update_pwm(double frequency, uint8_t duty);

	void UpdateHandler_() override;

	void HeartbeatHandler_() override;

};

} // namespace my_engineer

#endif // DEV_BUZZER_HPP
