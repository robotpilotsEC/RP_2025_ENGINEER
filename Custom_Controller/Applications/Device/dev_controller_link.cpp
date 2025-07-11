/******************************************************************************
 * @brief        
 * 
 * @file         dev_controller_link.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-05
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "dev_controller_link.hpp"

namespace my_engineer {

/**
 * @brief 初始化控制器通信设备
 * 
 * @param param 
 * @return EAppStatus 
 */
EAppStatus CDevControllerLink::InitDevice(const SDevInitParam_Base *pStructInitParam){

	// 检查param是否正确
	if (pStructInitParam == nullptr) return APP_ERROR;
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换
	auto &controllerLinkParam = *static_cast<const SDevInitParam_ControllerLink *>(pStructInitParam);
	deviceID = controllerLinkParam.deviceID;
	uartInterface_ = reinterpret_cast<CInfUART *>(InterfaceIDMap.at(controllerLinkParam.interfaceID));

	auto callback = [this](auto &buffer, auto len) {
		if (len > 512) return;
		std::copy(buffer.data(), buffer.data() + len , rxBuffer_.data());
		rxTimestamp_ = HAL_GetTick();
	};

	RegisterDevice_();
	uartInterface_->RegisterRxSaveDataCallback(callback);

	deviceStatus = APP_OK;
	controllerLinkStatus = EControllerLinkStatus::OFFLINE;

	return APP_OK;
}

/**
 * @brief 发送数据
 * 
 * @param buffer 
 * @param len 
 * @return EAppStatus 
 */
EAppStatus CDevControllerLink::SendPackage(EPackageID packageID, SPkgHeader &packageHeader) {

	// 检查设备状态
	if (deviceStatus == APP_RESET) return APP_ERROR;

	switch (packageID) {

		case ID_CONTROLLER_DATA: {
			auto pkg = reinterpret_cast<SControllerDataPkg *>(&packageHeader);
			pkg->header.SOF = 0xA5;
			pkg->header.seq++;
			pkg->header.pkgLen = sizeof(SControllerDataPkg) - sizeof(SPkgHeader) - 2; // 2 bytes for CRC16
			pkg->header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&(pkg->header)), 4);
			pkg->header.cmd_Id = 0x0302;
			pkg->CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(pkg), sizeof(SControllerDataPkg) - 2);

			return uartInterface_->Transmit(reinterpret_cast<uint8_t *>(pkg), sizeof(SControllerDataPkg));
		}

		case ID_ROBOT_DATA: {
			auto pkg = reinterpret_cast<SRobotDataPkg *>(&packageHeader);
			pkg->header.SOF = 0xA5;
			pkg->header.seq++;
			pkg->header.pkgLen = sizeof(SRobotDataPkg) - sizeof(SPkgHeader)- 2; // 2 bytes for CRC16
			pkg->header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&(pkg->header)), 4);
			pkg->header.cmd_Id = 0x0309;
			pkg->CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(pkg), sizeof(SRobotDataPkg) - 2);

			return uartInterface_->Transmit(reinterpret_cast<uint8_t *>(pkg), sizeof(SRobotDataPkg));
		}

		default:
			return APP_ERROR;
	}
}

/**
 * @brief 更新处理
 * 
 */
void CDevControllerLink::UpdateHandler_(){
	if (deviceStatus == APP_RESET) return;

	if (rxTimestamp_ > lastHeartbeatTime_) {
		ResolveRxPackage_();
	}
}

/**
 * @brief 心跳处理
 * 
 */
void CDevControllerLink::HeartbeatHandler_(){

	if (deviceStatus == APP_RESET) return;

	if (HAL_GetTick() - lastHeartbeatTime_ > 1000) {
		deviceStatus = APP_ERROR;
		controllerLinkStatus = EControllerLinkStatus::OFFLINE;
	}
	else {
		deviceStatus = APP_OK;
		controllerLinkStatus = EControllerLinkStatus::ONLINE;
	}
}

/**
 * @brief 解析接收数据包
 * 
 * @return EAppStatus 
 */
EAppStatus CDevControllerLink::ResolveRxPackage_(){

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

		switch (header->cmd_Id) {

			case 0x0302: {
				if (i + sizeof(SControllerDataPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SControllerDataPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SControllerDataPkg) - 2) != APP_OK)
					break;
				controllerData_info_pkg = *pkg;
				i += sizeof(SControllerDataPkg) - 1;
				break;
			}

			case 0x0309: {
				if (i + sizeof(SRobotDataPkg) > rxBuffer_.size())
					break;
				auto pkg = reinterpret_cast<SRobotDataPkg *>(header);
				if (CCrcValidator::Crc16Verify(reinterpret_cast<uint8_t *>(pkg), pkg->CRC16, sizeof(SRobotDataPkg) - 2) != APP_OK)
					break;
				robotData_info_pkg = *pkg;
				i += sizeof(SRobotDataPkg) - 1;
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

} // namespace my_engineer
