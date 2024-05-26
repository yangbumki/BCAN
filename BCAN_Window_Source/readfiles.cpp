#include "readfiles.h"

//public Function

ReadFiles::ReadFiles(const char* path) {
    if(path != nullptr)
        SetFilePath(path);
}
ReadFiles::~ReadFiles() {
}

bool ReadFiles::SetWindowFileOpen(HANDLE fileHandle) {
#if WINDOW_VER
    if(fileHandles != nullptr) {
        wDebug("Aleady exist fileHandles");
        wDebug("Check your ReadFiles");

        if(this->fileHandles != NULL) CloseHandle(fileHandles);
        fileHandles = fileHandle;
    }

    fileHandle = CreateFileA(this->filePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL ,NULL );
    if(fileHandle == INVALID_HANDLE_VALUE) eMessage("Failed to open files");
#endif

    return true;
}

void ReadFiles::SetFilePath(const char* path) {
    memset(filePath, 0, MAX_PATH);
    strcpy(filePath, path);
}

void ReadFiles::SetFilePath(const std::string path) {
    memset(filePath, 0, MAX_PATH);
    strcpy(filePath, path.c_str());
}

bool ReadFiles::SetReadsFile() {
#if WINDOW_VER
    int readedBytes = 0;
    if(!ReadFile(fileHandles, this->buffer, BUFSIZE, (LPDWORD)readedBytes, NULL)) {
        wDebug("Failed to read files");
        return false;
    }
#endif
    return true;
}

#if DEBUGGING
bool ReadFiles::SetDebuggingReadsFile(HANDLE fileHandle) {
#if WINDOW_VER
    int readedBytes = 0;
    while(ReadFile(fileHandle, this->buffer, BUFSIZE, (LPDWORD)&readedBytes, NULL)) {
        if(readedBytes == 0 ) {
            wDebug("Failed to find string");
            return false;
        }

        for(int i=0; i<BUFSIZE; i++) {
            if(buffer[i] == DEBUGGING_CAN_ID[0]) {
                if(strcmp(buffer+i,DEBUGGING_CAN_ID) == 0) {
                    wDebug("Find :) ");
                    return true;
                }
                int len = strlen(DEBUGGING_CAN_ID);
                if(tmp == nullptr) tmp = new char(len);
                memset(tmp, 0, len);
                strncpy(tmp, buffer+i, len);

                wDebug("-- Buffer --");
                wDebug(tmp);
            }
        }
    }
    delete(tmp);
    return true;
}
#endif
#endif
bool ReadFiles::SetXlsxFileOpen(const char* sheetName, QXlsx::Document* xlsx) {
#if XLSX
    if(filePath[0] == NULL) {
        wDebug("Failed to read filePath");
        wDebug("Check Your ReadFiles");
        return false;
    }
    if(xlsx != nullptr) {
        wDebug("Aleady exist xlsx");
        wDebug("Check your ReadFiles");

        if(this->exelFiles != nullptr) delete(exelFiles);
        exelFiles = xlsx;
    }
    if(workSheet != nullptr) {
        wDebug("Aleady exist Worksheet");
        wDebug("Check Your ReadFiles");
        workSheet = nullptr;
        delete(workSheet);
    }
    if(workBook != nullptr) {
        wDebug("Aleady exist workbook");
        wDebug("check Your ReadFiles");
    }

    exelFiles = new QXlsx::Document(this->filePath);
    if(!exelFiles->load()) {
        wDebug("Failed to load xlsx");
        return false;
    }

    workBook = exelFiles->currentWorksheet()->workbook();
    if(workBook == nullptr) {
        wDebug("Failed to load workbook");
        return false;
    }

    int totSheets = workBook->sheetCount();
    if(totSheets <= 0) {
        wDebug("Somethings wrong");
        wDebug("Check your exel-files");
        return false;
    }

    if (sheetName == nullptr) {
        if (!workBook->setActiveSheet(0)) {
            wDebug("Failed to active sheet");
            return false;
        }
        workSheet = (QXlsx::Worksheet*)workBook->activeSheet();
        if (workSheet == nullptr) {
            wDebug("Failed to load worksheet");
            return false;
        }
        GetXlsxCellData();
        return true;
    }

    QString currentSheetName = nullptr;

    for(int i=0; i<totSheets; i++) {
        currentSheetName = workBook->sheet(i)->sheetName();
        if(currentSheetName == nullptr) {
            wDebug("Somethings wrong");
            wDebug("Check your readfiles");
            return false;
        }

        if(strcmp(sheetName, currentSheetName.toStdString().c_str()) == 0) {
            if(!workBook->setActiveSheet(i)) {
                wDebug("Failed to active sheet");
                return false;
            }
            workSheet = (QXlsx::Worksheet*)workBook->activeSheet();
            if(workSheet == nullptr) {
                wDebug("Failed to load worksheet");
                return false;
            }
            break;
        }
    }
    GetXlsxCellData();
#if DEBUGGING
    wDebug("---Sheets Count---");
    int totalSheets = exelFiles->workbook()->sheetCount();

    for(int i=0; i<totalSheets; i++) {
        wDebug(exelFiles->workbook()->sheet(i)->sheetName().toStdString().c_str());
    }
#endif
    return true;
#endif
}

