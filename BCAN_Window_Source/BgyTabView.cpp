#include "BgyTabView.h"

BgyTabView::BgyTabView(BGY_CAN* can, BGY_EXEL* read, QWidget* parent)
	: QTabWidget(parent), bCan(can), bExel(read)
{
	if (bCan == nullptr) wDebug("Failed to receive can");
	SetDefaultViewComponent();
	SetQuickViewComponent();

}

bool BgyTabView::SetDefaultViewComponent() {
	InitializeWidget(&defaultWidget);
	InitializeWidget(&traceWidget);
	InitializeWidget(&defaultRWWidget);
	InitializeWidget(&readWidget);
	InitializeWidget(&writeMainWidget);
	InitializeWidget(&writeSubWidget);

	if (!SetSplitView(&defaultSplitView, Qt::Orientation::Horizontal)) {
		wDebug("Failed to create default-view");
		return false;
	}

#if LAYOUTOFFSET > 50
#define LAYOUTOFFSET 50
#endif
	auto width = (this->width() / 2) + LAYOUT_OFFSET;


	defaultSplitView->addWidget(defaultRWWidget);
	defaultRWWidget->setMinimumWidth(width);
	defaultRWWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (!SetTraceView()) {
		eMessage("Failed to create Trace-View");
		return false;
	}

	defaultSplitView->addWidget(traceWidget);
	traceWidget->setMinimumWidth(width);
	traceWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (!SetSplitView(&defaultRWSplitView, Qt::Orientation::Vertical)) {
		wDebug("Failed to create default-view");
		return false;
	}

	if (!SetReadView()) {
		eMessage("Failed to create Read-View");
		return false;
	}
	if (!SetWriteView()) {
		eMessage("Failed to create Write-View");
		return false;
	}

	defaultRWSplitView->addWidget(readWidget);
	defaultRWSplitView->addWidget(writeMainWidget);

	if (!SetDefaultLayout()) {
		wDebug("Failed to create defaults-layout");
		return false;
	}

	defaultLayout->addWidget(defaultSplitView);
	defaultRWLayout->addWidget(defaultRWSplitView);

	defaultWidget->setLayout(this->defaultLayout);
	defaultRWWidget->setLayout(defaultRWLayout);

	this->addTab(defaultWidget, "Default-View");
}
bool BgyTabView::SetAddPopupComponent() {
	if (addPopup != nullptr) goto RECLICK;
	if (!InitializeVariable(&addPopup)) return false;
	if (!InitializeVariable(&addPopupLayout)) return false;
	if (!InitializeVariable(&idLabel)) return false;
	if (!InitializeVariable(&dataLabel)) return false;
	if (!InitializeVariable(&idEdit)) return false;
	if (!InitializeVariable(&firstDataEdit)) return false;
	if (!InitializeVariable(&secondDataEdit)) return false;
	if (!InitializeVariable(&thirdDataEdit)) return false;
	if (!InitializeVariable(&fourthDataEdit)) return false;
	if (!InitializeVariable(&fifthDataEdit)) return false;
	if (!InitializeVariable(&sixthDataEdit)) return false;
	if (!InitializeVariable(&seventhDataEdit)) return false;
	if (!InitializeVariable(&eighthDataEdit)) return false;
	if (!InitializeVariable(&addPopupOkBtn)) return false;
	if (!InitializeVariable(&addPopupCloseBtn)) return false;
	if (!InitializeVariable(&dataValidator)) return false;
	if (!InitializeVariable(&idValidator)) return false;

	//idValidator->setRange(0, 999);
	idLabel->setText("ID : ");
	idEdit->setFixedWidth(POPUP_EDIT_SIZE);
	//idEdit->setValidator(idValidator);
	idEdit->setMaxLength(MAX_L_EDIT_ID);

	dataValidator->setRange(0, 99);
	dataLabel->setText("DATA : ");
	firstDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	firstDataEdit->setValidator(dataValidator);
	firstDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	secondDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	secondDataEdit->setValidator(dataValidator);
	secondDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	thirdDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	thirdDataEdit->setValidator(dataValidator);
	thirdDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	fourthDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	fourthDataEdit->setValidator(dataValidator);
	fourthDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	fifthDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	fifthDataEdit->setValidator(dataValidator);
	fifthDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	sixthDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	sixthDataEdit->setValidator(dataValidator);
	sixthDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	seventhDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	seventhDataEdit->setValidator(dataValidator);
	seventhDataEdit->setMaxLength(MAX_L_EDIT_DATA);
	eighthDataEdit->setFixedWidth(POPUP_EDIT_SIZE);
	eighthDataEdit->setValidator(dataValidator);
	eighthDataEdit->setMaxLength(MAX_L_EDIT_DATA);

	addPopupOkBtn->setText("OK");
	connect(addPopupOkBtn, &QPushButton::clicked, this, &BgyTabView::WritePopupOkBtnEvent);
	addPopupCloseBtn->setText("Close");
	connect(addPopupCloseBtn, &QPushButton::clicked, this, &BgyTabView::WritePopupCloseBtnEvent);

	addPopupLayout->addWidget(idLabel);
	addPopupLayout->addWidget(idEdit);
	addPopupLayout->addWidget(dataLabel);
	addPopupLayout->addWidget(firstDataEdit);
	addPopupLayout->addWidget(secondDataEdit);
	addPopupLayout->addWidget(thirdDataEdit);
	addPopupLayout->addWidget(fourthDataEdit);
	addPopupLayout->addWidget(fifthDataEdit);
	addPopupLayout->addWidget(sixthDataEdit);
	addPopupLayout->addWidget(seventhDataEdit);
	addPopupLayout->addWidget(eighthDataEdit);
	addPopupLayout->addWidget(addPopupOkBtn);
	addPopupLayout->addWidget(addPopupCloseBtn);

	addPopup->setFixedSize(POPUP_FIXED_WIDTH, POPUP_FIXED_HEIGHT);
	addPopup->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	addPopup->setStyleSheet(WRITE_POPUP_STYLES);
	addPopup->setWindowTitle("Add Write CAN Message");

	addPopup->setLayout(addPopupLayout);

RECLICK:
	idEdit->setText("");
	firstDataEdit->setText("");
	secondDataEdit->setText("");
	thirdDataEdit->setText("");
	fourthDataEdit->setText("");
	fifthDataEdit->setText("");
	sixthDataEdit->setText("");
	seventhDataEdit->setText("");
	eighthDataEdit->setText("");

	int posX = (this->window()->pos().x() + (this->window()->width() / POPUP_POS_OFFSET)) - (addPopup->width() / POPUP_POS_OFFSET);
	int posY = (this->window()->pos().y() + (this->window()->height() / POPUP_POS_OFFSET)) - (addPopup->height() / POPUP_POS_OFFSET);

	addPopup->move(posX, posY);
	addPopup->setModal(true);
	addPopup->show();

	return true;
}
bool BgyTabView::SetQuickViewComponent() {
	if (!InitializeWidget(&quickWidget)) return false;
	if (!SetSplitView(&quickMainSplitView, Qt::Orientation::Horizontal)) return false;
	if (!InitializeVariable(&quickMainLayout)) return false;
	if (!InitializeVariable(&quickLeftWidget)) return false;
	if (!InitializeVariable(&quickRigthtWidget)) return false;

	if (!SetQucikTraceView()) return false;

	quickMainSplitView->addWidget(quickLeftWidget);
	quickMainSplitView->addWidget(quickRigthtWidget);

	quickMainLayout->addWidget(quickMainSplitView);

	quickWidget->setLayout(quickMainLayout);


	this->addTab(quickWidget, "Quick-View");

	return true;
}


