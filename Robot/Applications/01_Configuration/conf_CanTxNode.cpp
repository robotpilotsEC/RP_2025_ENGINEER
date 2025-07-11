/**
 * @file conf_CanTxNode.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义can的发送节点
 * @version 1.0
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "conf_CanTxNode.hpp"

namespace my_engineer {

CInfCAN::CCanTxNode TxNode_Can1_200;
CInfCAN::CCanTxNode TxNode_Can1_1FF;
CInfCAN::CCanTxNode TxNode_Can1_3FE;
CInfCAN::CCanTxNode TxNode_Can2_200;
CInfCAN::CCanTxNode TxNode_Can2_1FF;
CInfCAN::CCanTxNode TxNode_Can2_3FE;
CInfCAN::CCanTxNode TxNode_Can3_200;
CInfCAN::CCanTxNode TxNode_Can3_1FF;
CInfCAN::CCanTxNode TxNode_Can3_3FE;
CInfCAN::CCanTxNode TxNode_Can3_7FF;

EAppStatus InitAllCanTxNode()
{
    TxNode_Can1_200.InitTxNode(EInterfaceID::INF_CAN1, 0x200, 
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can1_1FF.InitTxNode(EInterfaceID::INF_CAN1, 0x1FF,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can1_3FE.InitTxNode(EInterfaceID::INF_CAN1, 0x3FE,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can2_200.InitTxNode(EInterfaceID::INF_CAN2, 0x200,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can2_1FF.InitTxNode(EInterfaceID::INF_CAN2, 0x1FF,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can2_3FE.InitTxNode(EInterfaceID::INF_CAN2, 0x3FE,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can3_200.InitTxNode(EInterfaceID::INF_CAN3, 0x200,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can3_1FF.InitTxNode(EInterfaceID::INF_CAN3, 0x1FF,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);

    TxNode_Can3_3FE.InitTxNode(EInterfaceID::INF_CAN3, 0x3FE,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);
    
    TxNode_Can3_7FF.InitTxNode(EInterfaceID::INF_CAN3, 0x7FF,
                                CInfCAN::ECanFrameType::DATA, CInfCAN::ECanFrameDlc::DLC_8);
	return APP_OK;
}

} // namespace my_engineer
