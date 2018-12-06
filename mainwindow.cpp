//http://doc.qt.io/qt-5/qtwidgets-mainwindows-menus-example.html
//names: prot (protocol) vs stimulator
//forbit to open inappropriate extensions
//editor
#include <QtWidgets>
#include "canvaswidget.h"
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QtSerialPort>
#include <QLabel>
#include <QStackedWidget>
#include <QTimer>



QStackedWidget* stackedWidget;

bool protPlayOn=0, patternPlayOn=0;
QPixmap* pixmapPlay;
QPixmap* pixmapStop;
QPixmap* pixmapLoop;
QPixmap* pixmapLoopStop;
QString prot_name=QString("untitled.ptcl");
QString pattern_name=QString("untitled.ptn");
QPoint mouse_p;
QPushButton* save_OK_btn;
QPushButton* open_OK_btn;
QPushButton* prot_play_btn;
QPushButton* pattern_play_btn;
QPushButton* prot_loop_btn;
QPushButton* linear_mapping_btn;
QPushButton* config_mapping_btn;
QLineEdit* save_le;
QLineEdit* open_le;
QLineEdit* serial_le;
QLineEdit* PWM_le;
QLineEdit* Nx_le;
QLineEdit* Ny_le;
QLineEdit* interval_le;
QMainWindow* saveWindow;
QMainWindow* openWindow;
//////////////////////
QLineEdit* prot_le;
QWidget *protWidget;
QWidget *patternWidget;
QGridLayout *patternCentralLayout;
QGridLayout *protCentralLayout;
canvasWidget* patternFiller;
QSerialPort port;
QTimer timer;
//Serial* hSerial;
QString qstr;

bool protPlayFlag;
bool mode;//0-editor mode, 1-prot?

QFile* prot_file;
QFile* pattern_file;
QTextStream* out;


int prot_N=1;
int *vib_from_s; //s - посылка
int *s_from_vib;
int vibro_rad_stat=17;
int prot_le_s=10;
int *vibro_rad;
int vibro_step;
int *vibro_x;
int *vibro_y;
int *vibro_state;
int **vibro_n;
int Nx;
int Ny;
int checked_n=0;
int prot_ind;
bool prot_loop_ON=1;
bool serial_inited=0;

int getVibroNum(int i,int j);

int getVibroI(int n);

int getVibroJ(int n);

template<typename T>
void memory_alloc(T** x, int s);

template<typename T>
void memory_dealloc(T** x);

void main_alloc(int Nx, int Ny);

void main_dealloc(int Nx, int Ny);

