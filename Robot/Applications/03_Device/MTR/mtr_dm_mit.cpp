/******************************************************************************
 * @brief        
 * 
 * @file         mtr_dm_mit.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-05-09
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "mtr/mtr_dm_mit.hpp"

namespace my_engineer
{

/**
 * @brief 初始化DM电机设备
 * 
 */
EAppStatus CDevMtrDM_MIT::InitDevice(const SDevInitParam_Base *pStructInitParam)
{
	// 检查参数是否为空
	if (pStructInitParam == nullptr)
		return APP_ERROR;
	// 检查设备ID
	if (pStructInitParam->deviceID == EDeviceID::DEV_NULL)
		return APP_ERROR;

	// 类型转换并解引用
	auto &param = *static_cast<const SMtrInitParam_DM_MIT *>(pStructInitParam);

	// 变量初始化
	deviceID = pStructInitParam->deviceID;
	pInfCAN_ = static_cast<CInfCAN *>(InterfaceIDMap.at(param.interfaceID));
	encoderResolution_ = param.encoderResolution;
	useAngleToPosit_ = param.useAngleToPosit;

	// 电机参数
	limit_param_.Q_MAX = param.Q_MAX;
	limit_param_.DQ_MAX = param.DQ_MAX;
	limit_param_.TAU_MAX = param.TAU_MAX;

	// 初始化CAN接收节点
	auto canRxID = static_cast<uint32_t>(param.MasterID);
	canRxNode_.InitRxNode(param.interfaceID, canRxID,
						  CInfCAN::ECanFrameType::DATA,
						  CInfCAN::ECanFrameDlc::DLC_8);

	// 初始化CAN发送节点
	auto canTxID = static_cast<uint32_t>(param.SlaveID);
	canTxNode_.InitTxNode(param.interfaceID, canTxID,
						  CInfCAN::ECanFrameType::DATA,
						  CInfCAN::ECanFrameDlc::DLC_8);

	
	RegisterDevice_();
	RegisterMotor_();

	deviceStatus = APP_OK;
	motorStatus = EMotorStatus::OFFLINE;

	return APP_OK;
}

/**
 * @brief MIT控制模式
 * 
 */
void CDevMtrDM_MIT::Control_MIT(float kp, float kd, float q, float dq, float tau)
{
	// 检查设备状态
	if (deviceStatus == APP_RESET)
		return;


	// 位置、速度和扭矩采用线性映射的关系将浮点型数据转换成有符号的定点数据
	static auto float_to_uint = [](float x, float xmin, float xmax, uint8_t bits) -> uint16_t {
		if (x < xmin)
			x = xmin;
		if (x > xmax)
			x = xmax;
		float span = xmax - xmin;
		float data_norm = (x - xmin) / span;
		uint16_t data_uint = data_norm * ((1 << bits) - 1);
		return data_uint;
	};

	// 计算比例系数、微分系数、位置、速度和扭矩的定点数据
	uint16_t kp_uint = float_to_uint(kp, 0, 500, 12);
	uint16_t kd_uint = float_to_uint(kd, 0, 5, 12);
	Limit_param limit_param_cmd = limit_param_;
	uint16_t q_uint = float_to_uint(q, -limit_param_cmd.Q_MAX, limit_param_cmd.Q_MAX, 16);
	uint16_t dq_uint = float_to_uint(dq, -limit_param_cmd.DQ_MAX, limit_param_cmd.DQ_MAX, 12);
	uint16_t tau_uint = float_to_uint(tau, -limit_param_cmd.TAU_MAX, limit_param_cmd.TAU_MAX, 12);


	// 发送数据帧
	std::array<uint8_t, 8> data_buf{};
	data_buf[0] = (q_uint >> 8) & 0xff;
	data_buf[1] = q_uint & 0xff;
	data_buf[2] = dq_uint >> 4;
	data_buf[3] = ((dq_uint & 0xf) << 4) | ((kp_uint >> 8) & 0xf);
	data_buf[4] = kp_uint & 0xff;
	data_buf[5] = kd_uint >> 4;
	data_buf[6] = ((kd_uint & 0xf) << 4) | ((tau_uint >> 8) & 0xf);
	data_buf[7] = tau_uint & 0xff;

	// 填充数据帧
	Modify_CanTxData(data_buf.data());

	// 发送数据帧
	canTxNode_.Transmit();
}