bool BgyTabView::InitializeWidget(QWidget** widget) {
	if (*widget != nullptr) {
		wDebug("Aleady exist tab-view-widget");
		return false;
	}

	*widget = new QWidget;
	if (*widget == nullptr) {
		wDebug("Failed to create tab-view-widget");
		return false;
	}
}
template <typename T>
bool BgyTabView::InitializeVariable(T** val) {
	if (*val != nullptr) {
		wDebug("Failed to Initialzie variable");
		wDebug("Check your code tab-view : init val");
		return false;
	}

	*val = new T;
	if (*val == nullptr) {
		wDebug("Failed to Initialzie variable");
		wDebug("Somethings wrong");
		wDebug("Check your code tab-view : init val");
		return false;
	}
	return true;
}
int BgyTabView::QStringToHex(QString str) {
	int size = str.size();
	if (size == 0) {
		wDebug("Failed to convert Hex");
		return NOT_EXIST;
	}

	int hex = std::strtol(str.toStdString().c_str(), NULL, HEX);
	return hex;
}

//default-view
bool BgyTabView::SetDefaultLayout() {
	if (defaultLayout != nullptr) {
		wDebug("Aleady exist default-layout");
		return false;
	}

	defaultLayout = new QVBoxLayout;
	if (defaultLayout == nullptr) {
		wDebug("Failed to create default-layout");
		return false;
	}

	if (defaultRWLayout != nullptr) {
		wDebug("Aleady exist default-RW-layout");
		return false;
	}

	defaultRWLayout = new QHBoxLayout;
	if (defaultRWLayout == nullptr) {
		wDebug("Failed to create default-RW-layout");
		return false;
	}

	return true;
}
bool BgyTabView::SetSplitView(QSplitter** splitView, Qt::Orientation ori) {
	if (*splitView != nullptr) {
		wDebug("Aleady exist split-view");
		return false;
	}

	if (defaultWidget == nullptr) {
		wDebug("Failed to create split-view");
		wDebug("Check your split-view : default-view");
		return false;
	}
	*splitView = new QSplitter(ori);
	if (splitView == nullptr) {
		wDebug("Failed to create split-view");
		return false;
	}
}
bool BgyTabView::SetReadView() {
	if (readWidget == nullptr) {
		wDebug("Failed to create read-view");
		return false;
	}
	if (readLayout != nullptr) {
		wDebug("Failed to create read-view");
		wDebug("Check your tab-view : read-view");
		return false;
	}
	if (readTableWidget != nullptr) {
		wDebug("Failed to create read-view");
		wDebug("Check your tab-view : read-view");
		return false;
	}

	readLayout = new QVBoxLayout;
	if (readLayout == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : read-view");
		return false;
	}

	readTableWidget = new QTableWidget;
	if (readTableWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : read-view");
		return false;
	}

	readTableWidget->setColumnCount(canDataHeaders.size());
	readTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	readTableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
	readTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	readTableWidget->setHorizontalHeaderLabels(canDataHeaders);

	int headHeight = readTableWidget->horizontalHeader()->height();
	readTableWidget->horizontalHeader()->setFixedHeight(headHeight + HEDAER_OFFSET);

	readLayout->addWidget(readTableWidget);
	readWidget->setLayout(readLayout);

	return true;
}
bool BgyTabView::SetWriteView() {
	if (writeMainWidget == nullptr) {
		wDebug("Failed to create write-view");
		return false;
	}
	if (writeSubWidget == nullptr) {
		wDebug("Failed to create write-view");
		return false;
	}
	if (writeLLayout != nullptr) {
		wDebug("Failed to create write-view");
		wDebug("Check your tab-view : write-view");
		return false;
	}
	if (writeRLayout != nullptr) {
		wDebug("Failed to create write-view");
		wDebug("Check your tab-view : write-view");
		return false;
	}
	if (writeTableWidget != nullptr) {
		wDebug("Failed to create write-view");
		wDebug("Check your tab-view : write-view");
		return false;
	}
	if (writeAddBtn != nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your write-veiw");
		return false;
	}
	if (writeDelBtn != nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your write-veiw");
		return false;
	}

	writeLLayout = new QHBoxLayout;
	if (writeLLayout == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : write-view");
		return false;
	}

	writeRLayout = new QVBoxLayout;
	if (writeRLayout == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : write-view");
		return false;
	}

	writeTableWidget = new QTableWidget;
	if (writeTableWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : write-view");
		return false;
	}

	writeTableWidget->setColumnCount(canDataHeaders.size());
	writeTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	writeTableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
	writeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	writeTableWidget->setHorizontalHeaderLabels(canDataHeaders);

	updateTimer.start(TICK_TIME);

	connect(&updateTimer, SIGNAL(timeout()), this, SLOT(DataUpdateTimerEvent()));

	writeAddBtn = new QPushButton("Add");
	if (writeAddBtn == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your write-view");
		return false;
	}
	connect(writeAddBtn, &QPushButton::clicked, this, &BgyTabView::WriteAddBtnEvent);

	writeDelBtn = new QPushButton("Del");
	if (writeDelBtn == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your write-view");
		return false;
	}
	connect(writeDelBtn, &QPushButton::clicked, this, &BgyTabView::WriteDelBtnEvent);

	writeLLayout->addWidget(writeTableWidget);
	writeLLayout->addWidget(writeSubWidget);
	writeRLayout->addWidget(writeAddBtn);
	writeRLayout->addWidget(writeDelBtn);
	writeSubWidget->setLayout(writeRLayout);
	writeMainWidget->setLayout(writeLLayout);

	return true;
}
bool BgyTabView::SetTraceView() {
	if (traceWidget == nullptr) {
		wDebug("Failed to create trace-view");
		return false;
	}
	if (readLayout != nullptr) {
		wDebug("Failed to create trace-view");
		wDebug("Check your tab-view : trace-view");
		return false;
	}
	if (traceTableWidget != nullptr) {
		wDebug("Failed to create trace-view");
		wDebug("Check your tab-view : trace-view");
		return false;
	}

	traceLayout = new QVBoxLayout;
	if (traceLayout == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : trace-view");
		return false;
	}

	traceTableWidget = new QTableWidget;
	if (traceTableWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : tarce-view");
		return false;
	}

	traceTableWidget->setColumnCount(canDataHeaders.size());
	traceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	traceTableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
	traceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	traceTableWidget->setHorizontalHeaderLabels(canDataHeaders);

	traceLayout->addWidget(traceTableWidget);
	traceWidget->setLayout(traceLayout);

	return true;
}

