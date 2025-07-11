/**
 * @file inf_spi.cpp
 * @author Zoe
 * @brief 完成串口spi类的定义和实现
 * @email 2328339747@qq.com
 * @date 2024-10-31
 * 
 * @details
 * 定义spi的map容器，用于存储所有spi的实例。
 * (本来是没必要的，因为只有一个spi，但是为了和风格统一，还是加上了)
 * 定义spi的初始化、发送、接收、心跳函数。
 */

#include "inf_spi.hpp"

namespace my_engineer {

std::map<SPI_HandleTypeDef*, CInfSPI*> SpiHandleMap; ///< spi实例map容器

CInfSPI::~CInfSPI(){
    // 注销spi在IDmap和Handlemap中的实例
    UnregisterInterface_();

    if(interfaceID != EInterfaceID::INF_NULL)
    {
        SpiHandleMap.erase(halSpiHandle_);
    }
}

/**
 * @brief 初始化spi
 * @param pStructInitParam - 参数结构体指针
 * 
 */
EAppStatus CInfSPI::InitInterface(const SInfInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_BUSY) return APP_ERROR;

    // 强制类型转换
    auto &param = *static_cast<const SInfInitParam_SPI*>(pStructInitParam);
    interfaceID = param.interfaceID;
    halSpiHandle_ = param.halSpiHandle;

    // HAL_SPI_Abort(halSpiHandle_); // 中断spi

    // 注册spi在IDmap和Handlemap中的实例
    RegisterInterface_();
    SpiHandleMap.insert(std::make_pair(halSpiHandle_, this));

    interfaceStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 发送数据
 * @param buffer - 要发送的数据
 * 
 */
EAppStatus CInfSPI::Transmit(const DataBuffer<uint8_t> &buffer){
    return Transmit(buffer.data(), buffer.size());
}

/**
 * @brief 发送数据
 * @param buffer - 要发送的数据
 * @param size - 数据大小
 * 
 */
EAppStatus CInfSPI::Transmit(const uint8_t *buffer, size_t size){
    // 检查spi是否为可发送数据状态
    if(interfaceStatus != APP_OK) return APP_ERROR;

    interfaceStatus = APP_BUSY;
    // 启动spi发送
    if(HAL_SPI_Transmit(halSpiHandle_, const_cast<uint8_t*>(buffer), size, 10) != HAL_OK)
    {
        return APP_ERROR;
    }
    interfaceStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 接收数据
 * @param buffer - 接收数据的缓冲区
 * 
 */
EAppStatus CInfSPI::Receive(DataBuffer<uint8_t> &buffer){
    return Receive(buffer.data(), buffer.size());
}

/**
 * @brief 接收数据
 * @param buffer - 接收数据的缓冲区
 * @param size - 数据大小
 * 
 */
EAppStatus CInfSPI::Receive(uint8_t *buffer, size_t size){
    // 检查spi是否为可接收数据状态
    if(interfaceStatus != APP_OK) return APP_ERROR;

    interfaceStatus = APP_BUSY;
    // 启动spi接收
    if(HAL_SPI_Receive(halSpiHandle_, buffer, size, 10) != HAL_OK)
    {
        return APP_ERROR;
    }
    interfaceStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 同时发送和接收数据
 * @param txBuffer - 要发送的数据
 * @param rxBuffer - 接收数据的缓冲区
 * 
 */
EAppStatus CInfSPI::TransmitReceive(const DataBuffer<uint8_t> &txBuffer, 
                                    DataBuffer<uint8_t> &rxBuffer) {
    return TransmitReceive(txBuffer.data(), rxBuffer.data(), txBuffer.size());
}

/**
 * @brief 同时发送和接收数据
 * @param txBuffer - 要发送的数据
 * @param rxBuffer - 接收数据的缓冲区
 * @param size - 数据大小
 * 
 */
EAppStatus CInfSPI::TransmitReceive(const uint8_t *txBuffer, 
                                    uint8_t *rxBuffer, size_t size){
    // 检查spi是否为可发送数据状态
    if(interfaceStatus != APP_OK) return APP_ERROR;

    interfaceStatus = APP_BUSY;
    // 启动spi发送和接收
    if(HAL_SPI_TransmitReceive(halSpiHandle_, const_cast<uint8_t*>(txBuffer), rxBuffer, size, 10) != HAL_OK)
    {
        return APP_ERROR;
    }
    interfaceStatus = APP_OK;

    return APP_OK;
}


/**
 * @brief 心跳处理函数
 * 
 */
void CInfSPI::HeartbeatHandler_(){
    //TODO
}

} // namespace my_engineer

extern "C"{ //全部中断回调都用不到，所以都是空函数
/**
 * @brief
 * @param hspi
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {

  UNUSED(hspi);
}


/**
 * @brief
 * @param hspi
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {

  UNUSED(hspi);
}


/**
 * @brief
 * @param hspi
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {

  UNUSED(hspi);
}


/**
 * @brief
 * @param hspi
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {

  UNUSED(hspi);
}

}   // extern "C"
