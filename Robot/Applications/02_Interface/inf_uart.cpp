/**
 * @file inf_uart.cpp
 * @author Zoe
 * @brief 完成串口uart类的定义和实现
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 定义串口的map容器，用于存储所有串口的实例。
 * 定义串口的初始化、启动、停止、发送、接收、心跳、各种回调函数。
 */

#include "inf_uart.hpp"

namespace my_engineer {

std::map<UART_HandleTypeDef*, CInfUART*> UartHandleMap; ///< 串口实例map容器

CInfUART::~CInfUART(){
    // 注销串口在IDmap和Handlemap中的实例
    UnregisterInterface_();

    if(interfaceID != EInterfaceID::INF_NULL)
    {
        UartHandleMap.erase(halUartHandle_);
    } 
}

/**
 * @brief 初始化串口
 * @param pStructInitParam - 参数结构体指针
 * 
 */
EAppStatus CInfUART::InitInterface(const SInfInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_BUSY) return APP_ERROR;

    // 强制类型转换
    auto &param = *static_cast<const SInfInitParam_Uart*>(pStructInitParam);
    interfaceID = param.interfaceID;
    halUartHandle_ = param.halUartHandle;
    useTxDma_ = param.useTxDma;
    useRxDma_ = param.useRxDma;

    HAL_UART_Abort(halUartHandle_); // 中断串口
    rxDataProcessCallbackList_.clear(); // 清空回调函数列表

    if(useTxDma_)
    {
        txDmaQueueNum_ = param.txDmaQueueNum;
        txDmaBuffSize_ = param.txDmaBuffSize;
        //调整队列大小，并将新数据类型定义为DataBuffer<uint8_t>(txDmaBuffSize_, 0)
        txDmaQueue_.resize(txDmaQueueNum_, DataBuffer<uint8_t>(txDmaBuffSize_, 0));
    }
    if(useRxDma_)
    {
        rxDmaQueueNum_ = param.rxDmaQueueNum;
        rxDmaBuffSize_ = param.rxDmaBuffSize;
        //调整队列大小，并将新数据类型定义为DataBuffer<uint8_t>(rxDmaBuffSize_, 0)
        rxDmaQueue_.resize(rxDmaQueueNum_, DataBuffer<uint8_t>(rxDmaBuffSize_, 0));
    }

    // 注册串口在IDmap和Handlemap中的实例
    RegisterInterface_();  
    UartHandleMap.insert(std::make_pair(halUartHandle_, this));

    interfaceStatus = APP_OK;

    return APP_OK;

}

/**
 * @brief 启用串口数据传输
 * 
 */
EAppStatus CInfUART::StartTransfer(){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus != APP_OK) return APP_ERROR;

    StopTransfer(); // 停止串口数据传输

    if(useRxDma_)
    {   
        rxDmaQueueIter_ = rxDmaQueue_.begin();
        // 启动DMA接收
        if(HAL_UARTEx_ReceiveToIdle_DMA(halUartHandle_, rxDmaQueueIter_->data(), rxDmaQueueIter_->size()) != HAL_OK)
        {
            return APP_ERROR;
        }
    }
    if(useTxDma_)
    {
        //将队列头和尾指针指向队列的开始
        txDmaQueuePushIter_ = txDmaQueue_.begin();
        txDmaQueuePopIter_ = txDmaQueue_.begin();
    }

    interfaceStatus = APP_BUSY;

    return APP_OK;
}

/**
 * @brief 停止串口数据传输
 * 
 */
EAppStatus CInfUART::StopTransfer(){
    // 检查通信接口是否是重置状态
    if(interfaceStatus == APP_RESET) return APP_ERROR;

    // 停止串口接收
    if(HAL_UART_Abort(halUartHandle_) != HAL_OK)
    {
        return APP_ERROR;
    }

    if(interfaceStatus == APP_BUSY)
    {
        interfaceStatus = APP_OK;
        return APP_OK;
    }

    else
    {
        return APP_ERROR;
    }
}

/**
 * @brief 发送数据
 * @param buffer - 要发送的数据
 * 
 */
EAppStatus CInfUART::Transmit(const DataBuffer<uint8_t> &buffer){
    return Transmit(buffer.data(), buffer.size());
}

/**
 * @brief 发送数据
 * @param buffer - 要发送的数据
 * @param size - 数据大小
 * 
 */
