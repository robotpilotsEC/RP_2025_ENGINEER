/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2024-12-20
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-05-11   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 */

#ifndef MOD_TOILETPLUNGER_HPP
#define MOD_TOILETPLUNGER_HPP

#include "mod_common.hpp"

namespace my_engineer {

/*由于本人的恶趣味，刺雷在此命名为马桶搋子*/
class C_ModToiletPlunger final : public C_ModInstance {
public:

  struct S_ModInitParam_ToiletPlunger : public S_ModInitParam {
    E_InterfaceID ToiletPlungerMtrCanInfID = E_InterfaceID::INF_NULL;
    /*Control the Lift of Toilet Plunger.The plunger,on the right, has a wider range of motion */
    E_DeviceID liftMotorID_L = E_DeviceID::DEV_NULL;
    E_DeviceID liftMotorID_R = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid liftPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid liftSpdPidParam;
    C_CanInterface::C_CanTxNode *liftmotorTxNode_R = nullptr;
    C_CanInterface::C_CanTxNode *liftmotorTxNode_L = nullptr;
    /*Control the stretch of Toilet Plunger.*/
    E_DeviceID stretchMotorID_L = E_DeviceID::DEV_NULL;
    E_DeviceID stretchMotorID_R = E_DeviceID::DEV_NULL;
    C_AlgoPid::S_AlgoInitParam_Pid stretchPosPidParam;
    C_AlgoPid::S_AlgoInitParam_Pid stretchSpdPidParam;
    C_CanInterface::C_CanTxNode *stretchmotorTxNode_R = nullptr;
    C_CanInterface::C_CanTxNode *stretchmotorTxNode_L = nullptr;

    /*air pump and Solenoid Valve GPIO setting*/
    GPIO_TypeDef *pumpGpioPort_L = nullptr;
    uint16_t pumpGpioPin_L = 0;
    GPIO_TypeDef *pumpGpioPort_R = nullptr;
    uint16_t pumpGpioPin_R = 0;
    GPIO_TypeDef *pumpGpioPort_ARM = nullptr;
    uint16_t pumpGpioPin_ARM = 0;
    GPIO_TypeDef *svGpioPort_R = nullptr;
    uint16_t svGpioPin_R = 0;
    GPIO_TypeDef *svGpioPort_L = nullptr;
    uint16_t svGpioPin_L = 0;
    GPIO_TypeDef *svGpioPort_M = nullptr;
    uint16_t svGpioPin_M = 0;
  };

  struct S_ToiletPlungerInfo {
    E_FuncStatus isModuleAvailable = false;      ///< Is Toilet Plunger Module Available
    float_t posit_Lift_L = 0.0f;                ///< ToiletPlunger Lift Posit Left (Range: 0mm ~ mm)
    float_t posit_Lift_R = 0.0f;                ///< ToiletPlunger Lift Posit Right (Range: 0mm ~ mm)
    float_t posit_Stretch_L = 0.0f;             ///< ToiletPlunger Stretch Posit Left (Range: 0mm ~ mm)
    float_t posit_Stretch_R = 0.0f;             ///< ToiletPlunger Stretch Posit Right (Range: 0mm ~ mm)
    bool isPositArrived_Lift_L = false;         ///< Is ToiletPlunger Lift Posit Left Arrived
    bool isPositArrived_Lift_R = false;         ///< Is ToiletPlunger Lift Posit Right Arrived
    bool isPositArrived_Stretch_L = false;      ///< Is ToiletPlunger Stretch Posit Left Arrived
    bool isPositArrived_Stretch_R = false;      ///< Is ToiletPlunger Stretch Posit Right Arrived
    bool isPumpOn_L = false;                    ///< Is Left Toilet Plunger Pump On 
    bool isPumpOn_R = false;                    ///< Is right Toilet Plunger Pump On
    bool isPumpOn_ARM = false;                  ///< Is Robotic Arm Pump On
    bool isSVOn_L = false;                      ///< Is Left Solenoid Valve On
    bool isSVOn_R = false;                      ///< Is Right Solenoid Valve On
    bool isSVOn_M = false;                      ///< Is Middle Solenoid Valve On
  } ToiletPlungerInfo;

  struct S_ToiletPlungerCmd {
    bool isAutoCtrl = false;
    float_t setPosit_Lift_L = 0.0f;             ///< Set ToiletPlunger Lift Posit Left (Range: 0mm ~  mm)
    float_t setPosit_Lift_R = 0.0f;             ///< Set ToiletPlunger Lift Posit Right (Range: 0mm ~  mm)
    float_t setPosit_Stretch_L = 0.0f;          ///< Set ToiletPlunger Stretch Posit Left (Range: 0mm ~  mm)
    float_t setPosit_Stretch_R = 0.0f;          ///< Set ToiletPlunger Stretch Posit Right (Range: 0mm ~  mm)
    bool setPumpOn_L = false;                   ///< Set Left Toilet Plunger Pump
    bool setPumpOn_R = false;                   ///< Set right Toilet Plunger Pump
    bool setPumpOn_ARM = false;                 ///< Set Robotic Arm Pump
    bool setSVOn_L = false;                     ///< Set Left Solenoid Valve
    bool setSVOn_R = false;                     ///< Set Right Solenoid Valve
    bool setSVOn_M = false;                     ///< Set Middle Solenoid Valve
  } ToiletPlungerCmd;

  C_ModToiletPlunger() = default;

  explicit C_ModToiletPlunger(S_ModInitParam_ToiletPlunger &param) { InitModule(param); }

