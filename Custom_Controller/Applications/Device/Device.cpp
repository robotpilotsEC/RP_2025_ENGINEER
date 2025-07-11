/**
 * @file Device.cpp
 * @author Zoe
 * @brief 设备层的汇总源文件
 * @email 2328339747@qq.com
 * @date 2024-11-2
 * 
 * @details
 * 定义设备的map容器，用于存储所有设备的实例。
 * 在这里重写注册和注销方法(因为容器被定义在这里)
 */

#include "Device.hpp"

namespace my_engineer
{

// 设备map容器
std::map<EDeviceID, CDevBase *> DeviceIDMap;

/**
 * @brief 注册设备
 * 
 * @return EAppStatus 
 */
EAppStatus CDevBase::RegisterDevice_()
{
    if (deviceID != EDeviceID::DEV_NULL)
    {
        DeviceIDMap.insert(std::make_pair(deviceID, this));
        return APP_OK;
    }
    return APP_ERROR;
}

/**
 * @brief 注销设备
 * 
 * @return EAppStatus 
 */ 
EAppStatus CDevBase::UnregisterDevice_()
{
    if (deviceID != EDeviceID::DEV_NULL)
    {
        DeviceIDMap.erase(deviceID);
        return APP_OK;
    }
    return APP_ERROR;
}

} // namespace my_engineer 
