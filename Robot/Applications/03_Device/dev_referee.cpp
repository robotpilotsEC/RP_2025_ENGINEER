/******************************************************************************
 * @brief        
 * 
 * @file         dev_referee.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-17
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "dev_referee.hpp"

namespace my_engineer {

/**
 * @brief 初始化裁判系统设备
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CDevReferee::InitDevice(const SDevInitParam_Base *pStructInitParam) {
	// 检查param是否正确
	if (pStructInitParam == nullptr) return APP_ERROR;
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换
	auto &refereeParam = *static_cast<const SDevInitParam_Referee *>(pStructInitParam);
	deviceID = refereeParam.deviceID;
	uartInterface_ = reinterpret_cast<CInfUART *>(InterfaceIDMap.at(refereeParam.interfaceID));

	auto callback = [this](auto &buffer, auto len) {
		if (len > 512) return;
		std::copy(buffer.data(), buffer.data() + len , rxBuffer_.data());
		rxTimestamp_ = HAL_GetTick();
	};

	RegisterDevice_();
	uartInterface_->RegisterRxSaveDataCallback(callback);

	deviceStatus = APP_OK;
	refereeState = ERefereeStatus::OFFLINE;

	return APP_OK;
}

/**
 * @brief 发送数据
 * 
 * @param buffer 
 * @param len 
 * @return EAppStatus 
 */
EAppStatus CDevReferee::SendPackage(ECommandID commandID, SPkgHeader &packageHeader) {

	if (deviceStatus == APP_RESET) return APP_ERROR;

	return APP_OK;
}

/**
 * @brief 
 */
void CDevReferee::UpdateHandler_() {

	if (deviceStatus == APP_RESET) return;

	if (rxTimestamp_ > lastHeartbeatTime_) {
		ResolveRxPackage_();
	}
}

/**
 * @brief 心跳处理
 * 
 */
void CDevReferee::HeartbeatHandler_() {

	if (deviceStatus == APP_RESET) return;

	if (HAL_GetTick() - lastHeartbeatTime_ > 1000) {
		deviceStatus = APP_ERROR;
		refereeState = ERefereeStatus::OFFLINE;
	} else {
		deviceStatus = APP_OK;
		refereeState = ERefereeStatus::ONLINE;
	}
}

/**
 * @brief 解析接收数据包
 * 
 * @return EAppStatus 
 */
EAppStatus CDevReferee::ResolveRxPackage_() {

	if (deviceStatus == APP_RESET) return APP_ERROR;

	for (size_t i = 0; i < rxBuffer_.size(); i++)
	{
		if (rxBuffer_[i] != 0xA5) {
			continue;
		}

		auto header = reinterpret_cast<SPkgHeader *>(&rxBuffer_[i]);
		if (CCrcValidator::Crc8Verify(rxBuffer_.data(), header->CRC8, 4) != APP_OK) {
			continue;
		}

		switch (header->cmdId) {
			case ECommandID::ID_RACE_STATUS: {
				if (i + sizeof(SRaceStatusPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SRaceStatusPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SRaceStatusPkg) - 2) != APP_OK)
					break;
				raceStatusPkg = *pkg;
				i += sizeof(SRaceStatusPkg) - 1;
				break;
			}

			case ECommandID::ID_RACE_RESULT: {
				if (i + sizeof(SRaceResultPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SRaceResultPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SRaceResultPkg) - 2) != APP_OK)
					break;
				raceResultPkg = *pkg;
				i += sizeof(SRaceResultPkg) - 1;
				break;
			}

			case ECommandID::ID_ROBOT_HP: {
				if (i + sizeof(SRobotHpPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SRobotHpPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SRobotHpPkg) - 2) != APP_OK)
					break;
				robotHpPkg = *pkg;
				i += sizeof(SRobotHpPkg) - 1;
				break;
			}

			case ECommandID::ID_ROBOT_STATUS: {
				if (i + sizeof(SRobotStatusPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SRobotStatusPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SRobotStatusPkg) - 2) != APP_OK)
					break;
				robotStatusPkg = *pkg;
				i += sizeof(SRobotStatusPkg) - 1;
				break;
			}

			case ECommandID::ID_ROBOT_PERF: {
				if (i + sizeof(SRobotPerfPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SRobotPerfPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SRobotPerfPkg) - 2) != APP_OK)
					break;
				robotPerfPkg = *pkg;
				i += sizeof(SRobotPerfPkg) - 1;
				break;
			}

			default: {
				break;
			}
		}
	}

	rxBuffer_.fill(0);
	lastHeartbeatTime_ = HAL_GetTick();
	return APP_OK;
}

size_t CDevReferee::GetDataSegmentLength(ECommandID cmdId) {

	switch (cmdId) {
    case ECommandID::ID_RACE_STATUS: return 11;
    case ECommandID::ID_RACE_RESULT: return 1;
    case ECommandID::ID_ROBOT_HP: return 32;
    case ECommandID::ID_VENUE_EVENT: return 4;
    case ECommandID::ID_WARNING: return 3;
    case ECommandID::ID_ROBOT_STATUS: return 13;
    case ECommandID::ID_ROBOT_PERF: return 16;
    case ECommandID::ID_ROBOT_POSIT: return 24;
    case ECommandID::ID_ROBOT_BUFF: return 6;
    case ECommandID::ID_DMG_INFO: return 1;
    case ECommandID::ID_RFID_STATUS: return 4;
    case ECommandID::ID_ROBOT_MSG: return 113;
    default: return 0;
  }
}

} // namespace my_engineer
