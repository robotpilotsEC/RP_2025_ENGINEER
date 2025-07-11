/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2024-12-19
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-03-19   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 */

#ifndef MOD_COMMON_HPP
#define MOD_COMMON_HPP

#include "Configuration.hpp"
#include "Interface.hpp"
#include "Device.hpp"
#include "conf_CanTxNode.hpp"

namespace my_engineer {

/**
 * @brief Module Instance Class
 */
class C_ModInstance {
  friend void StartUpdateTask(void *arg);
  friend void StartHeartbeatTask(void *arg);
protected:

  struct S_ModInitParam {
    E_ModuleID moduleId = E_ModuleID::MOD_NULL;
  };

  FSM_Flag Module_FSMFlag_ = FSM_RESET;

  virtual void UpdateHandler_() { }
  virtual void HeartbeatHandler_() { }
  /*Create module tasks (tasks are declared in subclass definitions and defined in proc_xxx.cpp)*/
  virtual EAppStatus CreateModuleTask_() { return APP_ERROR; }

  EAppStatus RegisterModule_();
  EAppStatus UnregisterModule_();

public:

  /**
   * @brief Module Component Class
   */
  class C_Component {
  protected:
    FSM_Flag Component_FSMFlag_ = FSM_RESET;
  public:

    EAppStatus ComponentState = COM_RESET;

    C_Component() = default;

    virtual ~C_Component() = default;

    virtual EAppStatus InitComponent(S_ModInitParam &param) = 0;
    virtual EAppStatus StartComponent();
    virtual EAppStatus StopComponent();
    virtual EAppStatus UpdateComponent() { return APP_ERROR; }
    virtual EAppStatus HeartbeatComponent() { return APP_ERROR; }
  };
  E_ModuleID moduleId = E_ModuleID::MOD_NULL;

  EAppStatus moduleState = COM_RESET;

  TaskHandle_t  moduleTaskHandle = nullptr;

  C_ModInstance() = default;

  virtual ~C_ModInstance() { UnregisterModule_(); };

  virtual EAppStatus InitModule(S_ModInitParam &param) = 0;

  virtual EAppStatus StartModule();

  virtual EAppStatus StopModule();

  virtual EAppStatus ModuleSubInfo() { return APP_ERROR; }
  virtual EAppStatus ModulePubInfo() { return APP_ERROR; }

};
extern std::map<E_ModuleID, C_ModInstance *> ModuleMap;

}

#endif 
