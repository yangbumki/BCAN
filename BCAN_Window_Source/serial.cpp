#include "serial.h"

//Public Function
#if WINDOW_VER
Serial::Serial(const char* port) {
	//InitCompVal();
	//SetSerialComponent(port);
	SetCanComponent();
}
Serial::~Serial() {
	traceThreadEnable = FALSE;
	traceThreadStatus = FALSE;
	ReleaseMutex(traceThreadMtx);
	CloseHandle(traceThreadMtx);
	WaitForSingleObject(serialHandle, INFINITE);
	CloseHandle(serialHandle);
}
#endif

int Serial::SetInitCanFrame(canST** preCanFrame, canST** currentCanFrame) {
	if (*preCanFrame != nullptr) {
		wDebug("Aleady exist CanFrame");
		delete(*preCanFrame);

		if (*currentCanFrame != nullptr) *preCanFrame = *currentCanFrame;

		*currentCanFrame = new canST;
		memset(*currentCanFrame, 0, sizeof(canST));

		return 1;
	}

	*preCanFrame = new canST;
	memset(*preCanFrame, 0, sizeof(canST));

	*currentCanFrame = new canST;
	memset(*currentCanFrame, 0, sizeof(canST));

	return 0;
}

//public fucntion
#if CANS
bool Serial::StartMsgTrace() {
#if DEBUGGING
	if (SetCanTrace())
		return true;
#endif
	if (!can->IsConnected()) {
		wDebug("Failed to start trace");
		wDebug("Plz connect can");
		return false;
	}

	if (traceStatus != false) {
		wDebug("Somethings wrong");
		wDebug("Check your Serial : start");
		return false;
	}

	/*if (traceThreadStatus != FALSE || traceThreadEnable != FALSE) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : start");
		return false;
	}

	traceThreadStatus = true;
	traceThreadEnable = true;*/

	switch (can->StartTrace()) {
	case CAN_OK:
		break;
	case CAN_FAIL:
	case CAN_CONNECTION_FAIL:
	case CAN_TIMEOUT:
	case CAN_INVALID_PARAMETER:
	case CAN_DATA_NOT_EXIST:
	case CAN_TX_MSG_BUFF_FULL:
		wDebug("Somethings wrong");
		wDebug("Check your write-data");
		return false;
		break;
	default:
		break;
	}


	if (!SetCanTrace()) {
		wDebug("Failed to start trace");
		wDebug("Check your serial : code");
		return false;
	}

	traceStatus = true;

	return true;
}
bool Serial::StopMsgTrace() {
#if DEBUGGING
	traceThreadEnable = FALSE;
	if (traceThreadHandle == NULL) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : trace");
		return false;
	}
	if (!CloseHandle(traceThreadHandle)) {
		wDebug("CloseHandle");
		return false;
	}
	traceThreadStatus = FALSE;
	traceThreadHandle = NULL;

	return true;
#endif
	if (!can->IsConnected()) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : stop-msg");
		return false;
	}

	if (traceStatus != true) {
		wDebug("Somethings wrong");
		wDebug("Check your Serial : stop");
		return false;
	}
	traceStatus = false;

	traceThreadEnable = FALSE;
	if (!CloseHandle(traceThreadHandle)) {
		wDebug("Failed to stop trace");
		wDebug("Check your serial : trace");
		return false;
	}

	traceThreadStatus = FALSE;
	traceThreadHandle = NULL;
	

	switch (can->StopTrace()) {
	case CAN_OK:
		break;
	case CAN_FAIL:
	case CAN_CONNECTION_FAIL:
	case CAN_TIMEOUT:
	case CAN_INVALID_PARAMETER:
	case CAN_DATA_NOT_EXIST:
	case CAN_TX_MSG_BUFF_FULL:
		wDebug("Somethings wrong");
		wDebug("Check your write-data");
		return false;
		break;
	default:
		break;
	}

	return true;
}
bool Serial::WriteCanMessage(QVector<CAN_MESSAGE>* writeMsg) {
#if DEBUGGING
	return true;
#endif
	if (!WriteData(writeMsg)) return false;
	 
	switch(can->StartTxMsg()) {
	case CAN_OK:
		break;
	case CAN_FAIL:
	case CAN_CONNECTION_FAIL:
	case CAN_TIMEOUT:
	case CAN_INVALID_PARAMETER:
	case CAN_DATA_NOT_EXIST:
	case CAN_TX_MSG_BUFF_FULL:
		wDebug("Somethings wrong");
		wDebug("Check your write-data");
		return false;
		break;
	default:
		break;
	}

	return true;
}
#endif

