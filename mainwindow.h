#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QVBoxLayout>

#include <QMainWindow>

class QMenu;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QMenu *fileMenu;
    QLabel* infoLabel;
    void createActions();
    void createMenus();
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU
};

#endif // MAINWINDOW_H
