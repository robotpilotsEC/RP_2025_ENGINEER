/**
 * @file algo_pid.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief PID算法类
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ALGO_PID_HPP
#define ALGO_PID_HPP

#include "Configuration.hpp"

namespace my_engineer {

/**
 * @brief PID算法类
 * 
 */
class CAlgoPid{
public:
    /**
     * @brief 定义pid的误差计算模式的枚举变量
     * 
     */
    enum class EPidErrorMode{
        NORMAL,      ///< 正常模式
        ANGLE,       ///< 角度模式(误差范围在-180.0f-180.0f)
        MACHINE,     ///< 机械模式(误差范围在-4096 - 4096(default))
    };

    /**
     * @brief 定义pid初始化结构体
     * 
     */
    struct SAlgoInitParam_Pid{
        uint8_t threadNum = 1; ///< 线程数(default:1)
        uint16_t tickRate = 1000; ///< 定时器频率(default:1000Hz)
        float_t kp = 0.0f; ///< 比例系数
        float_t ki = 0.0f; ///< 积分系数
        float_t kd = 0.0f; ///< 微分系数
        float_t Input_deadband = 0.0f; ///< 输入死区
        float_t maxOutput = 0.0f; ///< 最大输出
        float_t maxIntegral = 0.0f; ///< 最大积分
        float_t input_integralSeparation = 0.0f; ///< 积分分离值
        float_t sustainable_output = 0.0f; ///< 可持续输出
        uint32_t sustainable_time = 0; ///< 可持续时间
        uint32_t recover_time = 0; ///< 恢复时间
        uint16_t MachineModeErrorRange = 8192; ///< 机械模式误差范围
        EPidErrorMode errorMode = EPidErrorMode::NORMAL; ///< 误差计算模式
    };

    // pid状态
    EAppStatus PidStatus = EAppStatus::APP_RESET;

    /**
     * @brief 初始化PID
     * 
     */
    EAppStatus InitPID(const SAlgoInitParam_Pid *pStructInitParam);

    /**
     * @brief 根据目标值和测量值更新PID控制器并返回输出值
     * 
     */
    DataBuffer<float_t> UpdatePidController(const DataBuffer<float_t> &target, const DataBuffer<float_t> &measure);

    /**
     * @brief 重置PID控制器
     * 
     */
    EAppStatus ResetPidController();
    
private:
    /**
     * @brief 定义一个结构体，存放一个线程中pid的单次运算值
     * 
     */
    struct SPidThreadInfo{
        float_t pOut = 0.0f; ///< 比例输出
        float_t iOut = 0.0f; ///< 积分输出
        float_t dOut = 0.0f; ///< 微分输出
        float_t output = 0.0f; ///< 总输出
        float_t integral = 0.0f; ///< 积分值
        float_t derivative = 0.0f; ///< 微分值
        float_t lastError = 0.0f; ///< 上一次的误差
        uint32_t Time_exceed = 0; // 超出时间
        uint32_t Time_recover = 0; // 恢复时间
        bool is_limit = false; // 是否限制
    };

    // pid参数
    float_t kp_ = 0.0f;
    float_t ki_ = 0.0f;
    float_t kd_ = 0.0f;

    // pid额外参数
    float_t inputDeadband_ = 0.0f;
    float_t input_integralSeparation_ = 0.0f; // 积分分离值
    float_t maxOutput_ = 0.0f;
    float_t maxIntegral_ = 0.0f;
    uint16_t MachineModeErrorRange_ = 8192;

    float_t sustainable_output_ = 0.0f; ///< 可持续输出
    uint32_t sustainable_time_ = 0.0f; ///< 可持续时间
    uint32_t recover_time_ = 0.0f; ///< 恢复时间

    // pid线程数与定时器频率
    uint8_t threadNum_ = 1;
    uint16_t tickRate_ = 1000;

    // pid误差计算模式
    EPidErrorMode errorMode_ = EPidErrorMode::NORMAL;

    // pid各线程计算结果存储缓冲区
    DataBuffer<SPidThreadInfo> ThreadInfo_;

    /**
     * @brief 根据误差计算模式计算误差
     * 
     */
    DataBuffer<float_t> CalcError_(const DataBuffer<float_t> &target, const DataBuffer<float_t> &measure);

    /**
     * @brief 计算PID控制器的输出值，并存储运算值
     * 
     */
    float CalcOutput_(const float error, SPidThreadInfo &info);

};

} // namespace my_engineer

#endif // ALGO_PID_HPP
