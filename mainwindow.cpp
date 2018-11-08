//http://doc.qt.io/qt-5/qtwidgets-mainwindows-menus-example.html
//names: prot (protocol) vs stimulator
//editor
#include <QtWidgets>
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
//#include <QList>
QPoint mouse_p;
QPushButton* save_OK_btn;
QPushButton* open_OK_btn;
QPushButton* play_btn;
QLineEdit* save_le;
QLineEdit* open_le;
QMainWindow* saveWindow;
QMainWindow* openWindow;
QLineEdit* prot_le;
QWidget *protWidget;

bool mode;//0-editor mode, 1-prot?

QFile* pattern_file;
QTextStream* out;


int vibro_rad_stat=17;
int prot_le_s=3;
int *vibro_rad;
int vibro_step;
int *vibro_x;
int *vibro_y;
int *vibro_state;
int **vibro_n;
int Nx=5;
int Ny=5;
int checked_n=0;

int getVibroNum(int i,int j);

int getVibroI(int n);

int getVibroJ(int n);

template<typename T>
void memory_alloc(T** x, int s);

MainWindow::MainWindow()
{
    mode =1;

    prot_le=new QLineEdit[3];

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
    int shift=80;
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
        vibro_y[i]=shift*1.5+vibro_step*i;


    protWidget= new QWidget(this);
    setCentralWidget(protWidget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
                              "invoke a context menu</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    ////////
    QGridLayout *layout = new QGridLayout;
    play_btn=new QPushButton("");
//    layout->setMargin(5);
//    layout->addWidget(topFiller,0,0);

    QGroupBox* protSequenceGroup=new QGroupBox("protocol sequence");
    QGroupBox* protPanelGroup=new QGroupBox("settings");
    QGridLayout* protPanelLayout=new QGridLayout;
    QGridLayout* protSequenceLayout=new QGridLayout;

    protSequenceGroup->setMaximumWidth(200);

    for(int i=0;i<prot_le_s;i++)
        protSequenceLayout->addWidget(&prot_le[i],i+1,0);

    protPanelLayout->addWidget(play_btn);

    protPanelGroup->setLayout(protPanelLayout);
    protSequenceGroup->setLayout(protSequenceLayout);


    layout->addWidget(protSequenceGroup,0,0);
    layout->addWidget(protPanelGroup,0,1);
    //    layout->addWidget(infoLabel,1,0);
    //    layout->setRowMinimumHeight(1,500);

    //layout->addWidget(bottomFiller,2,0);
    protWidget->setLayout(layout);

    createActions();
    createMenus();

    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);

    setWindowTitle(tr("Pattern Editor"));
    setMinimumSize(480, 450);
    resize(480, 460);

    QPalette Pal(palette());

    Pal.setColor(QPalette::Background, Qt::white);
    //    setAutoFillBackground(true);
    setPalette(Pal);

//popup windows
    save_le=new QLineEdit("untitled.ptn");

    save_OK_btn=new QPushButton("save");
    connect(save_OK_btn,SIGNAL(released()),this,SLOT(saveWithName()));
    open_OK_btn=new QPushButton("open");
    connect(open_OK_btn,SIGNAL(released()),this,SLOT(openWithName()));



    auto central1 = new QWidget;
    saveWindow=new QMainWindow(this);
    QGridLayout *layout1 = new QGridLayout;
    layout1->addWidget(save_le);
    layout1->addWidget(save_OK_btn);
    central1->setLayout(layout1);
    saveWindow->setCentralWidget(central1);

    open_le=new QLineEdit("untitled.ptn");
    auto central2 = new QWidget;
    openWindow=new QMainWindow(this);
    QGridLayout *layout2 = new QGridLayout;
    layout2->addWidget(open_le);
    layout2->addWidget(open_OK_btn);
    central2->setLayout(layout2);
    openWindow->setCentralWidget(central2);
    update();


}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPen pen;


    //    pen.setStyle();
    QPainter* painter=new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing, true);

    if(mode==0)
    {

        QRect r=QRect(mouse_p,mouse_p+QPoint(100,100));
        for(int i=0;i<Ny;i++)
            for(int j=0;j<Nx;j++)
            {

                if(checked_n==vibro_n[i][j])
                {
                    pen.setWidth(6);
                    pen.setColor(QColor(0,150,0));
                    vibro_rad[checked_n]=vibro_rad_stat*1.15;
                }
                else
                {
                    //                pen.setStyle(Qt::SolidLine);
                    pen.setColor(QColor(0,0,0));
                    pen.setWidth(2);
                    vibro_rad[checked_n]=vibro_rad_stat;
                }
                painter->setPen(pen);

                QPainterPath path;
                QRadialGradient gradient=QRadialGradient(QPointF(vibro_x[j],vibro_x[i]),40,
                                                         QPointF(vibro_y[j],vibro_y[i])+QPointF(50,50));
                if(vibro_state[vibro_n[i][j]]!=0)
                {
                    gradient.setColorAt(1.0,QColor(170,170,120));
                    gradient.setColorAt(0.0,QColor(250,250,200));
                }
                else
                {
                    gradient.setColorAt(1.0,QColor(100,100,100));
                    gradient.setColorAt(0.0,QColor(200,200,200));
                }

                path.addEllipse ( QPointF(vibro_x[j],vibro_y[i]), vibro_rad[vibro_n[i][j]], vibro_rad[vibro_n[i][j]]);
                painter->fillPath(path,gradient);
                painter->drawPath(path);
            }

    }
    else
    {

    }
    delete painter;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mouse_p=event->pos();

    for(int i=0;i<Ny;i++)
        for(int j=0;j<Nx;j++)
        {
            float r2=vibro_rad[vibro_n[i][j]]*vibro_rad[vibro_n[i][j]];
            /////!!!!!!!!!!!!!!
            QPoint vibro_p=QPoint(vibro_x[j],vibro_y[i]);
            if(QPoint::dotProduct(vibro_p-mouse_p,vibro_p-mouse_p)<r2)
            {
                checked_n=vibro_n[i][j];
                vibro_state[checked_n]=!vibro_state[checked_n];
                qDebug()<<checked_n;
            }
        }
    update();
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
    saveWindow->show();
}

