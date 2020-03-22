#include "mainwindow.h"
#include "qtdatavisualizer.h"

#include <QApplication>

#define OLD_FILENAME "/home/rmasl/Desktop/Evan_Personal_Use/SeniorD/qt_data_display/qt_datalog_visualizer/logs/2017-04-26_10_49_0.txt"
#define FILENAME "/home/rmasl/Desktop/Evan_Personal_Use/SeniorD/qt_data_display/qt_datalog_visualizer/logs/2020-03-12_6_54_0.txt"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();
    QtDataVisualizer *qtdv = new QtDataVisualizer();
    qtdv->load_file(FILENAME);
    //qtdv->display_params(14, 19, 300, 320,-0.5,0.5,0);
    QList<int>* yparams = new QList<int>;
    yparams->append(17);
    yparams->append(18);
    yparams->append(19);
    qtdv->display_multiplot(yparams, 0, 0.0, 10.0, -10, 10, 0);
    //VRPN 17, ALT Setpoint is 14
    qtdv->show();

    return a.exec();
}
