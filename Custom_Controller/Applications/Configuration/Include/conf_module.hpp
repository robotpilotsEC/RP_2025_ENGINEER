/**
 * @file conf_module.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CONF_MODULE_HPP
#define CONF_MODULE_HPP

#include "conf_common.hpp"

namespace my_engineer {

/**
 * @brief 配置并初始化所有模块
 * @return APP_OK - 初始化成功
 * @return APP_ERROR - 初始化失败
 */
EAppStatus InitAllModule();

} // namespace my_engineer

#endif // CONF_MODULE_HPP
