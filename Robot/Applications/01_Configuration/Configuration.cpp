/**
 * @file Configuration.cpp
 * @author Zoe
 * @brief 所有配置的完成由此文件提供对外入口
 * @email 2328339747@qq.com
 * @date 2024-10-30
 * 
 * @details
 */

#include "Configuration.hpp"

namespace my_engineer{

/**
 * @brief Application的入口函数
 * @return None
 */
void ApplicationEntryPoint(){

    // 调用各配置初始化函数
    // 顺序一定不要搞错啊！！
    InitAllInterface();
    InitAllDevice();
    InitAllModule();
    InitProcess();

    vTaskStartScheduler(); // 启动任务调度器
}

} // namespace my_engineer
