/**
 * @file mtr_dji.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 大疆电机设备类的实现源文件
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mtr/mtr_dji.hpp"

namespace my_engineer {

/**
 * @brief 初始化DJI电机设备
 * 
 */
EAppStatus CDevMtrDJI::InitDevice(const SDevInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查设备ID
    if(pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

    // 类型转换并解引用
    auto &param = *static_cast<const SMtrInitParam_DJI*>(pStructInitParam);

    // 变量初始化
    if(param.djiMtrID == EDjiMtrID::ID_NULL) return APP_ERROR;
    deviceID = pStructInitParam->deviceID;
    djiMotorID = param.djiMtrID;
    pInfCAN_ = static_cast<CInfCAN*>(InterfaceIDMap.at(param.interfaceID));
    encoderResolution_ = param.encoderResolution;
    useAngleToPosit_ = param.useAngleToPosit;
    useStallMonit_ = param.useStallMonit;

    // 如果使用了堵转监测
    if(useStallMonit_){
        stallMonitDataSrc_ = param.stallMonitDataSrc;
        stallThreshold_ = param.stallThreshold;
        stallTime_ = param.stallTime;
    }

    // 初始化CAN接收节点
    auto canRxID = 0x200 + static_cast<uint32_t>(djiMotorID);
    canRxNode_.InitRxNode(param.interfaceID, canRxID, 
                                CInfCAN::ECanFrameType::DATA, 
                                CInfCAN::ECanFrameDlc::DLC_8);

    RegisterDevice_();
    RegisterMotor_();

    deviceStatus = APP_OK;
    motorStatus = EMotorStatus::OFFLINE;

    return APP_OK;
}

/**
 * @brief 根据电机ID,用目标电流值填充发送数据帧的指定位置
 * 
 * @param buffer - 待填充的数据帧
 * @param current - 目标电流值
 */
EAppStatus CDevMtrDJI::FillCanTxBuffer(DataBuffer<uint8_t> &buffer, const int16_t current) {
    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;
    // 检查数据帧长度
    if(buffer.size() != 8) return APP_ERROR;

    // 根据电机ID填充数据帧
    switch (djiMotorID) {

        case EDjiMtrID::ID_1:
        case EDjiMtrID::ID_5:
            buffer[0] = static_cast<uint8_t>(current >> 8);
            buffer[1] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDjiMtrID::ID_2:
        case EDjiMtrID::ID_6:
            buffer[2] = static_cast<uint8_t>(current >> 8);
            buffer[3] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDjiMtrID::ID_3:
        case EDjiMtrID::ID_7:
            buffer[4] = static_cast<uint8_t>(current >> 8);
            buffer[5] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDjiMtrID::ID_4:
        case EDjiMtrID::ID_8:
            buffer[6] = static_cast<uint8_t>(current >> 8);
            buffer[7] = static_cast<uint8_t>(current & 0xFF);
            break;

        default:
            return APP_ERROR;
    }

    return APP_OK;
}

/**
 * @brief 根据电机ID,用目标电流值填充发送数据帧的指定位置
 * 
 * @param buffer - 待填充的数据帧
 * @param current - 目标电流值
 */
EAppStatus CDevMtrDJI::FillCanTxBuffer(uint8_t *buffer, const int16_t current) {
    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;
    // 检查数据帧指针是否为空
    if(buffer == nullptr) return APP_ERROR;

    // 根据电机ID填充数据帧
    switch (djiMotorID) {

        case EDjiMtrID::ID_1:
        case EDjiMtrID::ID_5:
            buffer[0] = static_cast<uint8_t>(current >> 8);
            buffer[1] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDjiMtrID::ID_2:
        case EDjiMtrID::ID_6:
            buffer[2] = static_cast<uint8_t>(current >> 8);
            buffer[3] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDjiMtrID::ID_3:
        case EDjiMtrID::ID_7:
            buffer[4] = static_cast<uint8_t>(current >> 8);
            buffer[5] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDjiMtrID::ID_4:
        case EDjiMtrID::ID_8:
            buffer[6] = static_cast<uint8_t>(current >> 8);
            buffer[7] = static_cast<uint8_t>(current & 0xFF);
            break;

        default:
            return APP_ERROR;
    }

    return APP_OK;
}

/**
 * @brief 传入电机指针，根据电机ID,用目标电流值填充发送数据帧的指定位置
 * 
 * @param mtr   - 电机指针
 * @param buffer  - 待填充的数据帧
 * @param current  - 目标电流值
 * @return EAppStatus 
 */
EAppStatus CDevMtrDJI::FillCanTxBuffer(CDevMtr *mtr, DataBuffer<uint8_t> &buffer, const int16_t current) {
    // 检查设备指针是否为空
    if (mtr == nullptr) return APP_ERROR;

    // 转换一下指针类型
    auto djiMtr = static_cast<CDevMtrDJI*>(mtr);

    return djiMtr->FillCanTxBuffer(buffer, current);
}

/**
 * @brief 传入电机指针，根据电机ID,用目标电流值填充发送数据帧的指定位置
 * 
 * @param mtr  - 电机指针
 * @param buffer  - 待填充的数据帧
 * @param current  - 目标电流值
 * @return EAppStatus 
 */
EAppStatus CDevMtrDJI::FillCanTxBuffer(CDevMtr *mtr, uint8_t *buffer, const int16_t current) {
    // 检查设备指针是否为空
    if (mtr == nullptr) return APP_ERROR;

    // 转换一下指针类型
    auto djiMtr = static_cast<CDevMtrDJI*>(mtr);

    return djiMtr->FillCanTxBuffer(buffer, current);
}

/**
 * @brief 心跳处理函数
 * 
 */
void CDevMtrDJI::HeartbeatHandler_(){

    const auto tickRate          = 10;     // Unit: Hz
    const auto offlineDelay      = 500;    // Unit: ms
    const auto stallSpdThreshold = 50;     // Unit: rpm

    // 检查设备状态
    if (motorStatus == EMotorStatus::RESET) return;

    // 检查心跳时间
    if (HAL_GetTick() - lastHeartbeatTime_ > (offlineDelay / tickRate)) {
        motorStatus = EMotorStatus::OFFLINE;
        return;
    }

    // 检查堵转
    else {
        motorStatus = (abs(motorData[DATA_SPEED]) < stallSpdThreshold) ? 
        EMotorStatus::STOP : EMotorStatus::RUNNING;

        // 如果使用了堵转监测
        if (useStallMonit_) {
            if (abs(motorData[stallMonitDataSrc_]) > static_cast<int32_t>(stallThreshold_) 
                    && (motorStatus == EMotorStatus::STOP)) {
                stallCount_++;
                if (stallCount_ > stallTime_ / tickRate) {
                    motorStatus = EMotorStatus::STALL;
                }
            }
            else {
                stallCount_ = 0;
                motorStatus = EMotorStatus::RUNNING;
            }
        }
    }
}


/**
 * @brief 获取当前位置,经过减速比的处理
 * 
 * @return int32_t 
 */
int32_t CDevMtrDJI::getPosition_(){
    // 检查设备状态
    if(deviceStatus == APP_RESET) return 0;
    // 没应用减速比
    if(!useAngleToPosit_) return 0;

    // 第一次读取角度,存储一个lastAngle_值
    if(motorData[DATA_POSIT] == 0 && lastAngle_ == 0){
        lastAngle_ = motorData[DATA_ANGLE];
        return 0;
    }

    // 计算角度差
    int32_t err = motorData[DATA_ANGLE] - lastAngle_;
    // 处理经过零点情况
    if(abs(err) > static_cast<int32_t>(encoderResolution_ / 2)){
        err -= std::copysign(encoderResolution_, err);
    }

    lastAngle_ = motorData[DATA_ANGLE];

    return (motorData[DATA_POSIT] + err);
}
    

} // namespace my_engineer
