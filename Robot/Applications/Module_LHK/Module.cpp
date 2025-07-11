/**
 * @file        ${PACKAGE_NAME}
 * @version     1.0
 * @date        2025-01-08
 * @author      Sassinak(1752500338@qq.com)
 * @brief 
 * @par History:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author          <th>Description
 * <tr><td>2024-03-19   <td>1.0         <td>qianchen  <td>First Create.
 * </table>
 * Defines the map container of the module, which is used to store instances of all modules.
 * Override module start and stop methods here
 * Override module registration and unregistration methods (because the container is defined here)
 * Rewrite component start and stop methods
 */

#include "Module.hpp"

namespace my_engineer {

/**
 * @brief
 */
std::map<E_ModuleID, C_ModInstance *> ModuleMap;


/**
 * @brief
 * @return
 */
EAppStatus C_ModInstance::StartModule() {

  if (moduleState == COM_RESET) return APP_ERROR;

  Module_FSMFlag_ = FSM_INIT;

  moduleState = COM_BUSY;
  return APP_OK;
}

/**
 * @brief
 * @return
 */
EAppStatus C_ModInstance::StopModule() {

  if (moduleState == COM_RESET) return APP_ERROR;

  Module_FSMFlag_ = FSM_RESET;
  CreateModuleTask_();

  moduleState = APP_OK;
  return APP_OK;
}


/**
 * @brief
 * @return
 */
EAppStatus C_ModInstance::RegisterModule_() {

  if (moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  ModuleMap.insert(std::make_pair(moduleId, this));
  return APP_OK;
}

/**
 * @brief 
 * @return
 */
EAppStatus C_ModInstance::UnregisterModule_() {

  if (moduleId == E_ModuleID::MOD_NULL) return APP_ERROR;

  ModuleMap.erase(moduleId);
  return APP_OK;
}

/**
 * @brief
 * @return
 */
EAppStatus C_ModInstance::C_Component::StartComponent() {

  if (ComponentState == COM_RESET) return APP_ERROR;

  Component_FSMFlag_ = FSM_PREINIT;

  ComponentState = COM_BUSY;
  return APP_OK;
}


/**
 * @brief
 * @return
 */
EAppStatus C_ModInstance::C_Component::StopComponent() {

  if (ComponentState == COM_RESET) return APP_ERROR;

  Component_FSMFlag_ = FSM_RESET;

  ComponentState = APP_OK;
  return APP_OK;
}

}