//quick-view function
bool BgyTabView::SetQucikTraceView() {
	if (quickRigthtWidget == nullptr) {
		wDebug("Failed to create trace-view");
		return false;
	}
	if (quickTraceTableWidget != nullptr) {
		wDebug("Failed to create trace-view");
		wDebug("Check your quick-view : trace-view");
		return false;
	}

	quickTraceLayout = new QVBoxLayout;
	if (quickTraceLayout == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your quck-view : trace-view");
		return false;
	}

	quickTraceTableWidget = new QTableWidget;
	if (quickTraceTableWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your quick-view : tarce-view");
		return false;
	}

	quickTraceTableWidget->setColumnCount(canDataHeaders.size());
	quickTraceTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	quickTraceTableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
	quickTraceTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	quickTraceTableWidget->setHorizontalHeaderLabels(canDataHeaders);

	quickTraceLayout->addWidget(quickTraceTableWidget);
	quickRigthtWidget->setLayout(quickTraceLayout);

	return true;
}
bool BgyTabView::SetCanBtnsView() {
	if (!InitializeCanData()) return false;
	if (!InitializeCanWidget()) return false;
	if (quickBtnsLayout != nullptr) {
		delete(quickBtnsLayout);
		quickBtnsLayout = nullptr;
	}

	quickBtnsLayout = new QVBoxLayout;

	if (!SetCanBtnsData()) return false;
	if (!SetCanBtnsWidget()) return false;

	for (int btn = 0; btn < canBtns.size(); btn++) {
		quickBtnsLayout->addWidget(canBtns[btn]);
	}

	quickLeftWidget->setLayout(quickBtnsLayout);

	return true;
}
bool BgyTabView::SetCanBtnsWidget() {
	if (canBtnHeaders.isEmpty()) {
		wDebug("Failed to create Can-Btn-Widget");
		return false;
	}
	if (!canBtns.isEmpty()) {
		return false;
	}

	QPushButton* tmpBtn = nullptr;
	for (int ary = 0; ary < canBtnHeaders.size(); ary++) {
		tmpBtn = new QPushButton;
		if (tmpBtn == nullptr) {
			wDebug("Somethings wrong");
			wDebug("Check your tab-view : code");
			break;
		}
		tmpBtn->setText(QString::fromWCharArray(canBtnHeaders[ary]->name));
		tmpBtn->setCheckable(true);
		connect(tmpBtn, &QPushButton::toggled, this, &BgyTabView::CanBtnEvent);
		canBtns.push_back(tmpBtn);
	}
	return true;
}

