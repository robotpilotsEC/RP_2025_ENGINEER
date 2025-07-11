/**
 * @file remote.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 遥控器的通用控制层的实现
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 * @details
 * 因为不管用什么遥控器，都需要处理心跳和通道状态初始化和更新
 * 所以在这个文件里面定义这些通用的方法
 */

#include "rc/remote.hpp"

namespace my_engineer {

/**
 * @brief 心跳处理
 * 
 */
void CRcBase::HeartbeatHandler_(){

    if(deviceStatus == APP_RESET) return;

    // 检查遥控器是否在线(根据是否在500ms内收到数据包来判断)
    rcStatus = ((HAL_GetTick() - lastHeartbeatTime_) > 500) ? ERcStatus::OFFLINE : ERcStatus::ONLINE;
    
    // 如果在线，更新所有通道状态
    if(rcStatus == ERcStatus::ONLINE)
    {
        UpdateChannels_();
    }
}

/**
 * @brief 初始化单个通道
 * @param channelNum - 通道号
 * @param chType - 通道类型
 * @return EAppStatus - 返回初始化状态
 */
EAppStatus CRcBase::InitChannel_(size_t channelNum, ERcChannelType chType){
    // 检查通道号是否合法
    if(channelNum >= remoteData.size()) return APP_ERROR;

    // 初始化通道
    remoteData[channelNum].chType = chType;
    remoteData[channelNum].chStatus = ERcChannelStatus::RESET;
    remoteData[channelNum].chValue = 0;

    return APP_OK;
}

/**
 * @brief 更新所有的通道状态
 * @return EAppStatus - 返回更新状态
 */
EAppStatus CRcBase::UpdateChannels_(){

    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;

    // 遍历所有通道
    for(auto &channel : remoteData)
    {
        // 先将通道状态设置为重置
        channel.chStatus = ERcChannelStatus::RESET;
        // 更新通道状态
        switch (channel.chType)
        {
            case ERcChannelType::BUTTON:
                if (channel == 1) channel.chStatus = ERcChannelStatus::PRESS;
                break;
            case ERcChannelType::LEVER:
                // 摇杆类型的通道，绝对值大于一定值时，通道状态有效
                if (channel > 50) channel.chStatus = ERcChannelStatus::HIGH;
                else if (channel < -50) channel.chStatus = ERcChannelStatus::DOWN;
                break;
            default:
                break;
        }
    }

    return APP_OK;
}

} // namespace my_engineer
