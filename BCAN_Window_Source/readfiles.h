#ifndef READFILES_H
#define READFILES_H

#include "Common.h"

#if XLSX
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;
#endif

#if WINDOW_VER
#include <windows.h>
#endif

#include <QMessageBox>

#define BUFSIZE     1024

using namespace std;

typedef class ReadFiles
{
private:
// window variable
#if WINDOW_VER
    HANDLE fileHandles = NULL;
#endif

#if DEBUGGING
    const char* DEBUGGING_CAN_ID = "CAN_ID";
    char* tmp = nullptr;
#endif

//XLSX Variable
#if XLSX
//XLSX File Variable
    QXlsx::Document* exelFiles  = nullptr;
    QXlsx::Workbook* workBook   = nullptr;
    QXlsx::Worksheet* workSheet = nullptr;

//Cell Value Variable
    int maxRow = -1;
    int maxCol = -1;
    QVector<QXlsx::CellLocation> cellLoc;
    QVector<QVector<QString>> cellValues;
#endif

//common varialbe
    char filePath[MAX_PATH] = {NULL, };
    char buffer[BUFSIZE] = {0, };

    QMessageBox* preMsgBox = nullptr;

// XLSX function
    bool InitXlsxCellVal();
    bool GetXlsxTotalCellValues();
    bool GetXlsxCellData();

//common function
    void wMessage(const char* msg);
    void wDebug(const char* msg);
    void eMessage(const char* msg);


public:
    //windows
#if WINDOW_VER
    bool SetWindowFileOpen(HANDLE);
    bool SetReadsFile();
#if DEBUGGING
    bool SetDebuggingReadsFile(HANDLE);
#endif
#endif
#if XLSX
    //xlsx
    bool SetXlsxFileOpen(const char* sheetName=nullptr, QXlsx::Document* xlsx = nullptr);
    bool SetXlsxReadFile();
    bool GetXlsxCellRowValues(const char* cellName, QString storage[]);
    //bool GetXlsxCellRowValues(const char* cellName, QString* storage);
    bool GetXlsxCellColValues(const char* cellName, QString storage[]);
    //bool GetXlsxCellColValues(const char* cellName, QString* storage);
#endif
//common Function
    ReadFiles(const char* path = nullptr);
    ~ReadFiles();
    void SetFilePath(const char* path);
    void SetFilePath(const std::string path);
}BGY_EXEL;

#endif // READFILES_H