bool ReadFiles::GetXlsxCellData() {
    if(exelFiles == nullptr || workSheet == nullptr) {
        wDebug("Failed to Get Cell Data : Exel Files");
        return false;
    }
    cellLoc = workSheet->getFullCells(&maxRow, &maxCol);
    if(maxRow < 0 || maxCol < 0) {
        wDebug("Failed to get-cells");
        return false;
    }

    if(!InitXlsxCellVal()) { return false; }
    if(!GetXlsxTotalCellValues()) { return false; }

#if DEBUGGING
    wDebug("--- MAX_ROW & MAX_COL ---");
    wDebug(std::to_string(maxRow).c_str());
    wDebug(std::to_string(maxCol).c_str());
#endif
}

//private function
bool ReadFiles::InitXlsxCellVal() {
    if(maxRow == -1 || maxCol == -1) {
        wDebug("Somethings wrong");
        wDebug("Check your readfiles : cellData");
        return false;
    }

    if(!cellValues.empty()) {
        wDebug("Aleady exist Cell-Data");
        cellValues.clear();
        cellValues.shrink_to_fit();
    }

    QVector<QString> tmpVec;

    for(int r=0; r<maxRow; r++) {
        for(int c=0; c<maxCol; c++) {
            tmpVec.push_back(QString(""));
        }
        cellValues.push_back(tmpVec);
    }

    return true;
}

bool ReadFiles::GetXlsxTotalCellValues() {
    if(cellValues.empty()) {
        wDebug("Doesn't exist cell-data");
        wDebug("Check your read-files : initialize");
        return false;
    }

    QXlsx::CellLocation tmpLoc;
    QString tmpString = nullptr;
    int tmpRow = 0;
    int tmpCol = 0;

    for(int i=0; i< cellLoc.size(); ++i) {
        tmpLoc = cellLoc.at(i);

        tmpRow = tmpLoc.row -1;
        tmpCol = tmpLoc.col -1;
        if(tmpRow < 0 || tmpCol < 0) {
            wDebug("Somethings wrong");
            wDebug("Check your read-files : cell-data");
            return false;
        }

        tmpString = tmpLoc.cell->value().toString();
        /*if(tmpString == nullptr) {
            wDebug("Somethings wrong");
            wDebug("Check your read-files : cell-data");
            return false;
        }*/

        cellValues[tmpRow][tmpCol] = tmpString;
    }

    return true;
}

