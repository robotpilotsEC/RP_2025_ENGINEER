/**
 * @file mems_bmi088.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief bmi088的实现
 * @version 1.0
 * @date 2024-11-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string>
#include "mems/mems_bmi088.hpp"

#define BMI088_ACCEL_3G_SEN 0.0008974358974f
#define BMI088_ACCEL_6G_SEN 0.00179443359375f
#define BMI088_ACCEL_12G_SEN 0.0035888671875f
#define BMI088_ACCEL_24G_SEN 0.007177734375f


#define BMI088_GYRO_2000_SEN 0.00106526443603169529841533860381f
#define BMI088_GYRO_1000_SEN 0.00053263221801584764920766930190693f
#define BMI088_GYRO_500_SEN 0.00026631610900792382460383465095346f
#define BMI088_GYRO_250_SEN 0.00013315805450396191230191732547673f
#define BMI088_GYRO_125_SEN 0.000066579027251980956150958662738366f

namespace my_engineer {

EAppStatus CMemsBmi088::InitDevice(const SDevInitParam_Base *pStructInitParam){
    // 检查参数是否为空
    if(pStructInitParam == nullptr) return APP_ERROR;
    // 检查设备ID
    if(pStructInitParam->deviceID == EDeviceID::DEV_NULL) return APP_ERROR;

    // 强制类型转换并移除const属性
    auto &param = 
    *const_cast<SMemsInitParam_Bmi088*>(static_cast<const SMemsInitParam_Bmi088*>(pStructInitParam));
    // 变量初始化
    deviceID = pStructInitParam->deviceID;
    spiInterface_ = static_cast<CInfSPI*>(InterfaceIDMap.at(param.interfaceID));
    AccelUnitCsPort_ = param.AccelUnitCsPort;
    AccelUnitCsPin_ = param.AccelUnitCsPin;
    GyroUnitCsPort_ = param.GyroUnitCsPort;
    GyroUnitCsPin_ = param.GyroUnitCsPin;

    // 初始化温度控制的pid
    if (param.useTempControl) {
        useTempControl_ = true;
        tempTarget_ = param.tempTarget;
        halTimHandle_ = param.halTimHandle;
        halTimChannel_ = param.halTimChannel;
        param.tempPidParam.threadNum = 1;
        param.tempPidParam.tickRate = 100;
        param.tempPidParam.maxOutput = 100;
        tempPidController_.InitPID(&param.tempPidParam);
    }

    RegisterDevice_();
    RegisterMems_();

    bmi08Dev_.variant = BMI088_VARIANT;
    bmi08Dev_.intf = BMI08_SPI_INTF;

    deviceStatus = APP_OK;

    return APP_OK;
}

/**
 * @brief 启动设备
 * 
 */
EAppStatus CMemsBmi088::StartDevice(){

    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;

    if(memsStatus == EMemsStatus::RESET || memsStatus == EMemsStatus::ERROR){
        xTaskCreate(StartBmi088InitTask_, "BMI088InitTask", 
                    256, this, proc_DeviceInitTaskPriority, nullptr);
        return APP_OK;
    }

    return APP_ERROR;

}

/**
 * @brief 停止设备
 * 
 */
EAppStatus CMemsBmi088::StopDevice(){
    // 检查设备状态
    if(deviceStatus == APP_RESET) return APP_ERROR;

    if(memsStatus == EMemsStatus::NORMAL){
        memsStatus = EMemsStatus::RESET;
        return APP_OK;
    }

    return APP_OK;
}

/**
 * @brief 更新处理
 * 
 */
void CMemsBmi088::UpdateHandler_(){
    // 检查设备状态
    if(deviceStatus == APP_RESET) return;

    // 设备正常时，更新姿态数据
    if(memsStatus == EMemsStatus::NORMAL)
    {
        UpdateAccelGyroData_();
        UpdateEulerAngle_();
    }
}

/**
 * @brief 心跳处理
 * 
 */
void CMemsBmi088::HeartbeatHandler_(){
    // 检查设备状态
    if(deviceStatus == APP_RESET) return;

    // 设备正常时，更新温度数据
    if(memsStatus == EMemsStatus::NORMAL)
    {
        TempControl_();
    }
}

