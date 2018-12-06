
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serial.h"
#include <QMainWindow>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Serial* hSerial;
    MainWindow();
    void setTitle();
    void setMapping();


protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private slots:
    void setInterval();
    void oneSend();
    void setLinearMapping();
    void setConfigMapping();
    void loopChanged();
    void protocolRoutine();
    void changeDim();
    void COMInit();
    void patternPlayPressed();
    void protPlayPressed();
    void updateProtocol();
    void protChecked();
    void editorChecked();
    void newFile();
    void open();
    void save();
    void saveWithName();
    void openWithName();
    void openWithName(QString s);
    void saveAs();
    void print();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void bold();
    void italic();
    void leftAlign();
    void rightAlign();
    void justify();
    void center();
    void setLineSpacing();
    void setParagraphSpacing();
    void about();
    void aboutQt();

private:
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *editMenu;

    QMenu *formatMenu;
    QMenu *helpMenu;

    QActionGroup *alignmentGroup;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAsAct;
    QAction *saveAct;
    QAction *printAct;
    QAction *protAct;
    QAction *editorAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *boldAct;
    QAction *italicAct;
    QAction *leftAlignAct;
    QAction *rightAlignAct;
    QAction *justifyAct;
    QAction *centerAct;
    QAction *setLineSpacingAct;
    QAction *setParagraphSpacingAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QLabel *infoLabel;
};

#endif
