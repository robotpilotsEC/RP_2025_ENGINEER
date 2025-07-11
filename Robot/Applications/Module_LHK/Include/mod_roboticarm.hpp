/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-01-09
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-12-16   <td>1.0         <td>Sassinak  <td>First Create.
 * </table>
 */

#ifndef ROBOTICARM_HPP
#define ROBOTICARM_HPP

#include "mod_common.hpp"

namespace my_engineer {

/**
 * @brief Robotic Arm Module Class
 */
class C_ModRoboticArm final : public C_ModInstance {

public:
  /** 
   * Due to the large force on the first 4 axes of the robotic arm, 
   * the PID coefficient of each motor is different, so this module 
   * should define the motor PID structure separately
  */  
  struct S_ModInitParam_RoboticArm : public S_ModInitParam {
    /*control two motors at the end that include pitch and roll */
    E_DeviceID endMotor_PitchID = E_DeviceID::DEV_NULL;
    E_DeviceID endMotor_RollID = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid endPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid endSpdPidParam;
    C_CanInterface::C_CanTxNode *endMotorTxNode_Roll = nullptr;
    C_CanInterface::C_CanTxNode *endMotorTxNode_Pitch = nullptr;

    /*control the motor at the first joint Yaw*/
    E_DeviceID jointMotor_YawID = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid Joint1_YawPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid Joint1_YawSpdPidParam;
    C_CanInterface::C_CanTxNode *jointMotorTxNode_Yaw = nullptr;

    /*control the motor at the second joint Pitch*/
    E_DeviceID jointMotor_Pitch_1ID = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid Joint2_PitchPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid Joint2_PitchSpdPidParam;
    C_CanInterface::C_CanTxNode *jointMotorTxNode_Pitch_1 = nullptr;

    /*control the motor at the third joint Pitch*/
    E_DeviceID jointMotor_Pitch_2ID = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid Joint3_PitchPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid Joint3_PitchSpdPidParam;
    C_CanInterface::C_CanTxNode *jointMotorTxNode_Pitch_2 = nullptr;
    
    /*control the motor at the fourth joint Roll*/
    E_DeviceID jointMotor_RollID = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid Joint4_RollPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid Joint4_RollSpdPidParam;
    C_CanInterface::C_CanTxNode *jointMotorTxNode_Roll = nullptr;
  };

  struct S_RoboticArmInfo {
    E_FuncStatus isModuleAvailable = false;    ///< Is Robotic Arm Module Available
    /*The following is Euler angle and spatial pose after solving at the end of the robotic arm*/
    float_t Euler_roll = 0.0f;                 ///< Roll Angle (Range: -180° ~ 180°)
    float_t Euler_pitch = 0.0f;                ///< Pitch Angle (Range: -180° ~ 180°)
    float_t Euler_yaw = 0.0f;                  ///< Yaw Angle (Range: -180° ~ 180°)
    int32_t posit_X = 0;                       ///< X Position (Range: mm ~ mm)
    int32_t posit_Y = 0;                       ///< Y Position (Range: mm ~ mm)
    int32_t posit_Z = 0;                       ///< Z Position (Range: mm ~ mm)
    /*The following is the angle of each joint of the robotic arm*/
    float_t Angle_Joint1_yaw = 0.0f;           ///< Yaw Angle (Range: -180° ~ 180°)
    float_t Angle_Joint2_pitch = 0.0f;         ///< Pitch Angle (Range: -180° ~ 180°)
    float_t Angle_Joint3_pitch = 0.0f;         ///< Pitch Angle (Range: -180° ~ 180°)
    float_t Angle_Joint4_roll = 0.0f;          ///< Roll Angle (Range: -180° ~ 180°)
    float_t Angle_Joint5_pitch = 0.0f;         ///< Roll Angle (Range: -180° ~ 180°)
    float_t Angle_Joint6_roll = 0.0f;          ///< Roll Angle (Range: -180° ~ 180°)
    bool isEulerArrived_roll = false;          ///< Is Euler Angle Roll Arrived
    bool isEulerArrived_pitch = false;         ///< Is Euler Angle Pitch Arrived
    bool isEulerArrived_yaw = false;           ///< Is Euler Angle Yaw Arrived
    bool isPositArrived_X = false;             ///< Is Position X Arrived
    bool isPositArrived_Y = false;             ///< Is Position Y Arrived
    bool isPositArrived_Z = false;             ///< Is Position Z Arrived
    bool isAngleArrived_joint1_yaw = false;    ///< Is Angle Joint1 Yaw Arrived
    bool isAngleArrived_joint2_pitch = false;  ///< Is Angle Joint2 Pitch Arrived
    bool isAngleArrived_joint3_pitch = false;  ///< Is Angle Joint3 Pitch Arrived
    bool isAngleArrived_joint4_roll = false;   ///< Is Angle Joint4 Roll Arrived
    bool isAngleArrived_joint5_pitch = false;  ///< Is Angle Joint5 Pitch Arrived
    bool isAngleArrived_joint6_roll = false;   ///< Is Angle Joint6 Roll Arrived
  } roboticArmInfo;