/**
 * @brief 加速度计的原始数据（LSB）转换为物理单位（m/s²）
 * 
 * @param lsb 
 * @param gRange 
 * @param bitWidth 
 * @return float_t 
 * 
 * @note 这个函数和下面那个函数其实都没被用到，因为读取数据的时候直接将原始数据乘了一个灵敏度常数
 */
inline float_t CMemsBmi088::Lsb2Mps2_(int16_t lsb, int8_t gRange, uint8_t bitWidth) {

  auto power = 2.0f;

  auto half_scale = pow(power, bitWidth) / 2.0f;

  return (earthGravity_ * lsb * gRange) / half_scale;
}

/**
 * @brief 陀螺仪的原始数据（LSB）转换为物理单位（rad/s）
 * 
 * @param lsb 
 * @param dpsRange 
 * @param bitWidth 
 * @return float_t 
 */
inline float_t CMemsBmi088::Lsb2Dps_(int16_t lsb, float_t dpsRange, uint8_t bitWidth) {

  auto power = 2.0f;

  auto half_scale = pow(power, bitWidth) / 2.0f;

  return (dpsRange / half_scale) * lsb;
}

/**
 * @brief 将片选信号置低，使能加速度计
 * 
 */
inline void CMemsBmi088::AccelUnitCsEnable_() {

  HAL_GPIO_WritePin(AccelUnitCsPort_, AccelUnitCsPin_, GPIO_PIN_RESET);
}

/**
 * @brief 将片选信号置高，禁用加速度计
 * 
 */
inline void CMemsBmi088::AccelUnitCsDisable_() {

  HAL_GPIO_WritePin(AccelUnitCsPort_, AccelUnitCsPin_, GPIO_PIN_SET);
}

/**
 * @brief 将片选信号置低，使能陀螺仪
 * 
 */
inline void CMemsBmi088::GyroUnitCsEnable_() {

  HAL_GPIO_WritePin(GyroUnitCsPort_, GyroUnitCsPin_, GPIO_PIN_RESET);
}

/**
 * @brief 将片选信号置高，禁用陀螺仪
 * 
 */
inline void CMemsBmi088::GyroUnitCsDisable_() {

  HAL_GPIO_WritePin(GyroUnitCsPort_, GyroUnitCsPin_, GPIO_PIN_SET);
}

/**
 * @brief 向加速度计的单个寄存器写入数据
 * 
 * @param regAddr 
 * @param regData 
 */
inline void CMemsBmi088::AccelUnitWriteSingleRegister_(uint8_t regAddr,
                                                        uint8_t regData) {

  AccelUnitCsEnable_();

  uint8_t cmd1[] = {regAddr, regData};
  spiInterface_->Transmit(cmd1, sizeof(cmd1));

  AccelUnitCsDisable_();
}


/**
 * @brief 从加速度计的单个寄存器读取数据
 * 
 * @param regAddr 
 * @param buffer 
 */
inline void CMemsBmi088::AccelUnitReadSingleRegister_(uint8_t regAddr,
                                                       uint8_t *buffer) {

  AccelUnitCsEnable_();

  uint8_t cmd1[] = { static_cast<uint8_t>(regAddr | 0x80), 0x55 };
  spiInterface_->Transmit(cmd1, sizeof(cmd1));

  uint8_t cmd2[] = { 0x55 };
  spiInterface_->TransmitReceive(cmd2, buffer, sizeof(cmd2));

  AccelUnitCsDisable_();
}


/**
 * @brief 从加速度计的多个寄存器读取数据
 * 
 * @param regAddr 
 * @param buffer 
 * @param len 
 */
inline void CMemsBmi088::AccelUnitReadMultiRegisters_(uint8_t regAddr,
                                                       uint8_t *buffer,
                                                       size_t len) {

  AccelUnitCsEnable_();

  uint8_t cmd1[] = { static_cast<uint8_t>(regAddr | 0x80) };
  spiInterface_->Transmit(cmd1, sizeof(cmd1));
  spiInterface_->Transmit(cmd1, sizeof(cmd1));

  uint8_t cmd2[len];
  std::fill(cmd2, cmd2 + len, 0x55);
  spiInterface_->TransmitReceive(cmd2, buffer, sizeof(cmd2));

  AccelUnitCsDisable_();
}


/**
 * @brief 向陀螺仪的单个寄存器写入数据
 * 
 * @param regAddr 
 * @param regData 
 */
