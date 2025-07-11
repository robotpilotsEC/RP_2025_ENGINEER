/**
 * @file conf_common.hpp
 * @author Zoe
 * @brief 定义一些经常用到的枚举类型，以及一些模版。
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 */

#ifndef CONF_COMMON_HPP
#define CONF_COMMON_HPP

#include "main.h"
#include "arm_math.h"

#include <ctime>
#include <cstdio>
#include <cstdarg>

#include <functional>
#include <vector>
#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>

#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "task.h"

namespace my_engineer {

/**
 * @brief Application Status枚举类型
 * @note 用作Application中函数的返回值
 */
enum EAppStatus{
    APP_RESET = 0, ///< 重置
    APP_OK = 1,     ///< 正常
    APP_ERROR,      ///< 错误
    APP_BUSY,       ///< 忙
    APP_TIMEOUT,    ///< 超时
    APP_FULL,       ///< 满
    APP_EMPTY,      ///< 空
    APP_INVALID,    ///< 无效
    APP_UNKNOWN     ///< 未知
};

enum FSM_Flag{
    FSM_RESET = 0,
    FSM_PREINIT = 1,
    FSM_INIT,
    FSM_CTRL,
};

// 将变量状态类型定义为bool类型
using EVarStatus = bool;

/**
 * @brief 数据缓冲区模版
 * @tparam T为要存储的数据类型
 */
template<typename T> using DataBuffer = std::vector<T>;


/**
 * @brief   字节流缓冲区模版
 * @details 将非平凡类型的数据转换为字节流，以便于进行数据传输
 *          data - 要转换的数据(原始数据)
 *          stream - 转换后的字节流
 * @tparam T为要转换的数据类型
 */
template<typename T>
union StreamBuffer{
    static_assert(!std::is_trivial<T>(), "T must be a non-trivial type");
    T data;
    uint8_t stream[sizeof(T)];
};

enum class EInterfaceID;
enum class EDeviceID;
enum class EModuleID;


} // namespace my_engineer

#endif // CONF_COMMON_HPP
