
#ifndef StimulatorMainWindow_H
#define StimulatorMainWindow_H

#include "udp_receiver.h"
#include <QMainWindow>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

class StimulatorMainWindow : public QMainWindow
{
    Q_OBJECT

public: 
    StimulatorMainWindow();
    void setTitle();
    void setMapping();
    void setPattern();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private slots:

    void getRemoteSig(uint8_t);
    void changeDir();
    void changePWMmode();
    void openFileHere(QString);
    void set_LEs();
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
    void save11();
    void saveWithName();
    void openFile();
    void openFile(QString s);
    void saveAs();
    void setLineSpacing();
    void setParagraphSpacing();
    void about();


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
    QAction *protAct;
    QAction *editorAct;
    QAction *exitAct;
    QAction *setLineSpacingAct;
    QAction *setParagraphSpacingAct;
    QAction *aboutAct;
    QLabel *infoLabel;
};

#endif
