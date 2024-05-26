#ifndef SERIAL_H
#define SERIAL_H

#include "Common.h"

#include <QMessageBox>
#include <qqueue.h>

#if WINDOW_VER
#include <Windows.h>
#include <queue>

#if CANS
#pragma comment(lib, "lib\\CANProPlus_64.lib")

#define HIGH_MODE_SPEED       500
#define LOW_MODE_SPEED        100

#define CAN_LISTEN_MODE         1
#define CAN_NO_LISTEN_MODE      1

#endif
#endif

#if CANS
#define HIGH_RX_TIME    500
#define LOW_RX_TIME     125

#define HIGH_TX_TIME    500
#define LOW_TX_TIME     125

#define QUE_MAX         4096

const unsigned char HIGH_SPD_CONFIG[] = { 7, 3, 3, 2 };
const unsigned char LOW_SPD_CONFIG[] = { 39, 3, 3, 2 };
#endif

// CAN Message Structure (22 byte)
//typedef struct
//{
//    union _info
//    {
//        unsigned char info;
//        struct
//        {
//            unsigned char reserved : 3;
//            unsigned char hi_volt : 1;		// 0 : Normal, 1 : High-Voltage Message(Only Single-Wire CAN)
//            unsigned char extended : 1;		// 0 : Standard, 1 : Extended
//            unsigned char remote : 1;		// 0 : Data, 1 : Remote
//            unsigned char type : 2;		// 0 : RX, 1 : TX, 2 : Error
//        } BITS;
//    } info;
//
//    unsigned long long time;		// Time Stamp
//    unsigned long id;				// ID
//    unsigned char dlc;				// Data length count
//    unsigned char data[8];			// Data
//} CAN_MESSAGE;


typedef class Serial{
    //Q_OBJECT // Signal 전달 방식으로 처러할려헀으나, vs2022 qt 에러 발생
private:
    //window variable
#if WINDOW_VER
    HANDLE serialHandle = NULL;
    HANDLE traceThreadHandle = NULL;
    HANDLE traceThreadMtx = NULL;
    BOOL traceThreadEnable = FALSE;
    BOOL traceThreadStatus = FALSE;
    DCB* currentDcb = nullptr;
    DCB* preDcb = nullptr;
    COMMTIMEOUTS* preTimeout = nullptr;
    COMMTIMEOUTS* currentTimeout = nullptr;
    //CAN Variable
#if CANS
    CAN* can                                = nullptr;
    CAN_DEVICE_INFO* devLists               = nullptr;
    CAN_CONFIGURATION* canBaudrateSetting   = nullptr;
    bool traceStatus                        = NULL;
    QQueue<CAN_MESSAGE> canMsgQue;
#endif
#endif

    //linux variable
#if LINUX_VER

#endif
    //common variable
    char* serialPort = nullptr;
    int rxTime = 0, txTime = 0;
    canST* writePreCanFrame = nullptr, * writeCurrentCanFrame = nullptr;
    canST* readPreCanFrame = nullptr, * readCurrentCanFrame = nullptr;

    //qt value
    QMessageBox* preMsgBox = nullptr;
    QCheckBox* portBox = nullptr;

    //Window Function
    //CAN Function
#if CANS
private:
    bool SetCanComponent();
    bool SetCanTrace();
    static DWORD WINAPI GetCanTrace(void* arg = nullptr);
    
public:
    bool SetCanDeviceCon(int devNum);
    bool GetCanDevices(QVector<CommonDeviceList>* lists);
    bool GetCanConStatus();
    bool CanDisconnect();
    bool SetBitrateConfig(int bps);
    bool GetBitrateConfig();
    bool SetTraceThreadControl(bool ctl);
    bool GetCanMessages(QVector<CAN_MESSAGE>* canMsgs);
    
#endif

protected:
    HANDLE GetTraceThreadMtx();

private:
    //Common Function
    bool GetPortInfoSetting();
    void SetPortInfoSetting();
    bool SetSerialComponent(const char port[]);
    bool ReadData(void* readmsg = nullptr);
    bool WriteData(void* writeMsg);

    int SetInitCanFrame(canST** preCanFrame, canST** currentCanFrame);

    void InitCompVal(int readTime = 0, int writeTime = 0);
    void wMessage(const char* msg);
    void wDebug(const char* msg);
    void eMessage(const char* msg);

    bool SetSerialPort(const char* port, int len);

public:
    Serial(const char* port = nullptr);
    ~Serial();
    bool StartMsgTrace();
    bool StopMsgTrace();
    bool WriteCanMessage(QVector<CAN_MESSAGE>* writeMsg);

}BGY_CAN;


#endif // SERIAL_H
