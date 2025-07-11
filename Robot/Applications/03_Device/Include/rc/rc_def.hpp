/**
 * @file rc_def.hpp
 * @author Zoe
 * @brief 定义各种枚举类型
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * @details
 */

#ifndef RC_DEF_HPP
#define RC_DEF_HPP

namespace my_engineer {

/**
 * @brief 定义遥控器产品类型的枚举类型
 * 
 */
enum class ERcType{
    RC_UNDEF = -1, ///< 未定义
    RC_DR16,       ///< 大疆DR16遥控器
};

/**
 * @brief 定义遥控器状态的枚举类型
 * 
 */
enum class ERcStatus{
    RESET = -1, ///< 重置
    OFFLINE,    ///< 离线
    ONLINE,     ///< 在线
};

/**
 * @brief 定义遥控器通道的枚举类型
 * 
 */
enum class ERcChannelType{
    UNDEF = -1, ///< 未定义
    SWITCH,     ///< 开关类型
    BUTTON,     ///< 按键类型
    LEVER,      ///< 摇杆类型
};

/**
 * @brief 定义表示通道状态的枚举类型
 * 
 */
enum class ERcChannelStatus{
    RESET = -1, ///< 重置
    DOWN,       ///< 低状态
    HIGH,         ///< 高状态
    PRESS,      ///< 按下状态
};

} // namespace my_engineer

#endif // RC_DEF_HPP