//Private Function
//Window Function
//CAN Function
#if CANS
bool Serial::SetCanComponent() {
	if (can != nullptr) {
		wDebug("Aleady exist can");
		wDebug("Check your serial : can");
		CAN::DestroyInstance(can);
	}

	can = CAN::CreateInstance();
	if (can == nullptr) {
		wDebug("Failed to create CAN-instance");
		return false;
	}
}
bool Serial::SetCanTrace() {
	if (traceThreadHandle != NULL) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : thread");
		goto RE_TRACE_THREAD;
	}


	if (traceThreadStatus != FALSE || traceThreadEnable != FALSE) {
		wDebug("Somethings wrong");
		wDebug("Check your seiral : code");

		traceThreadStatus = FALSE;
		traceThreadEnable = FALSE;

		return false;
	}

	if (traceThreadMtx != NULL) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : trace-mtx");
		CloseHandle(traceThreadMtx);
		traceThreadMtx = NULL;
	}
	

	traceThreadEnable = TRUE;
	traceThreadStatus = TRUE;
	traceThreadMtx = CreateMutex(NULL, TRUE, NULL);
	if (traceThreadMtx == NULL) {
		wDebug("Failed to create Mtx");
		wDebug("Check your serial : trace-mtx");
		return false;
	}

	
	traceThreadHandle = CreateThread(NULL, NULL, GetCanTrace, this, NULL, NULL);
	if (traceThreadHandle == NULL) {
		wDebug("Failed to create thread");
		wDebug("Check your serial : trhead");
		return false;
	}
RE_TRACE_THREAD:
	return true;
}
DWORD WINAPI Serial::GetCanTrace(void* arg) {
	if (arg == nullptr) return -1;
	Serial* serial = (Serial*)arg;
	int msgCnt = -1;
#if DEBUGGING 
	while (serial->traceThreadEnable) {
		Sleep(100);
		if (serial->traceThreadStatus == FALSE) {
			if(WaitForSingleObject(serial->GetTraceThreadMtx(), INFINITE))
				continue;
			ReleaseMutex(serial->GetTraceThreadMtx());
		}
		msgCnt = 1;
		if (msgCnt > 0) {
			if (!serial->ReadData(&serial->canMsgQue)) {
				serial->eMessage("Failed to read-data");
				return 0;
			}

			int size = serial->canMsgQue.size();
			if (size > QUE_MAX) {
				serial->wDebug("Too many Information");
				serial->wDebug("Check your serial : code");
				serial->canMsgQue.clear();
			}
		}

	}

	return 0;
#else
	while (serial->traceThreadEnable) {
		Sleep(1); //CPU »ç¿ë·ü Á¶Á¤

		if (serial->traceThreadStatus == FALSE) {
		if (WaitForSingleObject(serial->GetTraceThreadMtx(), INFINITE))
				continue;
			ReleaseMutex(serial->GetTraceThreadMtx());
		}
		else {
			msgCnt = serial->can->GetTraceMsgCount();
			if (msgCnt > 0) {
				if (!serial->ReadData(&serial->canMsgQue)) {
					serial->eMessage("Failed to read-data");
					return 0;
				}

				int size = serial->canMsgQue.size();
				if (size > QUE_MAX) {
					serial->wDebug("Too many Information");
					serial->wDebug("Check your serial : code");
					serial->canMsgQue.clear();
				}
			}
		}
	}

	return 0;
#endif
}

