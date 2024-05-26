#define WINDOW 1

#include <QApplication>
#include <QTime>
#include <conio.h>

#include "serial.h"
#include "readfiles.h"

//UI
#include "BGYMainWindow.h"

#define UI_ONLY 1

#if DEBUGGING       1
#define TICK        1
#define ESC         27
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("BGY_CAN");

    BGY_CAN bCan;
    BGY_EXEL bExel;

#if UI_ONLY
    BGYMainWindow mw(&bCan, &bExel);
    mw.show();
#else
#if DEBUGGING
    QTime time;

    Serial s1("COM8");

    ReadFiles rf("C:\\Users\\bgyang\\Desktop\\CAN\\Can_QT\\BGYCan\\TestReadFiles\\TestReadFile.xlsx");
    rf.SetXlsxFileOpen("Sheet1");

    QString storage[100];

    rf.GetXlsxCellRowValues("CAN_ID", storage);
    qDebug("--- Cell-Data : Raw ---");
    memset(storage, 0, sizeof(QString)*100);
    rf.GetXlsxCellColValues("CAN_ID", storage);
    qDebug("--- Cell-Data : Col ---");
    qDebug("%s", storage);


    int currentTime = 0, tickTime = TICK, lastTime = 0;


    currentTime = lastTime = time.currentTime().second();

    while(1) {
        currentTime = time.currentTime().second();
        if((currentTime - lastTime) >= tickTime) {
            lastTime = currentTime;

            if(_kbhit()) { if(_getch() == ESC) break; }


        }
    }
#endif
#endif

    return app.exec();
}
