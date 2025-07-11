/**
 * @file rc_dr16.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 大疆DR16遥控器的实现
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "rc/rc_dr16.hpp"

namespace my_engineer {

/**
 * @brief 定义一个数组，依次存放DR16的每个通道类型
 * 
 */
static const ERcChannelType channelTypeList[] = {
    ERcChannelType::LEVER,   ///< DR16_CH_0
    ERcChannelType::LEVER,   ///< DR16_CH_1
    ERcChannelType::LEVER,   ///< DR16_CH_2
    ERcChannelType::LEVER,   ///< DR16_CH_3
    ERcChannelType::LEVER,   ///< DR16_CH_TW
    ERcChannelType::SWITCH,  ///< DR16_CH_SW1
    ERcChannelType::SWITCH,  ///< DR16_CH_SW2
    ERcChannelType::LEVER,   ///< DR16_CH_MOUSE_VX
    ERcChannelType::LEVER,   ///< DR16_CH_MOUSE_VY
    ERcChannelType::LEVER,   ///< DR16_CH_MOUSE_VZ
    ERcChannelType::BUTTON, ///< DR16_CH_MOUSE_L
    ERcChannelType::BUTTON, ///< DR16_CH_MOUSE_R
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_W
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_S
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_A
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_D
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_SHIFT
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_CTRL
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_Q
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_E
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_R
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_F
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_G
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_Z
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_X
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_C
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_V
    ERcChannelType::BUTTON, ///< DR16_CH_KEY_B
};

/**
 * @brief DR16遥控器初始化
 * @param pStructInitParam - 初始化参数结构体指针
 * 
 */
EAppStatus CRcDR16::InitDevice(const SDevInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查设备是否已完成初始化
    if(deviceStatus == APP_OK) return APP_ERROR;

    // 强制类型转换
    auto &param = *static_cast<const SRcDR16InitParam*>(pStructInitParam);
    deviceID = param.deviceID;
    pInfUart_ = static_cast<CInfUART*>(InterfaceIDMap.at(param.interfaceID));

    // 初始化通道
    remoteData.resize(COUNT_);
    for(size_t i = 0; i < COUNT_; i++)
    {
        InitChannel_(i, channelTypeList[i]);
    }

    // 定义一个回调函数，用于接收DR16的数据
    auto callback = [this](auto &buffer, auto size){
        if(size != 18) return;
        std::copy(buffer.data(), buffer.data() + size, rxBuffer_);
        rxTimestamp_ = HAL_GetTick();
    };

    RegisterDevice_();
    // 注册串口接收数据处理回调函数
    pInfUart_->RegisterRxSaveDataCallback(callback);

    deviceStatus = APP_OK;
    return APP_OK;
}

/**
 * @brief 根据缓冲区中的数据更新通道的数值
 * 
 */
void CRcDR16::UpdateHandler_()
{
    // 检查设备是否已完成初始化
    if(deviceStatus == APP_RESET) return;

    // 检查数据是否更新
    if(rxTimestamp_ > lastHeartbeatTime_)
    {
        // 解析DR16的数据
        uint16_t key = rxBuffer_[14] | rxBuffer_[15] << 8;
        remoteData[CH_0].chValue = ((rxBuffer_[0] | rxBuffer_[1] << 8) & 0x07FF) - 1024;
        remoteData[CH_1].chValue = ((rxBuffer_[1] >> 3 | rxBuffer_[2] << 5) & 0x07FF) - 1024;
        remoteData[CH_2].chValue = ((rxBuffer_[2] >> 6 | rxBuffer_[3] << 2 | rxBuffer_[4] << 10) & 0x07FF) - 1024;
        remoteData[CH_3].chValue = ((rxBuffer_[4] >> 1 | rxBuffer_[5] << 7) & 0x07FF) - 1024;
        remoteData[CH_TW].chValue = ((rxBuffer_[16] | rxBuffer_[17] << 8) & 0x07FF) - 1024;
        remoteData[CH_SW1].chValue = ((rxBuffer_[5] >> 4) & 0x000C) >> 2;
        remoteData[CH_SW2].chValue = ((rxBuffer_[5] >> 4) & 0x0003);
        remoteData[CH_MOUSE_VX].chValue = rxBuffer_[6] | rxBuffer_[7] << 8;
        remoteData[CH_MOUSE_VY].chValue = rxBuffer_[8] | rxBuffer_[9] << 8;
        remoteData[CH_MOUSE_VZ].chValue = rxBuffer_[10] | rxBuffer_[11] << 8;
        remoteData[CH_MOUSE_L].chValue = rxBuffer_[12];
        remoteData[CH_MOUSE_R].chValue = rxBuffer_[13];
        remoteData[CH_KEY_W].chValue = (key >> 0) & 0x0001;
        remoteData[CH_KEY_S].chValue = (key >> 1) & 0x0001;
        remoteData[CH_KEY_A].chValue = (key >> 2) & 0x0001;
        remoteData[CH_KEY_D].chValue = (key >> 3) & 0x0001;
        remoteData[CH_KEY_SHIFT].chValue = (key >> 4) & 0x0001;
        remoteData[CH_KEY_CTRL].chValue = (key >> 5) & 0x0001;
        remoteData[CH_KEY_Q].chValue = (key >> 6) & 0x0001;
        remoteData[CH_KEY_E].chValue = (key >> 7) & 0x0001;
        remoteData[CH_KEY_R].chValue = (key >> 8) & 0x0001;
        remoteData[CH_KEY_F].chValue = (key >> 9) & 0x0001;
        remoteData[CH_KEY_G].chValue = (key >> 10) & 0x0001;
        remoteData[CH_KEY_Z].chValue = (key >> 11) & 0x0001;
        remoteData[CH_KEY_X].chValue = (key >> 12) & 0x0001;
        remoteData[CH_KEY_C].chValue = (key >> 13) & 0x0001;
        remoteData[CH_KEY_V].chValue = (key >> 14) & 0x0001;
        remoteData[CH_KEY_B].chValue = (key >> 15) & 0x0001;
        lastHeartbeatTime_ = rxTimestamp_;
    }
}



} // namespace my_engineer
