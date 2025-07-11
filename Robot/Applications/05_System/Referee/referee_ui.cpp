/******************************************************************************
 * @brief        
 * 
 * @file         referee_ui.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-04-17
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "Core.hpp"

namespace my_engineer {

// 初始化UI
void CSystemReferee::UI_InitDrawing() {

  /* Text - Pump Config */
  pumpTextMsg.header = CDevReferee::SPkgHeader();
  pumpTextMsg.header.len = sizeof(pumpTextMsg) - 9;
  pumpTextMsg.header.cmdId = CDevReferee::ECommandID::ID_ROBOT_MSG;
  pumpTextMsg.header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&pumpTextMsg.header), 4);
  pumpTextMsg.transmitterID = (refereeInfo.robot.robotCamp == 2) ? 100 : 0;
  pumpTextMsg.transmitterID += (refereeInfo.robot.robotID);
  pumpTextMsg.receiverID = (refereeInfo.robot.robotCamp == 2) ? 0x164 : 0x100;
  pumpTextMsg.receiverID += (refereeInfo.robot.robotID);
  pumpTextMsg.messageID = CDevReferee::EMessageID::ID_UI_DRAW_TEXT;
  pumpTextMsg.message.figureConfig.figureName[0] = 0;    // Frame ID
  pumpTextMsg.message.figureConfig.figureName[1] = 0;    // Layer ID
  pumpTextMsg.message.figureConfig.figureName[2] = 0;    // Figure ID
  pumpTextMsg.message.figureConfig.operate = 1;
  pumpTextMsg.message.figureConfig.figureType = 7;
  pumpTextMsg.message.figureConfig.layerID = 0;
  pumpTextMsg.message.figureConfig.details_1 = 20;       // Font Size
  pumpTextMsg.message.figureConfig.posit_X = 920;
  pumpTextMsg.message.figureConfig.posit_Y = 270,
  pumpTextMsg.message.figureConfig.color = 4;
  pumpTextMsg.message.figureConfig.details_2 = 4;        // String Length
  pumpTextMsg.message.figureConfig.width = 2;            // Line Width
  strcpy(reinterpret_cast<char *>(pumpTextMsg.message.text), "PUMP");

  /* Text - Mode Config */
  modeTextMsg.header = CDevReferee::SPkgHeader();
  modeTextMsg.header.len = sizeof(modeTextMsg) - 9;
  modeTextMsg.header.cmdId = CDevReferee::ECommandID::ID_ROBOT_MSG;
  modeTextMsg.header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&modeTextMsg.header), 4);
  modeTextMsg.transmitterID = (refereeInfo.robot.robotCamp == 2) ? 100 : 0;
  modeTextMsg.transmitterID += (refereeInfo.robot.robotID);
  modeTextMsg.receiverID = (refereeInfo.robot.robotCamp == 2) ? 0x164 : 0x100;
  modeTextMsg.receiverID += (refereeInfo.robot.robotID);
  modeTextMsg.messageID = CDevReferee::EMessageID::ID_UI_DRAW_TEXT;
  modeTextMsg.message.figureConfig.figureName[0] = 0;    // Frame ID
  modeTextMsg.message.figureConfig.figureName[1] = 0;    // Layer ID
  modeTextMsg.message.figureConfig.figureName[2] = 1;    // Figure ID
  modeTextMsg.message.figureConfig.operate = 1;
  modeTextMsg.message.figureConfig.figureType = 7;
  modeTextMsg.message.figureConfig.layerID = 0;
  modeTextMsg.message.figureConfig.details_1 = 20;       // Font Size
  modeTextMsg.message.figureConfig.posit_X = 920;
  modeTextMsg.message.figureConfig.posit_Y = 820,
  modeTextMsg.message.figureConfig.color = 4;
  modeTextMsg.message.figureConfig.details_2 = 4;        // String Length
  modeTextMsg.message.figureConfig.width = 2;            // Line Width
  strcpy(reinterpret_cast<char *>(modeTextMsg.message.text), "MODE");

  /* Text - CurMode Config */
  curModeTextMsg.header = CDevReferee::SPkgHeader();
  curModeTextMsg.header.len = sizeof(curModeTextMsg) - 9;
  curModeTextMsg.header.cmdId = CDevReferee::ECommandID::ID_ROBOT_MSG;
  curModeTextMsg.header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&curModeTextMsg.header), 4);
  curModeTextMsg.transmitterID = (refereeInfo.robot.robotCamp == 2) ? 100 : 0;
  curModeTextMsg.transmitterID += (refereeInfo.robot.robotID);
  curModeTextMsg.receiverID = (refereeInfo.robot.robotCamp == 2) ? 0x164 : 0x100;
  curModeTextMsg.receiverID += (refereeInfo.robot.robotID);
  curModeTextMsg.messageID = CDevReferee::EMessageID::ID_UI_DRAW_TEXT;
  curModeTextMsg.message.figureConfig.figureName[0] = 0;    // Frame ID
  curModeTextMsg.message.figureConfig.figureName[1] = 0;    // Layer ID
  curModeTextMsg.message.figureConfig.figureName[2] = 2;    // Figure ID
  curModeTextMsg.message.figureConfig.operate = 1;
  curModeTextMsg.message.figureConfig.figureType = 7;
  curModeTextMsg.message.figureConfig.layerID = 0;
  curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 2);
  curModeTextMsg.message.figureConfig.posit_Y = 780;
  curModeTextMsg.message.figureConfig.color = 1;
  curModeTextMsg.message.figureConfig.details_1 = 25;       // Font Size
  curModeTextMsg.message.figureConfig.details_2 = 4;        // String Length
  curModeTextMsg.message.figureConfig.width = 4;            // Line Width
  strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "NONE");

  /* Figure - State Config */
  stateFigureMsg.header = CDevReferee::SPkgHeader();
  stateFigureMsg.header.len = sizeof(stateFigureMsg) - 9;
  stateFigureMsg.header.cmdId = CDevReferee::ECommandID::ID_ROBOT_MSG;
  stateFigureMsg.header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&stateFigureMsg.header), 4);
  stateFigureMsg.transmitterID = (refereeInfo.robot.robotCamp == 2) ? 100 : 0;
  stateFigureMsg.transmitterID += (refereeInfo.robot.robotID);
  stateFigureMsg.receiverID = (refereeInfo.robot.robotCamp == 2) ? 0x164 : 0x100;
  stateFigureMsg.receiverID += (refereeInfo.robot.robotID);
  stateFigureMsg.messageID = CDevReferee::EMessageID::ID_UI_DRAW_HEPTA;

  stateFigureMsg.message.figureConfig[0].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[0].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[0].figureName[2] = 3;    // Figure ID
  stateFigureMsg.message.figureConfig[0].operate = 1;
  stateFigureMsg.message.figureConfig[0].figureType = 2;
  stateFigureMsg.message.figureConfig[0].layerID = 1;
  stateFigureMsg.message.figureConfig[0].posit_X = 960 - 5;
  stateFigureMsg.message.figureConfig[0].posit_Y = 300,
  stateFigureMsg.message.figureConfig[0].color = 7;
  stateFigureMsg.message.figureConfig[0].details_3 = 10;       // Radius
  stateFigureMsg.message.figureConfig[0].width = 14;           // Line Width

  stateFigureMsg.message.figureConfig[1].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[1].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[1].figureName[2] = 4;    // Figure ID
  stateFigureMsg.message.figureConfig[1].operate = 1;
  stateFigureMsg.message.figureConfig[1].figureType = 2;
  stateFigureMsg.message.figureConfig[1].layerID = 1;
  stateFigureMsg.message.figureConfig[1].posit_X = 920 - 5;
  stateFigureMsg.message.figureConfig[1].posit_Y = 300,
  stateFigureMsg.message.figureConfig[1].color = 7;
  stateFigureMsg.message.figureConfig[1].details_3 = 10;       // Radius
  stateFigureMsg.message.figureConfig[1].width = 14;           // Line Width

  stateFigureMsg.message.figureConfig[2].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[2].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[2].figureName[2] = 5;    // Figure ID
  stateFigureMsg.message.figureConfig[2].operate = 1;
  stateFigureMsg.message.figureConfig[2].figureType = 2;
  stateFigureMsg.message.figureConfig[2].layerID = 1;
  stateFigureMsg.message.figureConfig[2].posit_X = 1000 - 5;
  stateFigureMsg.message.figureConfig[2].posit_Y = 300,
  stateFigureMsg.message.figureConfig[2].color = 7;
  stateFigureMsg.message.figureConfig[2].details_3 = 10;       // Radius
  stateFigureMsg.message.figureConfig[2].width = 14;           // Line Width

  stateFigureMsg.message.figureConfig[3].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[3].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[3].figureName[2] = 6;    // Figure ID
  stateFigureMsg.message.figureConfig[3].operate = 1;
  stateFigureMsg.message.figureConfig[3].figureType = 0;
  stateFigureMsg.message.figureConfig[3].layerID = 1;
  stateFigureMsg.message.figureConfig[3].posit_X = 960 - 420;
  stateFigureMsg.message.figureConfig[3].posit_Y = 0,
  stateFigureMsg.message.figureConfig[3].color = 8;
  stateFigureMsg.message.figureConfig[3].details_4 = 960 - 200;      // End Posit X
  stateFigureMsg.message.figureConfig[3].details_5 = 360;      // End Posit Y
  stateFigureMsg.message.figureConfig[3].width = 2;            // Line Width

  stateFigureMsg.message.figureConfig[4].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[4].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[4].figureName[2] = 7;    // Figure ID
  stateFigureMsg.message.figureConfig[4].operate = 1;
  stateFigureMsg.message.figureConfig[4].figureType = 0;
  stateFigureMsg.message.figureConfig[4].layerID = 1;
  stateFigureMsg.message.figureConfig[4].posit_X = 960 + 420;
  stateFigureMsg.message.figureConfig[4].posit_Y = 0,
  stateFigureMsg.message.figureConfig[4].color = 8;
  stateFigureMsg.message.figureConfig[4].details_4 = 960 + 200;      // End Posit X
  stateFigureMsg.message.figureConfig[4].details_5 = 360;      // End Posit Y
  stateFigureMsg.message.figureConfig[4].width = 2;            // Line Width

  stateFigureMsg.message.figureConfig[5].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[5].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[5].figureName[2] = 8;    // Figure ID
  stateFigureMsg.message.figureConfig[5].operate = 1;
  stateFigureMsg.message.figureConfig[5].figureType = 0;
  stateFigureMsg.message.figureConfig[5].layerID = 1;
  stateFigureMsg.message.figureConfig[5].posit_X = 960 + 200;
  stateFigureMsg.message.figureConfig[5].posit_Y = 360 + 40,
  stateFigureMsg.message.figureConfig[5].color = 8;
  stateFigureMsg.message.figureConfig[5].details_4 = 960 + 200;      // End Posit X
  stateFigureMsg.message.figureConfig[5].details_5 = 360 - 40;      // End Posit Y
  stateFigureMsg.message.figureConfig[5].width = 3;            // Line Width

  stateFigureMsg.message.figureConfig[6].figureName[0] = 0;    // Frame ID
  stateFigureMsg.message.figureConfig[6].figureName[1] = 1;    // Layer ID
  stateFigureMsg.message.figureConfig[6].figureName[2] = 9;    // Figure ID
  stateFigureMsg.message.figureConfig[6].operate = 1;
  stateFigureMsg.message.figureConfig[6].figureType = 0;
  stateFigureMsg.message.figureConfig[6].layerID = 1;
  stateFigureMsg.message.figureConfig[6].posit_X = 960 - 200;
  stateFigureMsg.message.figureConfig[6].posit_Y = 360 + 40,
  stateFigureMsg.message.figureConfig[6].color = 8;
  stateFigureMsg.message.figureConfig[6].details_4 = 960 - 200;      // End Posit X
  stateFigureMsg.message.figureConfig[6].details_5 = 360 - 40;      // End Posit Y
  stateFigureMsg.message.figureConfig[6].width = 3;            // Line Width

  /* Figure - Vision Config */
  visionFigureMsg.header = CDevReferee::SPkgHeader();
  visionFigureMsg.header.len = sizeof(visionFigureMsg) - 9;
  visionFigureMsg.header.cmdId = CDevReferee::ECommandID::ID_ROBOT_MSG;
  visionFigureMsg.header.CRC8 = CCrcValidator::Crc8Calculate(reinterpret_cast<uint8_t *>(&visionFigureMsg.header), 4);
  visionFigureMsg.transmitterID = (refereeInfo.robot.robotCamp == 2) ? 100 : 0;
  visionFigureMsg.transmitterID += (refereeInfo.robot.robotID);
  visionFigureMsg.receiverID = (refereeInfo.robot.robotCamp == 2) ? 0x164 : 0x100;
  visionFigureMsg.receiverID += (refereeInfo.robot.robotID);
  visionFigureMsg.messageID = CDevReferee::EMessageID::ID_UI_DRAW_PENTA;

  visionFigureMsg.message.figureConfig[0].figureName[0] = 0;    // Frame ID
  visionFigureMsg.message.figureConfig[0].figureName[1] = 2;    // Layer ID
  visionFigureMsg.message.figureConfig[0].figureName[2] = 10;    // Figure ID
  visionFigureMsg.message.figureConfig[0].operate = 1;
  visionFigureMsg.message.figureConfig[0].figureType = 1;
  visionFigureMsg.message.figureConfig[0].layerID = 2;
  visionFigureMsg.message.figureConfig[0].posit_X = 960 - 400;
  visionFigureMsg.message.figureConfig[0].posit_Y = 540 + 300;
  visionFigureMsg.message.figureConfig[0].color = 7;
  visionFigureMsg.message.figureConfig[0].width = 2;
  visionFigureMsg.message.figureConfig[0].details_4 = 960 + 400;        // End Posit X
  visionFigureMsg.message.figureConfig[0].details_5 = 540 - 300;        // End Posit Y

  visionFigureMsg.message.figureConfig[1].figureName[0] = 0;    // Frame ID
  visionFigureMsg.message.figureConfig[1].figureName[1] = 2;    // Layer ID
  visionFigureMsg.message.figureConfig[1].figureName[2] = 11;    // Figure ID
  visionFigureMsg.message.figureConfig[1].operate = 1;
  visionFigureMsg.message.figureConfig[1].figureType = 2;
  visionFigureMsg.message.figureConfig[1].layerID = 2;
  visionFigureMsg.message.figureConfig[1].posit_X = 960 + 100;
  visionFigureMsg.message.figureConfig[1].posit_Y = 540 + 100,
  visionFigureMsg.message.figureConfig[1].color = 4;
  visionFigureMsg.message.figureConfig[1].details_3 = 6;       // Radius
  visionFigureMsg.message.figureConfig[1].width = 8;           // Line Width

  visionFigureMsg.message.figureConfig[2].figureName[0] = 0;    // Frame ID
  visionFigureMsg.message.figureConfig[2].figureName[1] = 2;    // Layer ID
  visionFigureMsg.message.figureConfig[2].figureName[2] = 12;    // Figure ID
  visionFigureMsg.message.figureConfig[2].operate = 1;
  visionFigureMsg.message.figureConfig[2].figureType = 2;
  visionFigureMsg.message.figureConfig[2].layerID = 2;
  visionFigureMsg.message.figureConfig[2].posit_X = 960 + 100;
  visionFigureMsg.message.figureConfig[2].posit_Y = 540 - 100,
  visionFigureMsg.message.figureConfig[2].color = 1;
  visionFigureMsg.message.figureConfig[2].details_3 = 6;       // Radius
  visionFigureMsg.message.figureConfig[2].width = 8;           // Line Width

  visionFigureMsg.message.figureConfig[3].figureName[0] = 0;    // Frame ID
  visionFigureMsg.message.figureConfig[3].figureName[1] = 2;    // Layer ID
  visionFigureMsg.message.figureConfig[3].figureName[2] = 13;    // Figure ID
  visionFigureMsg.message.figureConfig[3].operate = 1;
  visionFigureMsg.message.figureConfig[3].figureType = 2;
  visionFigureMsg.message.figureConfig[3].layerID = 2;
  visionFigureMsg.message.figureConfig[3].posit_X = 960 - 100;
  visionFigureMsg.message.figureConfig[3].posit_Y = 540 - 100,
  visionFigureMsg.message.figureConfig[3].color = 2;
  visionFigureMsg.message.figureConfig[3].details_3 = 6;       // Radius
  visionFigureMsg.message.figureConfig[3].width = 8;           // Line Width

  visionFigureMsg.message.figureConfig[4].figureName[0] = 0;    // Frame ID
  visionFigureMsg.message.figureConfig[4].figureName[1] = 2;    // Layer ID
  visionFigureMsg.message.figureConfig[4].figureName[2] = 14;    // Figure ID
  visionFigureMsg.message.figureConfig[4].operate = 1;
  visionFigureMsg.message.figureConfig[4].figureType = 2;
  visionFigureMsg.message.figureConfig[4].layerID = 4;
  visionFigureMsg.message.figureConfig[4].posit_X = 960 + 100;
  visionFigureMsg.message.figureConfig[4].posit_Y = 540 - 100,
  visionFigureMsg.message.figureConfig[4].color = 1;
  visionFigureMsg.message.figureConfig[4].details_3 = 6;       // Radius
  visionFigureMsg.message.figureConfig[4].width = 8;           // Line Width
}

