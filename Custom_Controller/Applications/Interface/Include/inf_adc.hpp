/******************************************************************************
 * @brief        
 * 
 * @file         inf_adc.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-27
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef INF_ADC_HPP
#define INF_ADC_HPP

#include "inf_common.hpp"
#include "stm32h7xx_hal_adc.h"

namespace my_engineer {

class CInfADC : public CInfBase {
public:

	enum class EAdcChannel {
		CHANNEL_NULL = -1,
		CHANNEL_0 = 0,
		CHANNEL_1,
		CHANNEL_2,
		CHANNEL_3,
		CHANNEL_4,
		CHANNEL_5,
		CHANNEL_6,
		CHANNEL_7,
		CHANNEL_8,
		CHANNEL_9,
		CHANNEL_10,
		CHANNEL_11,
		CHANNEL_12,
		CHANNEL_13,
		CHANNEL_14,
		CHANNEL_15,
		CHANNEL_16,
		CHANNEL_17,
		CHANNEL_18,
	};

	/**
	 * @brief 定义一个派生结构体，用于包含ADC的初始化参数
	 * 
	 */
	struct SInfInitParam_ADC : public SInfInitParam_Base {
		ADC_HandleTypeDef *halAdcHandle = nullptr; ///< ADC句柄
		EVarStatus useDma = false; ///< 是否使用DMA
		EVarStatus useTimer = false; ///< 是否使用定时器
		TIM_HandleTypeDef *halTimerHandle = nullptr; ///< 定时器句柄
		uint8_t channelNum = 1; ///< 通道数量
		DataBuffer<EAdcChannel> channelList; ///< 通道列表 sorted by rank
	};

	CInfADC() {interfaceType = EInfType::INF_ADC;}
	~CInfADC() override;

	// 重写基类的方法
	EAppStatus InitInterface(const SInfInitParam_Base *pStructInitParam) override; 
	EAppStatus StartTransfer() override;
	EAppStatus StopTransfer() override;

	// ADC特有的方法    
	// 读取数据
	uint16_t Read(EAdcChannel channel);

	// 校准数据
	EAppStatus Calibrate(void);

	// 应用层回调函数
	void _ADC_HalConvCpltCallback(ADC_HandleTypeDef *hadc);

private:
	ADC_HandleTypeDef *halAdcHandle_ = nullptr; ///< ADC句柄

	EVarStatus useDma_ = false; ///< 是否使用DMA
	EVarStatus useTimer_ = false; ///< 是否使用定时器
	TIM_HandleTypeDef *halTimerHandle_ = nullptr; ///< 定时器句柄
	uint8_t channelNum_ = 1; ///< 通道数量

	DataBuffer<EAdcChannel> channelList_; ///< 通道列表 sorted by rank

	// 定义DMA接收缓冲区
	DataBuffer<uint16_t> adcDataBuffer_;

	uint16_t adc_data[3];

	// 重写心跳处理函数
	void HeartbeatHandler_() override;
};

} // namespace my_engineer

#endif // INF_ADC_HPP
