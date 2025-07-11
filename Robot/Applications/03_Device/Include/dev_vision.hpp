/******************************************************************************
 * @brief        
 * 
 * @file         dev_vision.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-13
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef DEV_VISION_HPP
#define DEV_VISION_HPP

#include "dev_common.hpp"

#include "algo_crc.hpp"
#include "inf_usb_cdc.hpp"

namespace my_engineer {

/**
 * @brief 视觉设备类
 * 
 */
class CDevVision final: public CDevBase{
public:

	struct SDevInitParam_Vision: public SDevInitParam_Base{
		EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 串口ID
	};

	enum EPackageID: uint8_t {
		ID_NULL = 0,        ///< Null Package ID
		ID_HEARTBEAT,       ///< Heartbeat Package ID
		ID_RACE_INFO,       ///< Race Information Package ID
		ID_ORETANK_INFO,    ///< Ore Tank Position Information Package ID
		ID_DEBUG_INFO,      ///< Debug Information Package ID
	};

	struct SPkgHeader {
		uint8_t SOF = 0xA5; ///< 包头
		uint8_t pkgID = ID_NULL; ///< 包ID
		uint8_t pkgLen = 0; ///< 包长度
		uint8_t CRC8 = 0x00; ///< CRC8校验
	} __packed; //禁止编译器的内存对齐优化

	struct SRaceinfoPkg {
    SPkgHeader header;          ///< Package Header
    int16_t raceState = 0;      ///< Race State (0 - Unknown, 1 - Start, 2 - Finish)
    int16_t raceCamp = 0;       ///< Race Camp ID (0 - Unknown, 1 - Red, 2 - Blue)
    int16_t exchangeState = 0;  ///< Exchange State (0 - None, 1 - On Exchange)
    uint16_t CRC16 = 0x0000;    ///< Package CRC16 Checksum (Default: 0x0000)
  } __packed raceinfoPkg = { };

  struct SOretankinfoPkg {
    SPkgHeader header;
    int16_t isFoundOreTank = 0;    ///< Is Found Ore Tank (0 - Not Found, 1 - Found)
    float_t atti[3] = {0};         ///< Ore Tank Attitude (Yaw, Pitch, Roll; unit: degree)
    float_t posit[3] = {0};        ///< Ore Tank Position (X, Y, Z; unit: cm)
    float_t uiPoint_X[5] = {0};    ///< UI Point X Position (Up-Left, Up-Right, Down-Right, Down-Left, Center; unit: cm)
    float_t uiPoint_Y[5] = {0};    ///< UI Point Y Position (Up-Left, Up-Right, Down-Right, Down-Left, Center; unit: cm)
    uint16_t CRC16 = 0x0000;       ///< Package CRC16 Checksum (Default: 0x0000)
  } __packed oretankinfoPkg = { };

  struct SDebuginfoPkg {
    int16_t a = 0xffff;
    int16_t b = 0xffff;
    int16_t c = 0xffff;
    int16_t d = 0xffff;
    int16_t e = 0xffff;
    int16_t f = 0x0000;
  } __packed debuginfoPkg = { };

  enum class EVisionStatus {
    RESET,
    OFFLINE,
    ONLINE
  } visionStatus = EVisionStatus::RESET;

  CDevVision() { deviceType = EDevType::DEV_VISION; }

  EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

  EAppStatus SendPackage(EPackageID packageID, SPkgHeader &packageHeader);

private:

	CInfUSB_CDC *usbInterface_ = nullptr; ///< USB CDC接口

	uint8_t rxBuffer_[256] = {0}; ///< 接收缓冲区

	uint32_t rxTimestamp_ = 0; ///< 接收时间戳

	void UpdateHandler_() override;

	void HeartbeatHandler_() override;

	EAppStatus ResolveRxPackage_();
};

} // namespace my_engineer

#endif // DEV_VISION_HPP
