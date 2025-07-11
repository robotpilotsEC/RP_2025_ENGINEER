/**
 * @file remote.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief mems的通用控制层的实现
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 * @details
 * 在这个文件里面定义mems通用的方法以及mems映射图
 */

#include "mems/mems.hpp"

namespace my_engineer
{

// MEMS映射图
std::map<EDeviceID, CMemsBase *> MemsIDMap;

EAppStatus CMemsBase::RegisterMems_()
{
    if(deviceID != EDeviceID::DEV_NULL)
    {
        MemsIDMap.insert(std::make_pair(deviceID, this));
        return APP_OK;
    }
    return APP_ERROR;
}

EAppStatus CMemsBase::UnregisterMems_()
{
    if(deviceID != EDeviceID::DEV_NULL)
    {
        MemsIDMap.erase(deviceID);
        return APP_OK;
    }
    return APP_ERROR;
}

void CMemsBase::DelayMs_(uint32_t ms)
{
    DelayUs_(ms * 1000);
}

void CMemsBase::DelayUs_(uint32_t us)
{
    
    auto ticks = us * 480; // 系统时钟频率为480MHz，因此每微妙有480个时钟周期
    auto tcnt = 0ul; // 计数器，初始为0
    auto tnow = SysTick->VAL; // 当前时间
    auto told = SysTick->VAL; // 上一次的时间
    auto reload = SysTick->LOAD; // 重装载值

    /**
     * 该计数器是向下计数的
     * 如果当前tnow > told，说明经过了一次重载，要把重载值加上
     */
    while (true) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            tcnt += (tnow < told) ? (told - tnow) : (reload - tnow + told);
            told = tnow;

            if (tcnt >= ticks) return;
        }
    }
}

} // namespace my_engineer
