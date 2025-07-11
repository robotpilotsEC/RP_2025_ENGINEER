/**
 * @file mtr_kt.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 
 * @version 1.0
 * @date 2024-12-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mtr/mtr_kt.hpp"

namespace my_engineer {

/**
 * @brief 初始化KT电机设备
 * 
 */
EAppStatus CDevMtrKT::InitDevice(const SDevInitParam_Base *pStructInitParam){
	// 检查参数是否为空
	if(pStructInitParam == nullptr) return APP_ERROR;
	// 检查设备ID
	if(pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

	// 类型转换并解引用
	auto &param = *static_cast<const SMtrInitParam_KT*>(pStructInitParam);

	// 变量初始化
	if(param.ktMtrID == EKtMtrID::ID_NULL) return APP_ERROR;
	deviceID = pStructInitParam->deviceID;
	ktMotorID = param.ktMtrID;
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
	auto canRxID = 0x140 + static_cast<uint32_t>(ktMotorID);
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
EAppStatus CDevMtrKT::FillCanTxBuffer(DataBuffer<uint8_t> &buffer, const int16_t current) {
	// 检查设备状态
	if(deviceStatus == APP_RESET) return APP_ERROR;

	// 根据电机ID填充数据帧
	switch (ktMotorID) {

		case EKtMtrID::ID_1:
			buffer[1] = static_cast<uint8_t>(current >> 8);
			buffer[0] = static_cast<uint8_t>(current & 0xFF);
			break;

		case EKtMtrID::ID_2:
			buffer[3] = static_cast<uint8_t>(current >> 8);
			buffer[2] = static_cast<uint8_t>(current & 0xFF);
			break;

		case EKtMtrID::ID_3:
			buffer[5] = static_cast<uint8_t>(current >> 8);
			buffer[4] = static_cast<uint8_t>(current & 0xFF);
			break;

		case EKtMtrID::ID_4:
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
EAppStatus CDevMtrKT::FillCanTxBuffer(uint8_t *buffer, const int16_t current) {
	// 检查设备状态
	if(deviceStatus == APP_RESET) return APP_ERROR;
	// 检查数据帧指针是否为空
	if(buffer == nullptr) return APP_ERROR;

	// 根据电机ID填充数据帧
	switch (ktMotorID) {

		case EKtMtrID::ID_1:
			buffer[1] = static_cast<uint8_t>(current >> 8);
			buffer[0] = static_cast<uint8_t>(current & 0xFF);
			break;

		case EKtMtrID::ID_2:
			buffer[3] = static_cast<uint8_t>(current >> 8);
			buffer[2] = static_cast<uint8_t>(current & 0xFF);
			break;

		case EKtMtrID::ID_3:
			buffer[5] = static_cast<uint8_t>(current >> 8);
			buffer[4] = static_cast<uint8_t>(current & 0xFF);
			break;

		case EKtMtrID::ID_4:
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
EAppStatus CDevMtrKT::FillCanTxBuffer(CDevMtr *mtr, DataBuffer<uint8_t> &buffer, const int16_t current) {
	// 检查设备指针是否为空
	if (mtr == nullptr) return APP_ERROR;

	// 转换一下指针类型
	auto ktMtr = static_cast<CDevMtrKT*>(mtr);

	return ktMtr->FillCanTxBuffer(buffer, current);
}

/**
 * @brief 传入电机指针，根据电机ID,用目标电流值填充发送数据帧的指定位置
 * 
 * @param mtr  - 电机指针
 * @param buffer  - 待填充的数据帧
 * @param current  - 目标电流值
 * @return EAppStatus 
 */
EAppStatus CDevMtrKT::FillCanTxBuffer(CDevMtr *mtr, uint8_t *buffer, const int16_t current) {
	// 检查设备指针是否为空
	if (mtr == nullptr) return APP_ERROR;

	// 转换一下指针类型
	auto ktMtr = static_cast<CDevMtrKT*>(mtr);

	return ktMtr->FillCanTxBuffer(buffer, current);
}

/**
 * @brief 心跳处理函数
 * 
 */
void CDevMtrKT::HeartbeatHandler_(){
	const auto tickRate          = 10;     // Unit: Hz
	const auto offlineDelay      = 500;    // Unit: ms
	const auto stallSpdThreshold = 100;     // Unit: rpm

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

        // // 如果使用了堵转监测
        // if (useStallMonit_) {
		// 	if (motorStatus == EMotorStatus::STOP) {
		// 		int32_t stallCurrent = static_cast<int32_t>(stallThreshold_);
		// 		int32_t current = motorData[stallMonitDataSrc_];
        //     	if (abs(motorData[stallMonitDataSrc_]) > static_cast<int32_t>(stallThreshold_)) {
		// 			stallCount_++;
		// 			if (stallCount_ > stallTime_ / tickRate) {
		// 				motorStatus = EMotorStatus::STALL;
		// 			}
		// 		}
        //     }
        //     else {
        //         stallCount_ = 0;
        //         motorStatus = EMotorStatus::RUNNING;
        //     }
        // }

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
 * @brief 更新处理函数
 * 
 */
void CDevMtrKT::UpdateHandler_(){
	// 检查设备状态
	if(deviceStatus == APP_RESET) return;

	// 读取CAN数据
	if (canRxNode_.timestamp >= lastHeartbeatTime_){
		motorData[DATA_TEMP]   = (int8_t)(canRxNode_.dataBuffer[1]);
		motorData[DATA_ANGLE]  = (int16_t)(canRxNode_.dataBuffer[7] << 8 | canRxNode_.dataBuffer[6]);
		motorData[DATA_SPEED]  = (int16_t)(canRxNode_.dataBuffer[5] << 8 | canRxNode_.dataBuffer[4]);
		motorData[DATA_CURRENT] = (int16_t)(canRxNode_.dataBuffer[3] << 8 | canRxNode_.dataBuffer[2]);
		motorData[DATA_POSIT]  = (useAngleToPosit_) ? getPosition_() : 0;
		lastHeartbeatTime_  = canRxNode_.timestamp;
	}
}

/**
 * @brief 获取当前位置,经过减速比的处理
 * 
 * @return int32_t 
 */
int32_t CDevMtrKT::getPosition_(){
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



