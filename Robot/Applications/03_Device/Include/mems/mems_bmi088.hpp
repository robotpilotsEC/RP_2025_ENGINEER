/**
 * @file mems_bmi088.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义基于mems_common中定义的类的应用于BMI088的派生类
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MEMS_BMI088_HPP
#define MEMS_BMI088_HPP

#include "mems_common.hpp"
#include "inf_spi.hpp"
#include "algo_pid.hpp"

#include "bmi08x.h"

namespace my_engineer {

/**
 * @brief BMI088 MEMS类
 * 
 */
class CMemsBmi088 : public CMemsBase{
public:
    /**
     * @brief 定义一个派生结构体，用于包含BMI088 MEMS的初始化参数
     * 
     */
    struct SMemsInitParam_Bmi088 : public SMemsInitParam_Base{
        GPIO_TypeDef *AccelUnitCsPort = nullptr;              ///<
        uint16_t AccelUnitCsPin = 0;                          ///<
        GPIO_TypeDef *GyroUnitCsPort = nullptr;               ///<
        uint16_t GyroUnitCsPin = 0;                           ///<
        EVarStatus useTempControl = false;                   ///<
        float_t tempTarget = 40.0f;                           ///<
        TIM_HandleTypeDef *halTimHandle = nullptr;            ///<
        uint32_t halTimChannel = 0;                           ///<
        CAlgoPid::SAlgoInitParam_Pid tempPidParam; ///<
  };

    CMemsBmi088() {memsType = EMemsType::MEMS_BMI088;}

    // 重写基类的方法
    EAppStatus InitDevice(const SDevInitParam_Base *pStructInitParam) override;
    EAppStatus StartDevice() override;
    EAppStatus StopDevice() override;

private:
    bmi08_dev bmi08Dev_ = {0};
    // CI2cInterface *i2cInterface_ = nullptr;
    CInfSPI *spiInterface_ = nullptr;
    std::array<float_t, 4> quaternion_ = {0.0f, 0.0f, 0.0f, 0.0f};
    std::array<float_t, 3> gyroBias_ = {0.0f, 0.0f, 0.0f};
    GPIO_TypeDef *AccelUnitCsPort_ = nullptr;
    uint16_t AccelUnitCsPin_ = 0;
    GPIO_TypeDef *GyroUnitCsPort_ = nullptr;
    uint16_t GyroUnitCsPin_ = 0;
    EVarStatus useTempControl_ = false;
    float_t tempTarget_ = 40.0f;
    TIM_HandleTypeDef *halTimHandle_ = nullptr;
    uint32_t halTimChannel_ = 0;
    CAlgoPid tempPidController_;
    float_t earthGravity_ = 9.80665f;
    float_t gx_ = 0.0f, gy_ = 0.0f, gz_ = 0.0f;

    void UpdateHandler_() override;

    void HeartbeatHandler_() override;

    // 由于代码量比较小而且被频繁调用，所以将这些函数定义为内联函数，提高性能
    inline void AccelUnitCsEnable_();

    inline void AccelUnitCsDisable_();

    inline void GyroUnitCsEnable_();

    inline void GyroUnitCsDisable_();

    inline void AccelUnitReadMultiRegisters_(uint8_t regAddr, uint8_t *buffer, size_t len);

    inline void GyroUnitReadMultiRegisters_(uint8_t regAddr, uint8_t *buffer, size_t len);

    inline float_t Lsb2Mps2_(int16_t lsb, int8_t gRange, uint8_t bitWidth);

    inline float_t Lsb2Dps_(int16_t lsb, float_t dpsRange, uint8_t bitWidth);

    static BMI08_INTF_RET_TYPE portBmiReadReg_(uint8_t regAddr, uint8_t *regData, uint32_t length, void *intfPtr);

    static BMI08_INTF_RET_TYPE portBmiWriteReg_(uint8_t regAddr, const uint8_t *regData, uint32_t length, void *intfPtr);

    inline void AccelUnitWriteSingleRegister_(uint8_t regAddr, uint8_t regData);

    inline void AccelUnitReadSingleRegister_(uint8_t regAddr, uint8_t *buffer);

    inline void GyroUnitWriteSingleRegister_(uint8_t regAddr, uint8_t regData);

    inline void GyroUnitReadSingleRegister_(uint8_t regAddr, uint8_t *buffer);

    EAppStatus AccelUnitInit_();

    EAppStatus GyroUnitInit_();

    EAppStatus UpdateAccelGyroData_();

    EAppStatus TempControl_();

    static void StartBmi088InitTask_(void *arg);

};
        
} // namespace my_engineer

#endif // MEMS_BMI088_HPP
