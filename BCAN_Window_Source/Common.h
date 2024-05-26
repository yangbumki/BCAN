#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <cstdlib>


#define WINDOW_VER                  1
#define LINUX_VER                   0
#define DEBUGGING                   1
#define SERIAL                      0
#define XLSX                        1
#define CANS                        1

#define NOT_EXIST                   -1

#define CAN_BAUDRATE_PRESET_NUM		11

#if CANS
#include "Extern/CANProPlus.h"
#pragma comment(lib, "lib\\CANProPlus_64.lib")
// CAN Message Structure (22 byte)

#define HIGH_MODE_SPEED       500
#define LOW_MODE_SPEED        100

class QString;

typedef struct canFrameStructs {
    unsigned int cnaID = NULL;
    unsigned char canDlc = NULL;
    unsigned char canData[8] = { NULL };
}canST;

typedef struct CommonDeviceList {
    int devNumber = NULL;
    QString* devType;
}comDevList;

typedef struct canSpeedProperty
{
    unsigned char presdiv;
    unsigned char propseg;
    unsigned char pseg1;
    unsigned char pseg2;
} CAN_SPEED_PROPERTY;

//CAN_SPEED_PROPERTY CANBaudratePreset[CAN_BAUDRATE_PRESET_NUM] =
//{
//    // {presdiv, propseg, pseg1, pseg2}
//    {3, 3, 3, 2},		// 1000 Kbps
//    {4, 3, 3, 2},		// 800 Kbps
//    {7, 3, 3, 2},		// 500 Kbps
//    {15, 3, 3, 2},		// 250 Kbps
//    {19, 3, 3, 2},		// 200 Kbps
//    {31, 3, 3, 2},		// 125 Kbps
//    {39, 3, 3, 2},		// 100 Kbps
//    {47, 3, 3, 2},		// 83.3 Kbps
//    {79, 3, 3, 2},		// 50 Kbps
//    {119, 3, 3, 2},		// 33.3 Kbps
//    {199, 3, 3, 2},		// 20 Kbps
//};
#endif







#endif // COMMON_H
