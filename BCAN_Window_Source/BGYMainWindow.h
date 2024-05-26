#pragma once

#include <iostream>

#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qdialog.h>
#include <qlistwidget.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qfiledialog.h>

#include "Common.h"
#include "BgyTabView.h"
#include "serial.h"
#include "readfiles.h"


#define DEFAULT_HEIGHT			35

#define DEFAULT_WIDTH			800
#define DEFAULT_HEIGHT			600

#define MINIMUM_WIDTH			400
#define MINIMUM_HEIGHT			300

#define TOOLBAR_ACTION_CNT		6

#define DIALOG_FIXED_WIDTH		300	
#define DIALOG_FIXED_HEIGHT		200

#define POPUP_POS_OFFSET		2
#define POPUP_BACK_OPACITY		0.8 // 0.0 ~ 1.0

static const QString POPUP_STYLES = "QDialog { border-radius: 12px; border: 1.0px solid; }";

class QApplication;
 
class BGYMainWindow : public QMainWindow
{
	Q_OBJECT
private:
//System Function Variable
	BGY_CAN* bCan;
	BGY_EXEL* bExel;
	QVector<CommonDeviceList> devices;

//UI Variable
	QToolBar* toolBar							= nullptr;
	QAction* toolBarActions[TOOLBAR_ACTION_CNT] = {nullptr, };
	
//ToolBar-Connect Variable
	QDialog* conPopupDialog						= nullptr;
	QVBoxLayout* conMainLayout					= nullptr;
	QVBoxLayout* conSubLayout					= nullptr;
	QSplitter* conPopupSplit					= nullptr;
	QListWidget* conListBox						= nullptr;
	QWidget* conSubWidget						= nullptr;
	QPushButton* conAcceptBtn					= nullptr;
	QPushButton* conExitBtn						= nullptr;
	QPushButton* conReloadBtn					= nullptr;
//ToolBar-Bit-rate Setting Variable
	QDialog* settPopupDialog					= nullptr;
	QHBoxLayout* settLayout						= nullptr;
	QRadioButton* settHighBtn					= nullptr;
	QRadioButton* settLowBtn					= nullptr;
	QPushButton* settOKBtn						= nullptr;
	QPushButton* settCloseBtn					= nullptr;
//File Load Variable
	QFileDialog* openExelPopup					= nullptr;


	typedef enum TOOLBAR_ACTIONS {
		Connect = 0,
		DisConnect,
		BitRate_Setting,
		Start,
		Stop,
		Load_Exel
	}TBACTIONS;

	QMessageBox* preMsgBox	= nullptr;
	BgyTabView* bgyTabView	= nullptr;


	bool SetToolBarComponent(int height = DEFAULT_HEIGHT);
	bool SetTabViewComponent();
	bool SetCanConnectComponent();
	bool SetCanSettingComponent();

	bool SetSplitLayout();
	bool DeviceListup();
	bool SetToolBarActions();
	bool GetDevices();

	template <typename T>
	bool CreateDynamicData(T** pointer);

	void wMessage(const char* msg);
	void wDebug(const char* msg);
	void eMessage(const char* msg);

private slots:
	bool SetDeviceConnection();
	bool SetDeviceDisConnection();
	bool SetDeviceBitRateSetting();
	bool StartCANTrace();
	bool StopCANTrace();
	bool GetExeIInfo();
	
	//Connect pop-up slots
	bool ConAcceptBtnEvent();
	bool ConReloadBtnEvent();
	bool ConExitBtnEvent();
	//Setting pop-up slots
	bool settRadioBtnEvent();
	bool settOKBtnEvent();
	bool settCloseBtnEvent();

public:
	BGYMainWindow(BGY_CAN* can, BGY_EXEL* read, QWidget* parent = nullptr);

};