EAppStatus CInfUART::Transmit(const uint8_t *buffer, size_t size){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_RESET) return APP_ERROR; 

    // 检查是否使用DMA
    if(useTxDma_)
    {   
        // 检查数据长度是否大于单个缓冲区以及队列是否已满
        if(size > txDmaBuffSize_
          || std::distance(txDmaQueuePushIter_, txDmaQueuePopIter_) == 1
          || std::distance(txDmaQueuePopIter_, txDmaQueuePushIter_) == static_cast<int32_t>(txDmaQueueNum_) - 1)
        {
            return APP_ERROR;
        }

        // 将数据分配给队列的当前尾部
        txDmaQueuePushIter_->assign(buffer, buffer + size);
        
        // 当队列头和队列尾指针相等时，说明当前队列是唯一的未发送队列，启动DMA发送
        // ps:接收用户层回调函数中会一直进行DMA发送，直到队列为空
        if(std::distance(txDmaQueuePushIter_, txDmaQueuePopIter_) == 0)
        {
            HAL_UART_Transmit_DMA(halUartHandle_, txDmaQueuePopIter_->data(), txDmaQueuePopIter_->size());
        }

        // 将队列的尾部指针指向下一个位置，并检查是否到达队列的末尾将其重置为队列的开始
        if(++txDmaQueuePushIter_ == txDmaQueue_.end())
        {
            txDmaQueuePushIter_ = txDmaQueue_.begin();
        }
    }
    else
    {
        // 启动普通发送
        if(HAL_UART_Transmit(halUartHandle_, const_cast<uint8_t*>(buffer), size, 1000) != HAL_OK)
        {
            return APP_ERROR;
        }
    }
    return APP_OK;
}

/**
 * @brief 格式化发送数据
 * @param format - 格式化字符串
 * 
 */
EAppStatus CInfUART::FormatTransmit(const char *format, ...){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_RESET) return APP_ERROR;

    // 格式化字符串的标准流程
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return Transmit(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
}

/**
 * @brief 接收数据
 * @param buffer - 接收数据的缓冲区
 * 
 */
EAppStatus CInfUART::Receive(DataBuffer<uint8_t> &buffer){
    return Receive(buffer.data(), buffer.size());
}

/**
 * @brief 接收数据
 * @param buffer - 接收数据的缓冲区
 * @param size - 数据大小
 * 
 */
EAppStatus CInfUART::Receive(uint8_t *buffer, size_t size){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_RESET) return APP_ERROR;

    // 检查是否使用DMA
    if(useRxDma_)
    {
        // 启用了DMA接收，直接返回错误,因为DMA接收直接由回调函数处理
        return APP_ERROR;
    }
    else
    {
        // 启动普通接收
        if(HAL_UART_Receive(halUartHandle_, buffer, size, 1000) != HAL_OK)
        {
            return APP_ERROR;
        }
        return APP_OK;
    }
}

/**
 * @brief 注册接收数据处理回调函数
 * @param Callback - 回调函数
 * 
 */
EAppStatus CInfUART::RegisterRxSaveDataCallback(const UART_RxDataProcessCallbackTypeDef &Callback){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_RESET) return APP_ERROR;

    rxDataProcessCallbackList_.push_back(Callback);
    return APP_OK;
}

/**
 * @brief 清空接收数据处理回调函数列表
 * 
 */
EAppStatus CInfUART::ClearCallbackList(){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_RESET) return APP_ERROR;

    rxDataProcessCallbackList_.clear();
    return APP_OK;
}

/**
 * @brief 串口发送完成回调函数
 * @param huart - 串口句柄
 * 
 */
void CInfUART::_UART_HalTxCallback(UART_HandleTypeDef *huart){
    // 检查通信接口是否已完成初始化以及是否使用DMA
    if(interfaceStatus == APP_RESET || !useTxDma_) return;

    // 检查是否使用DMA
    if(useTxDma_)
    {
        // 将当前已发送的队列头指向的缓冲区清空
        // txDmaQueuePopIter_->assign(txDmaBuffSize_, 0);  //这句效率可能没那么高
        std::fill(txDmaQueuePopIter_->begin(), txDmaQueuePopIter_->end(), 0);

        // 将队列头指针指向下一个位置，并检查是否到达队列的末尾将其重置为队列的开始
        if(++txDmaQueuePopIter_ == txDmaQueue_.end())
        {
            txDmaQueuePopIter_ = txDmaQueue_.begin();
        }

        // 检查队列是否为空,若不为空则继续发送
        if(std::distance(txDmaQueuePushIter_, txDmaQueuePopIter_) != 0)
        {
            HAL_UART_Transmit_DMA(halUartHandle_, txDmaQueuePopIter_->data(), txDmaQueuePopIter_->size());
        }

    }
}

