/**
 * @file proc_subgantry.cpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 子龙门任务
 * @version 1.0
 * @date 2024-11-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mod_subgantry.hpp"

namespace my_engineer {

/**
 * @brief 创建子龙门任务
 * 
 * @param argument 
 */
void CModSubGantry::StartSubGantryModuleTask(void *argument) {

    // 要求参数为CModSubGantry类的实例，如果传入为空则删除任务并返回
    if (argument == nullptr) proc_return();

    // 类型转换
    auto &subGantry = *static_cast<CModSubGantry *>(argument);

    // 任务循环
    while (true) {
        
        // FSM
        switch (subGantry.Module_FSMFlag_) {
                
            case FSM_RESET: {

                subGantry.subGantryInfo.isModuleAvailable = false;
                subGantry.comPump_.StopComponent();
                subGantry.comLift_.StopComponent();
                subGantry.comStretch_.StopComponent();

                proc_waitMs(20);
                continue; // 跳过下面的代码，直接进入下一次循环
            }

            case FSM_INIT: {

                proc_waitMs(250); // 等待初始化完成

                subGantry.comPump_.StartComponent();
                subGantry.comLift_.StartComponent();
                subGantry.comStretch_.StartComponent();

            //    proc_waitUntil(subGantry.comLift_.componentStatus == APP_OK
            //                && subGantry.comStretch_.componentStatus == APP_OK
            //                && subGantry.comLeft15_.componentStatus == APP_OK);

                subGantry.subGantryCmd = SSubGantryCmd();
                subGantry.subGantryInfo.isModuleAvailable = true;
                subGantry.Module_FSMFlag_ = FSM_CTRL;
                subGantry.moduleStatus = APP_OK;
                continue;
            }

            case FSM_CTRL: {

                // 限制控制量
                subGantry.RestrictSubGantryCommand_();
                
                // 控制气泵
                subGantry.comPump_.pumpCmd.setPumpOn_Arm = subGantry.subGantryCmd.setPumpOn_Arm;
                subGantry.comPump_.pumpCmd.setPumpOn_Left = subGantry.subGantryCmd.setPumpOn_Left;
                subGantry.comPump_.pumpCmd.setPumpOn_Right = subGantry.subGantryCmd.setPumpOn_Right;

                // 将控制量转换后传递给组件
                subGantry.comLift_.liftCmd.setPosit_L = 
                    subGantry.comLift_.PhyPositToMtrPosit_L(subGantry.subGantryCmd.setLiftPosit_L);
                subGantry.comLift_.liftCmd.setPosit_R =
                    subGantry.comLift_.PhyPositToMtrPosit_R(subGantry.subGantryCmd.setLiftPosit_R);
                subGantry.comStretch_.stretchCmd.setPosit_L =
                    subGantry.comStretch_.PhyPositToMtrPosit_L(subGantry.subGantryCmd.setStretchPosit_L);
                subGantry.comStretch_.stretchCmd.setPosit_R =
                    subGantry.comStretch_.PhyPositToMtrPosit_R(subGantry.subGantryCmd.setStretchPosit_R);

                proc_waitMs(1); // 1000Hz
                continue;
            }

            default: { subGantry.StopModule(); }
        }
    }

    // 任务退出
    subGantry.moduleTaskHandle = nullptr;
    proc_return();
}

} // namespace my_engineer
