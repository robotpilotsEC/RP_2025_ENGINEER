/**
 * @file algo_pid.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief pid算法的实现
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "algo_pid.hpp"

namespace my_engineer
{
/**
 * @brief 初始化PID
 * 
 * @param pStructInitParam 
 * @return EAppStatus 
 */
EAppStatus CAlgoPid::InitPID(const SAlgoInitParam_Pid *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查状态
    if(PidStatus == APP_BUSY) return APP_ERROR;

    auto &param = *pStructInitParam;
    // 检查线程数
    if(param.threadNum == 0) return APP_ERROR;

    // 初始化pid参数
    tickRate_ = param.tickRate;
    threadNum_ = param.threadNum;
    kp_ = param.kp;
    ki_ = param.ki;
    kd_ = param.kd;
    inputDeadband_ = param.Input_deadband;
    maxOutput_ = param.maxOutput;
    maxIntegral_ = param.maxIntegral;
    errorMode_ = param.errorMode;
    input_integralSeparation_ = param.input_integralSeparation;
    MachineModeErrorRange_ = param.MachineModeErrorRange;
    sustainable_output_ = param.sustainable_output;
    sustainable_time_ = param.sustainable_time;
    recover_time_ = param.recover_time;

    // 重置一次pid
    ResetPidController();

    PidStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 根据目标值和测量值更新PID控制器并返回输出值
 * 
 */
DataBuffer<float_t> CAlgoPid::UpdatePidController(const DataBuffer<float_t> &target, 
                                                  const DataBuffer<float_t> &measure){
    DataBuffer<float_t> output(threadNum_, 0.0f);

    // 检查状态
    if(PidStatus == APP_RESET) return output;
    if(PidStatus == APP_BUSY) return output;

    // 检查输入数据是否与线程数相等
    if(target.size() != threadNum_ || measure.size() != threadNum_) return output;

    // 计算误差
    auto error = CalcError_(target, measure);

    // 计算输出
    for (size_t i = 0; i < threadNum_; i++)
    {
        output[i] = CalcOutput_(error[i], ThreadInfo_[i]);
    }

    return output;
}

/**
 * @brief 重置PID控制器
 * 
 */
EAppStatus CAlgoPid::ResetPidController(){
    // 检查状态
    if(PidStatus == APP_RESET) return APP_ERROR;
    if(PidStatus == APP_BUSY) return APP_ERROR;

    // 重置线程计算值
    ThreadInfo_ = DataBuffer<SPidThreadInfo>(threadNum_);

    return APP_OK;
}

/**
 * @brief 计算误差
 * 
 * @param target 
 * @param measure 
 * @return DataBuffer<float_t> 
 */
DataBuffer<float_t> CAlgoPid::CalcError_(const DataBuffer<float_t> &target, 
                                         const DataBuffer<float_t> &measure){
    DataBuffer<float_t> error(threadNum_, 0.0f);

    // 检查状态
    if(PidStatus == APP_RESET) return error;

    switch (errorMode_)
    {
        case EPidErrorMode::NORMAL:
            for (size_t i = 0; i < threadNum_; i++)
            {
                error[i] = target[i] - measure[i];
            }
            break;
        
        case EPidErrorMode::ANGLE:
            for (size_t i = 0; i < threadNum_; i++)
            {
                error[i] = target[i] - measure[i];
                // 将误差限制在-180 - 180内,并处理经过零点情况
                if(abs(error[i]) > 180.0f){
                    error[i] -= std::copysign(360.0f, error[i]);
                }
            }
            break;

        case EPidErrorMode::MACHINE:
            for (size_t i = 0; i < threadNum_; i++)
            {
                error[i] = target[i] - measure[i];
                // 将误差限制在-4096到4096内,并处理经过零点情况
                if(abs(error[i]) > static_cast<float_t>(MachineModeErrorRange_) / 2){
                    error[i] -= std::copysign(static_cast<float_t>(MachineModeErrorRange_), error[i]);
                }
            }
        
        default:
            break;
    }

    return error;
}

/**
 * @brief 计算输出
 * 
 * @param error 
 * @param threadInfo 
 * @return float_t 
 */
float CAlgoPid::CalcOutput_(const float error, SPidThreadInfo &info){

    

    // 检查死区
    if(inputDeadband_ > 0.0f && abs(error) < inputDeadband_){
        info.integral = 0.0f;
        info.lastError = 0.0f;
        return 0.0f;
    }

    // 计算积分分离
    if(input_integralSeparation_ > 0.0f && abs(error) > input_integralSeparation_){
        info.integral = 0.0f;
    }
    else
    {
        // 计算积分
        // 一秒钟增加error的平均值，方便直观估计输出以进行调参
        info.integral += error / static_cast<float_t>(tickRate_);
        info.integral = std::clamp(info.integral, -maxIntegral_, maxIntegral_);
    }
    

    // 计算微分
    // 进行低通滤波
    info.derivative = info.derivative + 0.1f * ((error - info.lastError) - info.derivative);

    // 计算输出
    info.pOut = kp_ * error;
    info.iOut = ki_ * info.integral;
    info.dOut = kd_ * info.derivative;

    // 计算并限制总输出
    info.output = info.pOut + info.iOut + info.dOut;
    info.output = std::clamp(info.output, -maxOutput_, maxOutput_);

    // 可持续输出
    if(sustainable_output_ > 0.0f){
        int8_t sign = info.output > 0.0f ? 1 : -1;
        // 计算可持续时间
        if(abs(info.output) > sustainable_output_){
            info.Time_exceed++;
            if(info.Time_exceed > sustainable_time_){
                info.output = sustainable_output_ * sign;
                info.is_limit = true;
            }
        }
        else {
            if (info.Time_exceed > 0) {
                info.Time_exceed--;
            }
        }
        
        if (info.is_limit) {
            info.Time_recover++;
            if (info.Time_recover > recover_time_) {
                info.Time_exceed = 0;
                info.Time_recover = 0;
                info.is_limit = false;
            }
        }
    }

    // 更新上一次的误差
    info.lastError = error;

    return info.output;
}

} // namespace my_engineer