bool Serial::SetCanDeviceCon(int devNum) {
#if DEBUGGING
	return true;
#endif
	if (devLists == nullptr) {
		wDebug("Failed to read devLists");
		return false;
	}

	if (!can->Connect(devLists[devNum].serial) == CAN_OK) {
		return false;
	}

	return true;
};
bool Serial::GetCanDevices(QVector<CommonDeviceList>* lists) {
	if (can == nullptr) {
		wDebug("Failed to Get CAN-Devices");
		wDebug("Plz Initialize CAN");
		return false;
	}

	if (devLists != nullptr) {
		wDebug("Aleady exist device-info");
		wDebug("Check your serial");
		can->Free(devLists); // may not delete / checking devLists
	}

	int devCnt = NOT_EXIST;
	devLists = can->GetDeviceList(&devCnt);
	if (devLists == NULL || devCnt <= 0) {
		wDebug("Finded 0 Devices");

#if DEBUGGING
		comDevList nullList = { 255, new QString("Zero") };
		lists->push_back(nullList);
		lists->push_back(nullList);
		lists->push_back(nullList);
#endif
		return true;
	}
	else if (devLists == NULL) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : can");
		return false;
	}

	comDevList tmpDevList = { NULL, };

	for (int i = 0; i < devCnt; i++) {
		switch (devLists[i].device_type) {
			tmpDevList.devNumber = i;
		case DEVICE_TYPE_CANPROH:
			tmpDevList.devType = new QString("CANPROVH");
			lists->push_back(tmpDevList);
			break;
		case DEVICE_TYPE_CANPROV3:
			tmpDevList.devType = new QString("CANPROV3");
			lists->push_back(tmpDevList);
			break;
		case DEVICE_TYPE_CANPROV4:
			tmpDevList.devType = new QString("CANPROV4");
			lists->push_back(tmpDevList);
			break;
		}
	}

	return true;
};
bool Serial::GetCanConStatus() {
	return can->IsConnected();
}
bool Serial::CanDisconnect() {
	can->Disconnect();
	return true;
}
bool Serial::SetBitrateConfig(int bps) {
	if (!can->IsConnected()) {
		wDebug("Failed to configure can-bitrate");
		wDebug("Check your serial : can-connect");
		return false;
	}
	if (canBaudrateSetting == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : code");
		return false;
	}

	if (bps == HIGH_MODE_SPEED) {
		canBaudrateSetting->presdiv = HIGH_SPD_CONFIG[0];
		canBaudrateSetting->propseg = HIGH_SPD_CONFIG[1];
		canBaudrateSetting->pseg1 = HIGH_SPD_CONFIG[2];
		canBaudrateSetting->pseg2 = HIGH_SPD_CONFIG[3];
		canBaudrateSetting->can_mode = CAN_NO_LISTEN_MODE;
		canBaudrateSetting->can_mode = CAN_MODE_HCAN;
	}
	else if (bps == LOW_MODE_SPEED) {
		canBaudrateSetting->presdiv = LOW_SPD_CONFIG[0];
		canBaudrateSetting->propseg = LOW_SPD_CONFIG[1];
		canBaudrateSetting->pseg1 = LOW_SPD_CONFIG[2];
		canBaudrateSetting->pseg2 = LOW_SPD_CONFIG[3];
		canBaudrateSetting->can_mode = CAN_NO_LISTEN_MODE;
		canBaudrateSetting->can_mode = CAN_MODE_LCAN;
	}
	else {
		wDebug("Failed to configure bit-rate");
		wDebug("Plz check your settings");
		return false;
	}

	switch (can->SetConfiguration(canBaudrateSetting)) {
	case CAN_OK:
		break;
	case CAN_FAIL:
		return false;
		break;
	case CAN_TIMEOUT:
		return false;
		break;
	case CAN_INVALID_PARAMETER:
		return false;
		break;
	default:
		return false;
	}

	return true;
}
bool Serial::GetBitrateConfig() {
	if (!can->IsConnected()) {
		wDebug("Failed to get can-settings");
		return false;
	}

	if (canBaudrateSetting == nullptr) {
		wDebug("Check your serial : can-settings");
		canBaudrateSetting = new CAN_CONFIGURATION;
		memset(canBaudrateSetting, 0, sizeof(CAN_CONFIGURATION));
	}

	switch (can->GetConfiguration(canBaudrateSetting)) {
	case CAN_OK:
		break;
	case CAN_FAIL:
		return false;
		break;
	case CAN_TIMEOUT:
		return false;
		break;
	case CAN_INVALID_PARAMETER:
		return false;
		break;
	default:
		return false;
	}

	return true;
}
bool Serial::SetTraceThreadControl(bool ctl) {
	if (traceThreadHandle == NULL) {
		wDebug("Failed to control thread : trace");
		return false;
	}

	if (traceThreadEnable == NULL || traceThreadEnable == FALSE) {
		wDebug("Somethings wrong");
		wDebug("Check your Serial : thread-control");
		return false;
	}
	
	this->traceThreadStatus = ctl;
	if (ctl == true) {
		ReleaseMutex(traceThreadMtx);
	}
	WaitForSingleObject(traceThreadMtx, INFINITE);

	return true;
}
bool Serial::GetCanMessages(QVector<CAN_MESSAGE>* canMsgs) {
	if (canMsgQue.empty()) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : que");
		return false;
	}

	int size = canMsgQue.size();
	for (int element = 0; element < size; element++) {
		canMsgs->push_back(canMsgQue.front());
		canMsgQue.pop_front();
	}

	return true;
}

