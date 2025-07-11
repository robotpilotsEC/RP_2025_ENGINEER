/******************************************************************************
 * @brief        
 * 
 * @file         sys_vision.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-13
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef SYS_VISION_HPP
#define SYS_VISION_HPP

#include "sys_common.hpp"
#include "Device.hpp"

namespace my_engineer {

/**
 * @brief 视觉系统类
 * 
 */
class CSystemVision final: public CSystemBase{
public:

	// 定义视觉系统初始化参数结构体
	struct SSystemInitParam_Vision: public SSystemInitParam_Base{
		EDeviceID visionDevID = EDeviceID::DEV_NULL; ///< 视觉设备ID
	};

	// 定义视觉信息结构体
	struct SOreTankInfo {
		bool isFoundOreTank = false;
		float_t atti_YAW = 0.0f;
		float_t atti_PITCH = 0.0f;
		float_t atti_ROLL = 0.0f;
		float_t posit_X = 0.0f;
		float_t posit_Y = 0.0f;
		float_t posit_Z = 0.0f;
	};

	struct SUiPointInfo {
		bool isFoundOreTank = false;
		float_t pointPosit_X[5] = {0};
		float_t pointPosit_Y[5] = {0};
	};

  	struct SSysVisionInfo{
		SOreTankInfo oreTank;
		SUiPointInfo uiPoint;
	} visionInfo;

	// 定义视觉系统构造函数
	EAppStatus InitSystem(SSystemInitParam_Base *param) final;

private:

	// 视觉设备指针
	CDevVision *pVision_ = nullptr;

	void UpdateHandler_() final;

	void HeartbeatHandler_() final;

	EAppStatus UpdateOreTankInfo_();

	EAppStatus UpdateUiPointInfo_();

};

extern CSystemVision SysVision;

} // namespace my_engineer

#endif // SYS_VISION_HPP
