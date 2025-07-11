/**
 * @file rc_dr16.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义基于rc_common中定义的类的应用于DR16控制器的派生类
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef RC_DR16_HPP
#define RC_DR16_HPP

#include "rc_common.hpp"
#include "inf_uart.hpp"

namespace my_engineer {

/**
 * @brief 大疆DR16遥控器类
 * 
 */
class CRcDR16 : public CRcBase{
public:

    //定义DR16的初始化参数结构体与其基类结构体一致
    using SRcDR16InitParam = SRcInitParam_Base;

    /**
     * @brief 定义DR16的通道枚举类型
     * 
     */
    enum EChannelList {
        CH_0 = 0,
        CH_1,
        CH_2,
        CH_3,
        CH_TW,
        CH_SW1,
        CH_SW2,
        CH_MOUSE_VX,
        CH_MOUSE_VY,
        CH_MOUSE_VZ,
        CH_MOUSE_L,
        CH_MOUSE_R,
        CH_KEY_W,
        CH_KEY_S,
        CH_KEY_A,
        CH_KEY_D,
        CH_KEY_SHIFT,
        CH_KEY_CTRL,
        CH_KEY_Q,
        CH_KEY_E,
        CH_KEY_R,
        CH_KEY_F,
        CH_KEY_G,
        CH_KEY_Z,
        CH_KEY_X,
        CH_KEY_C,
        CH_KEY_V,
        CH_KEY_B,
        COUNT_,
    };

    CRcDR16() { rcType = ERcType::RC_DR16;}
    ~CRcDR16() override = default;

    /**
     * @param pStructInitParam - 初始化参数结构体指针
     */
    EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

    // 启动和停止设备的方法这里不重写了，因为没用上？

private:
    CInfUART *pInfUart_ = nullptr; ///< 串口通信接口指针
    uint32_t rxTimestamp_ = 0; ///< 接收时间戳
    uint8_t rxBuffer_[18] = {0}; ///< 接收缓冲区

    /**
     * @brief 更新DR16的通道状态
     * 
     */
    void UpdateHandler_() override;

};

} // namespace my_engineer

#endif // RC_DR16_HPP