//data-function
bool BgyTabView::InitializeCanWidget() {
	if (canBtns.isEmpty()) return true;

	int size = canBtns.size();
	for (int ary = 0; ary < size; ary++) {
		delete(canBtns[ary]);
	}

	canBtns.clear();
	return true;
}
bool BgyTabView::InitializeCanData() {
	if (canBtnHeaders.isEmpty()) return true;

	int size = canBtnHeaders.size();
	for (int ary = 0; ary < size; ary++) {
		delete(canBtnHeaders[ary]);
	}
	canBtnHeaders.clear();
	return true;
}
bool BgyTabView::SetCanBtnsData() {
	QString tmpString[EXEL_MAX] = { 0, };
	if (!bExel->GetXlsxCellRowValues("Name", tmpString)) {
		wDebug("Failed to Create CanBtn");
		wDebug("Check yolur tab-view : exel");
		return false;
	}

	QString tmpString2[EXEL_MAX] = { 0, };
	//QString* tmpString2 = new QString[EXEL_MAX];
	if (!canBtnHeaders.isEmpty()) canBtnHeaders.clear();

	canBtnsHeader* tmpBtnHeader = nullptr;
	for (int i = 0; ; i++) {
		if (tmpString[i + EXEL_ROW_OFFSET].size() == 0) break;
		tmpBtnHeader = new canBtnsHeader;
		if (tmpBtnHeader == nullptr) {
			wDebug("Somethings wrong");
			wDebug("Check your tab-view : code");
			return false;
		}
		if (!bExel->GetXlsxCellColValues(tmpString[i + EXEL_ROW_OFFSET].toStdString().c_str(), tmpString2)) {
			wDebug("Failed to Create CanBtn");
			wDebug("Check your tab-view : exel");
			break;
		}
		int size = tmpString[i + EXEL_ROW_OFFSET].size();
		tmpString[i + EXEL_ROW_OFFSET].toWCharArray(tmpBtnHeader->name);
		tmpBtnHeader->id = QStringToHex(tmpString2[CAN_DATA_HLIST::ID + EXEL_COL_OFFSET]);
		tmpBtnHeader->dlc = QStringToHex(tmpString2[CAN_DATA_HLIST::DLC + EXEL_COL_OFFSET]);
		for (int byte = 0; byte < DATA_BYTE; byte++) {
			tmpBtnHeader->data[byte] = QStringToHex(tmpString2[CAN_DATA_HLIST::DATA + EXEL_COL_OFFSET + byte]);
		}

		canBtnHeaders.push_back(tmpBtnHeader);
		tmpString2[i].clear();
		tmpString2->clear();
	}

	return	 true;
}
bool BgyTabView::GetTableItemsData() {
	if (bCan == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Cehck your tab-view : data");
		return false;
	}

	//if (!bCan->SetTraceThreadControl(false)) return false;
	if (!bCan->GetCanMessages(&tmpMsg)) return false;
	//if (!bCan->SetTraceThreadControl(true)) return false;

	return true;
}
bool BgyTabView::TableItemsDataSort() {
	int size = traceItems.size();
	if (size == 0) {
		return false;
	}

	QString idTmpData = nullptr;
	QString checkData = nullptr;

	for (int element = size - 1; element > 0; element--) {
		idTmpData = traceItems[element][READ_ITEMARY_ID_NUM].text();
		if (idTmpData == nullptr) {
			wDebug("Soemthings wrong");
			wDebug("Check your tab-view : sort");
			return false;
		}

		for (int sortNum = 0; sortNum < element; sortNum++) {
			checkData = traceItems[sortNum][READ_ITEMARY_ID_NUM].text();
			idTmpData = traceItems[sortNum + 1][READ_ITEMARY_ID_NUM].text();
			//if (idTmpData.toInt() < checkData.toInt())  오름차순
			if (idTmpData.toInt() < checkData.toInt()) {
				traceItems[sortNum + 1].swap(traceItems[sortNum]);
			}
		}
	}

	for (int i = 0; i < traceItems.size(); i++) {
		auto test = traceItems[i][READ_ITEMARY_ID_NUM].text();
	}
	return true;

}
bool BgyTabView::UpdateTraceViewData(QTableWidget* traceTableWidget) {
	if (traceTableWidget == nullptr) return false;
	int size = traceItems.size();
	if (size == 0) return false;

	int rowCnt = traceTableWidget->rowCount();
	if (rowCnt > TRACE_MAX_ELEMENT) {
		InitializeTraceViewData(traceTableWidget);
		rowCnt = 0;
	}

	traceTableWidget->setRowCount(size + rowCnt);

	for (int element = 0; element < size; element++) {
		int itemSize = traceItems[element].size();
		for (int itemElement = 0; itemElement < itemSize; itemElement++)
			traceTableWidget->setItem(element + rowCnt, itemElement, traceItems[element][itemElement].clone());
	}

	if (!UpdateTraceToWrite(traceTableWidget)) return false;

	traceTableWidget->scrollToBottom();

	return true;
}
bool BgyTabView::UpdateTraceToWrite(QTableWidget* traceTableWidget) {
	if (traceTableWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your trace-view");
		return false;
	}
	if (writeTableWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your trace-view");
		return false;
	}

	int rowCnt = writeTableWidget->rowCount();
	int colCnt = writeTableWidget->columnCount();
	int tmpAryMax = rowCnt * colCnt;

	QTableWidgetItem** tmpItem = new QTableWidgetItem * [tmpAryMax];
	int tmpControl = 0;

	for (int row = 0; row < rowCnt; row++) {
		for (int col = 0; col < colCnt; col++) {
			if (writeTableWidget->item(row, col) == nullptr) continue;
			tmpItem[tmpControl++] = writeTableWidget->item(row, col)->clone();
			//tmpItem[tmpControl]->setBackground(writeColor);
		}
	}

	int trRowCnt = traceTableWidget->rowCount();
	int trColCnt = traceTableWidget->columnCount();
	tmpControl = 0;
	traceTableWidget->setRowCount(trRowCnt + rowCnt);

	for (int row = trRowCnt; row < trRowCnt + rowCnt; row++) {
		for (int col = 0; col < trColCnt; col++) {
			if (tmpControl > tmpAryMax) {
				wDebug("Somethings wrong");
				eMessage("Check your code tab-view : trace");
				return false;
			}
			traceTableWidget->setItem(row, col, tmpItem[tmpControl++]);
		}
	}

	return true;
}
bool BgyTabView::InitializeTraceViewData(QTableWidget*) {
	if (this->traceWidget == nullptr) {
		wDebug("Faield to init trace-view");
		wDebug("Check your tab-view : init");
		return false;
	}
	if (this->traceTableWidget == nullptr) {
		wDebug("Faield to init trace-view");
		wDebug("Check your tab-view : init");
		return false;
	}

	int traceItemCol = traceTableWidget->columnCount();
	int traceItemRow = traceTableWidget->rowCount();

	for (int row = 0; row < traceItemRow; row++) {
		for (int col = 0; col < traceItemCol; col++) {
			delete(traceTableWidget->item(row, col));
		}
	}

	//traceTableWidget->clear(); 원래설정
	traceTableWidget->clear();
	traceTableWidget->setRowCount(TRACE_RE_MIN_ROW);

	return true;
}
bool BgyTabView::UpdateReadViewData() {
	int size = traceItems.size();
	if (size == 0) return false;

	readTableWidget->setRowCount(size);
	if (!TableItemsDataSort()) return false;

	int rowCnt = readTableWidget->rowCount();
	int colCnt = readTableWidget->colorCount();
	if (rowCnt == 0 || colCnt == 0) {
		wDebug("Soemthings wrong");
		wDebug("Check your tab-view : udpate");
		return false;
	}

	auto tmpItems = traceItems;
	for (int row = 0; row < size; row++) {
		auto test = tmpItems[row][TRACE_RE_MIN_ROW].text();
		if ((row + 1) >= size) { break; }
		if (test == tmpItems[row + 1][TRACE_RE_MIN_ROW].text()) {
			tmpItems.erase(tmpItems.begin() + row--);
			size = tmpItems.size();
		}
	}


	size = tmpItems.size();
	readTableWidget->setRowCount(size);


	for (int element = 0; element < size; element++) {
		int itemSize = tmpItems[element].size();
		for (int itemElement = 0; itemElement < itemSize; itemElement++) {
			readTableWidget->setItem(element, itemElement, tmpItems[element][itemElement].clone());
		}
	}

	return true;
}
bool BgyTabView::UpdateWriteViewData() {
	int row = writeTableWidget->rowCount();
	if (row == 0) {
		if (writeTmpMsg.size() != 0) {
			writeTmpMsg.clear();
		}
		return true;
	}
	writeTmpMsg.clear();
	QLineEdit* tmpEdit = nullptr;

	CAN_MESSAGE tmpMsg;
	memset(&tmpMsg, 0, sizeof(CAN_MESSAGE));

	int msgSize = writeTmpMsg.size();
	//if (msgSize == row) goto RESEND;
	//해당 셀변경해도 미적용
	//msgSize = 0;

	/*for (int r = msgSize; r < row; r++) {
		tmpEdit = (QLineEdit*)writeTableWidget->cellWidget(r, ID);
		tmpMsg.id = QStringToHex(tmpEdit->text());
		tmpEdit = (QLineEdit*)writeTableWidget->cellWidget(r, DLC);
		tmpMsg.dlc = QStringToHex(tmpEdit->text());
		tmpEdit = (QLineEdit*)writeTableWidget->cellWidget(r, DATA);

		QString tmpStr;
		int dataNum = 0;

		for (int i = 0; i < tmpEdit->text().size(); i++) {
			while (tmpEdit->text()[i] != ' ') {
				tmpStr.append(tmpEdit->text()[i]);
				if (i + 1 >= tmpEdit->text().size())  break;
				i++;
			}
			tmpMsg.data[dataNum++] = QStringToHex(tmpStr);
			tmpStr.clear();
		}

		writeTmpMsg.push_back(tmpMsg);
	}*/
	for (int r = msgSize; r < row; r++) {
		tmpMsg.id = QStringToHex(writeTableWidget->item(r, ID)->text());
		tmpMsg.dlc = QStringToHex(writeTableWidget->item(r, DLC)->text());
		//data bit 계산 필요
		QString dataStr = writeTableWidget->item(r, DATA)->text();
		QString tmpStr;
		int dataNum = 0;

		for (int i = 0; i < dataStr.size(); i++) {
			while (dataStr[i] != ' ') {
				tmpStr.append(dataStr[i]);
				if (i + 1 >= dataStr.size())  break;
				i++;
			}
			tmpMsg.data[dataNum++] = QStringToHex(tmpStr.toStdString().c_str());
			tmpStr.clear();
		}

		writeTmpMsg.push_back(tmpMsg);
	}

// writeTmpMsg sort
	int size = writeTmpMsg.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < i; j++) {
			if (writeTmpMsg[i].id < writeTmpMsg[j].id) {
				auto temp = writeTmpMsg[i];
				writeTmpMsg[i] = writeTmpMsg[j];
				writeTmpMsg[j] = temp;
			}
		}
	}