  struct S_RoboticArmCmd {
    /*Spatial pose setting*/
    float_t setEuler_roll = 0.0f;                     ///< Roll Angle (Range: -180° ~ 180°)
    float_t setEuler_pitch = 0.0f;                    ///< Pitch Angle (Range: -180° ~ 180°)
    float_t setEuler_yaw = 0.0f;                      ///< Yaw Angle (Range: -180° ~ 180°)
    int32_t setPosit_X = 0;                           ///< X Position (Range: mm ~ mm)
    int32_t setPosit_Y = 0;                           ///< Y Position (Range: mm ~ mm)
    int32_t setPosit_Z = 0;                           ///< Z Position (Range: mm ~ mm)
    /*Each angle setting*/
    float_t setAngle_joint1_yaw = 0.0f;               ///< Yaw Angle (Range: -180° ~ 180°)
    float_t setAngle_joint2_pitch = 6.13f;             ///< Pitch Angle (Range: -180° ~ 180°)
    float_t setAngle_joint3_pitch = 4.8f;             ///< Pitch Angle (Range: -180° ~ 180°)
    float_t setAngle_joint4_roll = 0.0f;              ///< Roll Angle (Range: -180° ~ 180°)
    float_t setAngle_joint5_pitch = -90.0f;             ///< Roll Angle (Range: -180° ~ 180°)
    float_t setAngle_joint6_roll = 0.0f;              ///< Roll Angle (Range: -180° ~ 180°)
    bool isAutoCtrl = false;                           ///< Is Auto Control
    bool isCustomCtrl = false;                         ///< Is Custom Control
    bool isKeyboardCtrl = false;                      ///< Is Keyboard Control
  } roboticArmCmd;

  C_ModRoboticArm() = default;

  explicit C_ModRoboticArm(S_ModInitParam &param) { InitModule(param); }

  ~C_ModRoboticArm() final { UnregisterModule_(); }

  EAppStatus InitModule(S_ModInitParam &param) final;

private:
  class C_ComEnd : public C_Component {
    public:
      
      enum {R = 0, P = 1};

      const int32_t rangeLimit_pitch = RAmod_LIMIT_end;
      const int32_t rangeLimit_roll  = 0;

      struct S_EndInfo {
        int32_t angle_roll_end = 0.0f;
        int32_t angle_pitch_end = 0.0f;
        bool isAngleArrived_roll_end = false;
        bool isAngleArrived_pitch_end = false;
      } endInfo;

      struct S_EndCmd {
        int32_t setAngle_roll_end = 0.0f;
        int32_t setAngle_pitch_end = 0.0f;
      } endCmd;

      C_MtrInstance *motor[2] = {nullptr};

      C_AlgoPid pidPosCtrl;
      C_AlgoPid pidSpdCtrl;

      std::array<int16_t, 2> mtrOutputBuffer = {0};

      EAppStatus InitComponent(S_ModInitParam &param) final;

      EAppStatus UpdateComponent() final;