MainWindow::MainWindow()
{
    mode=1;

    Nx=8;
    Ny=2;

    //    main_alloc(Nx, Ny);
    //    main_dealloc(Nx, Ny);


    main_alloc(Nx, Ny);
    resize(vibro_x[Nx-1]+100,270+vibro_y[Ny-1]);

    linear_mapping_btn=new QPushButton("set linear mapping");
    config_mapping_btn= new QPushButton("set config mapping");
    connect(linear_mapping_btn,SIGNAL(pressed()),this,SLOT(setLinearMapping()));
    connect(config_mapping_btn,SIGNAL(pressed()),this,SLOT(setConfigMapping()));

    pixmapLoop= new QPixmap("loop.png");
    pixmapLoopStop= new QPixmap("loop-stop.png");
    pixmapPlay= new QPixmap("play_round1600.png");
    pixmapStop= new QPixmap("stop-512.png");


    patternWidget = new QWidget();
    protWidget= new QWidget();
    stackedWidget=new QStackedWidget;
    stackedWidget->addWidget(patternWidget);
    stackedWidget->addWidget(protWidget);
    patternCentralLayout=new QGridLayout();
    protCentralLayout=new QGridLayout();



    QWidget *panelFiller = new QWidget;
    panelFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    patternFiller = new canvasWidget;
    patternFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(patternFiller,SIGNAL(clickedSignal()),this,SLOT(oneSend()));

    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
                              "invoke a context menu</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QWidget *protFiller = new QWidget;
    protFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget *patternTopFiller= new QWidget;
    patternTopFiller->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum);

    QIcon ButtonIcon2(*pixmapLoop);
    prot_loop_btn=new QPushButton("");
    prot_loop_btn->setIcon(ButtonIcon2);
    prot_loop_btn->setIconSize(QSize(30,30));
    prot_loop_btn->setMaximumWidth(40);
    connect(prot_loop_btn,SIGNAL(pressed()),this,SLOT(loopChanged()));

    serial_le=new QLineEdit("COM12");
    auto layout11 = new QGridLayout;

    prot_play_btn=new QPushButton("");
    pattern_play_btn=new QPushButton();

    QIcon ButtonIcon(*pixmapPlay);
    prot_play_btn->setIcon(ButtonIcon);
    prot_play_btn->setIconSize(QSize(30,30));
    prot_play_btn->setMaximumWidth(40);

    pattern_play_btn->setIcon(ButtonIcon);
    pattern_play_btn->setIconSize(QSize(30,30));
    pattern_play_btn->setMaximumWidth(40);


    connect(prot_play_btn,SIGNAL(pressed()),this,SLOT(protPlayPressed()));
    connect(pattern_play_btn,SIGNAL(pressed()),this,SLOT(patternPlayPressed()));

    //    layout->setMargin(5);
    //    layout->addWidget(protFiller,0,0);

    auto protSequenceGroup=new QGroupBox("protocol sequence");
    auto protPanelGroup=new QGroupBox("settings and control");
    auto protPanelLayout=new QGridLayout;
    auto protSequenceLayout=new QGridLayout;

    auto patternTopGroup=new QGroupBox("settings and control");
    auto patternFillerGroup = new QGroupBox("pattern:");
    auto patternLayout = new QGridLayout;

    //    protSequenceGroup->setMaximumWidth(200);
    protPanelGroup->setMinimumWidth(200);

    Nx_le=new QLineEdit(QString::number(Nx));
    Ny_le=new QLineEdit(QString::number(Ny));
    Nx_le->setMaximumWidth(60);
    Ny_le->setMaximumWidth(60);

    connect(Nx_le,SIGNAL(returnPressed()),this,SLOT(changeDim()));
    connect(Ny_le,SIGNAL(returnPressed()),this,SLOT(changeDim()));

    auto layout3=new QGridLayout;

    auto label1=new QLabel("Ny = ");
    patternLayout->addWidget(label1,0,0);
    patternLayout->addWidget(Ny_le,0,1);
    auto label2=new QLabel("   Nx = ");
    patternLayout->addWidget(label2,0,2);
    patternLayout->addWidget(Nx_le,0,3);
    patternLayout->addWidget(pattern_play_btn,0,4);
    auto label4 = new QLabel("   PWM = ");
    PWM_le=new QLineEdit("40");
    connect(PWM_le,SIGNAL(editingFinished()),this,SLOT(patternPlayPressed()));
    patternLayout->addWidget(label4,0,5);
    patternLayout->addWidget(PWM_le,0,6);
    patternLayout->addWidget(patternTopFiller,0,7);
    patternLayout->addWidget(linear_mapping_btn,1,0,1,2);
    patternLayout->addWidget(config_mapping_btn,1,2,1,2);

    patternTopGroup->setLayout(patternLayout);
    layout3->addWidget(patternTopGroup);
    auto layout4=new QGridLayout();
    layout4->addWidget(patternFiller);
    patternFillerGroup->setLayout(layout4);
    layout3->addWidget(patternFillerGroup);
    patternWidget->setLayout(layout3);


    prot_le=new QLineEdit[prot_le_s];
    for(int i=0;i<prot_le_s;i++)
    {
        protSequenceLayout->addWidget(&prot_le[i],i+1,0);
        connect(&prot_le[i],SIGNAL(returnPressed()),this,SLOT(updateProtocol()));
    }
    prot_le[0].setText(QString("untitled.ptn"));
    updateProtocol();

    protSequenceLayout->addWidget(protFiller);

    protPanelLayout->addWidget(prot_play_btn,0,0,1,1);
    protPanelLayout->addWidget(prot_loop_btn,0,1,1,1);
    auto label3=new QLabel("interval, ms: ");
    interval_le=new QLineEdit("300");
    connect(interval_le,SIGNAL(editingFinished()),this,SLOT(setInterval()));
    //    protPanelLayout->addWidget(label3,0,1);
    //    protPanelLayout->addWidget(interval_le,1,1);
    protPanelLayout->addWidget(serial_le,1,0,1,2);
    protPanelLayout->addWidget(label3,2,0);
    protPanelLayout->addWidget(interval_le,2,1);

    protPanelLayout->addWidget(panelFiller,3,0,1,2);

    protPanelGroup->setLayout(protPanelLayout);
    protSequenceGroup->setLayout(protSequenceLayout);


    layout11->addWidget(protSequenceGroup,0,0);
    layout11->addWidget(protPanelGroup,0,1);
    //    layout->addWidget(infoLabel,1,0);
    //    layout->setRowMinimumHeight(1,500);

    //layout->addWidget(bottomFiller,2,0);
    protWidget->setLayout(layout11);

    createActions();
    createMenus();

    QString message = tr("Строка подсказок ЗДЕСЬ");
    statusBar()->showMessage(message);

    //    setWindowTitle(tr("Pattern Editor"));
    setMinimumSize(480, 450);
    //    resize(480, 460);

    QPalette Pal(palette());

    Pal.setColor(QPalette::Background, Qt::white);
    //    setAutoFillBackground(true);
    setPalette(Pal);

    //popup windows
    save_le=new QLineEdit("");
    open_le = new QLineEdit("");

    save_OK_btn=new QPushButton("save");
    connect(save_OK_btn,SIGNAL(released()),this,SLOT(saveWithName()));
    open_OK_btn=new QPushButton("open");
    connect(open_OK_btn,SIGNAL(released()),this,SLOT(openWithName()));


    /////////
    auto central1 = new QWidget;
    saveWindow=new QMainWindow(this);
    QGridLayout *layout1 = new QGridLayout;
    layout1->addWidget(save_le);
    layout1->addWidget(save_OK_btn);
    central1->setLayout(layout1);
    saveWindow->setCentralWidget(central1);

    //    save_open_le=new QLineEdit("untitled.ptcl");
    auto central2 = new QWidget;
    openWindow=new QMainWindow();
    QGridLayout *layout2 = new QGridLayout;
    layout2->addWidget(open_le);
    layout2->addWidget(open_OK_btn);
    central2->setLayout(layout2);
    openWindow->setCentralWidget(central2);


    //    save_open_le->setText(prot_name);

    setTitle();

    connect(serial_le,SIGNAL(editingFinished()),this,SLOT(COMInit()));

    //    setCentralWidget(protWidget);
    //    setCentralWidget(patternWidget);
    setCentralWidget(stackedWidget);
    stackedWidget->setCurrentIndex(1);
    //    setCentralWidget(patternWidget);

    patternFiller->update();

    timer.setInterval(800);
    connect(&timer,SIGNAL(timeout()),this,SLOT(protocolRoutine()));

    setMapping();
}