void CSystemReferee::UI_StartStaticTextDrawing_() {
	pumpTextMsg.message.figureConfig.operate = 1;
	pumpTextMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&pumpTextMsg), sizeof(pumpTextMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&pumpTextMsg), sizeof(pumpTextMsg));

	proc_waitMs(200);

	modeTextMsg.message.figureConfig.operate = 1;
	modeTextMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&modeTextMsg), sizeof(modeTextMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&modeTextMsg), sizeof(modeTextMsg));
}

void CSystemReferee::UI_StartCurModeTextDrawing_() {
	curModeTextMsg.message.figureConfig.operate = 1;
	curModeTextMsg.message.figureConfig.details_2 = 4;        // String Length
	curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 2);
	curModeTextMsg.message.figureConfig.posit_Y = 780;
	strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "NONE");
	curModeTextMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&curModeTextMsg), sizeof(curModeTextMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&curModeTextMsg), sizeof(curModeTextMsg));
}

void CSystemReferee::UI_StartStateFigureDrawing_() {
	stateFigureMsg.message.figureConfig[0].operate = 1;
	stateFigureMsg.message.figureConfig[1].operate = 1;
	stateFigureMsg.message.figureConfig[2].operate = 1;
	stateFigureMsg.message.figureConfig[3].operate = 1;
	stateFigureMsg.message.figureConfig[4].operate = 1;
	stateFigureMsg.message.figureConfig[5].operate = 1;
	stateFigureMsg.message.figureConfig[6].operate = 1;
	stateFigureMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&stateFigureMsg), sizeof(stateFigureMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&stateFigureMsg), sizeof(stateFigureMsg));
}

