#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 读取qss中的内容
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open /style/stylesheet.qss success!");
        // qss.readAll() 返回的是QByteArray， 需要将其转为QLatin1String类型
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("open /style/stylesheet.qss fault!");
    }

    MainWindow w;
    w.show();
    return a.exec();
}
