#include "BGYMainWindow.h"

BGYMainWindow::BGYMainWindow(BGY_CAN* can, BGY_EXEL* exel, QWidget* parent)
	: bCan(can), bExel(exel), QMainWindow(parent) {
	this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	this->setMinimumSize(MINIMUM_WIDTH, MINIMUM_HEIGHT);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setWindowTitle("BGY_CAN");

	SetToolBarComponent(); //return BOOL
	SetTabViewComponent();
}

bool BGYMainWindow::SetToolBarComponent(int height) {
	if (toolBar != nullptr) {
		wDebug("Aleady exist toolbar");
		return false;
	}

	toolBar = new QToolBar("Main tool-bar");
	if (toolBar == nullptr) {
		wDebug("Failed to create toolbar");
		return false;
	}

	auto size = this->size();

	toolBar->resize(size.width(), height);
	toolBar->setMovable(false);

	SetToolBarActions();
	for (int i = 0; i < TOOLBAR_ACTION_CNT; i++) {
		toolBar->addAction(toolBarActions[i]);
	}

	this->addToolBar(toolBar);
}
bool BGYMainWindow::SetTabViewComponent() {
	if (bgyTabView != nullptr) {
		wDebug("Aleady exist main-tab-view");
		return false;
	}

	bgyTabView = new BgyTabView(bCan,bExel, this);
	if (bgyTabView == nullptr) {
		wDebug("Failed to create main-tab-view");
		return false;
	}

	this->setCentralWidget(bgyTabView);
}
bool BGYMainWindow::SetCanConnectComponent() {
#if POPUP_POS_OFFSET > 2
#define POPUP_POS_OFFSET 2
#endif

	if (conPopupDialog != nullptr) goto RESTART;

	if (!CreateDynamicData(&this->conPopupDialog)) return false;
	if (!CreateDynamicData(&this->conMainLayout)) return false;
	if (!CreateDynamicData(&this->conSubLayout)) return false;
	if (!CreateDynamicData(&this->conPopupSplit)) return false;
	if (!CreateDynamicData(&this->conListBox)) return false;
	if (!CreateDynamicData(&this->conSubWidget)) return false;
	if (!CreateDynamicData(&this->conAcceptBtn)) return false;
	if (!CreateDynamicData(&this->conExitBtn)) return false;
	if (!CreateDynamicData(&this->conReloadBtn)) return false;

	//Btn Setting
	conAcceptBtn->setText("Accept");
	conReloadBtn->setText("Reload");
	conExitBtn->setText("Exit");
	
	connect(conAcceptBtn, &QPushButton::clicked, this, &BGYMainWindow::ConAcceptBtnEvent);
	connect(conReloadBtn, &QPushButton::clicked, this, &BGYMainWindow::ConReloadBtnEvent);
	connect(conExitBtn, &QPushButton::clicked, this, &BGYMainWindow::ConExitBtnEvent);

	//Layout Setting
	SetSplitLayout();

	//Dialog Setting
	conPopupDialog->setFixedSize(DIALOG_FIXED_WIDTH, DIALOG_FIXED_HEIGHT); 
	conPopupDialog->setWindowTitle("CONNECTED DVICES");
	conPopupDialog->setFocus();
	conPopupDialog->setWindowTitle("Connect");
	conPopupDialog->setLayout(conMainLayout);
	conPopupDialog->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	conPopupDialog->setStyleSheet(POPUP_STYLES);
RESTART:
	int posX = (this->pos().x() + (this->width() / POPUP_POS_OFFSET)) - (conPopupDialog->width() / POPUP_POS_OFFSET);
	int posY = (this->pos().y() + (this->height() / POPUP_POS_OFFSET)) - (conPopupDialog->height() / POPUP_POS_OFFSET);

	conPopupDialog->move(posX, posY);
	DeviceListup();
}
bool BGYMainWindow::SetCanSettingComponent() {
	if (!CreateDynamicData(&settPopupDialog)) return false;
	if (!CreateDynamicData(&settLayout)) return false;;
	if (!CreateDynamicData(&settHighBtn)) return false;;
	if (!CreateDynamicData(&settLowBtn)) return false;;
	if (!CreateDynamicData(&settOKBtn)) return false;;
	if (!CreateDynamicData(&settCloseBtn)) return false;;

	settHighBtn->setText("High-CAN");
	settLowBtn->setText("Low-CAN");
	settOKBtn->setText("OK");
	settCloseBtn->setText("Close");

	connect(settOKBtn, &QPushButton::clicked, this, &BGYMainWindow::settOKBtnEvent);
	connect(settCloseBtn, &QPushButton::clicked, this, &BGYMainWindow::settCloseBtnEvent);

	settLayout->addWidget(settHighBtn);
	settLayout->addWidget(settLowBtn);
	settLayout->addWidget(settOKBtn);
	settLayout->addWidget(settCloseBtn);

	settPopupDialog->setLayout(settLayout);
	settPopupDialog->setWindowTitle("Bit-Rate Setting");
	settPopupDialog->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	settPopupDialog->setStyleSheet(POPUP_STYLES);
	

	return true;
}

