/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-04-02
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-03-30   <td>1.0         <td>qianchen        <td>First Create.
 * </table>
 */

#ifndef MOD_GIMBAL_HPP
#define MOD_GIMBAL_HPP

#include "mod_common.hpp"

namespace my_engineer {

class C_ModGimbal final : public C_ModInstance {
public:

  struct S_ModGimbalInitParam : public S_ModInitParam{
    TIM_HandleTypeDef *gimbalPitchHalTimHandle = nullptr;
    uint32_t gimbalPitchTimChannel = 0;
  };
  struct S_GimbalInfo {
    E_FuncStatus isModuleAvailable = false;    ///< Is Gimbal Module Available
    uint32_t step_pitch           = 0;        ///< Gimbal Pitch Step (0 - MID, 1 - UP, 2 - DOWN, 3 - VISION)
  } gimbalInfo;

  struct S_GimbalCmd {
    bool isAutoCtrl       = false;         ///< Is Auto Control
    float_t setStep_Pitch = 0;            ///< Set Gimbal Pitch Step (0 - MID, 1 - UP, 2 - DOWN)
  } gimbalCmd;


  C_ModGimbal() = default;

  explicit C_ModGimbal(S_ModInitParam &param) { InitModule(param); }

  ~C_ModGimbal() final { UnregisterModule_(); }

  EAppStatus InitModule(S_ModInitParam &param) final;

private:
  class C_ComPitch final : public C_Component {
    public:

      struct S_PitchCmd {
        uint32_t setStep = 0; // 0 - MID, 1 - UP, 2 - DOWN, 3 - VISION
        uint16_t setMid = 130;
        uint16_t setUp = 195;
        uint16_t setDown = 100;
        uint16_t setVision = 200;
      } pitchCmd;

      TIM_HandleTypeDef *halTimHandle = nullptr;

      uint32_t timChannel = 0;

      EAppStatus InitComponent(S_ModInitParam &param) final;

      EAppStatus UpdateComponent() final;

  } comPitch_;

  void UpdateHandler_() final;

  void HeartbeatHandler_() final;

  EAppStatus CreateModuleTask_() final;

  static void StartGimbalModuleTask(void *arg);

  EAppStatus RestrictGimbalCommand_();
};

}

#endif // MOD_GIMBAL_HPP
