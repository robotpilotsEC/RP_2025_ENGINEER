/******************************************************************************
 * @brief        
 * 
 * @file         algo_other.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-01
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "algo_other.hpp"

namespace my_engineer {

float_t inVSqrt(float_t x) {
	union {
		int32_t i;
		float_t x;
	} u;
	u.x = x;
	u.i = 0x5f3759df - (u.i >> 1);
	return u.x * (1.5f - 0.5f * x * u.x * u.x);
}

float_t LowPassFilter(float_t last_data, float_t current_data, float_t alpha) {
	return last_data * alpha + current_data * (1 - alpha);
}

} // namespace my_engineer
