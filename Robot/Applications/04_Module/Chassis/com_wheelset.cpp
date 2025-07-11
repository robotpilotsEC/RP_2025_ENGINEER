/**
 * @file com_wheelset.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 底盘轮组
 * @version 1.0
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_chassis.hpp"

namespace my_engineer {

CMemsBase *pmems_test = nullptr;


/**
 * @brief 初始化底盘轮组组件
 * 
 * @param param
 * @return EAppStatus 
 */
EAppStatus CModChassis::CComWheelset::InitComponent(SModInitParam_Base &param){
    // 检查param是否正确
    if (param.moduleID == EModuleID::MOD_NULL) return APP_ERROR;

    // 类型转换
    auto chassisParam = static_cast<SModInitParam_Chassis &>(param);

    // 保存电机和传感器指针
    mems = MemsIDMap.at(chassisParam.memsDevID);
    motor[LF] = MotorIDMap.at(chassisParam.wheelsetMotorID_LF);
    motor[RF] = MotorIDMap.at(chassisParam.wheelsetMotorID_RF);
    motor[LB] = MotorIDMap.at(chassisParam.wheelsetMotorID_LB);
    motor[RB] = MotorIDMap.at(chassisParam.wheelsetMotorID_RB);

    // 设置发送节点
    mtrCanTxNode[LF] = chassisParam.wheelsetMotorTxNode_LF;
    mtrCanTxNode[RF] = chassisParam.wheelsetMotorTxNode_RF;
    mtrCanTxNode[LB] = chassisParam.wheelsetMotorTxNode_LB;
    mtrCanTxNode[RB] = chassisParam.wheelsetMotorTxNode_RB;

    // 初始化PID控制器
    chassisParam.wheelsetSpdPidParam.threadNum = 4;
    pidSpdCtrl.InitPID(&chassisParam.wheelsetSpdPidParam);

    chassisParam.lineCorrectionPidParam.threadNum = 3;
    pidLineCorrectionCtrl.InitPID(&chassisParam.lineCorrectionPidParam);

    chassisParam.yawCorrectionPidParam.threadNum = 1;
    pidYawCtrl.InitPID(&chassisParam.yawCorrectionPidParam);

    // test
    pmems_test = mems;

    mems->StartDevice();

    // 初始化电机数据输出缓冲区
    mtrOutputBuffer.fill(0);

    Component_FSMFlag_ = FSM_RESET;
    componentStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 更新组件
 * 
 */
EAppStatus CModChassis::CComWheelset::UpdateComponent(){
    // 检查组件状态
    if (componentStatus == APP_RESET) return APP_ERROR;

    switch (Component_FSMFlag_)
    {
        case FSM_RESET: {
            // 重置状态下，电机数据输出缓冲区始终为0
            mtrOutputBuffer.fill(0);
            return APP_OK;
        }
        case FSM_PREINIT: {
            // 预初始化状态下，清空电机的position和输出缓冲区，并重置PID控制器
            motor[LF]->motorData[CDevMtr::DATA_POSIT] = 0;
            motor[RF]->motorData[CDevMtr::DATA_POSIT] = 0;
            motor[LB]->motorData[CDevMtr::DATA_POSIT] = 0;
            motor[RB]->motorData[CDevMtr::DATA_POSIT] = 0;
            mtrOutputBuffer.fill(0);
            pidSpdCtrl.ResetPidController();
            pidYawCtrl.ResetPidController();
            Component_FSMFlag_ = FSM_INIT;
        }
        case FSM_INIT: {
            if(mems->memsStatus == CMemsBase::EMemsStatus::NORMAL) {
                wheelsetCmd = SWheelsetCommand();
                Component_FSMFlag_ = FSM_CTRL;
                componentStatus = APP_OK;
            }
            return _UpdateOutput(wheelsetCmd.speed_X, wheelsetCmd.speed_Y, wheelsetCmd.speed_W);
        }
        case FSM_CTRL: {
            DataBuffer<float_t> yawSpd = {wheelsetCmd.speed_W / 10.0f};
            DataBuffer<float_t> yawSpdMeasure = {mems->memsData[CMemsBase::DATA_GYRO_Z]};
            
            // 底盘角速度是一个双环控制，外环输入为目标真实角速度，输出一个映射到电机速度的目标速度
            auto output = pidYawCtrl.UpdatePidController(yawSpd, yawSpdMeasure);
            return _UpdateOutput(wheelsetCmd.speed_X, wheelsetCmd.speed_Y, output[0]);
            
        }
    
    
        default: {
            Component_FSMFlag_ = FSM_RESET;
            return APP_ERROR;
        }
    }
}

/**
 * @brief 更新输出
 * 
 * @param speed_X 
 * @param speed_Y 
 * @param speed_W 
 * @return EAppStatus 
 */
EAppStatus CModChassis::CComWheelset::_UpdateOutput(float speed_X, float speed_Y, float speed_W){

    // 从底盘电机中读取当前速度存入缓冲区
    DataBuffer<float_t> wheelSpdMeasure = {
        static_cast<float_t>(motor[LF]->motorData[CDevMtr::DATA_SPEED]),
        static_cast<float_t>(motor[RF]->motorData[CDevMtr::DATA_SPEED]),
        static_cast<float_t>(motor[LB]->motorData[CDevMtr::DATA_SPEED]),
        static_cast<float_t>(motor[RB]->motorData[CDevMtr::DATA_SPEED]),
    };

    // 直线校准
    DataBuffer<float_t> target_speed = {
        speed_X,
        speed_Y,
        speed_W,
    };

    DataBuffer<float_t> current_speed = {
        (wheelSpdMeasure[LF] + wheelSpdMeasure[RF] - wheelSpdMeasure[LB] - wheelSpdMeasure[RB]) / 4,
        (wheelSpdMeasure[LF] - wheelSpdMeasure[RF] + wheelSpdMeasure[LB] - wheelSpdMeasure[RB]) / 4,
        (wheelSpdMeasure[LF] + wheelSpdMeasure[RF] + wheelSpdMeasure[LB] + wheelSpdMeasure[RB]) / 4,
    };

    auto lineCorrection_output = pidLineCorrectionCtrl.UpdatePidController(target_speed, current_speed);

    // 根据直线校准的输出值调整底盘目标速度
    speed_X += lineCorrection_output[0];
    speed_Y += lineCorrection_output[1];
    // speed_W += lineCorrection_output[2];

    // 根据底盘目标速度解算出四个轮子的速度
    DataBuffer<float_t> wheelSpd = {
        speed_Y + speed_X + speed_W,
      - speed_Y + speed_X + speed_W,
        speed_Y - speed_X + speed_W,
      - speed_Y - speed_X + speed_W,
    };
     
    // 计算输出
    auto output = pidSpdCtrl.UpdatePidController(wheelSpd, wheelSpdMeasure);

    // 将输出值存入电机数据输出缓冲区
    mtrOutputBuffer = {
        static_cast<int16_t>(output[LF]),
        static_cast<int16_t>(output[RF]),
        static_cast<int16_t>(output[LB]),
        static_cast<int16_t>(output[RB]),
    };

    return APP_OK;

}


} // namespace my_engineer
