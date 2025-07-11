/******************************************************************************
 * @brief        
 * 
 * @file         dev_referee.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-17
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef DEV_REFEREE_HPP
#define DEV_REFEREE_HPP

#include "dev_common.hpp"

#include "inf_uart.hpp"

#include "algo_crc.hpp"

namespace my_engineer 
{
/**
 * @brief 裁判系统设备类
 * 
 */
class CDevReferee final: public CDevBase {
public:
	// 定义裁判系统设备初始化参数结构体
	struct SDevInitParam_Referee: public SDevInitParam_Base {
		EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 串口ID
	};

	enum class ECommandID:uint16_t {
		ID_NULL = 0,
		ID_RACE_STATUS  = 0x0001,
		ID_RACE_RESULT  = 0x0002,
		ID_ROBOT_HP     = 0x0003,
		ID_VENUE_EVENT  = 0x0101,
		ID_WARNING      = 0x0104,
		ID_ROBOT_STATUS = 0x0201,
		ID_ROBOT_PERF   = 0x0202,
		ID_ROBOT_POSIT  = 0x0203,
		ID_ROBOT_BUFF   = 0x0204,
		ID_DMG_INFO     = 0x0206,
		ID_RFID_STATUS  = 0x0209,
		ID_ROBOT_MSG    = 0x0301,
		ID_CUSTOM_CTRL  = 0x0302,
		ID_NORMAL_CTRL  = 0x0304,
		ID_MAP_MARK     = 0x0308,
		ID_ROBOT_TO_CUSTOM = 0x0309,
	};

	enum class EMessageID:uint16_t {
		ID_NULL = 0,
		ID_UI_DEL_LAYER = 0x0100,
		ID_UI_DRAW_SINGLE = 0x0101,
		ID_UI_DRAW_DOUBLE = 0x0102,
		ID_UI_DRAW_PENTA = 0x0103,
		ID_UI_DRAW_HEPTA = 0x0104,
		ID_UI_DRAW_TEXT = 0x0110,
	};

	enum KEY_STATUS  {RELEASE = 0, PRESS = 1, LONG_PRESS = 2,};
	enum PITCH_POSIT { LEFT = 0, MIDDLE = 1, RIGHT = 2 };

	struct SPkgHeader {
		uint8_t SOF = 0xA5;
		uint16_t len = 0;
		uint8_t seq = 0;
		uint8_t CRC8 = 0x00;
		ECommandID cmdId = ECommandID::ID_NULL;
	} __packed;

	struct SRaceStatusPkg { // ID: ID_RACE_STATUS (0x0001)
		SPkgHeader header;    ///< Package Header
		uint8_t raceType:4;   ///< Race Type (1 - RMUC, 2 - Deleted, 3 - ICRA, 4 - RMUL 3v3, 5 - RMUL 1v1)
		uint8_t raceStage:4;  ///< Race Stage (1 - Prepare, 2 - 15s Self-check, 3 - 5s Countdown, 4 - Racing, 5 - Settle)
		uint16_t remainTime;  ///< Remaining Time (unit: s)
		time_t timestamp;     ///< Unix Timestamp
		uint16_t CRC16;       ///< CRC16 Checksum
	} __packed raceStatusPkg = { };

	struct SRaceResultPkg { // ID: ID_RACE_RESULT (0x0002)
		SPkgHeader header;    ///< Package Header
		uint8_t winner;       ///< Winner (0 - Draw, 1 - Red, 2 - Blue)
		uint16_t CRC16;       ///< CRC16 Checksum
	} __packed raceResultPkg = { };

	struct SRobotHpPkg {    // ID: ID_ROBOT_HP (0x0003)
		SPkgHeader header;    ///< Package Header
		uint16_t robotHp_R1;  ///< Hero Robot HP (Red)
		uint16_t robotHp_R2;  ///< Engineer Robot HP (Red)
		uint16_t robotHp_R3;  ///< Infantry Robot HP (Red)
		uint16_t robotHp_R4;  ///< Infantry Robot HP (Red)
		uint16_t robotHp_R5;  ///< Infantry Robot HP (Red)
		uint16_t robotHp_R7;  ///< Sentry Robot HP (Red)
		uint16_t outpostHp_R; ///< Outpost HP (Red)
		uint16_t baseHp_R;    ///< Base HP (Red)
		uint16_t robotHp_B1;  ///< Hero Robot HP (Blue)
		uint16_t robotHp_B2;  ///< Engineer Robot HP (Blue)
		uint16_t robotHp_B3;  ///< Infantry Robot HP (Blue)
		uint16_t robotHp_B4;  ///< Infantry Robot HP (Blue)
		uint16_t robotHp_B5;  ///< Infantry Robot HP (Blue)
		uint16_t robotHp_B7;  ///< Sentry Robot HP (Blue)
		uint16_t outpostHp_B; ///< Outpost HP (Blue)
		uint16_t baseHp_B;    ///< Base HP (Blue)
		uint16_t CRC16;       ///< CRC16 Checksum
	} __packed robotHpPkg = { };