bool BGYMainWindow::SetSplitLayout() {
	conSubLayout->addWidget(conAcceptBtn);
	conSubLayout->addWidget(conReloadBtn);
	conSubLayout->addWidget(conExitBtn);
	conSubWidget->setLayout(conSubLayout);

	conPopupSplit->addWidget(conListBox);
	conPopupSplit->addWidget(conSubWidget);
	conMainLayout->addWidget(conPopupSplit);

	return true;
}
bool BGYMainWindow::DeviceListup() {
	if (conPopupDialog == nullptr) {
		wDebug("Failed to create Device-Listup");
		wDebug("Plz initialize pop-up");
		return false;
	}
	
	if (conListBox == nullptr) {
		wDebug("Failed to create Device-Listup");
		wDebug("Somethings wrong");
		wDebug("Check your main-window");
		return false;
	}

	if (!conListBox->size().isEmpty()) {
		wDebug("Aleady exist list-up");
		conListBox->clear();
	}

	int devCnt = devices.size();
	
	if (devCnt == 0) {
		conListBox->addItem("No devices");
	}
	else {
		QStringList tmpList;
		for (int i = 0; i < devCnt; i++) {
			tmpList.push_back(*devices[i].devType);
		}
		
		conListBox->addItems(tmpList);
	}
	
	return true;
}
bool BGYMainWindow::SetToolBarActions() {
	toolBarActions[TOOLBAR_ACTIONS::Connect] = new QAction("Connect");
	toolBarActions[TOOLBAR_ACTIONS::Connect]->setShortcut(QKeySequence("Alt+C"));

	this->connect(toolBarActions[TOOLBAR_ACTIONS::Connect], &QAction::triggered, this, &BGYMainWindow::SetDeviceConnection);

	toolBarActions[TOOLBAR_ACTIONS::DisConnect] = new QAction("DisConnect");
	toolBarActions[TOOLBAR_ACTIONS::DisConnect]->setShortcut(QKeySequence("Alt+D"));
	toolBarActions[TOOLBAR_ACTIONS::DisConnect]->setEnabled(false);

	this->connect(toolBarActions[TOOLBAR_ACTIONS::DisConnect], &QAction::triggered, this, &BGYMainWindow::SetDeviceDisConnection);

	toolBarActions[TOOLBAR_ACTIONS::BitRate_Setting] = new QAction("BitRate_Setting");
	toolBarActions[TOOLBAR_ACTIONS::BitRate_Setting]->setShortcut(QKeySequence("Shift+S"));
	toolBarActions[TOOLBAR_ACTIONS::BitRate_Setting]->setEnabled(false);

	this->connect(toolBarActions[TOOLBAR_ACTIONS::BitRate_Setting], &QAction::triggered, this, &BGYMainWindow::SetDeviceBitRateSetting);

	toolBarActions[TOOLBAR_ACTIONS::Start] = new QAction("Start");
	toolBarActions[TOOLBAR_ACTIONS::Start]->setShortcut(QKeySequence("Alt+S"));
	toolBarActions[TOOLBAR_ACTIONS::Start]->setEnabled(false);

	this->connect(toolBarActions[TOOLBAR_ACTIONS::Start], &QAction::triggered, this, &BGYMainWindow::StartCANTrace);

	toolBarActions[TOOLBAR_ACTIONS::Stop] = new QAction("Stop");
	toolBarActions[TOOLBAR_ACTIONS::Stop]->setShortcut(QKeySequence("Ctrl+S"));
	toolBarActions[TOOLBAR_ACTIONS::Stop]->setEnabled(false);

	this->connect(toolBarActions[TOOLBAR_ACTIONS::Stop], &QAction::triggered, this, &BGYMainWindow::StopCANTrace);

	toolBarActions[TOOLBAR_ACTIONS::Load_Exel] = new QAction("Load_Exel");
	toolBarActions[TOOLBAR_ACTIONS::Load_Exel]->setShortcut(QKeySequence("Alt+L"));
	toolBarActions[TOOLBAR_ACTIONS::Load_Exel]->setEnabled(false);

	this->connect(toolBarActions[TOOLBAR_ACTIONS::Load_Exel], &QAction::triggered, this, &BGYMainWindow::GetExeIInfo);

	return true;
}
bool BGYMainWindow::GetDevices() {
	if (bCan == nullptr) {
		wDebug("Somethings wrong : CAN");
		wDebug("Check your main-window");
		return false;
	}

	if (!devices.isEmpty()) {
		wDebug("Aleady exist devices");
		devices.clear();
	}

	if (!bCan->GetCanDevices(&devices)) {
		wDebug("Failed to get can-devices");
		return false;
	}

	return true;
}