      static int32_t PhyPositToMtrPosit_Roll(float_t phyPosit);
      static int32_t PhyPositToMtrPosit_Pitch(float_t phyPosit);

      static float_t MtrPositToPhyPosit_Roll(int32_t mtrPosit);
      static float_t MtrPositToPhyPosit_Pitch(int32_t mtrPosit);

      EAppStatus _UpdateOutput(float_t posit_roll, float_t posit_pitch);

      std::array<C_CanInterface::C_CanTxNode*,2> mtrCanTxNode;
  }Comend_;
  /*It include 4 motor*/
  class C_ComJoint : public C_Component {
    public:
      enum {Y = 0, P1 = 1, P2 = 2, R = 3};

      const int32_t rangeLimit_yaw = RAmod_LIMIT_yaw;                 ///< The first joint
      const int32_t rangeLimit_pitch_1 = RAmod_LIMIT_pitch_1;         ///< The second joint
      const int32_t rangeLimit_pitch_2 = RAmod_LIMIT_pitch_2;         ///< The third joint
      const int32_t rangeLimit_roll = RAmod_LIMIT_roll;               ///< The fourth joint

      struct S_JointInfo {
        int32_t angle_joint1_yaw = 0;
        int32_t angle_joint2_pitch = 0;
        int32_t angle_joint3_pitch = 13.0f;
        int32_t angle_joint4_roll = 0;
        bool isAngleArrived_joint1_yaw   = false;
        bool isAngleArrived_joint2_pitch = false;
        bool isAngleArrived_joint3_pitch = false;
        bool isAngleArrived_joint4_roll  = false;
      } jointInfo;

      struct S_JointCmd {
        int32_t setAngle_joint1_yaw   = 0;
        int32_t setAngle_joint2_pitch = 0;
        int32_t setAngle_joint3_pitch = 0;
        int32_t setAngle_joint4_roll  = 0;
      } jointCmd;

      C_MtrInstance *motor[4] = {nullptr};

      C_AlgoPid pidPosCtrl_Joint1_Yaw;
      C_AlgoPid pidSpdCtrl_Joint1_Yaw;
      C_AlgoPid pidPosCtrl_Joint2_Pitch;
      C_AlgoPid pidSpdCtrl_Joint2_Pitch;
      C_AlgoPid pidPosCtrl_Joint3_Pitch;
      C_AlgoPid pidSpdCtrl_Joint3_Pitch;
      C_AlgoPid pidPosCtrl_Joint4_Roll;
      C_AlgoPid pidSpdCtrl_Joint4_Roll;

      std::array<int16_t, 4> mtrOutputBuffer = {0};

      EAppStatus InitComponent(S_ModInitParam &param) final;

      EAppStatus UpdateComponent() final;

      static int32_t PhyPositToMtrPosit_Yaw(float_t phyPosit);
      static int32_t PhyPositToMtrPosit_Pitch_1(float_t phyPosit);
      static int32_t PhyPositToMtrPosit_Pitch_2(float_t phyPosit);
      static int32_t PhyPositToMtrPosit_Roll(float_t phyPosit);

      static float_t MtrPositToPhyPosit_Yaw(int32_t mtrPosit);
      static float_t MtrPositToPhyPosit_Pitch_1(int32_t mtrPosit); 
      static float_t MtrPositToPhyPosit_Pitch_2(int32_t mtrPosit);
      static float_t MtrPositToPhyPosit_Roll(int32_t mtrPosit);

      EAppStatus _UpdateOutput(float_t yaw, float_t pitch_1, float_t pitch_2, float_t roll);

      std::array<C_CanInterface::C_CanTxNode*,4> mtrCanTxNode;
  }Comjoint_;

  void UpdateHandler_() final;

  void HeartbeatHandler_() final;

  EAppStatus CreateModuleTask_() final;
  /*Declear Robotic module task*/
  static void StartRoboticArmModuleTask(void *arg);
  /*Limit*/
  EAppStatus RestrictRoboticArmCommand_();
  
};

}

#endif