/**
 * @brief 串口接收完成回调函数
 * @param huart - 串口句柄
 * 
 */
void CInfUART::_UART_HalRxCallback(UART_HandleTypeDef *huart){
    // 检查通信接口是否已完成初始化以及是否使用DMA
    if(interfaceStatus == APP_RESET || !useRxDma_) return;

    // 检查是否使用DMA
    if(useRxDma_)
    {
        // 获取接收到的数据大小并将buffer指针指向接收到的数据队列
        // size_t size = rxDmaBuffSize_ - HAL_DMA_GetDataCounter(huart->hdmarx);  //这句也可以
        size_t size = huart->RxXferSize - huart->RxXferCount;
        auto &buffer = *rxDmaQueueIter_;

        // 将队列的迭代器指向下一个位置，并检查是否到达队列的末尾将其重置为队列的开始
        if(++rxDmaQueueIter_ == rxDmaQueue_.end())
        {
            rxDmaQueueIter_ = rxDmaQueue_.begin();
        }

        // 启动DMA接收
        HAL_UARTEx_ReceiveToIdle_DMA(halUartHandle_, rxDmaQueueIter_->data(), rxDmaQueueIter_->size());

        // 调用所有回调函数处理接收到的数据
        for(const auto &callback : rxDataProcessCallbackList_)
        {
            callback(buffer, size);
        }

        // 清空当前的这个缓冲区
        std::fill(buffer.begin(), buffer.end(), 0);
    }
}

/**
 * @brief 串口错误回调函数
 * @param huart - 串口句柄
 * 
 */
void CInfUART::_UART_HalErrorCallback(UART_HandleTypeDef *huart){
    
    StopTransfer(); // 停止串口数据传输
    interfaceStatus = APP_ERROR;
}

/**
 * @brief 心跳处理函数
 * 
 */
void CInfUART::HeartbeatHandler_(){
    // TODO: UART Interface Health Check
  if (useRxDma_ && interfaceStatus == APP_ERROR) {
    interfaceStatus = APP_OK;
    StartTransfer();
  }
}

} // namespace my_engineer




extern "C"{

/**
 * @brief 重写串口发送完成回调函数
 * @param huart - 串口句柄
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    // 检查串口句柄是否在HandleMap中
    auto item = my_engineer::UartHandleMap.find(huart);
    if(item != my_engineer::UartHandleMap.end())
    {
        item->second->_UART_HalTxCallback(huart);
    }
}

/**
 * @brief 重写串口接收完成回调函数
 * @param huart - 串口句柄
 * @details
 * 这个函数会在DMA接收数据量达到设定的缓冲区最大值时被调用
 * 场景是接收数据比较多，一次接收的数据量超过了缓冲区的大小
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    // 检查串口句柄是否在HandleMap中
    auto item = my_engineer::UartHandleMap.find(huart);
    if(item != my_engineer::UartHandleMap.end())
    {
        item->second->_UART_HalRxCallback(huart);
    }
}

/**
 * @brief 重写串口接收空闲回调函数
 * @param huart - 串口句柄
 * @param Size - 接收数据的大小，这里用不到，因为要和上一个函数共用一个用户层回调函数
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
    // 检查串口句柄是否在HandleMap中
    auto item = my_engineer::UartHandleMap.find(huart);
    if(item != my_engineer::UartHandleMap.end())
    {
        item->second->_UART_HalRxCallback(huart);
    }
    UNUSED(Size);
}

/**
 * @brief 重写串口错误回调函数
 * @param huart - 串口句柄
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
    // 检查串口句柄是否在HandleMap中
    auto item = my_engineer::UartHandleMap.find(huart);
    if(item != my_engineer::UartHandleMap.end())
    {
        item->second->_UART_HalErrorCallback(huart);
    }
}

} // extern "C"