void CSystemReferee::UI_StartVisionFigureDrawing_() {
	visionFigureMsg.message.figureConfig[0].operate = 1;
	visionFigureMsg.message.figureConfig[1].operate = 1;
	visionFigureMsg.message.figureConfig[2].operate = 1;
	visionFigureMsg.message.figureConfig[3].operate = 1;
	visionFigureMsg.message.figureConfig[4].operate = 1;
	visionFigureMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&visionFigureMsg), sizeof(visionFigureMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&visionFigureMsg), sizeof(visionFigureMsg));
}

void CSystemReferee::UI_UpdateCurModeTextDrawing_() {

	std::fill(&curModeTextMsg.message.text[0], &curModeTextMsg.message.text[29], 0);
  if (SystemCore.use_Controller_) {
    curModeTextMsg.message.figureConfig.details_2 = 6;
    curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 3);
    curModeTextMsg.message.figureConfig.posit_Y = 780;
    strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "CUSTOM");
    return;
  }
	switch (SystemCore.currentAutoCtrlProcess_) {

		case CSystemCore::EAutoCtrlProcess::NONE: {
			curModeTextMsg.message.figureConfig.details_2 = 4;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 2);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "NONE");
			break;
		}

		case CSystemCore::EAutoCtrlProcess::RETURN_ORIGIN: {
			curModeTextMsg.message.figureConfig.details_2 = 6;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 3);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "ORIGIN");
			break;
		}

		case CSystemCore::EAutoCtrlProcess::RETURN_DRIVE: {
			curModeTextMsg.message.figureConfig.details_2 = 5;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 2.5);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "DRIVE");
			break;
		}

		case CSystemCore::EAutoCtrlProcess::GROUND_ORE: {
			curModeTextMsg.message.figureConfig.details_2 = 6;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 3);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "GROUND");
			break;
		}


		case CSystemCore::EAutoCtrlProcess::GOLD_ORE: {
			curModeTextMsg.message.figureConfig.details_2 = 4;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 2);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "GOLD");
			break;
		}

		case CSystemCore::EAutoCtrlProcess::EXCHANGE: {
			curModeTextMsg.message.figureConfig.details_2 = 11;
      curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 5.5);
      curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "EXCHANGE");
			break;
		}


    case CSystemCore::EAutoCtrlProcess::DOGHOLE: {
      curModeTextMsg.message.figureConfig.details_2 = 7;
      curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 3.5);
      curModeTextMsg.message.figureConfig.posit_Y = 780;
      strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "DOGHOLE");
      break;
    }

		case CSystemCore::EAutoCtrlProcess::PUSH_ORE: {
			curModeTextMsg.message.figureConfig.details_2 = 4;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 2);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "PUSH");
			break;
		}

		case CSystemCore::EAutoCtrlProcess::POP_ORE: {
			curModeTextMsg.message.figureConfig.details_2 = 3;
			curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 1.5);
			curModeTextMsg.message.figureConfig.posit_Y = 780;
			strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "POP");
			break;
		}

    case CSystemCore::EAutoCtrlProcess::TURNOVER: {
      curModeTextMsg.message.figureConfig.details_2 = 8;
      curModeTextMsg.message.figureConfig.posit_X = 960 - (25 * 4);
      curModeTextMsg.message.figureConfig.posit_Y = 780;
      strcpy(reinterpret_cast<char *>(curModeTextMsg.message.text), "TURNOVER");
      break;
    }
	}

	curModeTextMsg.message.figureConfig.operate = 2;
	curModeTextMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&curModeTextMsg), sizeof(curModeTextMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&curModeTextMsg), sizeof(curModeTextMsg));
}

