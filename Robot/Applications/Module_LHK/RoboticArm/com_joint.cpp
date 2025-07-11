/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-01-10
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-12-16   <td>1.0         <td>Sassinak  <td>First Create.
 * </table>
 */

 #include "mod_roboticarm.hpp"
 namespace my_engineer {
 /**
  * @brief
  * @param param
  * @return
  */
 EAppStatus C_ModRoboticArm::C_ComJoint::InitComponent(S_ModInitParam &param) {
 
   if (param.moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;  
 
   auto paramJoint = static_cast<S_ModInitParam_RoboticArm &>(param);
   /*Attendtion! if you don't register motor in motorMap, you may enter an endless loop*/
   motor[Y]  = MotorMap.at(paramJoint.jointMotor_YawID);
   motor[P1] = MotorMap.at(paramJoint.jointMotor_Pitch_1ID);
   motor[P2] = MotorMap.at(paramJoint.jointMotor_Pitch_2ID);
   motor[R]  = MotorMap.at(paramJoint.jointMotor_RollID);
 
   mtrCanTxNode[Y]   = paramJoint.jointMotorTxNode_Yaw;
   mtrCanTxNode[P1]  = paramJoint.jointMotorTxNode_Pitch_1;
   mtrCanTxNode[P2]  = paramJoint.jointMotorTxNode_Pitch_2;
   mtrCanTxNode[R]   = paramJoint.jointMotorTxNode_Roll;
 
   /*Initialize PID*/
   paramJoint.Joint1_YawPosPidParam.threadNum = 1;
   pidPosCtrl_Joint1_Yaw.InitPID(&paramJoint.Joint1_YawPosPidParam);
   paramJoint.Joint1_YawSpdPidParam.threadNum = 1;
   pidSpdCtrl_Joint1_Yaw.InitPID(&paramJoint.Joint1_YawSpdPidParam);
 
   paramJoint.Joint2_PitchPosPidParam.threadNum = 1;
   pidPosCtrl_Joint2_Pitch.InitPID(&paramJoint.Joint2_PitchPosPidParam);
   paramJoint.Joint2_PitchSpdPidParam.threadNum = 1;
   pidSpdCtrl_Joint2_Pitch.InitPID(&paramJoint.Joint2_PitchSpdPidParam);
 
   paramJoint.Joint3_PitchPosPidParam.threadNum = 1;
   pidPosCtrl_Joint3_Pitch.InitPID(&paramJoint.Joint3_PitchPosPidParam);
   paramJoint.Joint3_PitchSpdPidParam.threadNum = 1;
   pidSpdCtrl_Joint3_Pitch.InitPID(&paramJoint.Joint3_PitchSpdPidParam);
 
   paramJoint.Joint4_RollPosPidParam.threadNum = 1;
   pidPosCtrl_Joint4_Roll.InitPID(&paramJoint.Joint4_RollPosPidParam);
   paramJoint.Joint4_RollSpdPidParam.threadNum = 1;
   pidSpdCtrl_Joint4_Roll.InitPID(&paramJoint.Joint4_RollSpdPidParam);
 
   mtrOutputBuffer.fill(0);
 
   Component_FSMFlag_ = FSM_RESET;
   ComponentState = APP_OK;
 
   return APP_OK;
 
 }
 
 EAppStatus C_ModRoboticArm::C_ComJoint::UpdateComponent(){
   
   if (ComponentState == COM_RESET) return APP_ERROR;
   /*In the Component the Data always raw data,The information at the module level is usually processed*/
   jointInfo.angle_joint1_yaw     = motor[Y]->motorData[C_MtrInstance::DATA_POSIT];
   jointInfo.angle_joint2_pitch   = motor[P1]->motorData[C_MtrInstance::DATA_POSIT];
   jointInfo.angle_joint3_pitch   = motor[P2]->motorData[C_MtrInstance::DATA_POSIT];
   jointInfo.angle_joint4_roll    = motor[R]->motorData[C_MtrInstance::DATA_POSIT];
 
   jointInfo.isAngleArrived_joint1_yaw    = (abs(jointCmd.setAngle_joint1_yaw   - jointInfo.angle_joint1_yaw)   < 1000);
   jointInfo.isAngleArrived_joint2_pitch  = (abs(jointCmd.setAngle_joint2_pitch - jointInfo.angle_joint2_pitch) < 1500);
   jointInfo.isAngleArrived_joint3_pitch  = (abs(jointCmd.setAngle_joint3_pitch - jointInfo.angle_joint3_pitch) < 1500);
   jointInfo.isAngleArrived_joint4_roll   = (abs(jointCmd.setAngle_joint4_roll  - jointInfo.angle_joint4_roll)  < 1500);
 
   switch (Component_FSMFlag_) {
     case FSM_RESET: {
       mtrOutputBuffer.fill(0);
       pidPosCtrl_Joint1_Yaw.ResetPidController();
       pidSpdCtrl_Joint1_Yaw.ResetPidController();
       pidPosCtrl_Joint2_Pitch.ResetPidController();
       pidSpdCtrl_Joint2_Pitch.ResetPidController();
       pidPosCtrl_Joint3_Pitch.ResetPidController();
       pidSpdCtrl_Joint3_Pitch.ResetPidController();
       pidPosCtrl_Joint4_Roll.ResetPidController();
       pidSpdCtrl_Joint4_Roll.ResetPidController();
       return APP_OK;
     }
 
     case FSM_PREINIT:{
       motor[Y]->motorData[C_MtrInstance::DATA_POSIT]  = motor[Y]->motorData[C_MtrInstance::DATA_ANGLE] + POSIT_JOINT1_YAW_MACH;
       while(motor[Y]->motorData[C_MtrInstance::DATA_POSIT] > 65535)
         motor[Y]->motorData[C_MtrInstance::DATA_POSIT] -= 131070;
 
       motor[P1]->motorData[C_MtrInstance::DATA_POSIT] = motor[P1]->motorData[C_MtrInstance::DATA_ANGLE] + POSIT_JOINT2_PITCH_MACH;
       while (motor[P1]->motorData[C_MtrInstance::DATA_POSIT] > 32767)
         motor[P1]->motorData[C_MtrInstance::DATA_POSIT] -= 65536;
 
       motor[P2]->motorData[C_MtrInstance::DATA_POSIT] = motor[P2]->motorData[C_MtrInstance::DATA_ANGLE] + POSIT_JOINT3_PITCH_MACH;
       while (motor[P2]->motorData[C_MtrInstance::DATA_POSIT] > 32767)
         motor[P2]->motorData[C_MtrInstance::DATA_POSIT] -= 65536;
 
       motor[R]->motorData[C_MtrInstance::DATA_POSIT]  = motor[R]->motorData[C_MtrInstance::DATA_ANGLE] - POSIT_JOINT4_ROLL_MACH;
       while (motor[R]->motorData[C_MtrInstance::DATA_POSIT] > 32767)
         motor[R]->motorData[C_MtrInstance::DATA_POSIT] -= 65536;
 
       Component_FSMFlag_ = FSM_INIT;
       return APP_OK;
     }
 
     case FSM_INIT: {
       mtrOutputBuffer.fill(0);
       if( abs(jointInfo.angle_joint1_yaw) < 500 
           && abs(jointInfo.angle_joint2_pitch) < 500 
           && abs(jointInfo.angle_joint3_pitch) < 500
           && abs(jointInfo.angle_joint4_roll) < 500
         ){
           jointCmd.setAngle_joint1_yaw   = 0;
           jointCmd.setAngle_joint2_pitch = 0;
           jointCmd.setAngle_joint3_pitch = 0;
           jointCmd.setAngle_joint4_roll  = 0;
 
           pidPosCtrl_Joint1_Yaw.ResetPidController();
           pidSpdCtrl_Joint1_Yaw.ResetPidController();
           pidPosCtrl_Joint2_Pitch.ResetPidController();
           pidSpdCtrl_Joint2_Pitch.ResetPidController();
           pidPosCtrl_Joint3_Pitch.ResetPidController();
           pidSpdCtrl_Joint3_Pitch.ResetPidController();
           pidPosCtrl_Joint4_Roll.ResetPidController();
           pidSpdCtrl_Joint4_Roll.ResetPidController();
           Component_FSMFlag_ = FSM_CTRL;
           ComponentState = APP_OK;
           return APP_OK;
         }
       return _UpdateOutput(0.0f,0.0f,0.0f,0.0f);
     }
 
     case FSM_CTRL:{
       jointCmd.setAngle_joint1_yaw   = std::clamp(jointCmd.setAngle_joint1_yaw,   static_cast<int32_t>(-(rangeLimit_yaw/2)), (rangeLimit_yaw/2));
       jointCmd.setAngle_joint2_pitch = std::clamp(jointCmd.setAngle_joint2_pitch, static_cast<int32_t>(-(POSIT_JOINT2_PITCH_DOGHOLE+rangeLimit_pitch_1-POSIT_JOINT2_PITCH_MACH)),static_cast<int32_t>((POSIT_JOINT2_PITCH_MACH-POSIT_JOINT2_PITCH_DOGHOLE)));
       jointCmd.setAngle_joint3_pitch = std::clamp(jointCmd.setAngle_joint3_pitch, static_cast<int32_t>(-(POSIT_JOINT3_PITCH_DOGHOLE+rangeLimit_pitch_2-POSIT_JOINT3_PITCH_MACH)),static_cast<int32_t>((POSIT_JOINT3_PITCH_MACH-POSIT_JOINT3_PITCH_DOGHOLE)));
       jointCmd.setAngle_joint4_roll  = std::clamp(jointCmd.setAngle_joint4_roll,  static_cast<int32_t>(-(rangeLimit_roll/2)),  rangeLimit_roll/2);
       return _UpdateOutput(static_cast<float_t>(jointCmd.setAngle_joint1_yaw),
                             static_cast<float_t>(jointCmd.setAngle_joint2_pitch),
                             static_cast<float_t>(jointCmd.setAngle_joint3_pitch),
                             static_cast<float_t>(jointCmd.setAngle_joint4_roll));
     }
     default: {
       StopComponent();
       mtrOutputBuffer.fill(0);
       pidPosCtrl_Joint1_Yaw.ResetPidController();
       pidSpdCtrl_Joint1_Yaw.ResetPidController();
       pidPosCtrl_Joint2_Pitch.ResetPidController();
       pidSpdCtrl_Joint2_Pitch.ResetPidController();
       pidPosCtrl_Joint3_Pitch.ResetPidController();
       pidSpdCtrl_Joint3_Pitch.ResetPidController();
       pidPosCtrl_Joint4_Roll.ResetPidController();
       pidSpdCtrl_Joint4_Roll.ResetPidController();
       ComponentState = APP_ERROR;
       return APP_ERROR;
     }
   }
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166 *2 
  */
 float_t C_ModRoboticArm::C_ComJoint::MtrPositToPhyPosit_Yaw(int32_t mtrPosit) {
 
   const int32_t zeroOffset = 0;
   const float_t scale = 364.08333f;
 
   return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 float_t C_ModRoboticArm::C_ComJoint::MtrPositToPhyPosit_Pitch_1(int32_t mtrPosit) {
 
   const int32_t zeroOffset = 1773.0f;     
   const float_t scale = -182.04166f;
 
   return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 float_t C_ModRoboticArm::C_ComJoint::MtrPositToPhyPosit_Pitch_2(int32_t mtrPosit) {
 
   const int32_t zeroOffset = 100.0f;
   const float_t scale = -182.04166f;
 
   return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 float_t C_ModRoboticArm::C_ComJoint::MtrPositToPhyPosit_Roll(int32_t mtrPosit) {
 
   const int32_t zeroOffset = 0;
   const float_t scale = 182.04166f;
 
   return (static_cast<float_t>(mtrPosit - zeroOffset) / scale);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 int32_t C_ModRoboticArm::C_ComJoint::PhyPositToMtrPosit_Yaw(float_t phyPosit) {
 
   const int32_t zeroOffset = 0;
   const float_t scale = 364.08333f;
 
   return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 int32_t C_ModRoboticArm::C_ComJoint::PhyPositToMtrPosit_Pitch_1(float_t phyPosit) {
 
   const int32_t zeroOffset = 1773.0f;
   const float_t scale = -182.04166f;
 
   return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 int32_t C_ModRoboticArm::C_ComJoint::PhyPositToMtrPosit_Pitch_2(float_t phyPosit) {
 
   const int32_t zeroOffset = 100.0f;
   const float_t scale = -182.04166f;
 
   return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
 }
 
 /**
  * @brief the scale is encoder/360 -> 65535/360 = 182.04166
  */
 int32_t C_ModRoboticArm::C_ComJoint::PhyPositToMtrPosit_Roll(float_t phyPosit) {
 
   const int32_t zeroOffset = 0;
   const float_t scale = 182.04166f;
 
   return (static_cast<int32_t>(phyPosit * scale) + zeroOffset);
 }
 
 
 EAppStatus C_ModRoboticArm::C_ComJoint::_UpdateOutput(float_t yaw, float_t pitch_1, float_t pitch_2, float_t roll) {
 
   DataBuffer<float_t> jointPos_Yaw =      {static_cast<float_t>(yaw)};
   DataBuffer<float_t> jointPos_Pitch_1 =  {static_cast<float_t>(pitch_1)};
   DataBuffer<float_t> jointPos_Pitch_2 =  {static_cast<float_t>(pitch_2)};
   DataBuffer<float_t> jointPos_Roll =     {static_cast<float_t>(roll)};
 
 
   DataBuffer<float_t> jointPosMeasure_Yaw =     {static_cast<float_t>(motor[Y]->motorData[C_MtrInstance::DATA_POSIT])};
   DataBuffer<float_t> jointPosMeasure_Pitch_1 = {static_cast<float_t>(motor[P1]->motorData[C_MtrInstance::DATA_POSIT])};
   DataBuffer<float_t> jointPosMeasure_Pitch_2 = {static_cast<float_t>(motor[P2]->motorData[C_MtrInstance::DATA_POSIT])};
   DataBuffer<float_t> jointPosMeasure_Roll =    {static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_POSIT])};
 
   auto jointYawSpd = pidPosCtrl_Joint1_Yaw.UpdatePidController(jointPos_Yaw, jointPosMeasure_Yaw);
   auto jointPitch1Spd = pidPosCtrl_Joint2_Pitch.UpdatePidController(jointPos_Pitch_1, jointPosMeasure_Pitch_1);
   auto jointPitch2Spd = pidPosCtrl_Joint3_Pitch.UpdatePidController(jointPos_Pitch_2, jointPosMeasure_Pitch_2);
   auto jointRollSpd = pidPosCtrl_Joint4_Roll.UpdatePidController(jointPos_Roll, jointPosMeasure_Roll);
 
   DataBuffer<float_t> jointYawSpdMeasure    = {static_cast<float_t>(motor[Y]->motorData[C_MtrInstance::DATA_SPEED])};
   DataBuffer<float_t> jointPitch1SpdMeasure = {static_cast<float_t>(motor[P1]->motorData[C_MtrInstance::DATA_SPEED])};
   DataBuffer<float_t> jointPitch2SpdMeasure = {static_cast<float_t>(motor[P2]->motorData[C_MtrInstance::DATA_SPEED])};
   DataBuffer<float_t> jointRollSpdMeasure   = {static_cast<float_t>(motor[R]->motorData[C_MtrInstance::DATA_SPEED])};
 
   auto outputYaw = pidSpdCtrl_Joint1_Yaw.UpdatePidController(jointYawSpd, jointYawSpdMeasure);
   auto outputPitch1 = pidSpdCtrl_Joint2_Pitch.UpdatePidController(jointPitch1Spd, jointPitch1SpdMeasure);
   auto outputPitch2 = pidSpdCtrl_Joint3_Pitch.UpdatePidController(jointPitch2Spd, jointPitch2SpdMeasure);
   auto outputRoll = pidSpdCtrl_Joint4_Roll.UpdatePidController(jointRollSpd, jointRollSpdMeasure);
 
   mtrOutputBuffer = {
     static_cast<int16_t>(outputYaw[0]),
     static_cast<int16_t>(outputPitch1[0]),
     static_cast<int16_t>(outputPitch2[0]),
     static_cast<int16_t>(outputRoll[0]),
   };
 
   return APP_OK;
 }
 
 }
