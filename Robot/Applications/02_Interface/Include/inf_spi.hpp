/**
 * @file inf_spi.hpp
 * @author Zoe
 * @brief 定义spi类
 * @email 2328339747@qq.com
 * @date 2024-10-31
 * 
 * @details
 * 这个相比串口类少写好多东西
 * 比如开启关闭传输和DMA等等，当然也没用上
 */

#ifndef INF_SPI_HPP
#define INF_SPI_HPP

#include "inf_common.hpp"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_spi_ex.h"

namespace my_engineer {

class CInfSPI : public CInfBase{
public:

    /**
     * @brief 定义一个派生结构体，用于包含spi的初始化参数
     * 
     */
    struct SInfInitParam_SPI : public SInfInitParam_Base{
        SPI_HandleTypeDef *halSpiHandle = nullptr; ///< spi句柄
    };

    CInfSPI() {interfaceType = EInfType::INF_SPI;}
    ~CInfSPI() override;

    // 重写基类的方法
    EAppStatus InitInterface(const SInfInitParam_Base *pStructInitParam) override;  

    // spi特有的方法    
    // 发送数据
    EAppStatus Transmit(const DataBuffer<uint8_t> &buffer);
    EAppStatus Transmit(const uint8_t *buffer, size_t size);

    // 接收数据
    EAppStatus Receive(DataBuffer<uint8_t> &buffer);
    EAppStatus Receive(uint8_t *buffer, size_t size);

    // 同时发送和接收数据
    EAppStatus TransmitReceive(const DataBuffer<uint8_t> &txBuffer, DataBuffer<uint8_t> &rxBuffer);
    EAppStatus TransmitReceive(const uint8_t *txBuffer, uint8_t *rxBuffer, size_t size);

private:
    SPI_HandleTypeDef *halSpiHandle_ = nullptr; ///< spi句柄

    // 重写心跳处理函数
    void HeartbeatHandler_() override;
};  

}   // namespace my_engineer

#endif // INF_SPI_HPP