#endif
//proteceted function
HANDLE Serial::GetTraceThreadMtx() {
	return this->traceThreadMtx;
}

//common function
//private function
bool Serial::GetPortInfoSetting() {
#if WINDOW_VER
	if (this->serialPort == nullptr) {
		wDebug("Please Open Port");
		return false;
	}
	else if (this->preDcb != nullptr) {
		wDebug("Aleady Exist DCB");
		delete(preDcb);
	}

	preDcb = new DCB;
	memset(preDcb, 0, sizeof(DCB));

	GetCommState(serialHandle, preDcb);
#endif
}
void Serial::SetPortInfoSetting() {
#if WINDOW_VER
	if (currentDcb != nullptr) {
		wDebug("Aleady exist DCB");
		delete(currentDcb);
	}

	if (preDcb == nullptr) {
		wDebug("DCB is not exist");
		GetPortInfoSetting();
	}

	currentDcb = new DCB;
	memcpy(currentDcb, preDcb, sizeof(DCB));

	currentDcb->BaudRate = CBR_115200;
	currentDcb->ByteSize = 8;
	currentDcb->StopBits = ONESTOPBIT;
	currentDcb->Parity = NOPARITY;

	if (!SetCommState(this->serialHandle, currentDcb)) eMessage("Failed to set port-settings");

	if (currentTimeout != nullptr) {
		wDebug("Aleady exist timeout");
		delete(currentTimeout);
	}

	currentTimeout = new COMMTIMEOUTS;
	memset(currentTimeout, 0, sizeof(COMMTIMEOUTS));

	currentTimeout->ReadIntervalTimeout = rxTime;
	currentTimeout->ReadTotalTimeoutConstant = currentTimeout->ReadTotalTimeoutConstant = 0;
	currentTimeout->WriteTotalTimeoutConstant = currentTimeout->WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(serialHandle, currentTimeout)) eMessage("Failed to set timeout");
#endif
}
bool Serial::SetSerialComponent(const char port[]) {
#if WINDOW_VER
	SetSerialPort(port, strlen(port));
	serialHandle = CreateFileA(serialPort, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (serialHandle == NULL) eMessage("Failed to open Serial Port");

	if (!GetPortInfoSetting()) return false;
	SetPortInfoSetting();
	return true;
#endif

}
bool Serial::SetSerialPort(const char* port, int len) {
	if (serialPort != nullptr) {
		wDebug("Aleady Exist SerialPort");
		delete(serialPort);
	}

	serialPort = new char(len);
	memset(serialPort, 0, len);
	strcpy(serialPort, port);

	return true;
}
#if CANS
bool Serial::ReadData(void* readMsg) {
#if DEBUGGING
	QVector<CAN_MESSAGE>* canQue = (QVector<CAN_MESSAGE>*)readMsg;

	int msgCnt = 15;

	if (msgCnt == 0) return false;

	CAN_MESSAGE tmpMsg;

	for (int i = 0; i < msgCnt; i++) {
		tmpMsg.dlc = msgCnt - i;
		tmpMsg.id = msgCnt - i;
		for (int j = 0; j < sizeof(tmpMsg.data); j++) {
			tmpMsg.data[j] = j;
		}
		canQue->push_back(tmpMsg);
	}
	for (int i = 0; i < msgCnt / 2; i++) {
		tmpMsg.dlc = msgCnt;
		tmpMsg.id = msgCnt;
		for (int j = 0; j < sizeof(tmpMsg.data); j++) {
			tmpMsg.data[j] = j;
		}
		canQue->push_back(tmpMsg);
	}

	Sleep(100);
	return true;
}
#else
	if (readMsg == nullptr) {
		wDebug("Failed to read-data");
		wDebug("Check your serial : read-data");
		return false;
	}
	if (!can->IsConnected()) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : read-data");
		return false;
	}

	QVector<CAN_MESSAGE>* canQue = (QVector<CAN_MESSAGE>*)readMsg;

	int msgCnt = can->GetTraceMsgCount();
	if (msgCnt == 0) return false;

	CAN_MESSAGE tmpMsg;

	for (int i = 0; i < msgCnt; i++) {
		switch (can->PopTraceMsg(&tmpMsg)) {
		case CAN_OK:
			break;
		case CAN_FAIL:
		case CAN_CONNECTION_FAIL:
		case CAN_TIMEOUT:
		case CAN_INVALID_PARAMETER:
		case CAN_DATA_NOT_EXIST:
		case CAN_TX_MSG_BUFF_FULL:
			wDebug("Somethings wrong");
			wDebug("Check your read-data");
			return false;
			break;
		default:
			break;
		}
		canQue->push_back(tmpMsg);
	}

	return true;
}
#endif


