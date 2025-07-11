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
extern USBD_HandleTypeDef hUsbDeviceHS;
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;
extern SPI_HandleTypeDef hspi2;

namespace my_engineer {

/**
 * @brief 配置并初始化所有通讯接口
 * @return APP_OK - 初始化成功
 * @return APP_ERROR - 初始化失败
 */
EAppStatus InitAllInterface(){
    
    /* REFEREE (UART10) */
    static CInfUART inf_board_link;
    CInfUART::SInfInitParam_Uart inf_board_link_initparam;
    inf_board_link_initparam.interfaceID = EInterfaceID::INF_UART10;
    inf_board_link_initparam.halUartHandle = &huart10;
    inf_board_link_initparam.useTxDma = true;
    inf_board_link_initparam.txDmaQueueNum = 4;
    inf_board_link_initparam.txDmaBuffSize = 512;
    inf_board_link_initparam.useRxDma = true;
    inf_board_link_initparam.rxDmaQueueNum = 2;
    inf_board_link_initparam.rxDmaBuffSize = 512;
    inf_board_link.InitInterface(&inf_board_link_initparam);

    /* DBUS (UART5) */
    static CInfUART inf_dbus;
    CInfUART::SInfInitParam_Uart inf_dbus_initparam;
    inf_dbus_initparam.interfaceID = EInterfaceID::INF_DBUS;
    inf_dbus_initparam.halUartHandle = &huart5;
    inf_dbus_initparam.useRxDma = true;
    inf_dbus_initparam.rxDmaQueueNum = 2;
    inf_dbus_initparam.rxDmaBuffSize = 64;
    inf_dbus.InitInterface(&inf_dbus_initparam);

    /* BOARD_LINK (UART1) */
    static CInfUART inf_referee;
    CInfUART::SInfInitParam_Uart inf_referee_initparam;
    inf_referee_initparam.interfaceID = EInterfaceID::INF_UART1;
    inf_referee_initparam.halUartHandle = &huart1;
    inf_referee_initparam.useRxDma = true;
    inf_referee_initparam.rxDmaQueueNum = 4;
    inf_referee_initparam.rxDmaBuffSize = 512;
    inf_referee_initparam.useTxDma = true;
    inf_referee_initparam.txDmaQueueNum = 4;
    inf_referee_initparam.txDmaBuffSize = 512;
    inf_referee.InitInterface(&inf_referee_initparam);

    /* Custom Controller (UART7) */
    static CInfUART inf_vision;
    CInfUART::SInfInitParam_Uart inf_vision_initparam;
    inf_vision_initparam.interfaceID = EInterfaceID::INF_UART7;
    inf_vision_initparam.halUartHandle = &huart7;
    inf_vision_initparam.useRxDma = true;
    inf_vision_initparam.rxDmaQueueNum = 4;
    inf_vision_initparam.rxDmaBuffSize = 512;
    inf_vision_initparam.useTxDma = true;
    inf_vision_initparam.txDmaQueueNum = 2;
    inf_vision_initparam.txDmaBuffSize = 512;
    inf_vision.InitInterface(&inf_vision_initparam);

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
