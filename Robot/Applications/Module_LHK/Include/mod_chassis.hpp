/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2024-12-19
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-05-10   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 */

#ifndef MOD_CHASSIS_HPP
#define MOD_CHASSIS_HPP

#include "mod_common.hpp"

namespace  my_engineer {

/**
 * @brief Chassis Module Class
 */
class C_ModChassis final : public C_ModInstance {
public:

  struct S_ModInitParam_Chassis : public S_ModInitParam {
    E_DeviceID memsDevID = E_DeviceID::DEV_NULL;
    E_DeviceID wheelsetMotorID_LF = E_DeviceID::DEV_NULL;
    E_DeviceID wheelsetMotorID_RF = E_DeviceID::DEV_NULL;
    E_DeviceID wheelsetMotorID_LB = E_DeviceID::DEV_NULL;
    E_DeviceID wheelsetMotorID_RB = E_DeviceID::DEV_NULL;
    C_CanInterface::C_CanTxNode *wheelsetMotorTxNode_LF;
    C_CanInterface::C_CanTxNode *wheelsetMotorTxNode_RF;
    C_CanInterface::C_CanTxNode *wheelsetMotorTxNode_LB;
    C_CanInterface::C_CanTxNode *wheelsetMotorTxNode_RB;
    C_AlgoPid::S_AlgoInitParam_Pid wheelsetSpeedPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid yawCorrectionPidParam;
  };

  struct S_ChassisInfo {
    E_FuncStatus isModuleAvailable = false;    ///< Is Chassis Module Available
    float_t speed_X = 0;                      ///< Chassis Speed X (Range: -100% ~ 100%)
    float_t speed_Y = 0;                      ///< Chassis Speed Y (Range: -100% ~ 100%)
    float_t speed_W = 0;                      ///< Chassis Speed W (Range: -100% ~ 100%)
  } chassisInfo;

  struct S_ChassisCmd {
    float_t speed_X = 0;    ///< Chassis Speed X (Range: -100% ~ 100%)
    float_t speed_Y = 0;    ///< Chassis Speed Y (Range: -100% ~ 100%)
    float_t speed_W = 0;    ///< Chassis Speed W (Range: -100% ~ 100%)
  } chassisCmd;

  C_ModChassis() = default;

  explicit C_ModChassis(S_ModInitParam &param) { InitModule(param); }

  ~C_ModChassis() final { UnregisterModule_(); }

  EAppStatus InitModule(S_ModInitParam &param) final;

private:

  /**
   * @brief Chassis Wheelset Component Class
   */
  class C_ComWheelset final : public C_Component {
  public:

    enum {LF = 0, RF = 1, LB = 2, RB = 3};

    struct S_WheelsetInfo {
      float_t speed_LF = 0.0f;    ///< Chassis Speed LF (Unit: rpm)
      float_t speed_RF = 0.0f;    ///< Chassis Speed RF (Unit: rpm)
      float_t speed_LB = 0.0f;    ///< Chassis Speed LB (Unit: rpm)
      float_t speed_RB = 0.0f;    ///< Chassis Speed RB (Unit: rpm)
    } wheelsetInfo;

    struct S_WheelsetCommand {
      float_t speed_X = 0.0f;    ///< Chassis Speed X (Range: -100% ~ 100%)
      float_t speed_Y = 0.0f;    ///< Chassis Speed Y (Range: -100% ~ 100%)
      float_t speed_W = 0.0f;    ///< Chassis Speed W (Range: -100% ~ 100%)
    } wheelsetCmd;

    C_MemsInstance *mems = nullptr;

    C_MtrInstance *motor[4] = {nullptr };
    /*PID Controller*/
    C_AlgoPid pidYawCtrl;
    C_AlgoPid pidSpdCtrl;
    /*Motor Data output buffer*/
    std::array<int16_t, 4> mtrOutputBuffer = {0};

    EAppStatus InitComponent(S_ModInitParam &param) final;

    EAppStatus UpdateComponent() final;

    EAppStatus _UpdateOutput(float_t speed_X, float_t speed_Y, float_t speed_W);

    std::array<C_CanInterface::C_CanTxNode*, 4> mtrCanTxNode;
  } comWheelset_;
  void UpdateHandler_() final;

  void HeartbeatHandler_() final;

  EAppStatus CreateModuleTask_() final;
  /*Declare Chassis task*/
  static void StartChassisModuleTask(void *arg);
  /*control amount limit function,*/
  EAppStatus RestrictChassisCommand_();

};

}

#endif