/**
 * @brief 使能电机
 * 
 */
void CDevMtrDM_MIT::EnableMotor()
{
	// 检查设备状态
	if (deviceStatus == APP_RESET)
		return;

	// 使能电机
	std::array<uint8_t, 8> data_buf = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc};
	Modify_CanTxData(data_buf.data());
	canTxNode_.Transmit();
}

/**
 * @brief 禁用电机
 * 
 */
void CDevMtrDM_MIT::DisableMotor()
{
	// 检查设备状态
	if (deviceStatus == APP_RESET)
		return;

	// 禁用电机
	std::array<uint8_t, 8> data_buf = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd};
	Modify_CanTxData(data_buf.data());
	canTxNode_.Transmit();
}

/**
 * @brief 更新处理函数
 * 
 */
void CDevMtrDM_MIT::UpdateHandler_() {
    // 检查设备状态
    if (deviceStatus == APP_RESET)
        return;

	static auto uint_to_float = [](uint16_t x_uint, float xmin, float xmax, uint8_t bits) -> float {
		float span = xmax - xmin;
		float data_norm = static_cast<float>(x_uint) / ((1 << bits) - 1);
		float x = data_norm * span + xmin;
		return x;
	};

    // 读取CAN数据
    if (canRxNode_.timestamp >= lastHeartbeatTime_) {
		motorData[DATA_ID] = (uint8_t)(canRxNode_.dataBuffer[0] & 0x0F); // 电机ID
		motorData[DATA_ERR] = (uint8_t)(canRxNode_.dataBuffer[0] >> 4); // 错误位
		motorData[DATA_ANGLE] = (int16_t)(canRxNode_.dataBuffer[1] << 8 | canRxNode_.dataBuffer[2]); // 角度
		motorData[DATA_SPEED] = (int16_t)(canRxNode_.dataBuffer[3] << 4 | canRxNode_.dataBuffer[4] >> 4); // 速度
		motorData[DATA_TORQUE] = (int16_t)(canRxNode_.dataBuffer[4] & 0x0F << 8 | canRxNode_.dataBuffer[5]); // 扭矩
		motorData[DATA_TEMP] = (int8_t)(canRxNode_.dataBuffer[6]); // 温度
		motorData[DATA_POSIT] = (useAngleToPosit_) ? getPosition_() : 0; // 位置
		motorPhyAngle = uint_to_float(
			motorData[DATA_ANGLE], -limit_param_.Q_MAX, limit_param_.Q_MAX, 16); // 物理角度

        // 更新心跳时间
        lastHeartbeatTime_ = canRxNode_.timestamp;
    }

}

/**
 * @brief 心跳处理函数
 * 
 */
void CDevMtrDM_MIT::HeartbeatHandler_() {

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
    
	// 每隔100ms使能一次
	if (HAL_GetTick() - lastEnableTime_ > 10) {
		// 使能电机
		EnableMotor();
		lastEnableTime_ = HAL_GetTick();
	}

    return ;
}

/**
 * @brief 获取当前位置,经过减速比的处理
 * 
 * @return int32_t 
 */
int32_t CDevMtrDM_MIT::getPosition_() {
	// 检查设备状态
	if (deviceStatus == APP_RESET)
		return 0;
	// 没应用减速比
	if (!useAngleToPosit_)
		return 0;

	// 第一次读取角度,存储一个lastAngle_值
	if (motorData[DATA_POSIT] == 0 && lastAngle_ == 0) {
		lastAngle_ = motorData[DATA_ANGLE];
		return 0;
	}

	// 计算角度差
	int32_t err = motorData[DATA_ANGLE] - lastAngle_;
	// 处理经过零点情况
	if (abs(err) > static_cast<int32_t>(encoderResolution_ / 2)) {
		err -= std::copysign(encoderResolution_, err);
	}

	lastAngle_ = motorData[DATA_ANGLE];

	return (motorData[DATA_POSIT] + err);
}


} // namespace my_engineer

