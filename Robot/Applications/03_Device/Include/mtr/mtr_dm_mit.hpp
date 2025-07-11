/******************************************************************************
 * @brief        
 * 
 * @file         mtr_dm_mit.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-05-09
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef MTR_DM_MIT_HPP
#define MTR_DM_MIT_HPP

#include "mtr_common.hpp"
#include "inf_can.hpp"

namespace my_engineer {

/**
 * @brief DM电机设备类
 * 
 */
class CDevMtrDM_MIT : public CDevMtr{
protected:
	
	// 对应的can指针
	CInfCAN *pInfCAN_ = nullptr;

	// 定义接收节点
	CInfCAN::CCanRxNode canRxNode_;

	// 定义发送节点
	CInfCAN::CCanTxNode canTxNode_;

	// 是否存在减速比
	EVarStatus useAngleToPosit_ = false;

	// 编码器分辨率
	uint32_t encoderResolution_ = 8192;

	// 上一次的角度
	int32_t lastAngle_ = 0;

	// 上一次使能时间
	uint32_t lastEnableTime_ = 0;

	// 电机参数
	typedef struct
    {
        float Q_MAX;
        float DQ_MAX;
        float TAU_MAX;
    }Limit_param;

	Limit_param limit_param_;

	void UpdateHandler_() override;
	void HeartbeatHandler_() override;

	// 获取当前位置,经过减速比的处理
	int32_t getPosition_();

public:

	/**
	 * @brief 定义初始化参数结构体
	 * 
	 */
	struct SMtrInitParam_DM_MIT : public SMtrInitParam_Base{
		uint32_t MasterID = 0x00; ///< 控制ID
		uint32_t SlaveID = 0x00; ///< 反馈ID
		bool useAngleToPosit = false; ///< 是否使用角度作为位置
		uint32_t encoderResolution = 8192; ///< 编码器分辨率
		float_t Q_MAX;
		float_t DQ_MAX;
		float_t TAU_MAX;
	};

	/**
	 * @brief 初始化电机
	 */
	EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

	/**
	 * @brief MIT控制模式
	 */
	void Control_MIT(float kp, float kd, float q, float dq, float tau);

	/**
	 * @brief 使能电机
	 */
	void EnableMotor();

	/**
	 * @brief 禁用电机
	 */
	void DisableMotor();

	float_t get_q_max() const { return limit_param_.Q_MAX; }
	float_t get_dq_max() const { return limit_param_.DQ_MAX; }
	float_t get_tau_max() const { return limit_param_.TAU_MAX; }

	float_t motorPhyAngle = 0.0f; ///< 电机物理角度

	/**
	 * @brief 修改CAN发送数据
	 */
	EAppStatus Modify_CanTxData(uint8_t* data) 
	{
		if (data == nullptr) return APP_ERROR;
		std::copy(data, data + canTxNode_.dataBuffer.size(), canTxNode_.dataBuffer.begin());
		return APP_OK;
	}

} ; // CDevMtrDM_MIT

} // namespace my_engineer

#endif