void MainWindow::setInterval()
{
    timer.setInterval(interval_le->text().toInt());
}

void MainWindow::COMInit()
{
    serial_inited=1;
    qstr=serial_le->text();
    std::string str1=qstr.toUtf8().constData();
    std::wstring str(str1.begin(),str1.end());
    //    hSerial=new Serial;
    //    port=new QSerialPort;
    port.setPortName(qstr);
    port.setBaudRate(38400);
    if(port.open(QIODevice::WriteOnly))
    {
        QString message = tr("com port is successfully opened");
        statusBar()->showMessage(message);
    }
    else
    {
        QString message = tr("com port is not opened");
        statusBar()->showMessage(message);
    }


    //    hSerial->InitCOM(str.c_str());//was L"COM5"
    serial_le->setDisabled(true);
}

void MainWindow::loopChanged()
{
    prot_loop_ON=!prot_loop_ON;
    QIcon ButtonIcon(prot_loop_ON?(*pixmapLoop):(*pixmapLoopStop));

    prot_loop_btn->setIcon(ButtonIcon);
}

void MainWindow::protPlayPressed()
{
    if(!serial_inited)
        COMInit();

    protPlayOn=!protPlayOn;
    if(protPlayOn)
    {
        prot_ind=0;
        timer.start();
        timer.setInterval(interval_le->text().toInt());
        protPlayFlag=true;
    }
    else
    {
        timer.stop();

        char pwm=PWM_le->text().toInt();
        byte b=pwm;
        port.write("a",1);
        port.write(&pwm,1);
        //        PWM_le->text();
        //        PWM_le->text().toLocal8Bit().data(
        for(int i=0;i<Nx*Ny;i++)
            port.write("d",1);
        port.write("c",1);
    }
    QIcon ButtonIcon(protPlayOn?(*pixmapStop):(*pixmapPlay));

    prot_play_btn->setIcon(ButtonIcon);
    prot_play_btn->setIconSize(QSize(30,30));
    prot_play_btn->setMaximumWidth(40);
}


