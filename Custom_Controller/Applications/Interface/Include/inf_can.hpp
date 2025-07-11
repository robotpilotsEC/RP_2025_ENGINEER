/**
 * @file inf_can.hpp
 * @author Zoe
 * @brief 定义can类
 * @email 2328339747@qq.com
 * @date 2024-10-31
 * 
 * @details
 * 
 */

#ifndef INF_CAN_HPP
#define INF_CAN_HPP

#include "inf_common.hpp"
#include "stm32h7xx_hal_fdcan.h"

namespace my_engineer {

/**
 * @brief Can通信接口类
 * 
 */
class CInfCAN : public CInfBase{
public:

    /**
     * @brief 定义can帧类型枚举类型
     * 
     */
    enum class ECanFrameType{
        DATA = FDCAN_DATA_FRAME, ///< 数据帧,用于传输实际的数据。
        REMOTE = FDCAN_REMOTE_FRAME ///< 远程帧,用于请求另一个节点发送数据。
    };

    /**
     * @brief 定义can帧数据长度代码枚举类型
     * 
     */
    enum class ECanFrameDlc{
        DLC_0 = FDCAN_DLC_BYTES_0, ///< 0字节
        DLC_1 = FDCAN_DLC_BYTES_1, ///< 1字节
        DLC_2 = FDCAN_DLC_BYTES_2, ///< 2字节
        DLC_3 = FDCAN_DLC_BYTES_3, ///< 3字节
        DLC_4 = FDCAN_DLC_BYTES_4, ///< 4字节
        DLC_5 = FDCAN_DLC_BYTES_5, ///< 5字节
        DLC_6 = FDCAN_DLC_BYTES_6, ///< 6字节
        DLC_7 = FDCAN_DLC_BYTES_7, ///< 7字节
        DLC_8 = FDCAN_DLC_BYTES_8, ///< 8字节
        DLC_12 = FDCAN_DLC_BYTES_12, ///< 12字节
        DLC_16 = FDCAN_DLC_BYTES_16, ///< 16字节
        DLC_20 = FDCAN_DLC_BYTES_20, ///< 20字节
        DLC_24 = FDCAN_DLC_BYTES_24, ///< 24字节
        DLC_32 = FDCAN_DLC_BYTES_32, ///< 32字节
        DLC_48 = FDCAN_DLC_BYTES_48, ///< 48字节
        DLC_64 = FDCAN_DLC_BYTES_64, ///< 64字节
    };

    /**
     * @brief 定义一个派生结构体，用于包含can的初始化参数
     * 
     */
    struct SInfInitParam_CAN : public SInfInitParam_Base{
        FDCAN_HandleTypeDef *halCanHandle = nullptr; ///< can句柄   
        FDCAN_FilterTypeDef halCanFilterConfig = {  ///< can过滤器设置
            .IdType = FDCAN_STANDARD_ID,
            .FilterIndex = 0,
            .FilterType = FDCAN_FILTER_MASK,
            .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
            .FilterID1 = 0x0000,
            .FilterID2 = 0x0000,
        };
    };

    /**
     * @brief 定义Can节点类
     * 
     */
    class CCanNode{
    public:
        EInterfaceID  canInterfaceId = EInterfaceID::INF_NULL; ///< can接口ID
        uint32_t stdId = 0x00000000;
        uint32_t timestamp = 0;
        ECanFrameType frameType = ECanFrameType::DATA;
        ECanFrameDlc dataLength = ECanFrameDlc::DLC_8;
        DataBuffer<uint8_t> dataBuffer;
    protected:
        CInfCAN *pInterface_;
    };

    /**
     * @brief 定义Can发送节点
     * 
     */
    class CCanTxNode : public CCanNode{
    public:
        /**
         * @brief 初始化发送节点
         * @param infID - 通信接口ID
         * @param nodestdID - 节点标准ID
         * @param nodeFrameType - 节点帧类型
         * @param nodeFrameDlc - 节点数据长度
         */
        void InitTxNode(EInterfaceID infID, uint32_t nodestdID, ECanFrameType nodeFrameType, ECanFrameDlc nodeFrameDlc);

        /**
         * @brief 发送数据
         * 
         */
        void Transmit();
        void Transmit(const uint8_t *buffer);
        void Transmit(const DataBuffer<uint8_t> &buffer);
    };

    /**
     * @brief 定义Can接收节点
     * 
     */
    class CCanRxNode : public CCanNode{
    public:
        /**
         * @brief 初始化接收节点
         * @param infID - 通信接口ID
         * @param nodestdID - 节点标准ID
         * @param nodeFrameType - 节点帧类型
         * @param nodeFrameDlc - 节点数据长度
         */
        void InitRxNode(EInterfaceID infID, uint32_t nodestdID, ECanFrameType nodeFrameType, ECanFrameDlc nodeFrameDlc);

        /**
         * @brief 接收数据
         * 
         */
        void Receive();
        void Receive(uint8_t *buffer);
        void Receive(DataBuffer<uint8_t> &buffer);
    };

    CInfCAN() {interfaceType = EInfType::INF_CAN;}  
    ~CInfCAN() override;

    // 重写基类的方法
    EAppStatus InitInterface(const SInfInitParam_Base *pStructInitParam) override;
    EAppStatus StartTransfer() override;
    EAppStatus StopTransfer() override;

    // Can特有的方法
    // 发送数据
    EAppStatus Transmit(CCanNode &node);

    // 接收数据
    EAppStatus Receive(CCanNode &node);

    // 将帧数据长度转化为字节长度
    static size_t FrameDlcToByteLength(ECanFrameDlc dlc);

    // 定义can的应用层回调函数
    void _CAN_HalTxCallback(FDCAN_HandleTypeDef *hcan);
    void _CAN_HalRxCallback(FDCAN_HandleTypeDef *hcan);
    void _CAN_HalErrorCallback(FDCAN_HandleTypeDef *hcan);

private:
    FDCAN_HandleTypeDef *halCanHandle_ = nullptr; ///< can句柄

    std::list<CCanTxNode *> txNodeList_; ///< 发送节点列表
    std::list<CCanRxNode *> rxNodeList_; ///< 接收节点列表

    // 重写心跳处理函数
    void HeartbeatHandler_() override;

    // 注册节点
    EAppStatus RegisterNode_(CCanTxNode &node);
    EAppStatus RegisterNode_(CCanRxNode &node);

    // 注销节点
    EAppStatus UnregisterNode_(CCanTxNode &node);
    EAppStatus UnregisterNode_(CCanRxNode &node);
};



} // namespace my_engineer

#endif // INF_CAN_HPP
