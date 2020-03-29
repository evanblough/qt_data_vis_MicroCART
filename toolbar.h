#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QTreeView>
#include <QPushButton>
#include <QFileSystemModel>

namespace Ui {
class toolbar;
}

class toolbar : public QWidget
{
    Q_OBJECT

public:
    explicit toolbar(QWidget *parent = nullptr);
    ~toolbar();
    QFileSystemModel *model;
    QTreeView* file_display;
    QPushButton* open_file, add_y;
    QList<int>* y_params;
    int x_index, graph_widht, graph_height;

private:
    Ui::toolbar *ui;
};

#endif // TOOLBAR_H