//char pwm=PWM_le->text().toInt();
//byte b=pwm;
//port.write("a",1);
//port.write(&pwm,1);
//for(int s=0;s<Nx*Ny;s++)
//    switch(vibro_state[vib_from_s[s]])
//    {
//    case 0:
//        port.write("d",1);
//        break;
//    case 1:
//        port.write("s",1);
//        break;
//    }
//port.write("c",1);

void MainWindow::oneSend()
{
    if(serial_inited)
        if(patternPlayOn)
        {
            char pwm=PWM_le->text().toInt();
            byte b=pwm;
            port.write("a",1);
            port.write(&pwm,1);
            for(int s=0;s<Nx*Ny;s++)
                switch(vibro_state[vib_from_s[s]])
                {
                case 0:
                    port.write("d",1);
                    break;
                case 1:
                    port.write("s",1);
                    break;
                }
            port.write("c",1);
        }
        else
        {
            port.write("a",1);
            for(int i=0;i<Nx*Ny;i++)
                port.write("d",1);
            port.write("c",1);
        }

}

void MainWindow::patternPlayPressed()
{
    if(!serial_inited)
        COMInit();

    patternPlayOn=!patternPlayOn;
    QIcon ButtonIcon(patternPlayOn?(*pixmapStop):(*pixmapPlay));

    pattern_play_btn->setIcon(ButtonIcon);
    pattern_play_btn->setIconSize(QSize(30,30));
    pattern_play_btn->setMaximumWidth(40);

    oneSend();

}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::saveAs()
{
    infoLabel->setText(tr("Invoked <b>File|Save</b>"));
    qDebug()<<prot_name;
    qDebug()<<pattern_name;
    if(mode)
        save_le->setText(prot_name);
    else
        save_le->setText(pattern_name);
    //    qDebug()<<save_open_le->text();
    saveWindow->show();
}

void MainWindow::openWithName(QString s)
{
    int nx, ny;
    QFile inputFile(s);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        QString line = in.readLine();
        PWM_le->setText(line);
        int i=0;
        QVector<QString> strs;
        QStringList s_list=line.split("   ");
        nx=s_list.size()-1;
        while (!line.isNull())
        {
            if(i==1)
            {
                QStringList s_list=line.split("   ");
                nx=s_list.size()-1;
            }
            strs.push_back(line);
            line=in.readLine();
            i++;
        }
        ny=i-1;
        Nx_le->setText(QString::number(nx));
        Ny_le->setText(QString::number(ny));

        if((nx==Nx)&&(ny==Ny))
        {
            qDebug()<<"all is nice";
        }
        else
            changeDim();
        //        qDebug()<<strs.size();
        for(int i=1;i<strs.size();i++)
        {
            QStringList s_list=strs[i].split("   ");

            qDebug()<<"\n";
            for(int j=0;j<nx;j++)
            {
                qDebug()<<s_list[j];
                vibro_state[vibro_n[i-1][j]]=s_list[j].toInt();
            }
        }
    }
    else
    {
        QString message = tr("wrong file name");
        statusBar()->showMessage(message);
    }
}

void MainWindow::openWithName()
{

    QFile inputFile(open_le->text());
    if (inputFile.open(QIODevice::ReadOnly))
    {
        if(mode==0)
        {
            pattern_name=open_le->text();
            save_le->setText(pattern_name);
            setTitle();
            openWithName(pattern_name);
            //            patternFiller->update();
        }
        else
        {
            prot_name=open_le->text();
            setTitle();
            QTextStream in(&inputFile);
            QString line = in.readLine();
            int i=0;
            prot_N=0;
            while(!line.isNull())
            {
                prot_le[i].setText(line);
                i++;
                prot_N++;
                line = in.readLine();
            }
            for(int i=prot_N;i<prot_le_s;i++)
                prot_le[i].setText("");
            updateProtocol();

            ////
        }
    }
    else
    {
        QString message = tr("wrong file name");
        statusBar()->showMessage(message);
    }
    //patternFiller->update();
    openWindow->hide();

    setTitle();
}

