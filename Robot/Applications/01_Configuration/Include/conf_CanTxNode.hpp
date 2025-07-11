/**
 * @file conf_CanTxNode.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 
 * @version 1.0
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CONF_CANTXNODE_HPP
#define CONF_CANTXNODE_HPP

#include "Interface.hpp"
#include "conf_common.hpp"

namespace my_engineer {

extern CInfCAN::CCanTxNode TxNode_Can1_200;
extern CInfCAN::CCanTxNode TxNode_Can1_1FF;
extern CInfCAN::CCanTxNode TxNode_Can1_3FE;
extern CInfCAN::CCanTxNode TxNode_Can2_200;
extern CInfCAN::CCanTxNode TxNode_Can2_1FF;
extern CInfCAN::CCanTxNode TxNode_Can2_3FE;
extern CInfCAN::CCanTxNode TxNode_Can3_200;
extern CInfCAN::CCanTxNode TxNode_Can3_1FF;
extern CInfCAN::CCanTxNode TxNode_Can3_3FE;
extern CInfCAN::CCanTxNode TxNode_Can3_7FF;

EAppStatus InitAllCanTxNode();

}


#endif // CONF_CANTXNODE_HPP