void MainWindow::openWithName()
{
    QFile inputFile(open_le->text());
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        QString line = in.readLine();
        int i=0;
        while (!line.isNull()) {
            QStringList s_list=line.split("   ");
            for(int j=0;j<Nx;j++)
                vibro_state[vibro_n[i][j]]=s_list[j].toInt();
            line = in.readLine();
            i++;
        }
    }
    update();
    openWindow->hide();
}

void MainWindow::saveWithName()
{


    pattern_file=new QFile(save_le->text());
    if (!pattern_file->open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    out=new QTextStream(pattern_file);
    out->flush();
    for(int i=0;i<Ny;i++)
    {
        for(int j=0;j<Nx;j++)
        {
            *out<<vibro_state[vibro_n[i][j]]<<"   ";
        }
        *out<<"\n";
    }
    pattern_file->close();

    saveWindow->hide();
}

void MainWindow::newFile()
{
    infoLabel->setText(tr("Invoked <b>File|New</b>"));
}

void MainWindow::open()
{
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
    QMessageBox::about(this, tr("About Menu"),
                       tr("This is a <b>Pattern Editor</b> program"
                          "\nv1.1"));
}

void MainWindow::aboutQt()
{
    infoLabel->setText(tr("Invoked <b>Help|About Qt</b>"));
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

    protAct = new QAction(tr("prot mode"),this);
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

void MainWindow::editorChecked()
{
    if(mode==1)
    {
    protAct->setChecked(false);
    mode=0;
//    for(int i=0;i<prot_le_s;i++)
//        prot_le[i].setVisible(false);
    protWidget->setVisible(false);
    }
    else
        editorAct->setChecked(true);
    update();
}

void MainWindow::protChecked()
{
    if(mode==0)
    {
    editorAct->setChecked(false);
    mode=1;
//    for(int i=0;i<prot_le_s;i++)
//        prot_le[i].setVisible(true);
    protWidget->setVisible(true);
    }
    else
        protAct->setChecked(true);

    update();
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