void MainWindow::saveWithName()
{
    if(mode==0)
    {
        pattern_file=new QFile(save_le->text());
        if (!pattern_file->open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        pattern_name=save_le->text();

        out=new QTextStream(pattern_file);
        out->flush();
        *out<<PWM_le->text();
        *out<<"\n";
        for(int i=0;i<Ny;i++)
        {
            for(int j=0;j<Nx;j++)
            {
                *out<<vibro_state[vibro_n[i][j]]<<"   ";
            }
            *out<<"\n";
        }
        pattern_file->close();
    }
    else
    {
        prot_file=new QFile(save_le->text());
        if (!prot_file->open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        prot_name=save_le->text();

        out=new QTextStream(prot_file);
        out->flush();
        for(int i=0;i<prot_N;i++)
        {
            *out<<prot_le[i].text()<<"\n";
        }
        prot_file->close();
    }

    saveWindow->hide();
    setTitle();
}



void MainWindow::updateProtocol()
{
    for(int i=0;i<prot_le_s;i++)
    {
        if(prot_le[i].text().isEmpty())
        {
            prot_N=i;
            prot_le[i].setFocus();
            break;
        }
    }

    for(int i=0;i<prot_le_s;i++)
    {
        if(i<(prot_N+1))
            prot_le[i].setVisible(true);
        else
            prot_le[i].setVisible(false);
    }
}

void MainWindow::changeDim()
{

    main_dealloc(Nx,Ny);
    Nx=Nx_le->text().toInt();
    Ny=Ny_le->text().toInt();
    //    main_alloc(Nx_le->text().toInt(),Ny_le->text().toInt());
    main_alloc(Nx,Ny);
    resize(vibro_x[Nx-1]+100,270+vibro_y[Ny-1]);
    patternFiller->update();
}

void MainWindow::protocolRoutine()
{


    openWithName(prot_le[prot_ind].text());
    patternFiller->update();

    prot_ind++;


    if(prot_ind>(prot_N-1))
    {
        prot_ind=0;

        //        protPlayFlag=0;
        if(!prot_loop_ON)
        {
            timer.stop();
            protPlayPressed();
        }
    }

    prot_le[prot_ind].setPalette(QPalette(QColor(255,0,0)));

    char pwm=PWM_le->text().toInt();
    port.write("a",1);
    port.write(&pwm,1);

    for(int s=0;s<Nx*Ny;s++)
        switch(vibro_state[vib_from_s[s]])
        {
        case 0:
            port.write("d",1);
            break;
        case 1:
            port.write("s",1);
            break;
        }
    port.write("c",1);

}

void MainWindow::newFile()
{
    infoLabel->setText(tr("Invoked <b>File|New</b>"));

}

void MainWindow::open()
{
    if(mode)
        open_le->setText(prot_name);
    else
        open_le->setText(pattern_name);
    openWindow->show();
    //    infoLabel->setText(tr("Invoked <b>File|Open</b>"));

}

void MainWindow::save()
{
    infoLabel->setText(tr("Invoked <b>File|Save</b>"));
}



void MainWindow::print()
{
    infoLabel->setText(tr("Invoked <b>File|Print</b>"));
}

void MainWindow::undo()
{
    infoLabel->setText(tr("Invoked <b>Edit|Undo</b>"));
}

void MainWindow::redo()
{
    infoLabel->setText(tr("Invoked <b>Edit|Redo</b>"));
}

void MainWindow::cut()
{
    infoLabel->setText(tr("Invoked <b>Edit|Cut</b>"));
}

void MainWindow::copy()
{
    infoLabel->setText(tr("Invoked <b>Edit|Copy</b>"));
}

void MainWindow::paste()
{
    infoLabel->setText(tr("Invoked <b>Edit|Paste</b>"));
}

void MainWindow::bold()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Bold</b>"));
}

void MainWindow::italic()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Italic</b>"));
}

void MainWindow::leftAlign()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Left Align</b>"));
}

void MainWindow::rightAlign()
{
    infoLabel->setText(tr("<i>Invoked  </i><b>Edit|Format|Right Align</b>"));
}

void MainWindow::justify()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Justify</b>"));
}

void MainWindow::center()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Center</b>"));
}

void MainWindow::setLineSpacing()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Set Line Spacing</b>"));
}

void MainWindow::setParagraphSpacing()
{
    infoLabel->setText(tr("Invoked <b>Edit|Format|Set Paragraph Spacing</b>"));
}