template <typename T>
bool BGYMainWindow::CreateDynamicData(T** pointer) {
	if (*pointer != nullptr) {
		wDebug("Aleady exist data");
		wDebug("Check your main-window : Dynamic-Data");
		return false;
	}

	*pointer = new T;

	if (*pointer == nullptr) {
		wDebug("Failed to create data");
		wDebug("Check your main-window : Dynamic-Data");
		return false;
	}

	return true;
}
void BGYMainWindow::wMessage(const char* msg) {
	if (preMsgBox != nullptr) {
		delete(preMsgBox);
	}
	preMsgBox = new QMessageBox;
	preMsgBox->setText(msg);
	preMsgBox->setWindowTitle("WARNINIG");
	preMsgBox->show();
}
void BGYMainWindow::wDebug(const char* msg) {
	std::cout << "MainWindow : " << msg << std::endl;
}
void BGYMainWindow::eMessage(const char* msg) {
	if (preMsgBox != nullptr) {
		delete(preMsgBox);
	}
	preMsgBox = new QMessageBox;
	preMsgBox->setText(msg);
	preMsgBox->setWindowTitle("ERROR");

	preMsgBox->exec();
	exit(1);
}

//private slots
bool BGYMainWindow::SetDeviceConnection() {
	if (!GetDevices()) return false;
	SetCanConnectComponent();

	this->setWindowOpacity(POPUP_BACK_OPACITY);
	conPopupDialog->exec();
	this->setWindowOpacity(1);

	return true;
}
bool BGYMainWindow::SetDeviceDisConnection() {
	if (bCan == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your main-window : disconnect-event");
		return false;
	}

	if (!bCan->GetCanConStatus()) {
		wDebug("Failed to disconnect");
		eMessage("Check your main-window : disconnect-event");
		return false;
	}

	if (!toolBarActions[Start]->isEnabled()) {
		if (!StopCANTrace()) {
			wDebug("somethings wrong");
			wDebug("check your disconnect");
			return false;
		}
	}

	bCan->CanDisconnect();

	this->toolBarActions[Connect]->setEnabled(true);
	this->toolBarActions[DisConnect]->setEnabled(false);
	this->toolBarActions[BitRate_Setting]->setEnabled(false);
	this->toolBarActions[Start]->setEnabled(false);
	this->toolBarActions[Stop]->setEnabled(false);
	this->toolBarActions[Load_Exel]->setEnabled(false);

	return true;
}
bool BGYMainWindow::SetDeviceBitRateSetting() {
	SetCanSettingComponent();

	settPopupDialog->exec();
	return true;
}
bool BGYMainWindow::StartCANTrace() {
	if (!bCan->StartMsgTrace()) return false;
	toolBarActions[TBACTIONS::Start]->setEnabled(false);
	toolBarActions[TBACTIONS::Stop]->setEnabled(true);
	return true;
}
bool BGYMainWindow::StopCANTrace() {
	if (!bCan->StopMsgTrace()) return false;
	toolBarActions[TBACTIONS::Start]->setEnabled(true);
	toolBarActions[TBACTIONS::Stop]->setEnabled(false);
	return true;
}
bool BGYMainWindow::GetExeIInfo() {
	if (openExelPopup != nullptr) {
		goto RECLICKED;
		wDebug("Somethings wrong");
		wDebug("Check your main-window : exel");
		delete(openExelPopup);
		delete(bExel);
		bExel = new BGY_EXEL;
	}
	openExelPopup = new QFileDialog;
	
RECLICKED:
	openExelPopup->selectedFiles().clear();
	QStringList* filterList = new QStringList;
	filterList->push_front(tr("Exel (*.xlsx)"));
	filterList->push_front(tr("All(*.*)"));
	openExelPopup->setNameFilters(*filterList);
	delete(filterList);
	if (openExelPopup->exec() == QDialog::Rejected) {
		return true;
	}

	std::string fileName = openExelPopup->selectedFiles()[0].toStdString();

	bExel->SetFilePath(fileName);
	if (!bExel->SetXlsxFileOpen()) return false;

	delete(openExelPopup);
	openExelPopup = nullptr;

	bgyTabView->SetCanBtnsView();

	return true;
}

