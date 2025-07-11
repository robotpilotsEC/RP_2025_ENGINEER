/******************************************************************************
 * @brief        
 * 
 * @file         sys_referee.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-17
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef SYS_REFEREE_HPP
#define SYS_REFEREE_HPP

#include "sys_common.hpp"

#include "Device.hpp"

namespace my_engineer {

/**
 * @brief 裁判系统类
 * 
 */
class CSystemReferee final: public CSystemBase{
public:
	// 定义裁判系统初始化参数结构体
	struct SSystemInitParam_Referee: public SSystemInitParam_Base{
		EDeviceID refereeDevID = EDeviceID::DEV_NULL; ///< 裁判系统设备ID
	};

	struct SRaceInfo {
		int16_t raceType;     ///< Race Type (1 - RMUC, 2 - Deleted, 3 - ICRA, 4 - RMUL 3v3, 5 - RMUL 1v1)
		int16_t raceStage;    ///< Race Stage (1 - Prepare, 2 - 15s Self-check, 3 - 5s Countdown, 4 - Racing, 5 - Settle)
		time_t timeStamp;     ///< Unix Timestamp
	};

	struct SRobotInfo {
		int16_t robotCamp;   ///< Robot Clamp (0 - Unknown, 1 - Red, 2 - Blue)
		int16_t robotID;     ///< Robot ID (0 - Unknown, 1 - 6)
	};

	struct SSysRefereeInfo {
		time_t unixTimestamp;
		SRaceInfo race;
		SRobotInfo robot;
	} refereeInfo;

	// 初始化系统
	EAppStatus InitSystem(SSystemInitParam_Base *pStruct) final;

private:
	enum EUiConfigID {
		TEXT_PUMP = 0,
		TEXT_MODE,
		TEXT_CURRENT_MODE,
		CIRCLE_PUMP_C,
		CIRCLE_PUMP_L,
		CIRCLE_PUMP_R,
		LINE_L,
		LINE_R,
	};

	std::array<CDevReferee::SUiFigureConfig, 8> uiConfig;

	CDevReferee::SRobotMsgPkg<CDevReferee::SUiDrawTextMsg> pumpTextMsg, modeTextMsg, curModeTextMsg;

	CDevReferee::SRobotMsgPkg<CDevReferee::SUiDrawPentaMsg> visionFigureMsg;

	CDevReferee::SRobotMsgPkg<CDevReferee::SUiDrawHeptaMsg> stateFigureMsg;

	CDevReferee *pRefereeDev_ = nullptr;

	CInfUART * pInterface_ = nullptr;

	void UpdateHandler_() final;

	void HeartbeatHandler_() final;

	EAppStatus UpdateRaceInfo_();

	EAppStatus UpdateRobotInfo_();

	EAppStatus UpdateControllerInfo_();

	void UI_InitDrawing();

	void UI_StartStaticTextDrawing_();

	void UI_StartCurModeTextDrawing_();

	void UI_StartStateFigureDrawing_();

	void UI_StartVisionFigureDrawing_();

	void UI_UpdateCurModeTextDrawing_();

	void UI_UpdateStateFigureDrawing_();

	void UI_UpdateVisionFigureDrawing_();

	static void StartSysRefereeUiTask(void *arg);

};

extern CSystemReferee SysReferee;

}   // namespace my_engineer

#endif // SYS_REFEREE_HPP
