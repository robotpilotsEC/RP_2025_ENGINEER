/**
 * @file mtr_dm.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义基于电机设备基类的DM电机设备类
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MTR_DM_HPP
#define MTR_DM_HPP

#include "mtr_common.hpp"
#include "inf_can.hpp"

namespace my_engineer {

/**
 * @brief DM电机设备类
 * 
 */
class CDevMtrDM : public CDevMtr{
protected:
    
    // 对应的can指针
    CInfCAN *pInfCAN_ = nullptr;

    // 定义接收节点
    CInfCAN::CCanRxNode canRxNode_;

    // 是否存在减速比
    EVarStatus useAngleToPosit_ = false;

    // 编码器分辨率
    uint32_t encoderResolution_ = 8192;

    // 上一次的角度
    int32_t lastAngle_ = 0;

    void UpdateHandler_() override;
    void HeartbeatHandler_() override;

    // 获取当前位置,经过减速比的处理
    int32_t getPosition_();

public:

    /**
     * @brief 定义电机ID枚举类型
     * 
     */
    enum class EDmMtrID{
        ID_NULL = 0,
        ID_1,
        ID_2,
        ID_3,
        ID_4,
        ID_5,
        ID_6,
        ID_7,
        ID_8,
    };

    /**
     * @brief 定义包含DM电机初始化参数的派生结构体
     * 
     */
    struct SMtrInitParam_DM : public SMtrInitParam_Base{
        EDmMtrID dmMtrID = EDmMtrID::ID_NULL; ///< DM电机ID
        uint32_t encoderResolution = 8192; ///< 编码器分辨率
        EVarStatus useAngleToPosit = false; ///< 是否使用角度转位置
    };

    EDmMtrID dmMotorID = EDmMtrID::ID_NULL; ///< DM电机ID

    /**
     * @brief 初始化DM电机设备的参数
     * 
     */
    EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;

    /**
     * @brief 根据电机ID,用目标电流值填充发送数据帧的指定位置
     * 
     */
    EAppStatus FillCanTxBuffer(DataBuffer<uint8_t> &buffer, const int16_t current);
    EAppStatus FillCanTxBuffer(uint8_t *buffer, const int16_t current);
    static EAppStatus FillCanTxBuffer(CDevMtr *mtr, DataBuffer<uint8_t> &buffer, const int16_t current);
    static EAppStatus FillCanTxBuffer(CDevMtr *mtr, uint8_t *buffer, const int16_t current);

};

} // namespace my_engineer

#endif // MTR_DM_HPP
