/******************************************************************************
 * @brief        
 * 
 * @file         sys_controller_link.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-05
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef SYS_CONTROLLER_LINK_HPP
#define SYS_CONTROLLER_LINK_HPP

#include "sys_common.hpp"
#include "Device.hpp"

namespace my_engineer {

/**
 * @brief 控制器通信系统类
 * 
 */
class CSystemControllerLink final: public CSystemBase{
public:

	// 定义控制器通信系统初始化参数结构体
	struct SSystemInitParam_ControllerLink: public SSystemInitParam_Base{
		EDeviceID controllerLinkDevID = EDeviceID::DEV_NULL; ///< 控制器通信设备ID
	};

	enum KEY_STATUS  {RELEASE = 0, PRESS = 1, LONG_PRESS = 2,};

	// ControllerLink信息结构体(Controller -> Robot)
	struct SControllerLinkInfo {
		bool controller_OK = false;
		bool return_success = false;
		int8_t Rocker_X = 0;
		int8_t Rocker_Y = 0;
		KEY_STATUS Rocker_Key = KEY_STATUS::RELEASE;
		float_t posit_stretch = 0.f;
		float_t posit_traverse = 0.f;
		float_t angle_yaw = 0.f;
		int8_t speed_roll = 0;
	} controllerInfo;

	// 机器人信息结构体(Robot -> Controller)
	struct SRobotInfo {
		bool ask_reset_flag = false; ///< 是否要求复位
		bool controlled_by_controller = false; ///< 是否被控制器控制
		bool ask_return_flag = false; ///< 是否要求归位
		float_t posit_traverse = 0; ///< 横移电机位置
		float_t posit_stretch = 0; ///< 前伸电机位置
		float_t posit_lift = 0; ///< 升降电机位置
		float_t angle_yaw = 0; ///< Yaw轴角度
		float_t angle_roll = 0; ///< Roll轴角度
		float_t angle_pitch = 0; ///< Pitch轴角度
	} robotInfo;

	// 初始化系统
	EAppStatus InitSystem(SSystemInitParam_Base *pStruct) final;

private:

	// 控制器通信设备指针
	CDevControllerLink *pcontrollerLink_ = nullptr;

	void UpdateHandler_() final;

	void HeartbeatHandler_() final;

	// 更新ControllerLink信息
	void UpdateControllerLinkInfo_();

	// 更新Robot信息
	void UpdateRobotInfo_();

	// 更新发送数据包 RobotData
	void UpdateRobotDataPkg_();

	// 更新发送数据包 ControllerData
	void UpdateControllerDataPkg_();

};

extern CSystemControllerLink SysControllerLink;

}   // namespace my_engineer

#endif // SYS_CONTROLLER_LINK_HPP
