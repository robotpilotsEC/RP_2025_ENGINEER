/**
 * @file conf_interface.hpp
 * @author Zoe
 * @brief 没啥说的
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 */

#ifndef CONF_INTERFACE_HPP
#define CONF_INTERFACE_HPP  

#include "conf_common.hpp"

namespace my_engineer {

/**
 * @brief 配置并初始化所有通讯接口
 * @return APP_OK - 初始化成功
 * @return APP_ERROR - 初始化失败
 */
EAppStatus InitAllInterface();

} // namespace my_engineer

#endif // CONF_INTERFACE_HPP
