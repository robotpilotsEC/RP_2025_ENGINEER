/**
 * @file inf_common.hpp
 * @author Zoe
 * @brief 定义通信接口的基类
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 定义通信接口的基类
 * 定义通信接口的心跳、注册、注销、初始化、发送、传输启动和关闭方法。 
 * 定义一个包含通信接口类型的枚举类型。
 */

#ifndef INF_COMMON_HPP
#define INF_COMMON_HPP

#include "Configuration.hpp"

namespace my_engineer {

class CInfBase{
    //定义一个友元函数，用于启动所有通信接口的心跳任务
    friend void StartHeartbeatTask(void *argument);

protected:
    /**
     * @brief 定义一个用于包含通信接口初始化参数的结构体基类
     * 
     */
    struct SInfInitParam_Base{
        EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 通信接口ID
    };

    /**
     * @brief 通信接口注册
     * 
     */
    EAppStatus RegisterInterface_();

    /**
     * @brief 通信接口注销
     * 
     */
    EAppStatus UnregisterInterface_();

    /**
     * @brief 通信接口心跳处理
     * 
     */
    virtual void HeartbeatHandler_() { }

public:
    /**
     * @brief 定义一个包含通信接口类型的枚举类型。
     * 并定义一个记录通信接口的类型的变量。
     * 
     */
    enum class EInfType{
        INF_UNDEF = -1, ///< 未定义
        INF_CAN,        ///< CAN通信接口
        INF_UART,       ///< UART通信接口   
        INF_SPI,        ///< SPI通信接口
        INF_I2C,        ///< I2C通信接口
        INF_USB_CDC,    ///< USB CDC通信接口
    } interfaceType = EInfType::INF_UNDEF;    ///< 通信接口类型

    EInterfaceID interfaceID = EInterfaceID::INF_NULL; ///< 通信接口ID

    EAppStatus interfaceStatus = APP_RESET; ///< 通信接口状态

    virtual ~CInfBase() {UnregisterInterface_();} ///< 析构函数

    /**
     * @brief 初始化通信接口
     * @param pStructInitParam - 参数结构体指针
     * 
     * @note 纯虚函数，必须由派生类实现
     */
    virtual EAppStatus InitInterface(const SInfInitParam_Base *pStructInitParam) = 0;

    /**
     * @brief 启动数据传输
     * 
     */
    virtual EAppStatus StartTransfer() { return APP_ERROR; }

    /**
     * @brief 关闭数据传输
     * 
     */
    virtual EAppStatus StopTransfer() { return APP_ERROR; }

}; // end of class CInfBase

extern std::map<EInterfaceID, CInfBase *> InterfaceIDMap; ///< 通信接口实例映射表 

} // end of namespace my_engineer

#endif // INF_COMMON_HPP