bool Serial::WriteData(void* writeMsg) {
	if (writeMsg == nullptr) {
		wDebug("Failed to write-data");
		wDebug("Check your serial : write-data");
		return false;
	}
	if (!can->IsConnected()) {
		wDebug("Somethings wrong");
		wDebug("Check your serial : write-data");
		return false;
	}

	QVector<CAN_MESSAGE>* canMsg = (QVector<CAN_MESSAGE>*)writeMsg;
	int size = canMsg->size();
	if (canMsg == 0) {
		wDebug("Somethings wrong");
		wDebug("Check your write-data");
		return false;
	}

	auto tmpMsg = *canMsg;

	for (int msgCnt = 0; msgCnt < size; msgCnt++) {
		tmpMsg.front().info.BITS.type = 1;
		switch (can->PushTxMsg(&tmpMsg.front())) {
		case CAN_OK:
			break;
		case CAN_FAIL:
		case CAN_CONNECTION_FAIL:
		case CAN_TIMEOUT:
		case CAN_INVALID_PARAMETER:
		case CAN_DATA_NOT_EXIST:
		case CAN_TX_MSG_BUFF_FULL:
			wDebug("Somethings wrong");
			wDebug("Check your write-data");
			return false;
			break;
		default:
			break;
		}
		tmpMsg.pop_front();
	}

	return true;
#endif
}
#if Serial
bool Serial::ReadData() {
	if (!SetInitCanFrame(&readPreCanFrame, &readCurrentCanFrame)) wDebug("Plz Check Can Data");

	int canSTSize = sizeof(canST);
	int readedBytes = NULL;

#if WINDOW_VER
	if (!ReadFile(this->serialHandle, readCurrentCanFrame, canSTSize, (LPDWORD)&readedBytes, NULL)) {
		wDebug("Failed to read data");
		return false;
	}

	return true;
#endif
}
bool Serial::WriteData() {
	if (!SetInitCanFrame(&writePreCanFrame, &writeCurrentCanFrame)) wDebug("Plz Check Can Data");

	int canSTSize = sizeof(canST);
	int readedBytes = NULL;

#if WINDOW_VER
	if (!WriteFile(this->serialHandle, writeCurrentCanFrame, canSTSize, (LPDWORD)&readedBytes, NULL)) {
		wDebug("Failed to write data");
		return false;
	}

	return true;
#endif
}
#endif

void Serial::InitCompVal(int readTime, int writeTime) {
#if DEBUGGING
	rxTime = HIGH_RX_TIME;
	txTime = HIGH_TX_TIME;
	return;
#endif

	if (readTime == 0 || writeTime == 0) {
		wDebug("Transmission and reception speed is abnormal");
		wDebug("Check your code");
	}

	rxTime = readTime;
	txTime = writeTime;
}
void Serial::wMessage(const char* msg) {
	if (preMsgBox != nullptr) {
		delete(preMsgBox);
	}
	preMsgBox = new QMessageBox;
	preMsgBox->setText(msg);
	preMsgBox->setWindowTitle("WARNINIG");
	preMsgBox->show();
}
void Serial::wDebug(const char* msg) {
	std::cout << "Serial : " << msg << std::endl;
}
void Serial::eMessage(const char* msg) {
	if (preMsgBox != nullptr) {
		delete(preMsgBox);
	}
	preMsgBox = new QMessageBox;
	preMsgBox->setText(msg);
	preMsgBox->setWindowTitle("ERROR");
	preMsgBox->show();

	while (!preMsgBox->exec());
	exit(1);
}