	struct SVenueEventPkg {                 // ID: ID_VENUE_EVENT (0x0101)
		SPkgHeader header;                    ///< Package Header
		uint8_t isOuterBloodPointOccupied:1;  ///< Is Outer Blood Point Occupied (0 - No, 1 - Yes)
		uint8_t isInnerBloodPointOccupied:1;  ///< Is Inner Blood Point Occupied (0 - No, 1 - Yes)
		uint8_t isSupplyPointOccupied:1;      ///< Is Supply Point Occupied (RMUL Only; 0 - No, 1 - Yes)
		uint8_t isPowerRunePointOccupied:1;   ///< Is Power Rune Point Occupied (0 - No, 1 - Yes)
		uint8_t isSmallPowerRuneActivate:1;   ///< Is Small Power Rune Activated (0 - No, 1 - Yes)
		uint8_t isBigPowerRuneActivate:1;     ///< Is Big Power Rune Activated (0 - No, 1 - Yes)
		uint8_t zone2OccupiedState:2;         ///< Zone 2 Occupied State (0 - Unknown, 1 - Teammate, 2 - Enemy)
		uint8_t zone3OccupiedState:2;         ///< Zone 3 Occupied State (0 - Unknown, 1 - Teammate, 2 - Enemy)
		uint8_t zone4OccupiedState:2;         ///< Zone 4 Occupied State (0 - Unknown, 1 - Teammate, 2 - Enemy)
		uint8_t baseShieldRemain:7;           ///< Base Shield Remaining Percentage (0 - 100%)
		uint16_t lastDartHitTime:9;           ///< Last Dart Hit Time (unit: s; 0~420s, Base on Race Start Time)
		uint8_t lastDartHitTarget:2;          ///< Last Dart Hit Target (0 - Unknown, 1 - Outpost, 2 - Static Base, 3 - Moving Base)
		uint8_t centerPointOccupiedState:2;   ///< Center Point Occupied State (RMUL Only; 0 - Unknown, 1 - Teammate, 2 - Enemy, 3 - Both)
		uint16_t CRC16;                       ///< CRC16 Checksum
	} __packed venueEventPkg = { };

	struct SWarningPkg {                    // ID: ID_PENALTY (0x0104)
		SPkgHeader header;                    ///< Package Header
		uint8_t lastOffendingLevel;           ///< Last Offending Level (1 - Both Yellow, 2 - Yellow, 3 - Red, 4 - Defeat)
		uint8_t lastOffendingRobotId;         ///< Last Offending Robot ID (Both: 0, Red: 1~7, Blue: 101~107)
		uint8_t lastOffendingLevelCount;      ///< Last Offending Level Count (Default: 0)
		uint16_t CRC16;                       ///< CRC16 Checksum
	} __packed warningPkg = { };

	struct SRobotStatusPkg {                // ID: ID_ROBOT_STATUS (0x0201)
		SPkgHeader header;                    ///< Package Header
		uint8_t robotId;                      ///< Robot ID (Red: 1~7, Blue: 101~107)
		uint8_t robotLevel;                   ///< Robot Level
		uint16_t robotHp;                     ///< Robot Current HP
		uint16_t robotMaxHp;                  ///< Robot HP Limit
		uint16_t robotCoolDown;               ///< Robot Cool Down Value
		uint16_t robotMaxHeat;                ///< Robot Heat Limit
		uint16_t robotMaxPower;               ///< Robot Power Limit
		uint8_t isGimbalOutputEnabled:1;      ///< Is Gimbal Power Output Enabled (0 - No, 1 - Yes)
		uint8_t isChassisOutputEnabled:1;     ///< Is Chassis Power Output Enabled (0 - No, 1 - Yes)
		uint8_t isShooterOutputEnabled:1;     ///< Is Shooter Power Output Enabled (0 - No, 1 - Yes)
		uint16_t CRC16;
	} __packed robotStatusPkg = { };

