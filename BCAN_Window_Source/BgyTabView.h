#pragma once

#include <iostream>

#include <qtabwidget.h>
#include <qwidget.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qboxlayout.h>
#include <QTableWidget>
#include <qheaderview.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <qmessagebox.h>

#include "serial.h"
#include "readfiles.h"

#define TICK_TIME				500
#define LAYOUT_OFFSET			50 // maximum 50
#define HEDAER_OFFSET			5
#define DATA_BYTE				8
#define READ_ITEMARY_ID_NUM		0
#define TRACE_RE_MIN_ROW		0
#define TRACE_MAX_ELEMENT		5000
#define POPUP_FIXED_WIDTH		600	
#define POPUP_FIXED_HEIGHT		50
#define POPUP_POS_OFFSET		2
#define POPUP_BACK_OPACITY		0.8 // 0.0 ~ 1.0
#define POPUP_EDIT_SIZE			35
#define HEX						16
#define MAX_L_EDIT_ID			3
#define MAX_L_EDIT_DATA			2
#define EXEL_MAX				20
#define MAX_NAME				100
#define EXEL_ROW_OFFSET			1
#define EXEL_COL_OFFSET			1 

// Data-Views
typedef enum CAN_DATA_HEADER_LIST {
	ID = 0,
	DLC,
	DATA
}CAN_DATA_HLIST;

typedef struct CAN_BTNS_HEADER {
	wchar_t name[MAX_NAME] = {0, };
	unsigned int id;
	unsigned int dlc;
	unsigned char data[DATA_BYTE];
} canBtnsHeader;

const QStringList canDataHeaders = { "ID", "DLC", "DATA" };
 static const QString WRITE_POPUP_STYLES = "QDialog { border-radius: 12px; border: 1.0px solid; }";

class BgyTabView : public QTabWidget
{
	Q_OBJECT

private:
	BGY_CAN* bCan						= nullptr;
	BOOL canStatus						= FALSE;
	BGY_EXEL* bExel						= nullptr;
	QMessageBox* preMsgBox				= nullptr;
	QTimer updateTimer;

	//default-view variable
	QLayout* defaultLayout				= nullptr;
	QLayout* defaultRWLayout			= nullptr;
	QLayout* readLayout					= nullptr;
	QLayout* writeLLayout				= nullptr;
	QLayout* writeRLayout				= nullptr; 
	QLayout* traceLayout				= nullptr;
	QSplitter* defaultSplitView			= nullptr;
	QSplitter* defaultRWSplitView		= nullptr;
	QWidget* defaultWidget				= nullptr;
	QWidget* traceWidget				= nullptr;
	QWidget* defaultRWWidget			= nullptr;
	QWidget* readWidget					= nullptr;
	QWidget* writeMainWidget			= nullptr;
	QWidget* writeSubWidget				= nullptr;
	QTableWidget* readTableWidget		= nullptr;
	QTableWidget* traceTableWidget		= nullptr;
	QTableWidget* writeTableWidget		= nullptr;
	QPushButton* writeAddBtn			= nullptr;
	QPushButton* writeDelBtn			= nullptr;
	QBrush writeColor					= Qt::GlobalColor::yellow;

	//data variable
	QVector<CAN_MESSAGE> tmpMsg;
	QVector<CAN_MESSAGE> writeTmpMsg;
	QVector<QVector<QTableWidgetItem>> traceItems;

	//default-Popup view variable
	QDialog* addPopup				= nullptr;
	QHBoxLayout* addPopupLayout		= nullptr;
	QLabel* idLabel					= nullptr;
	QLabel* dataLabel				= nullptr;
	QLineEdit* idEdit				= nullptr;
	QLineEdit* firstDataEdit		= nullptr;
	QLineEdit* secondDataEdit		= nullptr;
	QLineEdit* thirdDataEdit		= nullptr;
	QLineEdit* fourthDataEdit		= nullptr;
	QLineEdit* fifthDataEdit		= nullptr;
	QLineEdit* sixthDataEdit		= nullptr;
	QLineEdit* seventhDataEdit		= nullptr;
	QLineEdit* eighthDataEdit		= nullptr;
	QPushButton* addPopupOkBtn		= nullptr;
	QPushButton* addPopupCloseBtn	= nullptr;
	QIntValidator* idValidator		= nullptr;
	QIntValidator* dataValidator	= nullptr;
	
	//quick-view variable
	QWidget* quickWidget					= nullptr;
	QHBoxLayout* quickMainLayout			= nullptr;
	QSplitter* quickMainSplitView			= nullptr;
	QWidget* quickLeftWidget				= nullptr;
	QLayout* quickBtnsLayout				= nullptr;
	QWidget* quickRigthtWidget				= nullptr;
	QTableWidget* quickTraceTableWidget		= nullptr;
	QLayout* quickTraceLayout				= nullptr;
	
	QVector< CAN_BTNS_HEADER*> canBtnHeaders;
	QVector<QPushButton*> canBtns;
	

//functions
	bool SetDefaultViewComponent();
	bool SetAddPopupComponent();
	bool SetQuickViewComponent();
	
	
	bool InitializeWidget(QWidget** widget);
	template <typename T>
	bool InitializeVariable(T** val);
	int QStringToHex(QString str);

	//default-view
	bool SetDefaultLayout();
	bool SetSplitView(QSplitter** splitView, Qt::Orientation ori);
	bool SetReadView();
	bool SetWriteView();
	bool SetTraceView();

	//quick-view
	bool SetQucikTraceView();
public:
	bool SetCanBtnsView();
private:
	bool SetCanBtnsWidget();



	//data-function
	bool InitializeCanWidget();
	bool InitializeCanData();
	bool SetCanBtnsData();
	bool GetTableItemsData();
	bool TableItemsDataSort();
	bool UpdateTraceViewData(QTableWidget* traceTableWidget);
	bool UpdateTraceToWrite(QTableWidget* traceTableWidget);
	bool InitializeTraceViewData(QTableWidget* traceTableWidget);
	bool UpdateReadViewData();
	bool UpdateWriteViewData();
	bool GetWriteViewData(QString itemName, int* row = nullptr, int* col = nullptr);

	QString ConvertDataToString(int row = NULL);
	
	void wMessage(const char* msg);
	void wDebug(const char* msg);
	void eMessage(const char* msg);


private slots:
	bool DataUpdateTimerEvent();
	bool WriteAddBtnEvent();
	bool WriteDelBtnEvent();
	bool WritePopupOkBtnEvent();
	bool WritePopupCloseBtnEvent();
	bool CanBtnEvent();
	

public:
	BgyTabView(BGY_CAN* can,BGY_EXEL* read, QWidget* parent = nullptr);
};