//DATA 탐색
	for (int i = 0; i < size; i++) {
		if (i + 1 >=  size) break;
		if (writeTmpMsg[i].id == writeTmpMsg[i + 1].id) {
			for (int j = 0; j < DATA_BYTE; j++) {
				memset(&tmpMsg, 0, sizeof(CAN_MESSAGE));
				tmpMsg.data[i] = writeTmpMsg[i].data[j] + writeTmpMsg[i + 1].data[j];
				writeTmpMsg[i + 1].data[j] = tmpMsg.data[i];
			}
			writeTmpMsg.remove(i);
			size = writeTmpMsg.size();
		}
	}

RESEND:
	if (!bCan->WriteCanMessage(&writeTmpMsg)) return false;

	return true;
}
bool BgyTabView::GetWriteViewData(QString itemName, int* row, int* col) {
	int rowMax = writeTableWidget->rowCount();
	int colMax = writeTableWidget->columnCount();

	if (rowMax <= 0 || colMax <=0) return false;

	for (int currentRow = 0; currentRow < rowMax; currentRow++) {
		for (int currentCol = 0; currentCol < colMax; currentCol++) {
			QString currentItem = writeTableWidget->item(currentRow, currentCol)->text();
			if (itemName == currentItem) {
				if (row == nullptr || col == nullptr) return true;
				*row = currentRow;
				*col = currentCol;
				return true;
			}
		}
	}

	return false;
}

