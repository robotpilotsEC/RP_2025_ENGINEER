/**
 * @file Interface.cpp
 * @author Zoe
 * @brief 通信接口层的汇总源文件
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 定义通信接口的map容器，用于存储所有通信接口的实例。
 * 在这里重写注册和注销方法(因为容器被定义在这里)
 */

#include "Interface.hpp"

namespace my_engineer {
std::map<EInterfaceID, CInfBase*> InterfaceIDMap; ///< 通信接口实例map容器

/**
 * @brief 通信接口注册
 * 
 */
EAppStatus CInfBase::RegisterInterface_(){
    if (interfaceID != EInterfaceID::INF_NULL){
        InterfaceIDMap.insert(std::make_pair(interfaceID, this));
        return APP_OK;
    }

    return APP_ERROR;
}

/**
 * @brief 通信接口注销
 * 
 */
EAppStatus CInfBase::UnregisterInterface_(){
    if (interfaceID != EInterfaceID::INF_NULL){
        InterfaceIDMap.erase(interfaceID);
        return APP_OK;
    }
    return APP_ERROR;
}

}