inline void CMemsBmi088::GyroUnitWriteSingleRegister_(uint8_t regAddr,
                                                       uint8_t regData) {

  GyroUnitCsEnable_();

  uint8_t cmd1[] = {regAddr, regData };
  spiInterface_->Transmit(cmd1, sizeof(cmd1));

  GyroUnitCsDisable_();
}


/**
 * @brief 从陀螺仪的单个寄存器读取数据
 * 
 * @param regAddr 
 * @param buffer 
 */
inline void CMemsBmi088::GyroUnitReadSingleRegister_(uint8_t regAddr,
                                                      uint8_t *buffer) {

  GyroUnitCsEnable_();

  // uint8_t cmd1[] = { static_cast<uint8_t>(regAddr | 0x80), 0x55 };
  // spiInterface_->Transmit(cmd1, sizeof(cmd1));

  uint8_t cmd1[] = { static_cast<uint8_t>(regAddr | 0x80) };
  spiInterface_->Transmit(cmd1, sizeof(cmd1));

  uint8_t cmd2[] = { 0x55 };
  spiInterface_->TransmitReceive(cmd2, buffer, sizeof(cmd2));

  GyroUnitCsDisable_();
}


/**
 * @brief 从陀螺仪的多个寄存器读取数据
 * 
 * @param regAddr 
 * @param buffer 
 * @param len 
 */
inline void CMemsBmi088::GyroUnitReadMultiRegisters_(uint8_t regAddr,
                                                      uint8_t *buffer,
                                                      size_t len) {

  GyroUnitCsEnable_();

  uint8_t cmd1[] = { static_cast<uint8_t>(regAddr | 0x80) };
  spiInterface_->Transmit(cmd1, sizeof(cmd1));

  uint8_t cmd2[len];
  std::fill(cmd2, cmd2 + len, 0x55);
  spiInterface_->TransmitReceive(cmd2, buffer, sizeof(cmd2));

  GyroUnitCsDisable_();
}

/**
 * @brief 初始化BMI088加速度计单元
 * 
 */
EAppStatus CMemsBmi088::AccelUnitInit_() {

  uint8_t regData[] = {0x00};

  /* Check communication */
  AccelUnitReadSingleRegister_(BMI08_REG_ACCEL_CHIP_ID, regData);
  DelayUs_(150);
  AccelUnitReadSingleRegister_(BMI08_REG_ACCEL_CHIP_ID, regData);
  DelayUs_(150);

  /* Soft reset */
  AccelUnitWriteSingleRegister_(BMI08_REG_ACCEL_SOFTRESET, BMI08_SOFT_RESET_CMD);
  proc_waitMs(80);

  /* Check communication */
  AccelUnitReadSingleRegister_(BMI08_REG_ACCEL_CHIP_ID, regData);
  DelayUs_(150);
  AccelUnitReadSingleRegister_(BMI08_REG_ACCEL_CHIP_ID, regData);
  DelayUs_(150);

  /* Check register data */
  // 如果读取到的芯片ID不正确，则返回错误
  if (regData[0] != BMI088_ACCEL_CHIP_ID) return APP_ERROR;

  /* Set Accel unit configuration */
  // 每个配置参数包含寄存器地址和要写入的值
  uint8_t config[][2] = {
      {BMI08_REG_ACCEL_PWR_CTRL, BMI08_ACCEL_POWER_ENABLE},
      {BMI08_REG_ACCEL_PWR_CONF, BMI08_ACCEL_PM_ACTIVE},
      {BMI08_REG_ACCEL_CONF, (BMI08_ACCEL_BW_NORMAL << BMI08_ACCEL_BW_POS) | BMI08_ACCEL_ODR_800_HZ },
      {BMI08_REG_ACCEL_RANGE, BMI088_ACCEL_RANGE_3G},
  };

  for (auto & step : config) {
    AccelUnitWriteSingleRegister_(step[0], step[1]);
    DelayUs_(150);
    AccelUnitReadSingleRegister_(step[0], regData);
    DelayUs_(150);

    if (regData[0] != step[1]) return APP_ERROR;
  }

  return APP_OK;
}

/**
 * @brief 初始化BMI088陀螺仪单元
 * 
 */
