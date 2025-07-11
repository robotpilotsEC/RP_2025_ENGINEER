/**
 * @file mtr_kt.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义基于电机设备基类的KT电机设备类
 * @version 1.0
 * @date 2024-12-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MTR_KT_HPP
#define MTR_KT_HPP

#include "mtr_common.hpp"
#include "inf_can.hpp"

namespace my_engineer {

/**
 * @brief KT电机设备类
 * 
 */
class CDevMtrKT : public CDevMtr{
protected:
	
	// 对应的can指针
	CInfCAN *pInfCAN_ = nullptr;

	// 定义接收节点
	CInfCAN::CCanRxNode canRxNode_;

	// 是否存在减速比
	EVarStatus useAngleToPosit_ = false;

	// 编码器分辨率
	uint32_t encoderResolution_ = 65535;

	// 上一次的角度
	int32_t lastAngle_ = 0;

	// 是否使用堵转监测
    EVarStatus useStallMonit_ = false;

    // 堵转监测数据源
    EMotorDataType stallMonitDataSrc_ = DATA_CURRENT;

    // 堵转监测阈值
    uint32_t stallThreshold_ = 200;

    // 堵转认定时间
    uint32_t stallTime_ = 200;

    // 堵转认定计数
    uint32_t stallCount_ = 0;



	void UpdateHandler_() override;
	void HeartbeatHandler_() override;

	// 获取当前位置,经过减速比的处理
	int32_t getPosition_();

public:

	/**
	 * @brief 定义电机ID枚举类型
	 * 
	 */
	enum class EKtMtrID{
		ID_NULL = 0,
		ID_1,
		ID_2,
		ID_3,
		ID_4,
		ID_5,
		ID_6,
	};

	/**
	 * @brief KT电机设备初始化参数
	 * 
	 */
	struct SMtrInitParam_KT : public SMtrInitParam_Base{
		EKtMtrID ktMtrID = EKtMtrID::ID_NULL;
		uint32_t encoderResolution = 65535;
		EVarStatus useAngleToPosit = false;
		EVarStatus useStallMonit = false;
		EMotorDataType stallMonitDataSrc = DATA_CURRENT;
		int32_t stallThreshold = 200;
		uint32_t stallTime = 200;
	};

	EKtMtrID ktMotorID = EKtMtrID::ID_NULL; ///< KT电机ID

	/**
	 * @brief 初始化电机设备
	 * 
	 */
	EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

	/**
     * @brief 根据电机ID,用目标电流值填充发送数据帧的指定位置
     * 
     * @param buffer - 待填充的数据帧
     * @param current - 目标电流值
     */
    EAppStatus FillCanTxBuffer(DataBuffer<uint8_t> &buffer, const int16_t current);
    EAppStatus FillCanTxBuffer(uint8_t *buffer, const int16_t current);
    static EAppStatus FillCanTxBuffer(CDevMtr *mtr, DataBuffer<uint8_t> &buffer, const int16_t current);
    static EAppStatus FillCanTxBuffer(CDevMtr *mtr, uint8_t *buffer, const int16_t current);
	
};

} // namespace my_engineer

#endif // MTR_KT_HPP