  ~C_ModToiletPlunger() final { UnregisterModule_(); }


  EAppStatus InitModule(S_ModInitParam &param) final;

private:

  /*Air Pump Component Instance*/
  class C_ComPump:public C_Component{
    public:
      struct S_PumpInfo{
        bool isPumpOn_L   = false;
        bool isPumpOn_R   = false;
        bool isPumpOn_ARM = false;
        bool isSVOn_L     = false;
        bool isSVOn_R     = false;
        bool isSVOn_M     = false;
      }PumpInfo;

      struct S_PumpCmd{
        bool setPumpOn_L   = false;
        bool setPumpOn_R   = false;
        bool setPumpOn_ARM = false;
        bool setSVOn_L     = false;
        bool setSVOn_R     = false;
        bool setSVOn_M     = false;
      }PumpCmd;

      /*GPIO setting*/
      GPIO_TypeDef *pumpGpioPort_L = nullptr;
      uint16_t pumpGpioPin_L = 0;
      GPIO_TypeDef *pumpGpioPort_R = nullptr;
      uint16_t pumpGpioPin_R = 0;
      GPIO_TypeDef *pumpGpioPort_ARM = nullptr;
      uint16_t pumpGpioPin_ARM = 0;
      GPIO_TypeDef *svGpioPort_R = nullptr;
      uint16_t svGpioPin_R = 0;
      GPIO_TypeDef *svGpioPort_L = nullptr;
      uint16_t svGpioPin_L = 0;
      GPIO_TypeDef *svGpioPort_M = nullptr;
      uint16_t svGpioPin_M = 0;

      EAppStatus InitComponent(S_ModInitParam &apram) final;
      EAppStatus UpdateComponent() final;
      /*declear friend class inorder to call the functions and variables of the current class are called*/
      friend class C_ModRoboticArm;
  }comPump_;

  /*Lift Instance */
  class C_ComLift final : public C_Component {
  public:

    enum { L = 0, R = 1 };
    const int32_t rangeLimit_R = -TPmod_LIMIT_mtrLIFT_R;
    const int32_t rangeLimit_L = TPmod_LIMIT_mtrLIFT_L;

    struct S_LiftInfo {
      int32_t posit_L = 0;
      int32_t posit_R = 0;
      bool isPositArrived_L = false;
      bool isPositArrived_R = false;
    } liftInfo;

    struct S_LiftCmd {
      int32_t setPosit_L = 0;
      int32_t setPosit_R = 0;
    } liftCmd;
    /*Create an array of motor instance pointers*/
    C_MtrInstance *motor[2] = {nullptr};

    C_AlgoPid pidPosCtrl;
    C_AlgoPid pidSpdCtrl;

    std::array<int16_t, 2> mtrOutputBuffer = {0};

    EAppStatus InitComponent(S_ModInitParam &param) final;
    EAppStatus UpdateComponent() final;

    static int32_t PhyPositToMtrPosit_L(float_t phyPosit);
    static float_t MtrPositToPhyPosit_L(int32_t mtrPosit);
    static int32_t PhyPositToMtrPosit_R(float_t phyPosit);
    static float_t MtrPositToPhyPosit_R(int32_t mtrPosit);

    EAppStatus _UpdateOutput(float_t posit_L, float_t posit_R);

    std::array<C_CanInterface::C_CanTxNode*,2> mtrCanTxNode;
    /*declear friend class inorder to call the functions and variables of the current class are called*/
    friend class C_ModRoboticArm;

  } comLift_;

  /*Stretch Instance*/
  class C_ComStretch final : public C_Component {
  public:

    enum { L = 0, R = 1 };

    const int32_t rangeLimit_R =  TPmod_LIMIT_mtrSTRECH_R;
    const int32_t rangeLimit_L = -TPmod_LIMIT_mtrSTRECH_L;

    struct S_StretchInfo {
      int32_t posit_L = 0;
      int32_t posit_R = 0;
      bool isPositArrived_L = false;
      bool isPositArrived_R = false;
    } stretchInfo;

    struct S_StretchCmd {
      int32_t setPosit_L = 0;
      int32_t setPosit_R = 0;
    } stretchCmd;

    C_MtrInstance *motor[2] = {nullptr};

    C_AlgoPid pidPosCtrl;
    C_AlgoPid pidSpdCtrl;

    std::array<int16_t, 2> mtrOutputBuffer = {0};

    EAppStatus InitComponent(S_ModInitParam &param) final;
    EAppStatus UpdateComponent() final;

    static int32_t PhyPositToMtrPosit_L(float_t phyPosit);
    static float_t MtrPositToPhyPosit_L(int32_t mtrPosit);

    static int32_t PhyPositToMtrPosit_R(float_t phyPosit);
    static float_t MtrPositToPhyPosit_R(int32_t mtrPosit);    

    EAppStatus _UpdateOutput(float_t posit_L, float_t posit_R);

    /*declear friend class inorder to call the functions and variables of the current class are called*/
    friend class C_ModRoboticArm;

    std::array<C_CanInterface::C_CanTxNode*,2> mtrCanTxNode;
  } comStretch_;


  C_CanInterface::C_CanTxNode ToiletPlungerMtrCanTxNode_;

  void UpdateHandler_() final;
  void HeartbeatHandler_() final;
  EAppStatus CreateModuleTask_() final;

  static void StartToiletPlungerModuleTask(void *arg);

  EAppStatus RestrictToiletPlungerCommand_();

  /*declear friend class inorder to call the functions and variables of the current class are called*/
  friend class C_ModRoboticArm;

};

}

#endif