	struct SRobotPerfPkg {                  // ID: ID_ROBOT_PERF (0x0202)
		SPkgHeader header;                    ///< Package Header
		uint16_t chassisVoltage;              ///< PM Chassis Port Voltage (unit: mV)
		uint16_t chassisCurrent;              ///< PM Chassis Port Current (unit: mA)
		float_t chassisPower;                 ///< PM Chassis Port Power (unit: W)
		uint16_t energyBuffer;                ///< Energy Buffer (unit: J)
		uint16_t shooterHeat_17mm_1;          ///< First 17mm Shooter Heat
		uint16_t shooterHeat_17mm_2;          ///< Second 17mm Shooter Heat
		uint16_t shooterHeat_42mm;            ///< 42mm Shooter Heat
		uint16_t CRC16;                       ///< CRC16 Checksum
	} __packed robotPerfPkg = { };

	struct SRobotPositPkg {
		SPkgHeader header;
		float_t positX;
		float_t positY;
		float_t heading;
		uint16_t CRC16;
	} __packed robotPositPkg = { };

	template<typename T>
	struct SRobotMsgPkg {
		static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
		SPkgHeader header;
		EMessageID messageID;
		uint16_t transmitterID;
		uint16_t receiverID;
		T message;
		uint16_t CRC16;
	} __packed;

	struct SUiDelLayerMsg {                 // MsgID: ID_UI_DEL_LAYER (0x0100)
		uint8_t operate;                      ///< Operation (0 - Null, 1 - Delete, 2 - Add)
		uint8_t layerID;                      ///< Layer ID (Range: 0 ~ 9)
	} __packed;

	struct SUiFigureConfig {
		uint8_t figureName[3];                // Ui Figure Configure
		uint8_t operate:3;                    ///< Operation (0 - Null, 1 - Add, 2 - Edit, 3 - Delete)
		uint8_t figureType:3;                 ///< Figure Type (0 - Line, 1 - Rectangle, 2 - Circle, 3 - Ellipse, 4 - Arc, 5 - Float, 6 - Integer, 7 - String)
		uint8_t layerID:4;                    ///< Layer ID (Range: 0 ~ 9)
		uint8_t color:4;                      ///< Color (0 - Camp Color, 1 - Yellow, 2 - Green, 3 - Orange, 4 - Mulberry, 5 - Pink, 6 - Cyan, 7 - Black, 8 - White)
		uint16_t details_1:9;                 ///< Figure Details 1 (Arc - Start Angle, Text - Font Size)
		uint16_t details_2:9;                 ///< Figure Details 2 (Arc - End Angle, String - String Length)
		uint16_t width:10;                    ///< Line Width
		uint16_t posit_X:11;                  ///< Figure Screen Posit X (Range: 0 ~ 1920)
		uint16_t posit_Y:11;                  ///< Figure Screen Posit Y (Range: 0 ~ 1080)
		uint16_t details_3:10;                ///< Figure Details 3 (Round - Radius, Float - High 10 Bits, Integer - High 10 Bits)
		uint16_t details_4:11;                ///< Figure Details 4 (Graph - End Posit X, Float - Medium 11 Bits, Integer - Medium 11 Bits)
		uint16_t details_5:11;                ///< Figure Details 5 (Graph - End Posit Y, Float - Low 11 Bits, Integer - Low 11 Bits)
	} __packed;

	struct SUiDrawSingleMsg {
		SUiFigureConfig figureConfig[1];
	} __packed;

	struct SUiDrawDoubleMsg {
		SUiFigureConfig figureConfig[2];
	} __packed;

	struct SUiDrawPentaMsg {
		SUiFigureConfig figureConfig[5];
	};

	struct SUiDrawHeptaMsg {
		SUiFigureConfig figureConfig[7];
	};

	struct SUiDrawTextMsg {
		SUiFigureConfig figureConfig;
		uint8_t text[30];
	};

	enum class ERefereeStatus {
		RESET,
		OFFLINE,
		ONLINE,
	} refereeState = ERefereeStatus::RESET;

	CDevReferee() { deviceType = EDevType::DEV_REFEREE; }

	EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) final;

	EAppStatus SendPackage(ECommandID commandID, SPkgHeader &packageHeader);

	static size_t GetDataSegmentLength(ECommandID cmdId);

private:

	CInfUART *uartInterface_ = nullptr; ///< Serial interface pointer

	std::array<uint8_t, 512> rxBuffer_ = {0};

	uint32_t rxTimestamp_ = 0;

	void UpdateHandler_() final;

	void HeartbeatHandler_() final;

	EAppStatus ResolveRxPackage_();
};

} // namespace my_engineer

#endif // DEV_REFEREE_HPP
