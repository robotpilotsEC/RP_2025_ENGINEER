/**
 * @file sys_remote.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义遥控器系统
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SYS_REMOTE_HPP
#define SYS_REMOTE_HPP

#include "sys_common.hpp"
#include "Device.hpp"

#define GET_SIGN(value)  ((value > 0) - (value < 0))

namespace my_engineer {

/**
 * @brief 遥控器系统类
 * 
 */
class CSystemRemote final: public CSystemBase{
public:

    // 定义遥控器系统初始化参数结构体
    struct SSystemInitParam_Remote: public SSystemInitParam_Base{
        EDeviceID remoteDevID = EDeviceID::DEV_NULL; ///< 遥控器设备ID
        //EDeviceID refereeDevID = EDeviceID::DEV_NULL; ///< 裁判系统设备ID
    };

    // Remote信息结构体
    struct SRemoteInfo {
        float_t joystick_LX = 0;
        float_t joystick_LY = 0;
        float_t joystick_RX = 0;
        float_t joystick_RY = 0;
        float_t thumbWheel = 0;
        uint8_t switch_L = 0;
        uint8_t switch_R = 0;
    };

    // Keyboard信息结构体
    struct SKeyboardInfo {
        int16_t mouse_X = 0;
        int16_t mouse_Y = 0;
        int16_t mouse_Thumb = 0;
        bool mouse_L = false;
        bool mouse_R = false;
        bool key_W = false;
        bool key_A = false;
        bool key_S = false;
        bool key_D = false;
        bool key_Q = false;
        bool key_E = false;
        bool key_R = false;
        bool key_F = false;
        bool key_G = false;
        bool key_Z = false;
        bool key_X = false;
        bool key_C = false;
        bool key_V = false;
        bool key_B = false;
        bool key_Ctrl = false;
        bool key_Shift = false;
    };

    // 定义遥控器信息包结构体并实例化
    struct SremoteInfoPackage {
        SRemoteInfo remote;
        SKeyboardInfo keyboard;
    } remoteInfo;

    // 初始化系统
    EAppStatus InitSystem(SSystemInitParam_Base *pStruct) final;

    // 设置遥控器死区
    void SetRemoteDeadZone(float_t deadZone);

    // 复位标志
    bool ResetFlag = false;

private:

    // 遥控器设备指针
    CRcBase *pRemoteDev_ = nullptr;

    // 裁判系统设备指针
    // CDevReferee *pRefereeDev_ = nullptr;

    // 更新处理
    void UpdateHandler_() final;

    // 心跳处理
    void HeartbeatHandler_() final;

    // 更新遥控器
    EAppStatus UpdateRemote_();

    // 更新键盘
    EAppStatus UpdateKeyboard_();

    // 更新裁判系统
    // EAppStatus UpdateReferee_();

    // 遥控器死区
    float_t remoteDeadZone_ = 0;

    EAppStatus UpdateRemote_with_deadzone_();

};

extern CSystemRemote SysRemote;

}   // namespace my_engineer

#endif // SYS_REMOTE_HPP
