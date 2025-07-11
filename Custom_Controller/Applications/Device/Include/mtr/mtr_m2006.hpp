/**
 * @file mtr_m2006.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义基于DJI电机设备类的M2006电机设备类
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MTR_M2006_HPP
#define MTR_M2006_HPP

#include "mtr_dji.hpp"

namespace my_engineer {

/**
 * @brief M2006电机设备类
 * 
 */
class CDevMtrM2006 : public CDevMtrDJI{
private:
    void UpdateHandler_() override{
        // 检查设备状态
        if(deviceStatus == APP_RESET) return;

        // 读取CAN数据
        if (canRxNode_.timestamp >= lastHeartbeatTime_){
            motorData[DATA_ANGLE]  = (int16_t)(canRxNode_.dataBuffer[0] << 8 | canRxNode_.dataBuffer[1]);
            motorData[DATA_SPEED]  = (int16_t)(canRxNode_.dataBuffer[2] << 8 | canRxNode_.dataBuffer[3]);
            motorData[DATA_TORQUE] = (int16_t)(canRxNode_.dataBuffer[4] << 8 | canRxNode_.dataBuffer[5]);
            motorData[DATA_POSIT]  = (useAngleToPosit_) ? getPosition_() : 0;
            lastHeartbeatTime_  = canRxNode_.timestamp;
        }
    }

public:

    using SMtrInitParam_M2006 = SMtrInitParam_DJI;

    CDevMtrM2006() { motorType = EMotorType::MTR_M2006; }
};
    
} // namespace my_engineer

#endif // MTR_M2006_HPP