void CSystemReferee::UI_UpdateStateFigureDrawing_() {

	static auto &subgantry = reinterpret_cast<CModSubGantry *>(ModuleIDMap.at(EModuleID::MOD_SUBGANTRY))->subGantryInfo;

	stateFigureMsg.message.figureConfig[0].operate = 2;
	stateFigureMsg.message.figureConfig[0].color = (subgantry.isPumpOn_Arm) ? 2 : 7;

	stateFigureMsg.message.figureConfig[1].operate = 2;
	stateFigureMsg.message.figureConfig[1].color = (subgantry.isPumpOn_Left) ? 2 : 7;

	stateFigureMsg.message.figureConfig[2].operate = 2;
	stateFigureMsg.message.figureConfig[2].color = (subgantry.isPumpOn_Right) ? 2 : 7;

	stateFigureMsg.message.figureConfig[3].operate = 2;
	stateFigureMsg.message.figureConfig[4].operate = 2;
	stateFigureMsg.message.figureConfig[5].operate = 2;
	stateFigureMsg.message.figureConfig[6].operate = 2;

	stateFigureMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&stateFigureMsg), sizeof(stateFigureMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&stateFigureMsg), sizeof(stateFigureMsg));
}

void CSystemReferee::UI_UpdateVisionFigureDrawing_() {

	visionFigureMsg.message.figureConfig[0].operate = 2;
	visionFigureMsg.message.figureConfig[1].operate = 2;
	visionFigureMsg.message.figureConfig[2].operate = 2;
	visionFigureMsg.message.figureConfig[3].operate = 2;
	visionFigureMsg.message.figureConfig[4].operate = 2;

	if (SysVision.systemStatus != APP_OK) {
		visionFigureMsg.message.figureConfig[0].color = 7;
	} else {
		visionFigureMsg.message.figureConfig[0].color = (SysVision.visionInfo.oreTank.isFoundOreTank) ? 2 : 1;
	}

	if (SysVision.systemStatus == APP_OK && SysVision.visionInfo.uiPoint.isFoundOreTank) {
		visionFigureMsg.message.figureConfig[1].posit_X = 960 + SysVision.visionInfo.uiPoint.pointPosit_X[0] * 1.1;
		visionFigureMsg.message.figureConfig[1].posit_Y = 510 + SysVision.visionInfo.uiPoint.pointPosit_Y[0] * 1.1;
		visionFigureMsg.message.figureConfig[2].posit_X = 960 + SysVision.visionInfo.uiPoint.pointPosit_X[1] * 1.1;
		visionFigureMsg.message.figureConfig[2].posit_Y = 510 + SysVision.visionInfo.uiPoint.pointPosit_Y[1] * 1.1;
		visionFigureMsg.message.figureConfig[3].posit_X = 960 + SysVision.visionInfo.uiPoint.pointPosit_X[2] * 1.1;
		visionFigureMsg.message.figureConfig[3].posit_Y = 510 + SysVision.visionInfo.uiPoint.pointPosit_Y[2] * 1.1;
		visionFigureMsg.message.figureConfig[4].posit_X = 960 + SysVision.visionInfo.uiPoint.pointPosit_X[3] * 1.1;
		visionFigureMsg.message.figureConfig[4].posit_Y = 510 + SysVision.visionInfo.uiPoint.pointPosit_Y[3] * 1.1;
	} else {
		visionFigureMsg.message.figureConfig[1].posit_X = 960 + 400;
		visionFigureMsg.message.figureConfig[1].posit_Y = 540 + 300;
		visionFigureMsg.message.figureConfig[2].posit_X = 960 + 400;
		visionFigureMsg.message.figureConfig[2].posit_Y = 540 - 300;
		visionFigureMsg.message.figureConfig[3].posit_X = 960 - 400;
		visionFigureMsg.message.figureConfig[3].posit_Y = 540 - 300;
		visionFigureMsg.message.figureConfig[4].posit_X = 960 - 400;
		visionFigureMsg.message.figureConfig[4].posit_Y = 540 + 300;
	}

	visionFigureMsg.CRC16 = CCrcValidator::Crc16Calculate(reinterpret_cast<uint8_t *>(&visionFigureMsg), sizeof(visionFigureMsg) - 2);
	pInterface_->Transmit(reinterpret_cast<uint8_t *>(&visionFigureMsg), sizeof(visionFigureMsg));
}

void CSystemReferee::StartSysRefereeUiTask(void *arg) {
	proc_waitUntil(SysReferee.refereeInfo.robot.robotCamp != 0);

	// 初始化UI
	SysReferee.UI_InitDrawing();

	SysReferee.UI_StartStaticTextDrawing_();
	proc_waitMs(100);
	SysReferee.UI_StartCurModeTextDrawing_();
	proc_waitMs(100);
	SysReferee.UI_StartStateFigureDrawing_();
	proc_waitMs(100);
	SysReferee.UI_StartVisionFigureDrawing_();
	proc_waitMs(100);

	// 进入UI绘制循环
	while (true) {

		SysReferee.UI_UpdateCurModeTextDrawing_();
		proc_waitMs(100);

		SysReferee.UI_UpdateVisionFigureDrawing_();
		proc_waitMs(100);

		SysReferee.UI_UpdateStateFigureDrawing_();
		proc_waitMs(100);

		SysReferee.UI_UpdateVisionFigureDrawing_();
		proc_waitMs(100);
	}

	proc_return();
}

} // namespace my_engineer
