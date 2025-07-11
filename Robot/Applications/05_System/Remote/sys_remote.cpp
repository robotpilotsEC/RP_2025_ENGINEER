/**
 * @file sys_remote.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 遥控器系统源文件
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Core.hpp"

#include "FreeRTOS.h"
#include "task.h"

namespace my_engineer {

// 实例化一个遥控器系统
CSystemRemote SysRemote;

/**
 * @brief 初始化遥控器系统
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemRemote::InitSystem(SSystemInitParam_Base *pStruct) {
    
    // 检查参数及ID是否为空
    if (pStruct == nullptr) return APP_ERROR;
    if (pStruct->systemID == ESystemID::SYS_NULL) return APP_ERROR;

    // 类型转换
    auto &param = *reinterpret_cast<SSystemInitParam_Remote *>(pStruct);

    // 初始化遥控器设备
    systemID = param.systemID;
    pRemoteDev_ = static_cast<CRcBase *>(DeviceIDMap.at(param.remoteDevID));

    // 注册系统
    RegisterSystem_();

    systemStatus = APP_ERROR;
    return APP_OK;
}

/**
 * @brief 更新处理
 * 
 */
void CSystemRemote::UpdateHandler_() {
    // 检查系统状态
    if (systemStatus != APP_OK) return;

    UpdateRemote_();
    UpdateKeyboard_();
}

/**
 * @brief 心跳处理
 * 
 */
void CSystemRemote::HeartbeatHandler_() {
    // 检查系统状态
    if (systemStatus == APP_RESET) return;

    if(pRemoteDev_->rcStatus ==  ERcStatus::ONLINE)
        systemStatus = APP_OK;
    else
        systemStatus = APP_ERROR;
}

/**
 * @brief 更新遥控器
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemRemote::UpdateRemote_() {

    /* 软件复位 */
    // 复位顺序：右边在中间，然后左边在下面，最后右边在下面
    if (remoteInfo.remote.switch_R == 3
        && pRemoteDev_->remoteData[CRcDR16::CH_SW1].chValue == 2
        && pRemoteDev_->remoteData[CRcDR16::CH_SW2].chValue == 2) {
        ResetFlag = true;
    }

    remoteInfo.remote.joystick_RX = pRemoteDev_->remoteData[CRcDR16::CH_0].chValue / 6.6f;
    remoteInfo.remote.joystick_RY = pRemoteDev_->remoteData[CRcDR16::CH_1].chValue / 6.6f;
    remoteInfo.remote.joystick_LX = pRemoteDev_->remoteData[CRcDR16::CH_2].chValue / 6.6f;
    remoteInfo.remote.joystick_LY = pRemoteDev_->remoteData[CRcDR16::CH_3].chValue / 6.6f;
    remoteInfo.remote.thumbWheel  = pRemoteDev_->remoteData[CRcDR16::CH_TW].chValue / 6.6f;
    remoteInfo.remote.switch_L    = pRemoteDev_->remoteData[CRcDR16::CH_SW1].chValue;
    remoteInfo.remote.switch_R    = pRemoteDev_->remoteData[CRcDR16::CH_SW2].chValue;

    UpdateRemote_with_deadzone_();

    return APP_OK;
}

/**
 * @brief 更新键盘
 * 
 * @return EAppStatus 
 */
EAppStatus CSystemRemote::UpdateKeyboard_() {
    /* 软件复位 */
    if (pRemoteDev_->remoteData[CRcDR16::CH_KEY_CTRL] == 1
        && pRemoteDev_->remoteData[CRcDR16::CH_KEY_SHIFT].chValue == 1
        && pRemoteDev_->remoteData[CRcDR16::CH_KEY_R].chValue == 1) {
        ResetFlag = true;
    }

    remoteInfo.keyboard.mouse_X     =  pRemoteDev_->remoteData[CRcDR16::CH_MOUSE_VX].chValue;
    remoteInfo.keyboard.mouse_Y     =  pRemoteDev_->remoteData[CRcDR16::CH_MOUSE_VY].chValue;
    remoteInfo.keyboard.mouse_Thumb =  pRemoteDev_->remoteData[CRcDR16::CH_MOUSE_VZ].chValue;
    remoteInfo.keyboard.mouse_L     = (pRemoteDev_->remoteData[CRcDR16::CH_MOUSE_L].chValue == 1);
    remoteInfo.keyboard.mouse_R     = (pRemoteDev_->remoteData[CRcDR16::CH_MOUSE_R].chValue == 1);
    remoteInfo.keyboard.key_W       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_W].chValue == 1);
    remoteInfo.keyboard.key_A       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_A].chValue == 1);
    remoteInfo.keyboard.key_S       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_S].chValue == 1);
    remoteInfo.keyboard.key_D       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_D].chValue == 1);
    remoteInfo.keyboard.key_Q       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_Q].chValue == 1);
    remoteInfo.keyboard.key_E       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_E].chValue == 1);
    remoteInfo.keyboard.key_R       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_R].chValue == 1);
    remoteInfo.keyboard.key_F       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_F].chValue == 1);
    remoteInfo.keyboard.key_G       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_G].chValue == 1);
    remoteInfo.keyboard.key_Z       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_Z].chValue == 1);
    remoteInfo.keyboard.key_X       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_X].chValue == 1);
    remoteInfo.keyboard.key_C       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_C].chValue == 1);
    remoteInfo.keyboard.key_V       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_V].chValue == 1);
    remoteInfo.keyboard.key_B       = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_B].chValue == 1);
    remoteInfo.keyboard.key_Ctrl    = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_CTRL].chValue == 1);
    remoteInfo.keyboard.key_Shift   = (pRemoteDev_->remoteData[CRcDR16::CH_KEY_SHIFT].chValue == 1);

    return APP_OK;
}

EAppStatus CSystemRemote::UpdateRemote_with_deadzone_() {

    // 将死区之外的数值重新映射到0到100之间
    remoteInfo.remote.joystick_RX = GET_SIGN(remoteInfo.remote.joystick_RX) * 
        (fabs(remoteInfo.remote.joystick_RX) > remoteDeadZone_ ? 
            (fabs(remoteInfo.remote.joystick_RX) - remoteDeadZone_) / (100 - remoteDeadZone_) * 100 : 0);
    remoteInfo.remote.joystick_RY = GET_SIGN(remoteInfo.remote.joystick_RY) *
        (fabs(remoteInfo.remote.joystick_RY) > remoteDeadZone_ ?
            (fabs(remoteInfo.remote.joystick_RY) - remoteDeadZone_) / (100 - remoteDeadZone_) * 100 : 0);
    remoteInfo.remote.joystick_LX = GET_SIGN(remoteInfo.remote.joystick_LX) *
        (fabs(remoteInfo.remote.joystick_LX) > remoteDeadZone_ ?
            (fabs(remoteInfo.remote.joystick_LX) - remoteDeadZone_) / (100 - remoteDeadZone_) * 100 : 0);
    remoteInfo.remote.joystick_LY = GET_SIGN(remoteInfo.remote.joystick_LY) *
        (fabs(remoteInfo.remote.joystick_LY) > remoteDeadZone_ ?
            (fabs(remoteInfo.remote.joystick_LY) - remoteDeadZone_) / (100 - remoteDeadZone_) * 100 : 0);
	
	return APP_OK;
}

void CSystemRemote::SetRemoteDeadZone(float_t deadZone) {
    
    remoteDeadZone_ = deadZone > 100.f ? 100.f : deadZone;
}


}   // namespace my_engineer