void MainWindow::about()
{
    infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    //    QMessageBox::about(this, tr("About Menu"),
    //                       tr("This is a <b>Stimulator</b> program"
    //                          "\nv1.1"));
    QMessageBox msb;
    msb.setWindowTitle("About");
    msb.setText(tr("This is a Stimulator program v1.1 \n"
                   "    Обратите внимание на подсказки внизу\n"
                   "    Для соедининия с устройством в строке\n"
                   "названий серийного порта нажмите ENTER\n"
                   "    О состоянии подключения оповестит строка подсказок\n"
                   "    Возможно сохранение как паттерна, так и протокола в отдельности\n"
                   "в зависимости от запущенного режима\n"
                   "    Значение PWM - целое число в пределах от 0 до 20,"
                   "применяется для всего паттерна целиком"));
    msb.exec();
}

void MainWindow::aboutQt()
{
    infoLabel->setText(tr("Invoked <b>Help|About Qt</b>"));
}

void MainWindow::setMapping()
{
    QFile inputFile("mapping_config");
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        QString line = in.readLine();
        QStringList s_list=line.split("=");
        int s=0;
        int i;
        while ((!line.isNull())&&(i<Ny))
        {

            qDebug()<<s_list[1];
            vib_from_s[s]=s_list[1].toInt();

            s++;
            i=s/Nx;
            line=in.readLine();
            s_list=line.split("=");

        }
    }
}


