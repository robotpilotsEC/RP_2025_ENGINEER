/******************************************************************************
 * @brief        
 * 
 * @file         inf_adc.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-28
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "inf_adc.hpp"

namespace my_engineer {

std::map<ADC_HandleTypeDef*, CInfADC*> AdcHandleMap; ///< ADC实例map容器

CInfADC *pInfADC_test;

CInfADC::~CInfADC(){
	// 注销ADC在IDmap和Handlemap中的实例
	UnregisterInterface_();

	if(interfaceID != EInterfaceID::INF_NULL)
	{
		AdcHandleMap.erase(halAdcHandle_);
	}
}

/**
 * @brief 初始化ADC
 * @param pStructInitParam - 参数结构体指针
 * 
 */
EAppStatus CInfADC::InitInterface(const SInfInitParam_Base *pStructInitParam){
	// 检查参数是否为空
	if(pStructInitParam == nullptr) return APP_ERROR;
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_BUSY) return APP_ERROR;

	// 强制类型转换
	auto &param = *static_cast<const SInfInitParam_ADC*>(pStructInitParam);
	interfaceID = param.interfaceID;
	halAdcHandle_ = param.halAdcHandle;
	useDma_ = param.useDma;
	useTimer_ = param.useTimer;
	halTimerHandle_ = param.halTimerHandle;
	channelNum_ = param.channelNum;
	channelList_ = param.channelList;

	HAL_ADC_Stop(halAdcHandle_); // 停止ADC

	if(useDma_)
	{
		adcDataBuffer_.resize(channelNum_, 0);
	}

	// 注册ADC在IDmap和Handlemap中的实例
	RegisterInterface_();  
	AdcHandleMap.insert(std::make_pair(halAdcHandle_, this));

	interfaceStatus = APP_OK;

	pInfADC_test = this;

	return APP_OK;
}

/**
 * @brief 启动数据传输
 * 
 */
EAppStatus CInfADC::StartTransfer(){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus != APP_OK) return APP_ERROR;

	// 启动ADC
	if (useDma_ && useTimer_)
	{
		HAL_TIM_Base_Start(halTimerHandle_);
		HAL_ADC_Start_DMA(halAdcHandle_, reinterpret_cast<uint32_t*>(adcDataBuffer_.data()), channelNum_);
	}
	else if (useDma_)
	{
		HAL_ADC_Start_DMA(halAdcHandle_, reinterpret_cast<uint32_t*>(adcDataBuffer_.data()), channelNum_);
	}
	else
	{
		HAL_ADC_Start(halAdcHandle_);
	}

	return APP_OK;
}

/**
 * @brief 停止数据传输
 * 
 */
EAppStatus CInfADC::StopTransfer(){
	// 检查通信接口是否是重置状态
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 停止ADC
	if (useDma_ && useTimer_)
	{
		HAL_TIM_Base_Stop(halTimerHandle_);
		HAL_ADC_Stop_DMA(halAdcHandle_);
	}
	else if (useDma_)
	{
		HAL_ADC_Stop_DMA(halAdcHandle_);
	}
	else
	{
		HAL_ADC_Stop(halAdcHandle_);
	}
	
	return APP_OK;
}

/**
 * @brief 读取数据
 * @param channel - 通道
 * @param value - 读取到的值
 * 
 */
uint16_t CInfADC::Read(EAdcChannel channel){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	uint16_t value;
	// 读取数据
	if(useDma_)
	{
		auto iter = std::find(channelList_.begin(), channelList_.end(), channel);
		if (iter == channelList_.end()) return APP_ERROR;
		value = adcDataBuffer_[static_cast<uint8_t>(std::distance(channelList_.begin(), iter))];
	}
	else
	{
		// ADC_ChannelConfTypeDef sConfig = {0};
		// sConfig.Channel = static_cast<uint32_t>(channel);
		// sConfig.Rank = 1;
		// sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		// HAL_ADC_ConfigChannel(halAdcHandle_, &sConfig);
		// HAL_ADC_Start(halAdcHandle_);
		// HAL_ADC_PollForConversion(halAdcHandle_, 1000);
		// value = HAL_ADC_GetValue(halAdcHandle_);
	}

	return value;
}

/**
 * @brief DMA转换完成回调函数
 */
void CInfADC::_ADC_HalConvCpltCallback(ADC_HandleTypeDef *hadc){

	if (interfaceStatus == APP_RESET || !useDma_) return;

	adc_data[0] = Read(EAdcChannel::CHANNEL_4);
	adc_data[1] = Read(EAdcChannel::CHANNEL_14);
	adc_data[2] = Read(EAdcChannel::CHANNEL_16);

	return;
}

/**
 * @brief 心跳处理函数
 * 
 */
void CInfADC::HeartbeatHandler_(){
}

} // namespace my_engineer

extern "C"{

/**
 * @brief 重写ADC转换完成回调函数
 * @param hadc - ADC句柄
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	// 检查ADC句柄是否在HandleMap中
	auto item = my_engineer::AdcHandleMap.find(hadc);
	if(item != my_engineer::AdcHandleMap.end())
	{
		item->second->_ADC_HalConvCpltCallback(hadc);
	}
}

} // extern "C"