bool ReadFiles::GetXlsxCellRowValues(const char* cellName, QString storage[]) {
    if(maxRow == -1 || maxCol == -1) {
        wDebug("Somethings wrong");
        wDebug("Check your readfiles : cellData");
        return false;
    }

    if(cellValues.empty()) {
        wDebug("Doesn't exist cell-data");
        wDebug("Check your read-files : initialize");
        return false;
    }

    if(!storage->isEmpty()) {
        wDebug("Aleady exist storage");
        wDebug("Check your code : cell-sotrage");
        return false;
    }

    QString currentCellValue = nullptr;

    for(int r=0; r<maxRow; r++) {
        for(int c=0; c<maxCol; c++) {
            currentCellValue = cellValues[r][c];
            auto result = strcmp(cellName, currentCellValue.toStdString().c_str());
            if(result == 0) {
                int cnt = maxRow-r;
                for(int i=0; i<cnt; i++) {
                    if (cellValues[r+i][c] == nullptr) break;
                    storage[i] = cellValues[r+i][c];
                }
                return true;
            }
        }
    }

    wDebug("Failed to find cell");
    return false;
}
//bool ReadFiles::GetXlsxCellRowValues(const char* cellName, QString* storage) {
//    if (storage != nullptr) {
//        wDebug("Aleady exist storage");
//        wDebug("Check your code : read-files");
//        storage->clear();
//        delete(storage);
//    }
//
//    storage = new QString[maxRow * maxCol];
//    if (storage == nullptr) {
//        wDebug("Somethings wrong");
//        wDebug("Check yoru read-files");
//        return false;
//    }
//
//    QString currentCellValue = nullptr;
//
//    for (int r = 0; r < maxRow; r++) {
//        for (int c = 0; c < maxCol; c++) {
//            currentCellValue = cellValues[r][c];
//            auto result = strcmp(cellName, currentCellValue.toStdString().c_str());
//            if (result == 0) {
//                int cnt = maxRow - r;
//                for (int i = 0; i < cnt; i++) {
//                    storage->push_back(cellValues[r + i][c]);
//                }
//                return true;
//            }
//        }
//    }
//
//    wDebug("Failed to find cell");
//    return false;
//
//
//}

bool ReadFiles::GetXlsxCellColValues(const char* cellName, QString storage[]) {
    if(maxRow == -1 || maxCol == -1) {
        wDebug("Somethings wrong");
        wDebug("Check your readfiles : cellData");
        return false;
    }

    if(cellValues.empty()) {
        wDebug("Doesn't exist cell-data");
        wDebug("Check your read-files : initialize");
        return false;
    }

    if(!storage->isEmpty()) {
        wDebug("Aleady exist storage");
        wDebug("Check your code : cell-sotrage");
        return false;
    }

    QString currentCellValue = nullptr;

    for(int r=0; r<maxRow; r++) {
        for(int c=0; c<maxCol; c++) {
            currentCellValue = cellValues[r][c];
            auto result = strcmp(cellName, currentCellValue.toStdString().c_str());
            if(result == 0) {
                int cnt = maxCol-c;
                for(int i=0; i<cnt; i++) {
                    storage[i] = cellValues[r][c+i];
                }
                return true;
            }
        }
    }

    wDebug("Failed to find cell");
    return false;
}
//bool ReadFiles::GetXlsxCellColValues(const char* cellName, QString* storage) {
//    if (maxRow == -1 || maxCol == -1) {
//        wDebug("Somethings wrong");
//        wDebug("Check your readfiles : cellData");
//        return false;
//    }
//
//    if (cellValues.empty()) {
//        wDebug("Doesn't exist cell-data");
//        wDebug("Check your read-files : initialize");
//        return false;
//    }
//
//    if (storage != nullptr) {
//        wDebug("Aleady exist storage");
//        wDebug("Check your code : readfiles");
//        storage->clear();
//        delete(storage);
//    }
//    storage = new QString[maxRow*maxCol];
//    if (storage == nullptr) {
//        wDebug("Somethings wrong");
//        wDebug("Check your code : readfiles");
//        return false;
//    }
//    
//    
//    QString currentCellValue = nullptr;
//
//    for (int r = 0; r < maxRow; r++) {
//        for (int c = 0; c < maxCol; c++) {
//            currentCellValue = cellValues[r][c];
//            auto result = strcmp(cellName, currentCellValue.toStdString().c_str());
//            if (result == 0) {
//                int cnt = maxCol - c;
//                for (int i = 0; i < cnt; i++) {
//                    storage->push_back(cellValues[r][c + i]);
//                }
//                return true;
//            }
//        }
//    }
//
//    wDebug("Failed to find cell");
//    return false;
//}


void ReadFiles::wMessage(const char* msg) {
    if(preMsgBox != nullptr) {
        delete(preMsgBox);
    }
    preMsgBox = new QMessageBox;
    preMsgBox->setText(msg);
    preMsgBox->setWindowTitle("WARNINIG");
    preMsgBox->show();
}

void ReadFiles::wDebug(const char* msg) {
    cout<<"ReadFiles : "<<msg<<endl;
}

void ReadFiles::eMessage(const char* msg) {
    if(preMsgBox != nullptr) {
        delete(preMsgBox);
    }
    preMsgBox = new QMessageBox;
    preMsgBox->setText(msg);
    preMsgBox->setWindowTitle("ERROR");
    preMsgBox->show();

    while(!preMsgBox->exec());
    exit(1);
}
