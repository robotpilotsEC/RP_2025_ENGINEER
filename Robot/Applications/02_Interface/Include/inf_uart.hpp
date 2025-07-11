/**
 * @file inf_uart.hpp
 * @author Zoe
 * @brief 定义串口uart类
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 
 */

#ifndef INF_UART_HPP
#define INF_UART_HPP

#include "inf_common.hpp"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_uart_ex.h"

namespace my_engineer {

/**
 * @brief 串口通信接口类
 * 
 */
class CInfUART : public CInfBase{
public:

    /**
     * @brief 定义一个派生结构体，用于包含uart的初始化参数
     * 
     */
    struct SInfInitParam_Uart : public SInfInitParam_Base{
        UART_HandleTypeDef *halUartHandle = nullptr; ///< 串口句柄
        EVarStatus useTxDma = false; ///< 是否使用DMA
        size_t txDmaQueueNum = 4; ///< DMA发送队列数量
        size_t txDmaBuffSize = 128; ///< DMA发送缓冲区大小
        EVarStatus useRxDma = false; ///< 是否使用DMA
        size_t rxDmaQueueNum = 4; ///< DMA接收队列数量
        size_t rxDmaBuffSize = 128; ///< DMA接收缓冲区大小
    };

    // 定义接收数据处理回调函数类型
    using UART_RxDataProcessCallbackTypeDef = std::function<void(DataBuffer<uint8_t> &, size_t)>;

    CInfUART() {interfaceType = EInfType::INF_UART;}
    ~CInfUART() override;

    // 重写基类的方法
    EAppStatus InitInterface(const SInfInitParam_Base *pStructInitParam) override;
    EAppStatus StartTransfer() override;
    EAppStatus StopTransfer() override;

    // 串口特有的方法
    // 发送数据
    EAppStatus Transmit(const DataBuffer<uint8_t> &buffer);
    EAppStatus Transmit(const uint8_t *buffer, size_t size);
    EAppStatus FormatTransmit(const char *format, ...);

    // 接收数据
    EAppStatus Receive(DataBuffer<uint8_t> &buffer);
    EAppStatus Receive(uint8_t *buffer, size_t size);

    // 注册和清空接收数据处理回调函数列表
    EAppStatus RegisterRxSaveDataCallback(const UART_RxDataProcessCallbackTypeDef &Callback);
    EAppStatus ClearCallbackList();

    // 定义串口的应用层回调函数
    void _UART_HalTxCallback(UART_HandleTypeDef *huart);
    void _UART_HalRxCallback(UART_HandleTypeDef *huart);
    void _UART_HalErrorCallback(UART_HandleTypeDef *huart);

private:
    UART_HandleTypeDef *halUartHandle_ = nullptr; ///< 串口句柄

    // 定义DMA发送和接收缓冲区
    EVarStatus useTxDma_ = false, useRxDma_ = false; ///< 是否使用DMA

    // 定义DMA发送和接收队列数量以及缓冲区大小
    size_t txDmaQueueNum_ = 4, rxDmaQueueNum_ = 4; ///< DMA队列数量
    size_t txDmaBuffSize_ = 128, rxDmaBuffSize_ = 128; ///< DMA缓冲区大小

    // 定义DMA发送和接收队列
    DataBuffer<DataBuffer<uint8_t>> txDmaQueue_, rxDmaQueue_;

    // 定义队列的迭代器
    DataBuffer<DataBuffer<uint8_t>>::iterator txDmaQueuePushIter_, txDmaQueuePopIter_;
    DataBuffer<DataBuffer<uint8_t>>::iterator rxDmaQueueIter_;

    // 定义接收数据处理回调函数列表
    std::vector<UART_RxDataProcessCallbackTypeDef> rxDataProcessCallbackList_;

    // 重写心跳处理函数
    void HeartbeatHandler_() override;
};

    

} // namespace my_engineer

#endif // INF_UART_HPP

 