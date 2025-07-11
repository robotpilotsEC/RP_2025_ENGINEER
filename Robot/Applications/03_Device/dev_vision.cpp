/******************************************************************************
 * @brief        
 * 
 * @file         dev_vision.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-13
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "dev_vision.hpp"

namespace my_engineer {

/**
 * @brief 初始化视觉设备
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CDevVision::InitDevice(const SDevInitParam_Base *pStructInitParam) {

	// 检查param是否正确
	if (pStructInitParam == nullptr) return APP_ERROR;
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换
	auto &visionParam = *static_cast<const SDevInitParam_Vision *>(pStructInitParam);
	deviceID = visionParam.deviceID;
	usbInterface_ = reinterpret_cast<CInfUSB_CDC *>(InterfaceIDMap.at(visionParam.interfaceID));

	auto callback = [this](auto *buffer, auto len) {
		if (len > 256) return;
		uint8_t *first;
		for(first = buffer; first < buffer + len; first++) {
			if(*first == 0xA5) break;
		}
		if(first == buffer + len) return;
		std::copy(first, buffer + len , rxBuffer_);
		rxTimestamp_ = HAL_GetTick();
	};

	RegisterDevice_();
	usbInterface_->RegisterRxSaveDataCallback(callback);

	deviceStatus = APP_OK;
	visionStatus = EVisionStatus::OFFLINE;

	return APP_OK;
}

/**
 * @brief 发送数据
 * 
 * @param buffer 
 * @param len 
 * @return EAppStatus 
 */
EAppStatus CDevVision::SendPackage(EPackageID packageID, SPkgHeader &packageHeader) {

	if (deviceStatus == APP_RESET) return APP_ERROR;

	switch (packageID) {

		case ID_ORETANK_INFO: {
			auto pkg = reinterpret_cast<SOretankinfoPkg *>(&packageHeader);
			pkg->header.SOF = 0xA5;
			pkg->header.pkgID = ID_ORETANK_INFO;
			pkg->header.pkgLen = sizeof(SOretankinfoPkg) - sizeof(SPkgHeader) - 2; // 2 bytes for CRC16
			pkg->header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&(pkg->header)), sizeof(SPkgHeader) - 1);
			pkg->CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(pkg), sizeof(SOretankinfoPkg) - 2);

			return usbInterface_->Transmit(reinterpret_cast<uint8_t *>(pkg), sizeof(SOretankinfoPkg));
		}

		case ID_RACE_INFO: {
			auto pkg = reinterpret_cast<SRaceinfoPkg *>(&packageHeader);
			pkg->header.SOF = 0xA5;
			pkg->header.pkgID = ID_RACE_INFO;
			pkg->header.pkgLen = sizeof(SRaceinfoPkg) - sizeof(SPkgHeader) - 2; // 2 bytes for CRC16
			pkg->header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&(pkg->header)), sizeof(SPkgHeader) - 1);
			pkg->CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(pkg), sizeof(SRaceinfoPkg) - 2);

			return usbInterface_->Transmit(reinterpret_cast<uint8_t *>(pkg), sizeof(SRaceinfoPkg));
		}

		case ID_DEBUG_INFO: {
			auto pkg = reinterpret_cast<SDebuginfoPkg *>(&packageHeader);
			pkg->a = 0x0000;
			pkg->b = 0x0000;
			pkg->c = 0x0000;
			pkg->d = 0x0000;
			pkg->e = 0x0000;
			pkg->f = 0x0000;

			return usbInterface_->Transmit(reinterpret_cast<uint8_t *>(pkg), sizeof(SDebuginfoPkg));
		}

		default:
			return APP_ERROR;

	}
}

/**
 * @brief 更新处理
 * 
 */
void CDevVision::UpdateHandler_(){
	
	if (deviceStatus == APP_RESET) return;

	if (rxTimestamp_ > lastHeartbeatTime_) {
		ResolveRxPackage_();
	}
}

/**
 * @brief 心跳处理
 * 
 */
void CDevVision::HeartbeatHandler_(){
	
	if (deviceStatus == APP_RESET) return;

	if (HAL_GetTick() - rxTimestamp_ > 1000) {
		deviceStatus = APP_ERROR;
		visionStatus = EVisionStatus::OFFLINE;
	}
	else {
		deviceStatus = APP_OK;
		visionStatus = EVisionStatus::ONLINE;
	}
}

/**
 * @brief 解析接收数据包
 * 
 * @return EAppStatus 
 */
EAppStatus CDevVision::ResolveRxPackage_(){

	if (deviceStatus == APP_RESET) return APP_ERROR;

	SPkgHeader *header = reinterpret_cast<SPkgHeader *>(rxBuffer_);

	// 检查包头
	if (header->SOF != 0xA5) return APP_ERROR;
	// 检查CRC
	// if (CCrcValidator::Crc8Verify(rxBuffer_, header->CRC8, sizeof(SPkgHeader) - 1) != APP_OK) 
	// 	return APP_ERROR;

	switch (header->pkgID) {

		case ID_ORETANK_INFO: {
			auto pkg = reinterpret_cast<SOretankinfoPkg *>(rxBuffer_);
			// 检查CRC
			// if (CCrcValidator::Crc16Verify(rxBuffer_, pkg->CRC16, sizeof(SOretankinfoPkg) - 2) != APP_OK) 
			// 	return APP_ERROR;
			// 解析数据
			oretankinfoPkg = *pkg;
			break;
		}

		default:
			return APP_ERROR;
	}

	lastHeartbeatTime_ = HAL_GetTick();
	return APP_OK;
}

} // namespace my_engineer
