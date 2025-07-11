/**
 * @file inf_uart.cpp
 * @author Zoe
 * @brief 完成can类的定义和实现
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 定义can的map容器，用于存储所有can的实例。
 * 定义can的初始化、启动、停止、发送、接收、心跳、各种回调函数。
 */

#include "inf_can.hpp"

namespace my_engineer {

std::map<FDCAN_HandleTypeDef*, CInfCAN*> CanHandleMap; ///< can实例map容器

CInfCAN::~CInfCAN(){
    // 注销can在IDmap和Handlemap中的实例
    UnregisterInterface_();

    if(interfaceID != EInterfaceID::INF_NULL)
    {
        CanHandleMap.erase(halCanHandle_);
    }
}

/**
 * @brief 初始化can
 * @param pStructInitParam - 参数结构体指针
 * 
 */
EAppStatus CInfCAN::InitInterface(const SInfInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查通信接口是否已完成初始化
    if(interfaceStatus == APP_BUSY) return APP_ERROR;

    // 强制类型转换
    auto &param = *static_cast<const SInfInitParam_CAN*>(pStructInitParam);
    interfaceID = param.interfaceID;
    halCanHandle_ = param.halCanHandle;

    // HAL_FDCAN_Abort(halCanHandle_); // 中断can

    // 定义can过滤器
    HAL_FDCAN_ConfigFilter(halCanHandle_,
                        const_cast<FDCAN_FilterTypeDef*>(&param.halCanFilterConfig));
    HAL_FDCAN_ConfigGlobalFilter(halCanHandle_,
                                 FDCAN_REJECT, FDCAN_REJECT, 
                                 FDCAN_REJECT_REMOTE, FDCAN_FILTER_REJECT);
    HAL_FDCAN_ConfigFifoWatermark(halCanHandle_, FDCAN_RX_FIFO0, 1);
    HAL_FDCAN_ActivateNotification(halCanHandle_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    // 注册can在IDmap和Handlemap中的实例
    RegisterInterface_();
    CanHandleMap.insert(std::make_pair(halCanHandle_, this));

    interfaceStatus = APP_OK;

    return APP_OK;

} // namespace my_engineer

/**
 * @brief 启动数据传输
 * 
 */
EAppStatus CInfCAN::StartTransfer(){
    // 检查通信接口是否已完成初始化
    if(interfaceStatus != APP_OK) return APP_ERROR;

    // 启动can接收
    if(HAL_FDCAN_Start(halCanHandle_) != HAL_OK)
    {
        return APP_ERROR;
    }

    interfaceStatus = APP_BUSY;

    return APP_OK;
}

/**
 * @brief 停止数据传输
 * 
 */
EAppStatus CInfCAN::StopTransfer(){
    // 检查can是否为重置状态
    if(interfaceStatus == APP_RESET) return APP_ERROR;

    // 停止can接收
    if(HAL_FDCAN_Stop(halCanHandle_) != HAL_OK)
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
 * @param node - 要发送的节点
 * 
 */
EAppStatus CInfCAN::Transmit(CCanNode &node){
    // 检查can是否为工作状态
    if(interfaceStatus != APP_BUSY) return APP_ERROR;

    // 定义发送帧的头
    FDCAN_TxHeaderTypeDef txHeader = {
      .Identifier          = node.stdId,
      .IdType              = FDCAN_STANDARD_ID,
      .TxFrameType         = static_cast<uint32_t>(node.frameType),
      .DataLength          = static_cast<uint32_t>(node.dataLength),
      .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
      .BitRateSwitch       = FDCAN_BRS_OFF,
      .FDFormat            = FDCAN_CLASSIC_CAN,
      .TxEventFifoControl  = FDCAN_NO_TX_EVENTS,
      .MessageMarker       = 0,
    };


    //清理D-Cache(还不清楚为什么要这样做)
    #ifdef rp_USE_DCACHE
        SCB_CleanDCache();
    #endif

    // 发送数据
    if(HAL_FDCAN_AddMessageToTxFifoQ(halCanHandle_, &txHeader, node.dataBuffer.data()) != HAL_OK)
    {
        return APP_ERROR;
    }

    return APP_OK;
}

/**
 * @brief 接收数据
 * @param node - 要接收的节点
 * 
 * @note 还不理解为什么这里边要写东西，明明在回调函数里边已经写完接收数据的操作了
 */
EAppStatus CInfCAN::Receive(CCanNode &node){
    // 检查can是否为工作状态
    if(interfaceStatus != APP_BUSY) return APP_ERROR;                                    

    // 接收数据
    uint8_t data[64];
    FDCAN_RxHeaderTypeDef RxHeader;
    if (HAL_FDCAN_GetRxMessage(halCanHandle_, FDCAN_RX_FIFO0, &RxHeader, data) != HAL_OK)
    {
        return APP_ERROR;
    }

    // 将接收到的数据存入节点
    node.stdId = RxHeader.Identifier;
    node.frameType = static_cast<ECanFrameType>(RxHeader.RxFrameType);
    node.dataLength = static_cast<ECanFrameDlc>(RxHeader.DataLength);
    node.timestamp = HAL_GetTick();
    node.dataBuffer.resize(FrameDlcToByteLength(node.dataLength), 0);
    std::copy(data, data + node.dataBuffer.size(), node.dataBuffer.begin());

    //清理D-Cache(还不清楚为什么要这样做)
    #ifdef rp_USE_DCACHE
        SCB_CleanDCache();
    #endif

    return APP_OK;
}

/**
 * @brief 将帧数据长度转化为字节长度
 * @param dlc - 帧数据长度
 * 
 */
size_t CInfCAN::FrameDlcToByteLength(ECanFrameDlc dlc){
    switch (dlc){
    case ECanFrameDlc::DLC_0:
        return 0;
    case ECanFrameDlc::DLC_1:
        return 1;
    case ECanFrameDlc::DLC_2:
        return 2;
    case ECanFrameDlc::DLC_3:
        return 3;
    case ECanFrameDlc::DLC_4:
        return 4;
    case ECanFrameDlc::DLC_5:
        return 5;
    case ECanFrameDlc::DLC_6:
        return 6;
    case ECanFrameDlc::DLC_7:
        return 7;
    case ECanFrameDlc::DLC_8:
        return 8;
    case ECanFrameDlc::DLC_12:
        return 12;
    case ECanFrameDlc::DLC_16:
        return 16;
    case ECanFrameDlc::DLC_20:
        return 20;
    case ECanFrameDlc::DLC_24:
        return 24;
    case ECanFrameDlc::DLC_32:
        return 32;
    case ECanFrameDlc::DLC_48:
        return 48;
    case ECanFrameDlc::DLC_64:
        return 64;
    default:
        return 0;
    }
}

/**
 * @brief 心跳处理函数
 * 
 */
void CInfCAN::HeartbeatHandler_(){
    //TODO
}


/**
 * @brief 初始化发送节点
 * 
 */
void CInfCAN::CCanTxNode::InitTxNode(EInterfaceID canInfId, 
                            uint32_t nodestdID, 
                            ECanFrameType nodeFrameType, 
                            ECanFrameDlc nodeFrameDlc){
    pInterface_ = static_cast<CInfCAN *>(InterfaceIDMap.at(canInfId));
    stdId = nodestdID;
    frameType = nodeFrameType;
    dataLength = nodeFrameDlc;
    dataBuffer.resize(FrameDlcToByteLength(nodeFrameDlc), 0);

    pInterface_->RegisterNode_(*this);
}

/**
 * @brief 初始化接收节点
 * 
 */
void CInfCAN::CCanRxNode::InitRxNode(EInterfaceID canInfId, 
                            uint32_t nodestdID, 
                            ECanFrameType nodeFrameType, 
                            ECanFrameDlc nodeFrameDlc){
    pInterface_ = static_cast<CInfCAN *>(InterfaceIDMap.at(canInfId));
    stdId = nodestdID;
    frameType = nodeFrameType;
    dataLength = nodeFrameDlc;
    dataBuffer.resize(FrameDlcToByteLength(nodeFrameDlc), 0);

    pInterface_->RegisterNode_(*this);
}

/**
 * @brief 节点发送数据
 * 
 */
void CInfCAN::CCanTxNode::Transmit(){
    pInterface_->Transmit(*this);
}

void CInfCAN::CCanTxNode::Transmit(const uint8_t *buffer){
    std::copy(buffer, buffer + dataBuffer.size(), dataBuffer.begin());
    Transmit();
}

void CInfCAN::CCanTxNode::Transmit(const DataBuffer<uint8_t> &buffer){
    std::copy(buffer.begin(), buffer.end(), dataBuffer.begin());
    Transmit();
}

/**
 * @brief 节点接收数据
 * @note 接收节点其实就是自己本身，所以在回调函数里边接收数据就行
 */
void CInfCAN::CCanRxNode::Receive(){}
void CInfCAN::CCanRxNode::Receive(uint8_t *buffer){}
void CInfCAN::CCanRxNode::Receive(DataBuffer<uint8_t> &buffer){}

/**
 * @brief 注册发送节点
 * 
 */
EAppStatus CInfCAN::RegisterNode_(CCanTxNode &node){
    // 检查节点是否已经注册
    if (std::find(txNodeList_.begin(), txNodeList_.end(), &node) == txNodeList_.end()) {
        txNodeList_.push_back(&node);
        return APP_OK;
    }

  return APP_ERROR;
}

/**
 * @brief 注册接收节点
 * 
 */
EAppStatus CInfCAN::RegisterNode_(CCanRxNode &node){
    // 检查节点是否已经注册
    if (std::find(rxNodeList_.begin(), rxNodeList_.end(), &node) == rxNodeList_.end()) {
        rxNodeList_.push_back(&node);
        return APP_OK;
    }

  return APP_ERROR;
}

/**
 * @brief 注销发送节点
 * 
 */
EAppStatus CInfCAN::UnregisterNode_(CCanTxNode &node){
    // 检查节点是否已经注册
    auto iter = std::find(txNodeList_.begin(), txNodeList_.end(), &node);
    if (iter != txNodeList_.end()) {
        txNodeList_.erase(iter);
        return APP_OK;
    }

  return APP_ERROR;
}

/**
 * @brief 注销接收节点
 * 
 */
EAppStatus CInfCAN::UnregisterNode_(CCanRxNode &node){
    // 检查节点是否已经注册
    auto iter = std::find(rxNodeList_.begin(), rxNodeList_.end(), &node);
    if (iter != rxNodeList_.end()) {
        rxNodeList_.erase(iter);
        return APP_OK;
    }

  return APP_ERROR;
}

/**
 * @brief 重写can发送完成回调函数
 * @param hcan - can句柄
 * @note 没什么好写,can发送调用类里面的函数就可以了
 */ 
void CInfCAN::_CAN_HalTxCallback(FDCAN_HandleTypeDef *hcan){
    //TODO
}

/**
 * @brief 重写can接收完成回调函数
 * @param hcan - can句柄
 */
void CInfCAN::_CAN_HalRxCallback(FDCAN_HandleTypeDef *hcan){
    // 接收数据
    uint8_t data[64];
    FDCAN_RxHeaderTypeDef RxHeader;
    HAL_FDCAN_GetRxMessage(halCanHandle_, FDCAN_RX_FIFO0, &RxHeader, data);

    // 将接收到的数据存入节点
    for(auto &rxNode : rxNodeList_)
    {
        if(rxNode->stdId == RxHeader.Identifier)
        {
            rxNode->timestamp = HAL_GetTick();
            std::copy(data, data + rxNode->dataBuffer.size(), rxNode->dataBuffer.begin());
        }
    }

    //清理D-Cache(还不清楚为什么要这样做)
    #ifdef rp_USE_DCACHE
        SCB_CleanDCache();
    #endif
}

/**
 * @brief 重写can错误回调函数
 * @param hcan - can句柄
 */
void CInfCAN::_CAN_HalErrorCallback(FDCAN_HandleTypeDef *hcan){
    //TODO
    // interfaceStatus = APP_ERROR;
}
    
} // namespace my_engineer

extern "C"{
/**
 * @brief 重写can的fifo0接收完成回调函数
 * 
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs){
    if(RxFifo0ITs == FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
    {
        // 检查can句柄是否在HandleMap中
        auto item = my_engineer::CanHandleMap.find(hcan);
        if(item != my_engineer::CanHandleMap.end())
        {
            item->second->_CAN_HalRxCallback(hcan);
        }
    }
} 

/**
 * @brief 重写can的fifo1接收完成回调函数
 * @note 额，有用到fifo1吗
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo1ITs){
    if(RxFifo1ITs == FDCAN_IT_RX_FIFO1_NEW_MESSAGE)
    {
        // 检查can句柄是否在HandleMap中
        auto item = my_engineer::CanHandleMap.find(hcan);
        if(item != my_engineer::CanHandleMap.end())
        {
            item->second->_CAN_HalRxCallback(hcan);
        }
    }
} 

/**
 * @brief 重写can的错误回调函数
 * 
 */
void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hcan){
    // 检查can句柄是否在HandleMap中
    auto item = my_engineer::CanHandleMap.find(hcan);
    if(item != my_engineer::CanHandleMap.end())
    {
        item->second->_CAN_HalErrorCallback(hcan);
    }
}
    
}   // extern "C"
