/**
 * @file rc_common.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 基于Device基类的遥控器基类的定义
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef RC_COMMON_HPP
#define RC_COMMON_HPP

#include "dev_common.hpp"
#include "rc_def.hpp"

namespace my_engineer {

/**
 * @brief 遥控器基类
 * 
 */
class CRcBase : public CDevBase{
protected:
    /**
     * @brief 定义一个派生结构体，用于包含遥控器的初始化参数
     * 
     */
    struct SRcInitParam_Base : public SDevInitParam_Base{
        EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 通信接口ID
    };

    /**
     * @brief 心跳处理
     * 
     */
    void HeartbeatHandler_() override;

    /**
     * @brief 初始化单个通道
     * 
     */
    EAppStatus InitChannel_(size_t channelNum, ERcChannelType chType);

    /**
     * @brief 更新所有的通道状态
     * 
     */
    EAppStatus UpdateChannels_();

public:

    /**
     * @brief 定义一个通道类
     * @details
     * 通道类用于存储遥控器的通道信息,包括通道的类型、状态、值
     * 并提供操作符重载，以便对通道状态和数值进行比较和转换
     * 
     */
    class CRcChannel{
    public:
        ERcChannelType chType = ERcChannelType::UNDEF; ///< 通道类型
        ERcChannelStatus chStatus = ERcChannelStatus::RESET; ///< 通道状态
        int16_t chValue = 0; ///< 通道值

        // 将CRcChannel类转换为int16_t类型,返回通道值
        explicit operator int16_t() const { return chValue; }

        // 操作符重载
        bool operator== (const ERcChannelStatus &status) const { return chStatus == status; }
        bool operator!= (const ERcChannelStatus &status) const { return chStatus != status; }
        bool operator== (const int16_t &value) const { return chValue == value; }
        bool operator!= (const int16_t &value) const { return chValue != value; }
        bool operator> (const int16_t &value) const { return chValue > value; }
        bool operator< (const int16_t &value) const { return chValue < value; }

    };

    ERcType rcType = ERcType::RC_UNDEF; ///< 遥控器类型
    ERcStatus rcStatus = ERcStatus::RESET; ///< 遥控器状态
    std::vector<CRcChannel> remoteData; ///< 储存遥控器所有通道的数据

    CRcBase() {deviceType = EDevType::DEV_RC;} ///< 构造函数
    ~CRcBase() override { UnregisterDevice_(); } ///< 析构函数

};

} // namespace my_engineer

#endif // RC_COMMON_HPP