void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the pattern"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveWithName);


    saveAsAct = new QAction(tr("&Save as ..."), this);
    //    saveAsAct->setShortcuts(QKeySequence::Save);
    saveAsAct->setStatusTip(tr("name the pattern to save"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    editorAct = new QAction(tr("&editor mode"), this);
    editorAct->setCheckable(true);
    connect( editorAct, &QAction::triggered, this,&MainWindow::editorChecked);

    protAct = new QAction(tr("protocol mode"),this);
    protAct->setCheckable(true);
    protAct->setChecked(true);
    connect( protAct, &QAction::triggered, this,&MainWindow::protChecked);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    boldAct = new QAction(tr("&Bold"), this);
    boldAct->setCheckable(true);
    boldAct->setShortcut(QKeySequence::Bold);
    boldAct->setStatusTip(tr("Make the text bold"));
    connect(boldAct, &QAction::triggered, this, &MainWindow::bold);

    QFont boldFont = boldAct->font();
    boldFont.setBold(true);
    boldAct->setFont(boldFont);

    italicAct = new QAction(tr("&Italic"), this);
    italicAct->setCheckable(true);
    italicAct->setShortcut(QKeySequence::Italic);
    italicAct->setStatusTip(tr("Make the text italic"));
    connect(italicAct, &QAction::triggered, this, &MainWindow::italic);

    QFont italicFont = italicAct->font();
    italicFont.setItalic(true);
    italicAct->setFont(italicFont);

    setLineSpacingAct = new QAction(tr("Set &Line Spacing..."), this);
    setLineSpacingAct->setStatusTip(tr("Change the gap between the lines of a "
                                       "paragraph"));
    connect(setLineSpacingAct, &QAction::triggered, this, &MainWindow::setLineSpacing);

    setParagraphSpacingAct = new QAction(tr("Set &Paragraph Spacing..."), this);
    setParagraphSpacingAct->setStatusTip(tr("Change the gap between paragraphs"));
    connect(setParagraphSpacingAct, &QAction::triggered,
            this, &MainWindow::setParagraphSpacing);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(aboutQtAct, &QAction::triggered, this, &MainWindow::aboutQt);

    leftAlignAct = new QAction(tr("&Left Align"), this);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setShortcut(tr("Ctrl+L"));
    leftAlignAct->setStatusTip(tr("Left align the selected text"));
    connect(leftAlignAct, &QAction::triggered, this, &MainWindow::leftAlign);

    rightAlignAct = new QAction(tr("&Right Align"), this);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setShortcut(tr("Ctrl+R"));
    rightAlignAct->setStatusTip(tr("Right align the selected text"));
    connect(rightAlignAct, &QAction::triggered, this, &MainWindow::rightAlign);

    justifyAct = new QAction(tr("&Justify"), this);
    justifyAct->setCheckable(true);
    justifyAct->setShortcut(tr("Ctrl+J"));
    justifyAct->setStatusTip(tr("Justify the selected text"));
    connect(justifyAct, &QAction::triggered, this, &MainWindow::justify);

    centerAct = new QAction(tr("&Center"), this);
    centerAct->setCheckable(true);
    centerAct->setShortcut(tr("Ctrl+E"));
    centerAct->setStatusTip(tr("Center the selected text"));
    connect(centerAct, &QAction::triggered, this, &MainWindow::center);

    alignmentGroup = new QActionGroup(this);
    alignmentGroup->addAction(leftAlignAct);
    alignmentGroup->addAction(rightAlignAct);
    alignmentGroup->addAction(justifyAct);
    alignmentGroup->addAction(centerAct);
    leftAlignAct->setChecked(true);
}

void MainWindow::setLinearMapping()
{
    for(int i=0;i<Nx*Ny;i++)
        vib_from_s[i]=i;
}

void MainWindow::setConfigMapping()
{
    setMapping();
}



void MainWindow::editorChecked()
{
    if(mode==1)//change
    {
        mode=0;
        protAct->setChecked(false);
        //        save_open_le->setText(pattern_name);
        //        save_open_le->setText(pattern_name);
        patternFiller->update();
        stackedWidget->setCurrentIndex(0);
    }
    else
    {
        editorAct->setChecked(true);
        //        save_open_le->setText(prot_name);
        //        save_open_le->setText(prot_name);
    }
    //patternFiller->update();
    setTitle();
}

void MainWindow::protChecked()
{
    if(mode==0)
    {
        mode=1;
        editorAct->setChecked(false);
        //        save_open_le->setText(prot_name);
        //        save_open_le->setText(prot_name);
        stackedWidget->setCurrentIndex(1);
    }
    else
    {
        protAct->setChecked(true);
        //        save_open_le->setText(pattern_name);
        //        save_open_le->setText(pattern_name);
    }


    setTitle();
}

void MainWindow::setTitle()
{

    QString hm=mode?QString("protocol   "):QString("ptnEdit   ");
    QString hm2=mode?prot_name:pattern_name;
    setWindowTitle(QString("Stimulator->")+hm+hm2);
    //    setWindowTitle(tr("<b>hello</b>"));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(editorAct);
    editMenu->addAction(protAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

int getVibroNum(int i,int j)
{
    return(j+i*Nx);
}

int getVibroI(int n)
{
    return n/Nx;
}

int getVibroJ(int n)
{
    return(n%Nx);
}

template<typename T>
void memory_alloc(T** x, int s)
{
    *x = new T[s];
}

template<typename T>
void memory_dealloc(T** x)
{
    delete[] *x;
}

void main_alloc(int Nx, int Ny)
{
    //    memory_alloc<int>(&vibro_x,5000);


    memory_alloc<int>(&vib_from_s,Nx*Ny);
    memory_alloc<int>(&s_from_vib,Nx*Ny);
    memory_alloc<int>(&vibro_x,Nx);
    memory_alloc<int>(&vibro_y,Ny);
    memory_alloc<int>(&vibro_rad,Nx*Ny);
    memory_alloc<int>(&vibro_state,Nx*Ny);
    memory_alloc<int*>(&vibro_n,Ny);
    for(int i=0;i<Ny;i++)
    {
        memory_alloc<int>(&vibro_n[i],Nx);
    }


    vibro_step=70;
    int shift=30;
    for (int j=0;j<Nx;j++)
        for(int i=0;i<Ny;i++)
        {
            vibro_n[i][j]=getVibroNum(i,j);
            vibro_rad[vibro_n[i][j]]=vibro_rad_stat;
            vibro_state[vibro_n[i][j]]=0;
        }


    for (int i=0;i<Nx;i++)
        vibro_x[i]=shift+vibro_step*i;

    for (int i=0;i<Ny;i++)
        vibro_y[i]=shift+vibro_step*i;

    //////////////////
    //manually

    for(int i=0;i<Nx*Ny;i++)
        vib_from_s[i]=i;
    //    vib_from_s[0]=0;
    //    vib_from_s[1]=2;
    //    vib_from_s[2]=3;
    //    vib_from_s[3]=1;
    //    for(int i=0;i<4;i++)
    //    {
    //        s_from_vib[vib_from_s[i]]=i;
    //    }

}

void main_dealloc(int Nx, int Ny)
{
    memory_dealloc<int>(&vib_from_s);
    memory_dealloc<int>(&s_from_vib);
    memory_dealloc<int>(&vibro_x);
    memory_dealloc<int>(&vibro_y);
    memory_dealloc<int>(&vibro_rad);
    memory_dealloc<int>(&vibro_state);
    for(int i=0;i<Ny;i++)
    {
        memory_dealloc<int>(&vibro_n[i]);
    }
    memory_dealloc<int*>(&vibro_n);
}
