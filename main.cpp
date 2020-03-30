#include "mainwindow.h"
#include "qtdatavisualizer.h"
#include <QApplication>

#define OLD_FILENAME "/home/rmasl/Desktop/Evan_Personal_Use/SeniorD/qt_data_display/qt_datalog_visualizer/logs/2017-04-26_10_49_0.txt"
#define FILENAME "/home/rmasl/Desktop/Evan_Personal_Use/SeniorD/qt_data_display/qt_datalog_visualizer/logs/2020-03-12_6_54_0.txt"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //toolbar* tb = new toolbar();
    //tb->show();
    QtDataVisualizer *qtdv = new QtDataVisualizer();
    qtdv->load_file(FILENAME);
    qtdv->display_multiplot();
    qtdv->show();
    return a.exec();
}