EAppStatus CMemsBmi088::GyroUnitInit_() {

  uint8_t regData[] = {0x00};

  /* Check communication */
  GyroUnitReadSingleRegister_(BMI08_REG_GYRO_CHIP_ID, regData);
  DelayUs_(150);
  GyroUnitReadSingleRegister_(BMI08_REG_GYRO_CHIP_ID, regData);
  DelayUs_(150);

  /* Soft reset */
  GyroUnitWriteSingleRegister_(BMI08_REG_GYRO_SOFTRESET, BMI08_SOFT_RESET_CMD);
  proc_waitMs(80);

  /* Check communication */
  GyroUnitReadSingleRegister_(BMI08_REG_GYRO_CHIP_ID, regData);
  DelayUs_(150);
  GyroUnitReadSingleRegister_(BMI08_REG_GYRO_CHIP_ID, regData);
  DelayUs_(150);

  /* Check register data */
  // 如果读取到的芯片ID不正确，则返回错误
  if (regData[0] != BMI08_GYRO_CHIP_ID) return APP_ERROR;

  /* Set Gyro unit configuration */
  // 每个配置参数包含寄存器地址和要写入的值
  uint8_t config[][2] = {
      {BMI08_REG_GYRO_RANGE, BMI08_GYRO_RANGE_2000_DPS},
      {BMI08_REG_GYRO_BANDWIDTH, BMI08_GYRO_BW_116_ODR_1000_HZ | BMI08_GYRO_ODR_RESET_VAL},
      {BMI08_REG_GYRO_LPM1, BMI08_GYRO_PM_NORMAL},
  };

  for (auto & step : config) {
    GyroUnitWriteSingleRegister_(step[0], step[1]);
    DelayUs_(150);
    GyroUnitReadSingleRegister_(step[0], regData);
    DelayUs_(150);

    if (regData[0] != step[1]) return APP_ERROR;
  }

  return APP_OK;
}

/**
 * @brief 更新加速度计,陀螺仪,温度数据
 * 
 */
EAppStatus CMemsBmi088::UpdateAccelGyroData_() {


  uint8_t buffer[8] = {0x00};

  /* Read Accel Data */
  // 最后乘一个灵敏度常数(单位:m/s^2/LSB)       
  AccelUnitReadMultiRegisters_(BMI08_REG_ACCEL_X_LSB, buffer, 6);
  memsData[DATA_ACC_X] = static_cast<int16_t>(buffer[1] << 8 | buffer[0]) * BMI088_ACCEL_3G_SEN;
  memsData[DATA_ACC_Y] = static_cast<int16_t>(buffer[3] << 8 | buffer[2]) * BMI088_ACCEL_3G_SEN;
  memsData[DATA_ACC_Z] = static_cast<int16_t>(buffer[5] << 8 | buffer[4]) * BMI088_ACCEL_3G_SEN;

  /* Read Gyro Data */
  // 最后乘一个灵敏度常数(单位:rad/s/LSB)
  // 再减去一个零偏值 
  GyroUnitReadMultiRegisters_(BMI08_REG_GYRO_CHIP_ID, buffer, 8);
  if (buffer[0] == BMI08_GYRO_CHIP_ID) {
    memsData[DATA_GYRO_X] = static_cast<int16_t>(buffer[3] << 8 | buffer[2]) * BMI088_GYRO_2000_SEN - gx_;
    memsData[DATA_GYRO_Y] = static_cast<int16_t>(buffer[5] << 8 | buffer[4]) * BMI088_GYRO_2000_SEN - gy_;
    memsData[DATA_GYRO_Z] = static_cast<int16_t>(buffer[7] << 8 | buffer[6]) * BMI088_GYRO_2000_SEN - gz_;
  }

  /* Read Temperature Data */
  AccelUnitReadMultiRegisters_(BMI08_REG_TEMP_MSB, buffer, 2);
  memsData[DATA_TEMP] = static_cast<int16_t>((buffer[0] << 3) | (buffer[1] >> 5)) * 0.125f + 23.0f;

  return APP_OK;
}

/**
 * @brief 转换IMU数据
 * 
 */
