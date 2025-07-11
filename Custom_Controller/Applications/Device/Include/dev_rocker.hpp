/******************************************************************************
 * @brief        摇杆设备类
 * 
 * @file         dev_rocker.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-28
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef DEV_ROCKER_HPP
#define DEV_ROCKER_HPP

#include "dev_common.hpp"

#include "inf_adc.hpp"


namespace my_engineer {

/**
 * @brief 摇杆设备类
 * 
 */
class CDevRocker final: public CDevBase{
public:

	// 定义摇杆设备初始化参数结构体
	struct SDevInitParam_Rocker: public SDevInitParam_Base{
		EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< ADC接口ID
		CInfADC::EAdcChannel X_channel = CInfADC::EAdcChannel::CHANNEL_NULL; ///< X轴通道
		CInfADC::EAdcChannel Y_channel = CInfADC::EAdcChannel::CHANNEL_NULL; ///< Y轴通道
		GPIO_TypeDef *halGpioPort = nullptr; ///< 按键端口
		uint16_t halGpioPin = 0; ///< 按键引脚
	};

	struct SDevValues_Rocker {
		int32_t X = 0; ///< X轴值
		int32_t Y = 0; ///< Y轴值
		uint8_t key = 0; ///< 按键值
	} rockerValues = { };

	CDevRocker() {deviceType = EDevType::DEV_ROCKER; }

	EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

private:

	CInfADC *adcInterface_ = nullptr; ///< ADC设备指针

	CInfADC::EAdcChannel X_channel_ = CInfADC::EAdcChannel::CHANNEL_NULL; ///< X轴通道
	CInfADC::EAdcChannel Y_channel_ = CInfADC::EAdcChannel::CHANNEL_NULL; ///< Y轴通道
	GPIO_TypeDef *halGpioPort_ = nullptr; ///< 按键端口
	uint16_t halGpioPin_ = 0; ///< 按键引脚

	uint32_t key_udapte_time_ = 0; ///< 按键更新时间戳
	uint8_t key_new_value_ = 0; ///< 最新按键值

	void UpdateHandler_() override;

	void HeartbeatHandler_() override;

};

} // namespace my_engineer

#endif