QString BgyTabView::ConvertDataToString(int row) {
	if (tmpMsg.isEmpty()) {
		wDebug("Failed to Convert String");
		wDebug("check your tab-view : code");
		return NULL;
	}

	QString dataString;

	for (int byte = 0; byte < DATA_BYTE; byte++) {
		QString tmpData = QString::number(tmpMsg[row].data[byte]);
		dataString.append(" ");
		dataString.append(tmpData);
	}

	return dataString;
}

void BgyTabView::wMessage(const char* msg) {
	if (preMsgBox != nullptr) {
		delete(preMsgBox);
	}
	preMsgBox = new QMessageBox;
	preMsgBox->setText(msg);
	preMsgBox->setWindowTitle("WARNINIG");
	preMsgBox->show();
}
void BgyTabView::wDebug(const char* msg) {
	std::cout << "Serial : " << msg << std::endl;
}
void BgyTabView::eMessage(const char* msg) {
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

//private slots
bool BgyTabView::DataUpdateTimerEvent() {
	if (readWidget == nullptr || traceWidget == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your tab-view : event");
		return false;
	}

	tmpMsg.clear();
	if (!this->GetTableItemsData()) return false;

	if (tmpMsg.empty()) {
		return false;
	}

	int size = tmpMsg.size();
	if (size == 0) return false;

	if (traceItems.size() > 0) {
		/*for (int i = 0; i < traceItems.size(); i++) {
		}*/
		traceItems.clear();
	}

	QVector<QTableWidgetItem> tmpWidget;
	tmpWidget.clear();
	for (int i = 0; i < size; i++) {
		traceItems.push_back(tmpWidget);
	}

	for (int msgIdx = 0; msgIdx < size; msgIdx++) {
		QTableWidgetItem tmpItem;

		tmpItem.setText(QString::number(tmpMsg[msgIdx].id));
		traceItems[msgIdx].push_back(tmpItem);
		tmpItem.setText(QString::number(tmpMsg[msgIdx].dlc));
		traceItems[msgIdx].push_back(tmpItem);


		//auto reuslt = ConvertDataToString(msgIdx); //data check

		tmpItem.setText(ConvertDataToString(msgIdx));
		traceItems[msgIdx].push_back(tmpItem);
	}

	//순서 고정 : 정렬 알고리즘 클래스 변수에 직접 변경을 하기 때문

	UpdateTraceViewData(traceTableWidget);
	UpdateTraceViewData(quickTraceTableWidget);
	UpdateReadViewData();
	UpdateWriteViewData();

	return true;
}
bool BgyTabView::WriteAddBtnEvent() {
	if (writeAddBtn == nullptr) {
		wDebug("Failed to connect event");
		wDebug("Check your tab-view : code");
		return false;
	}

	if (!SetAddPopupComponent()) return false;

	return true;

}
bool BgyTabView::WriteDelBtnEvent() {
	if (writeDelBtn == nullptr) {
		wDebug("Failed to connect event");
		wDebug("Check your tab-view : code");
		return false;
	}

	int delRow = writeTableWidget->currentRow();
	if (delRow < 0) return false;
	for (int col = 0; col < writeTableWidget->colorCount(); col++) delete(writeTableWidget->item(delRow, col));

	/*if (writeTmpMsg.size() != 0)
		writeTmpMsg.removeAt(delRow);*/

	writeTableWidget->removeRow(delRow);

	return true;
}
bool BgyTabView::WritePopupOkBtnEvent() {
	if (addPopupOkBtn == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your Popup");
		return false;
	}
	if (addPopup == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your Popup");
		return false;
	}

	const char* empty = "";

	if (idEdit->text() == empty)			return false;
	if (firstDataEdit->text() == empty)		return false;
	if (secondDataEdit->text() == empty)	return false;
	if (thirdDataEdit->text() == empty)		return false;
	if (fourthDataEdit->text() == empty)	return false;
	if (fifthDataEdit->text() == empty)		return false;
	if (sixthDataEdit->text() == empty)		return false;
	if (seventhDataEdit->text() == empty)	return false;

	int row = writeTableWidget->rowCount();
	writeTableWidget->setRowCount(row + 1);



	/*int headerListCnt = canDataHeaders.count();
	  QVector<QLineEdit*> writeTmpEdit;

	for (int headDataCnt = 0; headDataCnt < headerListCnt; headDataCnt++) {
		writeTmpEdit.push_back(new QLineEdit);
	}

	for (int headDataCnt = 0; headDataCnt < headerListCnt; headDataCnt++) {
		if (headDataCnt == CAN_DATA_HEADER_LIST::ID) writeTmpEdit[headDataCnt]->setValidator(idValidator);
		else
			writeTmpEdit[headDataCnt]->setValidator(dataValidator);
	}

	QTableWidgetItem writeTmpItem;

	writeTmpEdit[CAN_DATA_HEADER_LIST::ID]->setText(idEdit->text());
	writeTableWidget->setCellWidget(row, CAN_DATA_HLIST::ID, writeTmpEdit[ID]);
	writeTmpEdit[CAN_DATA_HEADER_LIST::DLC]->setText(QString::number(DATA_BYTE));
	writeTableWidget->setCellWidget(row, CAN_DATA_HLIST::DLC, writeTmpEdit[DLC]);

	QString addData;
	addData.append(firstDataEdit->text());
	addData.append(" ");
	addData.append(secondDataEdit->text());
	addData.append(" ");
	addData.append(thirdDataEdit->text());
	addData.append(" ");
	addData.append(fourthDataEdit->text());
	addData.append(" ");
	addData.append(fifthDataEdit->text());
	addData.append(" ");
	addData.append(sixthDataEdit->text());
	addData.append(" ");
	addData.append(seventhDataEdit->text());
	addData.append(" ");
	addData.append(eighthDataEdit->text());

	writeTmpEdit[CAN_DATA_HEADER_LIST::DATA]->setText(addData);
	writeTableWidget->setCellWidget(row, CAN_DATA_HLIST::DATA, writeTmpEdit[DATA]);*/

	QTableWidgetItem writeTmpItem;
	writeTmpItem.setBackground(writeColor);
	writeTmpItem.setText(idEdit->text());
	writeTableWidget->setItem(row, CAN_DATA_HLIST::ID, writeTmpItem.clone());
	writeTmpItem.setText(QString::number(DATA_BYTE));
	writeTableWidget->setItem(row, CAN_DATA_HLIST::DLC, writeTmpItem.clone());

	QString addData;
	addData.append(firstDataEdit->text());
	addData.append(" ");
	addData.append(secondDataEdit->text());
	addData.append(" ");
	addData.append(thirdDataEdit->text());
	addData.append(" ");
	addData.append(fourthDataEdit->text());
	addData.append(" ");
	addData.append(fifthDataEdit->text());
	addData.append(" ");
	addData.append(sixthDataEdit->text());
	addData.append(" ");
	addData.append(seventhDataEdit->text());
	addData.append(" ");
	addData.append(eighthDataEdit->text());

	writeTmpItem.setText(addData);
	writeTableWidget->setItem(row, CAN_DATA_HLIST::DATA, writeTmpItem.clone());

	addPopup->close();
	return true;
}
bool BgyTabView::WritePopupCloseBtnEvent() {
	if (addPopupCloseBtn == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your Popup");
	}
	if (addPopup == nullptr) {
		wDebug("Somethings wrong");
		wDebug("Check your Popup");
		return false;
	}

	addPopup->close();

	return true;
}
bool BgyTabView::CanBtnEvent() {
	int size = canBtns.size();
	int row = 0;
	int col = 0;

	if (size == 0) return true;

	for (int btnAry = 0; btnAry < size; btnAry++) {
		if (canBtns[btnAry]->isChecked()) {
			wchar_t cmpText[MAX_NAME] = { 0, };
			canBtns[btnAry]->text().toWCharArray(cmpText);
			if (wcscmp(cmpText, canBtnHeaders[btnAry]->name) == 0) {
				if (GetWriteViewData(QString::number(canBtnHeaders[btnAry]->id, 16))) {
					QString* datas = new QString;
					for (int dataAry = 0; dataAry < DATA_BYTE; dataAry++) {
						datas->append(QString::number(canBtnHeaders[btnAry]->data[dataAry], 16));
						datas->append(" ");
					}
					if (GetWriteViewData(*datas)) {
						delete(datas);
						continue;
					}
				}

				int row = writeTableWidget->rowCount();
				writeTableWidget->setRowCount(row + 1);

				QTableWidgetItem* tmpItem = new QTableWidgetItem;
				tmpItem->setBackground(writeColor);

				tmpItem->setText(QString::number(canBtnHeaders[btnAry]->id, 16));
				writeTableWidget->setItem(row, CAN_DATA_HEADER_LIST::ID, tmpItem->clone());
				tmpItem->setText(QString::number(canBtnHeaders[btnAry]->dlc));
				writeTableWidget->setItem(row, CAN_DATA_HEADER_LIST::DLC, tmpItem->clone());

				//data-byte calc
				QString* addData = new QString;
				for (int dataAry = 0; dataAry < DATA_BYTE; dataAry++) {
					addData->append(QString::number(canBtnHeaders[btnAry]->data[dataAry], 16));
					addData->append(" ");
				}

				tmpItem->setText(*addData);
				writeTableWidget->setItem(row, CAN_DATA_HEADER_LIST::DATA, tmpItem->clone());

				//Initialize dynamic-val
				delete(tmpItem);
				tmpItem = nullptr;
				delete(addData);
				addData = nullptr;

				/*CAN_MESSAGE* tmpMsg = new CAN_MESSAGE;
				memset(tmpMsg, 0, sizeof(CAN_MESSAGE));

				tmpMsg->id = canBtnHeaders[btnAry]->id;
				tmpMsg->dlc = canBtnHeaders[btnAry]->dlc;
				for (int dataAry = 0; dataAry < DATA_BYTE; dataAry++) {
					tmpMsg->data[dataAry] = canBtnHeaders[btnAry]->data[dataAry];
				}

				writeTmpMsg.push_back(*tmpMsg);
				delete(tmpMsg);*/
			}
		}
		else {
			int rowMax = writeTableWidget->rowCount();
			/*if (rowMax == 0) return true;*/

			wchar_t cmpText[MAX_NAME] = { 0, };
			canBtns[btnAry]->text().toWCharArray(cmpText);

			if (wcscmp(cmpText, canBtnHeaders[btnAry]->name) == 0) {
				if (GetWriteViewData(QString::number(canBtnHeaders[btnAry]->id, 16))) {
					QString* datas = new QString;
					for (int dataAry = 0; dataAry < DATA_BYTE; dataAry++) {
						datas->append(QString::number(canBtnHeaders[btnAry]->data[dataAry], 16));
						datas->append(" ");
					}
					if (GetWriteViewData(*datas, &row, &col)) {
						for (int delCol = 0; delCol < col; delCol++) 
							delete(writeTableWidget->item(row, delCol));
						writeTableWidget->removeRow(row);
						writeTableWidget->setRowCount(rowMax-1);
						delete(datas);
						return true;
					}

				}
			}
		}
	}
	return true;
}