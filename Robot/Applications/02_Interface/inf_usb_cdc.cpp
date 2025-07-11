/******************************************************************************
 * @brief        
 * 
 * @file         inf_usb_cdc.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-19
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "inf_usb_cdc.hpp"

namespace my_engineer {

std::map<USBD_HandleTypeDef*, CInfUSB_CDC*> UsbCdcHandleMap; ///< USB CDC实例map容器

CInfUSB_CDC::~CInfUSB_CDC(){
	// 注销USB CDC在IDmap和Handlemap中的实例
	UnregisterInterface_();

	if(interfaceID != EInterfaceID::INF_NULL)
	{
		UsbCdcHandleMap.erase(halUsbHandle_);
	} 
}

/**
 * @brief 初始化USB CDC
 * @param pStructInitParam - 参数结构体指针
 * 
 */
EAppStatus CInfUSB_CDC::InitInterface(const SInfInitParam_Base *pStructInitParam){
	// 检查参数是否为空
	if(pStructInitParam == nullptr) return APP_ERROR;
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_BUSY) return APP_ERROR;

	// 强制类型转换
	auto &param = *static_cast<const SInfInitParam_USB_CDC*>(pStructInitParam);
	interfaceID = param.interfaceID;
	halUsbHandle_ = param.halUsbHandle;

	rxDataProcessCallbackList.clear(); // 清空回调函数列表

	// 注册USB CDC在IDmap和Handlemap中的实例
	RegisterInterface_();  
	UsbCdcHandleMap.insert(std::make_pair(halUsbHandle_, this));

	interfaceStatus = APP_BUSY;

	return APP_OK;
}

/**
 * @brief 启动USB CDC数据传输
 * 
 */
EAppStatus CInfUSB_CDC::StartTransfer(){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 启动USB CDC
	USBD_Start(halUsbHandle_);

	interfaceStatus = APP_BUSY;

	return APP_OK;
}

/**
 * @brief 停止USB CDC数据传输
 * 
 */
EAppStatus CInfUSB_CDC::StopTransfer(){
	// 检查通信接口是否是重置状态
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 停止USB CDC
	USBD_Stop(halUsbHandle_);

	if(interfaceStatus == APP_BUSY)
	{
		interfaceStatus = APP_OK;
		return APP_OK;
	}

	else
	{
		return APP_ERROR;
	}
}

/**
 * @brief 发送数据
 * @param buffer - 要发送的数据
 * 
 */
EAppStatus CInfUSB_CDC::Transmit(const DataBuffer<uint8_t> &buffer){
	return Transmit(buffer.data(), buffer.size());
}

/**
 * @brief 发送数据
 * @param buffer - 要发送的数据
 * @param size - 数据大小
 * 
 */
EAppStatus CInfUSB_CDC::Transmit(const uint8_t *buffer, size_t size){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 发送数据
	if(CDC_Transmit_HS((uint8_t*)buffer, size) != USBD_OK)
	{
		return APP_ERROR;
	}

	return APP_OK;
}

/**
 * @brief 格式化发送数据
 * @param format - 格式化字符串
 * 
 */
EAppStatus CInfUSB_CDC::FormatTransmit(const char *format, ...){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 格式化字符串的标准流程
	va_list args;
	va_start(args, format);
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	return Transmit(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
}

/**
 * @brief 注册接收数据处理回调函数
 * @param callback - 接收数据处理回调函数
 * 
 */
EAppStatus CInfUSB_CDC::RegisterRxSaveDataCallback(USB_CDC_RxDataProcessCallbackTypeDef callback){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 注册接收数据处理回调函数
	rxDataProcessCallbackList.push_back(callback);

	return APP_OK;
}

/**
 * @brief 清空接收数据处理回调函数列表
 * 
 */
EAppStatus CInfUSB_CDC::ClearRxSaveDataCallback(){
	// 检查通信接口是否已完成初始化
	if(interfaceStatus == APP_RESET) return APP_ERROR;

	// 清空接收数据处理回调函数列表
	rxDataProcessCallbackList.clear();
	return APP_OK;
}


/**
 * @brief USB CDC心跳处理函数
 * 
 */
void CInfUSB_CDC::HeartbeatHandler_(){
}

} // namespace my_engineer

extern "C" {

void USB_Receive_HS_UserCallback(uint8_t *Buf, uint32_t Len) {
	// 遍历USB CDC实例map容器
	for (auto& pair : my_engineer::UsbCdcHandleMap) {
		auto usbCdc = pair.second;
		if (usbCdc->interfaceStatus == my_engineer::APP_RESET) {
			return ;
		}
		// 调用USB CDC实例的接收数据处理回调函数
		for (const auto& callback : usbCdc->rxDataProcessCallbackList) {
			callback(Buf, Len);
		}
	}
}

}