//Connect pop-up functions
bool BGYMainWindow::ConAcceptBtnEvent() {
	int selectedCnt = this->conListBox->selectedItems().size();

	if (selectedCnt <= 0) return false;
	else if (!bCan->SetCanDeviceCon(conListBox->currentRow())) return false;
	
	this->toolBarActions[Connect]->setEnabled(false);
	this->toolBarActions[DisConnect]->setEnabled(true);
	this->toolBarActions[BitRate_Setting]->setEnabled(true);
	this->toolBarActions[Start]->setEnabled(true);
	this->toolBarActions[Load_Exel]->setEnabled(true);
	this->conPopupDialog->close();
	return true;
}
bool BGYMainWindow::ConReloadBtnEvent() {
	if (!GetDevices()) return false;
	if (!DeviceListup()) return false;
	return true;
}
bool BGYMainWindow::ConExitBtnEvent() {
	conPopupDialog->close();
	return true;
}

//Setting pop-up functions
bool BGYMainWindow::settRadioBtnEvent() {
	return true;
}
bool BGYMainWindow::settOKBtnEvent() {
	if (settPopupDialog == nullptr) {
		wDebug("Seomthings wrong");
		wDebug("Check your main-window : Settings");
		return false;
	}
	if (settHighBtn == nullptr || settLowBtn == nullptr) {
		wDebug("Seomthings wrong");
		wDebug("Check your main-window : Settings");
		return false;
	}
	if (settOKBtn == nullptr) {
		wDebug("Seomthings wrong");
		wDebug("Check your main-window : Settings");
		return false;
	}

	bCan->GetBitrateConfig();

	if (settHighBtn->isChecked()) {
		if (!bCan->SetBitrateConfig(HIGH_MODE_SPEED)) return false;
	}
	else if (settLowBtn->isChecked()) {
		if (!bCan->SetBitrateConfig(LOW_MODE_SPEED)) return false;
	}
	else {
		wDebug("Somethings wrong");
		wDebug("Check your code");
		return false;
	}

	settPopupDialog->close();
	return true;
}
bool BGYMainWindow::settCloseBtnEvent() {
	settPopupDialog->close();
	return true;
}