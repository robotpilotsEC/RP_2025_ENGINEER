/******************************************************************************
 * @brief        
 * 
 * @file         algo_other.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-01
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef ALGO_OTHER_HPP
#define ALGO_OTHER_HPP

#include "Configuration.hpp"

namespace my_engineer {

// 计算开平方的倒数
float_t inVSqrt(float_t x);

// 低通滤波
float_t LowPassFilter(float_t last_data, float_t current_data, float_t alpha);

// 四舍五入
float_t Round(float_t x);

} // namespace my_engineer

#endif // ALGO_OTHER_HPP
