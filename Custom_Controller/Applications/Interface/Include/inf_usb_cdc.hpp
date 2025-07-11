/******************************************************************************
 * @brief        
 * 
 * @file         inf_usb_cdc.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-19
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef INF_USB_CDC_HPP
#define INF_USB_CDC_HPP

#include "inf_common.hpp"
#include "usb_device.h"
#include "usbd_cdc_if.h"

namespace my_engineer {

/**
 * @brief USB CDC通信接口类
 * 
 */
class CInfUSB_CDC : public CInfBase{
public:
	/**
	 * @brief 定义一个派生结构体，用于包含USB CDC的初始化参数
	 * 
	 */
	struct SInfInitParam_USB_CDC : public SInfInitParam_Base{
		USBD_HandleTypeDef *halUsbHandle = nullptr; ///< USB句柄
	};

	using USB_CDC_RxDataProcessCallbackTypeDef = std::function<void(uint8_t *, size_t)>;

	CInfUSB_CDC() {interfaceType = EInfType::INF_USB_CDC;}
	~CInfUSB_CDC() override;

	// 重写基类的方法
	EAppStatus InitInterface(const SInfInitParam_Base *pStructInitParam) override;
	EAppStatus StartTransfer() override;
	EAppStatus StopTransfer() override;

	// USB CDC特有的方法
	EAppStatus Transmit(const DataBuffer<uint8_t> &buffer);
	EAppStatus Transmit(const uint8_t *buffer, size_t size);
	EAppStatus FormatTransmit(const char *format, ...);

	// 注册和清空接收数据处理回调函数列表
	EAppStatus RegisterRxSaveDataCallback(USB_CDC_RxDataProcessCallbackTypeDef callback);
	EAppStatus ClearRxSaveDataCallback();

	std::vector<USB_CDC_RxDataProcessCallbackTypeDef> rxDataProcessCallbackList; ///< 接收数据处理回调函数列表

private:
	USBD_HandleTypeDef *halUsbHandle_ = nullptr; ///< USB句柄

	// 重写心跳处理函数
	void HeartbeatHandler_() override;
};

} // namespace my_engineer

#endif // INF_USB_CDC_HPP
