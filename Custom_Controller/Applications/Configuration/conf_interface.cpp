/**
 * @file conf_interface.cpp
 * @author Zoe
 * @brief 完成通讯接口的配置
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 * 导入hal中的通讯接口句柄，并调用Interface中定义的类方法，完成通讯接口的配置。
 */

#include "conf_interface.hpp"
#include "Interface.hpp"
#include "conf_CanTxNode.hpp"

// 导入串口句柄
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart10;
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;
extern SPI_HandleTypeDef hspi2;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern USBD_HandleTypeDef hUsbDeviceHS;

namespace my_engineer {

/**
 * @brief 配置并初始化所有通讯接口
 * @return APP_OK - 初始化成功
 * @return APP_ERROR - 初始化失败
 */
EAppStatus InitAllInterface(){

    /* ROCKER (ADC1) */
    static CInfADC inf_rocker;
    CInfADC::SInfInitParam_ADC inf_rocker_initparam;
    inf_rocker_initparam.interfaceID = EInterfaceID::INF_ADC1;
    inf_rocker_initparam.halAdcHandle = &hadc1;
    inf_rocker_initparam.useDma = true;
    inf_rocker_initparam.useTimer = true;
    inf_rocker_initparam.halTimerHandle = &htim6;
    inf_rocker_initparam.channelNum = 3;
    inf_rocker_initparam.channelList = {CInfADC::EAdcChannel::CHANNEL_4, \
                                        CInfADC::EAdcChannel::CHANNEL_14, \
                                        CInfADC::EAdcChannel::CHANNEL_16};
    inf_rocker.InitInterface(&inf_rocker_initparam);
    
    /* CONTROLLER_BOARDLINK (UART7) */
    static CInfUART inf_laser_right;
    CInfUART::SInfInitParam_Uart inf_laser_right_initparam;
    inf_laser_right_initparam.interfaceID = EInterfaceID::INF_UART7;
    inf_laser_right_initparam.halUartHandle = &huart7;
    inf_laser_right_initparam.useRxDma = true;
    inf_laser_right_initparam.rxDmaQueueNum = 4;
    inf_laser_right_initparam.rxDmaBuffSize = 512;
    inf_laser_right_initparam.useTxDma = true;
    inf_laser_right_initparam.txDmaQueueNum = 2;
    inf_laser_right_initparam.txDmaBuffSize = 512;
    inf_laser_right.InitInterface(&inf_laser_right_initparam);

    /* USB CDC (USB) */
    static CInfUSB_CDC inf_usb_cdc;
    CInfUSB_CDC::SInfInitParam_USB_CDC inf_usb_cdc_initparam;
    inf_usb_cdc_initparam.interfaceID = EInterfaceID::INF_USB_CDC;
    inf_usb_cdc_initparam.halUsbHandle = &hUsbDeviceHS;
    inf_usb_cdc.InitInterface(&inf_usb_cdc_initparam);

    /* CAN1 */
    static CInfCAN inf_can1;
    CInfCAN::SInfInitParam_CAN inf_can1_initparam;
    inf_can1_initparam.interfaceID = EInterfaceID::INF_CAN1;
    inf_can1_initparam.halCanHandle = &hfdcan1;
    inf_can1.InitInterface(&inf_can1_initparam);

    /* CAN2 */
    static CInfCAN inf_can2;
    CInfCAN::SInfInitParam_CAN inf_can2_initparam;
    inf_can2_initparam.interfaceID = EInterfaceID::INF_CAN2;
    inf_can2_initparam.halCanHandle = &hfdcan2;
    inf_can2.InitInterface(&inf_can2_initparam);

    /* CAN3 */
    static CInfCAN inf_can3;
    CInfCAN::SInfInitParam_CAN inf_can3_initparam;
    inf_can3_initparam.interfaceID = EInterfaceID::INF_CAN3;
    inf_can3_initparam.halCanHandle = &hfdcan3;
    inf_can3.InitInterface(&inf_can3_initparam);

    /* SPI2 */
    static CInfSPI inf_spi2;
    CInfSPI::SInfInitParam_SPI inf_spi2_initparam;
    inf_spi2_initparam.interfaceID = EInterfaceID::INF_SPI2;
    inf_spi2_initparam.halSpiHandle = &hspi2;
    inf_spi2.InitInterface(&inf_spi2_initparam);

    // 初始化所有can发送节点
    InitAllCanTxNode();

    return APP_OK;
}

} // namespace my_engineer
