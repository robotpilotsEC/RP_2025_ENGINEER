/**
 * @file conf_device.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 没啥说的
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CONF_DEVICE_HPP
#define CONF_DEVICE_HPP

#include "conf_common.hpp"

namespace my_engineer {

/**
 * @brief 配置并初始化所有设备
 * @return APP_OK - 初始化成功
 * @return APP_ERROR - 初始化失败
 */
EAppStatus InitAllDevice();

} // namespace my_engineer

#endif // CONF_DEVICE_HPP