void CMemsBmi088::TransformImuData_(float_t *gx, float_t *gy, float_t *gz,
                                    float_t *ax, float_t *ay, float_t *az,
                                    float_t roll, float_t pitch, float_t yaw) {

  float_t rollRad = roll * 0.0174532925f;
  float_t pitchRad = pitch * 0.0174532925f;
  float_t yawRad = yaw * 0.0174532925f;

  float_t cosRoll = arm_cos_f32(rollRad);
  float_t sinRoll = arm_sin_f32(rollRad);
  float_t cosPitch = arm_cos_f32(pitchRad);
  float_t sinPitch = arm_sin_f32(pitchRad);
  float_t cosYaw = arm_cos_f32(yawRad);
  float_t sinYaw = arm_sin_f32(yawRad);

  // 旋转矩阵 R = Rz(yaw) * Ry(pitch) * Rx(roll)
  float_t R_Data[9] = {
    cosYaw * cosPitch, cosYaw * sinPitch * sinRoll - sinYaw * cosRoll, cosYaw * sinPitch * cosRoll + sinYaw * sinRoll,
    sinYaw * cosPitch, sinYaw * sinPitch * sinRoll + cosYaw * cosRoll, sinYaw * sinPitch * cosRoll - cosYaw * sinRoll,
    -sinPitch, cosPitch * sinRoll, cosPitch * cosRoll
  };

  float_t g_in[3] = {*gx, *gy, *gz};
  float_t a_in[3] = {*ax, *ay, *az};

  float_t g_out[3] = {0.0f};
  float_t a_out[3] = {0.0f};

  arm_matrix_instance_f32 R, G_in, G_out, A_in, A_out;

  arm_mat_init_f32(&R, 3, 3, R_Data);
  arm_mat_init_f32(&G_in, 3, 1, g_in);
  arm_mat_init_f32(&G_out, 3, 1, g_out);
  arm_mat_init_f32(&A_in, 3, 1, a_in);
  arm_mat_init_f32(&A_out, 3, 1, a_out);

  // 先对R求逆
  arm_mat_inverse_f32(&R, &R);
  arm_mat_mult_f32(&R, &G_in, &G_out);
  arm_mat_mult_f32(&R, &A_in, &A_out);

  *gx = g_out[0];
  *gy = g_out[1];
  *gz = g_out[2];

  *ax = a_out[0];
  *ay = a_out[1];
  *az = a_out[2];
}

/**
 * @brief 更新欧拉角
 */
EAppStatus CMemsBmi088::UpdateEulerAngle_() {

  static const float_t HalfT = 0.0005f;
  static const float_t Kp = 0.1f;
  static const float_t Yaw_Trans = 0.0f, Pitch_Trans = 0.0f, Roll_Trans = 90.0f;

  static float_t q0 = 1.0f, q1, q2, q3;
  static float_t q0_tmp, q1_tmp, q2_tmp, q3_tmp;
  static float_t gx, gy, gz;
  static float_t ax, ay, az;
  static float_t vx, vy, vz;
  static float_t ex, ey, ez;
  
  float_t norm;
  float_t sintmp, costmp;

  gx = LowPassFilter(gx, memsData[DATA_GYRO_X], 0.5f);
  gy = LowPassFilter(gy, memsData[DATA_GYRO_Y], 0.5f);
  gz = LowPassFilter(gz, memsData[DATA_GYRO_Z], 0.5f);

  ax = LowPassFilter(ax, memsData[DATA_ACC_X], 0.3f);
  ay = LowPassFilter(ay, memsData[DATA_ACC_Y], 0.3f);
  az = LowPassFilter(az, memsData[DATA_ACC_Z], 0.3f);

  // 转换IMU数据
  TransformImuData_(&gx, &gy, &gz, &ax, &ay, &az, Roll_Trans, Pitch_Trans, Yaw_Trans);

  if (ax * ay * az != 0) {
    norm = inVSqrt(ax * ax + ay * ay + az * az);
    ax *= norm;
    ay *= norm;
    az *= norm;

    vx = 2 * (q1 * q3 - q0 * q2);
    vy = 2 * (q0 * q1 + q2 * q3);
    vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);
    
    // 误差补偿，Kp越大，越信任加速度计
    gx = gx + Kp * ex;
    gy = gy + Kp * ey;
    gz = gz + Kp * ez;
  }

  q0_tmp = q0;
  q1_tmp = q1;
  q2_tmp = q2;
  q3_tmp = q3;

  q0 = q0_tmp + (-q1_tmp * gx - q2_tmp * gy - q3_tmp * gz) * HalfT;
  q1 = q1_tmp + (q0_tmp * gx + q2_tmp * gz - q3_tmp * gy) * HalfT;
  q2 = q2_tmp + (q0_tmp * gy - q1_tmp * gz + q3_tmp * gx) * HalfT;
  q3 = q3_tmp + (q0_tmp * gz + q1_tmp * gy - q2_tmp * gx) * HalfT;

  norm = inVSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= norm;
  q1 *= norm;
  q2 *= norm;
  q3 *= norm;

  sintmp = 2 * (q1 * q3 - q0 * q2);
  arm_sqrt_f32(1 - sintmp * sintmp, &costmp);

  arm_atan2_f32(sintmp, costmp, &EulerData[DATA_PITCH]);
  arm_atan2_f32(2*(q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3, &EulerData[DATA_YAW]);
  arm_atan2_f32(2*(q2*q3 + q0*q1), q0*q0 - q1*q1 - q2*q2 + q3*q3, &EulerData[DATA_ROLL]);

  EulerData[DATA_PITCH] = EulerData[DATA_PITCH] * 57.295773f;
  EulerData[DATA_YAW] = EulerData[DATA_YAW] * 57.295773f;
  EulerData[DATA_ROLL] = EulerData[DATA_ROLL] * 57.295773f;


  return APP_OK;
}

