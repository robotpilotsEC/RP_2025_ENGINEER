/**
 * @file mtr_dm.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief DM电机设备类的实现源文件
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mtr/mtr_dm.hpp"

namespace my_engineer {

/**
 * @brief 初始化DM电机设备
 * 
 */
EAppStatus CDevMtrDM::InitDevice(const SDevInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查设备ID
    if(pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

    // 类型转换并解引用
    auto &param = *static_cast<const SMtrInitParam_DM*>(pStructInitParam);

    // 变量初始化
    if(param.dmMtrID == EDmMtrID::ID_NULL) return APP_ERROR;
    deviceID = pStructInitParam->deviceID;
    dmMotorID = param.dmMtrID;
    pInfCAN_ = static_cast<CInfCAN*>(InterfaceIDMap.at(param.interfaceID));
    encoderResolution_ = param.encoderResolution;
    useAngleToPosit_ = param.useAngleToPosit;

    // 初始化CAN接收节点
    auto canRxID = 0x300 + static_cast<uint32_t>(dmMotorID);
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
EAppStatus CDevMtrDM::FillCanTxBuffer(DataBuffer<uint8_t> &buffer, const int16_t current) {
    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;
    // 检查数据帧长度
    if(buffer.size() != 8) return APP_ERROR;

    // 根据电机ID填充数据帧
    switch (dmMotorID)
    {
        case EDmMtrID::ID_1:
        case EDmMtrID::ID_5:
            buffer[1] = static_cast<uint8_t>(current >> 8);
            buffer[0] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDmMtrID::ID_2:
        case EDmMtrID::ID_6:
            buffer[3] = static_cast<uint8_t>(current >> 8);
            buffer[2] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDmMtrID::ID_3:
        case EDmMtrID::ID_7:
            buffer[5] = static_cast<uint8_t>(current >> 8);
            buffer[4] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDmMtrID::ID_4:
        case EDmMtrID::ID_8:
            buffer[7] = static_cast<uint8_t>(current >> 8);
            buffer[6] = static_cast<uint8_t>(current & 0xFF);
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
EAppStatus CDevMtrDM::FillCanTxBuffer(uint8_t *buffer, const int16_t current) {
    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;
    // 检查数据帧指针是否为空
    if(buffer == nullptr) return APP_ERROR;

    // 根据电机ID填充数据帧
    switch (dmMotorID)
    {
        case EDmMtrID::ID_1:
        case EDmMtrID::ID_5:
            buffer[1] = static_cast<uint8_t>(current >> 8);
            buffer[0] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDmMtrID::ID_2:
        case EDmMtrID::ID_6:
            buffer[3] = static_cast<uint8_t>(current >> 8);
            buffer[2] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDmMtrID::ID_3:
        case EDmMtrID::ID_7:
            buffer[5] = static_cast<uint8_t>(current >> 8);
            buffer[4] = static_cast<uint8_t>(current & 0xFF);
            break;

        case EDmMtrID::ID_4:
        case EDmMtrID::ID_8:
            buffer[7] = static_cast<uint8_t>(current >> 8);
            buffer[6] = static_cast<uint8_t>(current & 0xFF);
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
EAppStatus CDevMtrDM::FillCanTxBuffer(CDevMtr *mtr, DataBuffer<uint8_t> &buffer, const int16_t current) {
    // 检查设备指针是否为空
    if (mtr == nullptr) return APP_ERROR;

    // 转换一下指针类型
    auto dmMtr = static_cast<CDevMtrDM*>(mtr);

    return dmMtr->FillCanTxBuffer(buffer, current);
}

/**
 * @brief 传入电机指针，根据电机ID,用目标电流值填充发送数据帧的指定位置
 * 
 * @param mtr  - 电机指针
 * @param buffer  - 待填充的数据帧
 * @param current  - 目标电流值
 * @return EAppStatus 
 */
EAppStatus CDevMtrDM::FillCanTxBuffer(CDevMtr *mtr, uint8_t *buffer, const int16_t current) {
    // 检查设备指针是否为空
    if (mtr == nullptr) return APP_ERROR;

    // 转换一下指针类型
    auto dmMtr = static_cast<CDevMtrDM*>(mtr);

    return dmMtr->FillCanTxBuffer(buffer, current);
}

/**
 * @brief 更新处理函数
 * 
 */
void CDevMtrDM::UpdateHandler_(){
    // 检查设备状态
    if(deviceStatus == APP_RESET) return;

    // 读取CAN数据
    if (canRxNode_.timestamp >= lastHeartbeatTime_){
        motorData[DATA_ANGLE]   = (int16_t)(canRxNode_.dataBuffer[0] << 8 | canRxNode_.dataBuffer[1]);
        motorData[DATA_SPEED]   = (int16_t)(canRxNode_.dataBuffer[2] << 8 | canRxNode_.dataBuffer[3]);
        motorData[DATA_CURRENT] = (int16_t)(canRxNode_.dataBuffer[4] << 8 | canRxNode_.dataBuffer[5]);
        motorData[DATA_TEMP]    = (int8_t)(canRxNode_.dataBuffer[6]);
        motorData[DATA_POSIT]  = (useAngleToPosit_) ? getPosition_() : 0;
        lastHeartbeatTime_  = canRxNode_.timestamp;
    }
}

/**
 * @brief 心跳处理函数
 * 
 */
void CDevMtrDM::HeartbeatHandler_(){

    const auto tickRate          = 10;     // Unit: Hz
    const auto offlineDelay      = 500;    // Unit: ms
    const auto stallSpdThreshold = 50;     // Unit: rpm

    // 检查设备状态
    if(deviceStatus == APP_RESET) return;

    // 检查心跳时间
    if (HAL_GetTick() - lastHeartbeatTime_ > (offlineDelay / tickRate)) {
        motorStatus = EMotorStatus::OFFLINE;
        return ;
    }

    motorStatus = (abs(motorData[DATA_SPEED]) < stallSpdThreshold) ? 
        EMotorStatus::STOP : EMotorStatus::RUNNING;
    
    return ;
}

/**
 * @brief 获取当前位置,经过减速比的处理
 * 
 * @return int32_t 
 */
int32_t CDevMtrDM::getPosition_(){
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
