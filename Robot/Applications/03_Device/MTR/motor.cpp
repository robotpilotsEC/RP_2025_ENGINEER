/**
 * @file motor.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义电机设备层的汇总源文件
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 * @details
 * 定义map容器，存储电机设备类的实例
 * 定义在容器中注册和注销电机设备的方法
 */

#include "mtr/motor.hpp"

namespace my_engineer {

std::map<EDeviceID, CDevMtr *> MotorIDMap; ///< 电机设备类map容器

EAppStatus CDevMtr::RegisterMotor_(){
    // 判断设备ID是否为NULL
    if(deviceID != EDeviceID::DEV_NULL){
        MotorIDMap.insert(std::make_pair(deviceID, this));
        return APP_OK;
    }

    return APP_ERROR;
}

EAppStatus CDevMtr::UnregisterMotor_(){
    
    // 注销设备
    UnregisterDevice_();

    // 从map容器中删除设备
    if(deviceID != EDeviceID::DEV_NULL){
        MotorIDMap.erase(deviceID);
        return APP_OK;
    }

    return APP_ERROR;
}

} // namespace my_engineer