/**
 * @brief 温度控制
 * @note 外界温度会影响到BMI工作的稳定性，因此需要输出不同的PWM波来控制芯片温度
 */
EAppStatus CMemsBmi088::TempControl_() {

  if (!useTempControl_) return APP_ERROR;

  if (memsStatus == EMemsStatus::ERROR) {
    HAL_TIM_PWM_Stop(halTimHandle_, halTimChannel_);
    return APP_ERROR;
  }

  auto output =
      tempPidController_.UpdatePidController({tempTarget_}, {memsData[DATA_TEMP]});

  uint32_t arr = __HAL_TIM_GET_AUTORELOAD(halTimHandle_);
  uint32_t ccr = arr * output[0] / 100;
  __HAL_TIM_SET_COMPARE(halTimHandle_, halTimChannel_, ccr);

  return APP_OK;   
}

/**
 * @brief BMI088初始化任务
 * 
 * @param arg 
 */
void CMemsBmi088::StartBmi088InitTask_(void *arg) {

    if (arg == nullptr) proc_return();

    auto bmiDev = static_cast<CMemsBmi088 *>(arg);
    auto maxRetry = 10;
    //  auto stableCnt = 0;
    auto gx = 0.0f, gy = 0.0f, gz = 0.0f;
    const size_t sampleTime = 200;

    bmiDev->memsStatus = EMemsStatus::INIT;

    /* Init Accel & Gyro Unit */
    while (maxRetry--) {
        if (bmiDev->AccelUnitInit_() == APP_OK && bmiDev->GyroUnitInit_() == APP_OK)
            break;
        proc_waitMs(100);
    }

    if (maxRetry <= 0) {
        bmiDev->memsStatus = EMemsStatus::ERROR;
        proc_return();
    }

    /* Wait for Temperature Stabilized */
    //  HAL_TIM_PWM_Start(bmiDev->halTimHandle_, bmiDev->halTimChannel_);
    //  while(stableCnt < 250) {
    //    bmiDev->UpdateAccelGyroData_();
    //    bmiDev->TempControl_();
    //    stableCnt = (abs(40.0f - bmiDev->memsData[DATA_TEMP]) < 1.0f) ? stableCnt + 1 : 0;
    //    proc_waitMs(4);   // 250Hz
    //  }

    /* Clear Gyro Zero-Offset */
    bmiDev->gx_ = 0.0f;
    bmiDev->gy_ = 0.0f;
    bmiDev->gz_ = 0.0f;

    /* Update Gyro Zero-Offset */
    // 循环读取一段时间的陀螺仪数据，然后取平均值作为零偏值
    for (size_t i = 0; i < sampleTime; i++) {
        bmiDev->UpdateAccelGyroData_();
        //    bmiDev->TempControl_();
        gx += bmiDev->memsData[DATA_GYRO_X];
        gy += bmiDev->memsData[DATA_GYRO_Y];
        gz += bmiDev->memsData[DATA_GYRO_Z];
        proc_waitMs(4);   // 250Hz
    }

    /* Set Gyro Zero-Offset */
    bmiDev->gx_ = static_cast<float_t>(gx / sampleTime);
    bmiDev->gy_ = static_cast<float_t>(gy / sampleTime);
    bmiDev->gz_ = static_cast<float_t>(gz / sampleTime);

    bmiDev->memsStatus = EMemsStatus::NORMAL;

    proc_return();
}

} // namespace myengineer
