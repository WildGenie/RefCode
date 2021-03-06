#include "MainWindow.h"

QAssistantClient *MainWindow::assistant=0;

MainWindow::MainWindow():archiver(),archiver2(),archiver3(),archiver4(),signaldb()
{
	init();
}

MainWindow::~MainWindow()
{
}

void
MainWindow::init()
{
	displayDocklist
	<<"MPS_Monitoring"<<"MPS_Operation";

	passWord
	<<""<<"";


	makeUI();
	createActions();
	createConnects();
	registerPushButtonsEvent();
	attachAllWidget();
   	tabWidget->setCurrentIndex(0);
	setIndex(1);
#if 0
#endif


}
void
MainWindow::createActions()
{
#if 0
	menu_View->addAction(dockWidget->toggleViewAction());
#endif
}
void
MainWindow::createConnects()
{
	connect(dockWidget,SIGNAL(topLevelChanged(bool)), this, SLOT(showFullWindow(bool)) );
}
void
MainWindow::showFullWindow(bool toplevel)
{
	QPalette p;
	QPixmap bg;
	QSize normal_size;
	QSize full_size;

	if (toplevel == true)
	{
		if(dockWidget->geometry().x() < 1280)
		{
			dockWidget->setGeometry(0,0,1280,1024);
		} else if(dockWidget->geometry().x() > 2560)
		{
			dockWidget->setGeometry(2560,0,1280,1024);
		} else 
		{
			dockWidget->setGeometry(1280,0,1280,1024);
		}
		//stackedWidget->currentWidget()->showFullScreen();
		stackedWidget->currentWidget()->setGeometry(0,0,1280,1000);
		full_size = stackedWidget->currentWidget()->size();

		if (stackedWidget->currentWidget()->palette().brush(QPalette::Window).style() == Qt::TexturePattern)
		{
			bg = stackedWidget->currentWidget()->palette().brush(QPalette::Window).texture();
			bg = bg.scaled(full_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			p.setBrush(QPalette::Window, bg);
			stackedWidget->currentWidget()->setPalette(p);
		}
	} else
	{
		dockWidget->setGeometry(0,0,1080,821);
		stackedWidget->currentWidget()->setGeometry(0,0,1080,800);
		normal_size = stackedWidget->currentWidget()->size();

		if (stackedWidget->currentWidget()->palette().brush(QPalette::Window).style() == Qt::TexturePattern)
		{
			bg = stackedWidget->currentWidget()->palette().brush(QPalette::Window).texture();
			bg = bg.scaled(normal_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
			p.setBrush(QPalette::Window, bg);
			stackedWidget->currentWidget()->setPalette(p);
		}

	}
}
void
MainWindow::makeUI()
{
    setObjectName(QString::fromUtf8("MainWindow"));

    //action_Print = new QAction(this);
    //action_Print->setObjectName(QString::fromUtf8("action_Print"));

    action_Exit = new QAction(this);
    action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
	//connect(action_Exit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(action_Exit, SIGNAL(triggered()), this, SLOT(close()));

    action_Multiplot = new QAction(this);
    action_Multiplot->setObjectName(QString::fromUtf8("action_Multiplot"));
	connect(action_Multiplot, SIGNAL(triggered()), this, SLOT(showMultiplot()));

    action_Archiverviewer = new QAction(this);
    action_Archiverviewer->setObjectName(QString::fromUtf8("action_Archiverviewer"));
	connect(action_Archiverviewer, SIGNAL(triggered()), this, SLOT(showArchiverviewer()));

    action_SignalDB = new QAction(this);
    action_SignalDB->setObjectName(QString::fromUtf8("action_SignalDB"));
	connect(action_SignalDB, SIGNAL(triggered()), this, SLOT(showSignalDB()));

#if 0
	action_Manual = new QAction(this);
	action_Manual->setObjectName(QString::fromUtf8("action_Manual"));
	connect(action_Manual, SIGNAL(triggered()), this, SLOT(showManual()));
#endif

	action_AboutMPS = new QAction(this);
	action_AboutMPS->setObjectName(QString::fromUtf8("action_AboutMPS"));
	connect(action_AboutMPS, SIGNAL(triggered()), this, SLOT(showAboutMPS()));

    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    //centralwidget->setGeometry(QRect(0, 0, 1280, 821));
	
    widget = new QWidget(centralwidget);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(190, 0, 1080, 821));
    widget->setMinimumSize(QSize(400, 0));
    vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setSpacing(0);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    dockWidget = new QDockWidget(widget);
    dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
    dockWidget->setSizePolicy(sizePolicy);
    //dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::NoDockWidgetFeatures);
    //dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    //dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

    QPalette palettedw;

    QBrush brdw1(QColor(255, 255, 55, 255));
    brdw1.setStyle(Qt::SolidPattern);
    palettedw.setBrush(QPalette::Active, QPalette::Button, brdw1);

    QBrush brdw2(QColor(255, 0, 10, 255));
    brdw2.setStyle(Qt::SolidPattern);
    palettedw.setBrush(QPalette::Active, QPalette::Mid, brdw2);

	//dockWidget->setPalette(palettedw);

    //dockWidgetContents = new QWidget(dockWidget);
    dockWidgetContents = new QScrollArea(dockWidget);
	dockWidgetContents->setGeometry(QRect(0,0,1280,1024));
	dockWidgetContents->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));

    //stackedWidget = new QStackedWidget(dockWidgetContents);
    stackedWidget = new QStackedLayout(dockWidgetContents);
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    stackedWidget->setGeometry(QRect(0, 0, 1080, 821));
    dockWidget->setWidget(dockWidgetContents);

    vboxLayout->addWidget(dockWidget);

    tabWidget = new QTabWidget(centralwidget);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tabWidget->setGeometry(QRect(0, 0, 190, 821));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());

#if 0
    QPalette palettetw;
    QBrush brushtw1(QColor(0, 0, 55, 200));
    brushtw1.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Window, brushtw1);
    QBrush brushtw2(QColor(96, 96, 129, 200));
    brushtw2.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Base, brushtw2);
    QBrush brushtw3(QColor(100, 100, 100, 255));
    brushtw3.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::WindowText, brushtw3);
    //QBrush brushtw4(QColor(255, 255, 255, 50));
    QBrush brushtw4(QColor(55, 55, 55, 100));
    brushtw4.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Active, QPalette::Button, brushtw4);
	/*
    QBrush brushtw5(QColor(255, 55, 55, 100));
    brushtw5.setStyle(Qt::SolidPattern);
    palettetw.setBrush(QPalette::Inactive, QPalette::Button, brushtw5);
	*/
    tabWidget->setPalette(palettetw);
#endif
    tabWidget->setSizePolicy(sizePolicy1);
    tabWidget->setMaximumSize(QSize(16777215, 16777215));
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->setElideMode(Qt::ElideNone);

    tab_0 = new QWidget();
    tab_0->setObjectName(QString::fromUtf8("tab_0"));
    QFont font0;
    font0.setPointSize(14);
    vboxLayout0 = new QVBoxLayout(tab_0);
    vboxLayout0->setSpacing(2);
    vboxLayout0->setMargin(2);
    vboxLayout0->setAlignment(Qt::AlignTop);
    vboxLayout0->setObjectName(QString::fromUtf8("vboxLayout0"));

    pushButton[0] = new QPushButton(tab_0);
    pushButton[0]->setObjectName(QString::fromUtf8("pushButton_0"));
    pushButton[0]->setFont(font0);
    pushButton[0]->setText(QApplication::translate("MainWindow", "MPS Monitoring", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[0]);

    pushButton[1] = new QPushButton(tab_0);
    pushButton[1]->setObjectName(QString::fromUtf8("pushButton_1"));
    pushButton[1]->setFont(font0);
    pushButton[1]->setText(QApplication::translate("MainWindow", "MPS Operation", 0, QApplication::UnicodeUTF8));
    vboxLayout0->addWidget(pushButton[1]);

    spacerItem0 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vboxLayout0->addItem(spacerItem0);

    tabWidget->addTab(tab_0, QApplication::translate("MainWindow", "MPS Main panels", 0, QApplication::UnicodeUTF8));
    //setCentralWidget(centralwidget);
    menubar = new QMenuBar(this);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 1280, 30));
    menu_File = new QMenu(menubar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    menu_Util = new QMenu(menubar);
    menu_Util->setObjectName(QString::fromUtf8("menu_Util"));
#if 0
    menu_View = new QMenu(menubar);
    menu_View->setObjectName(QString::fromUtf8("menu_View"));
#endif
	menu_Help = new QMenu(menubar);
	menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
    setMenuBar(menubar);

    QLabel *slabel1 = new QLabel("Set your mouse on the dynamic value to read PVNAME.");
    slabel1->setAlignment(Qt::AlignHCenter);
    slabel1->setMinimumSize(slabel1->sizeHint());
    slabel1->setFrameStyle(QFrame::Panel | QFrame::Plain);

    QFrame *sframe = new QFrame();
    QVBoxLayout *svlayout = new QVBoxLayout(sframe);
    svlayout->setSpacing(1);
    svlayout->setMargin(2);

    statusBar()->addWidget(sframe,1);
    
    toolBar = new QToolBar(this);
    toolBar->setObjectName(QString::fromUtf8("toolBar"));
    QPalette palette;
    QBrush brush(QColor(108, 147, 255, 100));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush);
    palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush);
    QBrush brush1(QColor(44, 51, 91, 100));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, brush1);
    QBrush brush2(QColor(108, 147, 255, 100));
	/*
    brush2.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
    QBrush brush3(QColor(44, 51, 91, 100));
    brush3.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Inactive, QPalette::Window, brush3);
    QBrush brush4(QColor(44, 51, 91, 100));
    brush4.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush4);
    QBrush brush5(QColor(44, 51, 91, 100));
    brush5.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::Window, brush5);
	*/
    toolBar->setPalette(palette);
    toolBar->setOrientation(Qt::Horizontal);
    addToolBar(static_cast<Qt::ToolBarArea>(4), toolBar);

    menubar->addAction(menu_File->menuAction());
    menubar->addAction(menu_Util->menuAction());
#if 0
    menubar->addAction(menu_View->menuAction());
#endif
	menubar->addSeparator();
    menubar->addAction(menu_Help->menuAction());
    //menu_File->addAction(action_Print);
    menu_File->addAction(action_Exit);

    menu_Util->addAction(action_Multiplot);
    menu_Util->addAction(action_Archiverviewer);
    menu_Util->addAction(action_SignalDB);

#if 0
	menu_Help->addAction(action_Manual);
#endif
	menu_Help->addAction(action_AboutMPS);

	/*
    toolBar->addSeparator();
    toolBar->addAction(action_Print);
	*/

    QFrame *tbframe = new QFrame();
    toolBar->addWidget(tbframe);

    QHBoxLayout *tblayout = new QHBoxLayout(tbframe);
    tblayout->QLayout::setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tblayout->setSpacing(0);
    tblayout->setMargin(0);
    tblayout->setObjectName(QString::fromUtf8("toolBarLayout"));

    QSpacerItem *tbspacer = new QSpacerItem(780, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);

	CAGraphic *ioc1HB = new CAGraphic(this);
	ioc1HB->setLineWidth(2);
	ioc1HB->setMinimumSize(QSize(20,20));
	ioc1HB->setMaximumSize(QSize(20,20));
	ioc1HB->setFillColor(QColor("white"));
	ioc1HB->setLineColor(QColor("black"));
	ioc1HB->setPvname("CCS_HEARTBEAT");
	ioc1HB->setFillColor(QColor("gray"));
	ioc1HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc1HB->setObjectName("CAGraphic_CCSHB");
	ioc1HB->setToolTip("CCS Heart Beat");

	CAGraphic *ioc2HB = new CAGraphic(this);
	ioc2HB->setLineWidth(2);
	ioc2HB->setMinimumSize(QSize(20,20));
	ioc2HB->setMaximumSize(QSize(20,20));
	ioc2HB->setFillColor(QColor("white"));
	ioc2HB->setLineColor(QColor("black"));
	ioc2HB->setPvname("ICS_HEARTBEAT");
	ioc2HB->setFillColor(QColor("gray"));
	ioc2HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc2HB->setObjectName("CAGraphic_ICSHB");
	ioc2HB->setToolTip("ICS Heart Beat");

	CAGraphic *ioc3HB = new CAGraphic(this);
	ioc3HB->setLineWidth(2);
	ioc3HB->setMinimumSize(QSize(20,20));
	ioc3HB->setMaximumSize(QSize(20,20));
	ioc3HB->setFillColor(QColor("white"));
	ioc3HB->setLineColor(QColor("black"));
	ioc3HB->setPvname("MPS_TF_IO_BI_00");
	ioc3HB->setFillColor(QColor("gray"));
	ioc3HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc3HB->setObjectName("CAGraphic_TFMPSHB");
	ioc3HB->setToolTip("TF MPS Heart Beat");

	CAGraphic *ioc4HB = new CAGraphic(this);
	ioc4HB->setLineWidth(2);
	ioc4HB->setMinimumSize(QSize(20,20));
	ioc4HB->setMaximumSize(QSize(20,20));
	ioc4HB->setFillColor(QColor("white"));
	ioc4HB->setLineColor(QColor("black"));
	ioc4HB->setPvname("IVC_heartbeat_to_sis");
	ioc4HB->setFillColor(QColor("gray"));
	ioc4HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc4HB->setObjectName("CAGraphic_IVCMPSHB");
	ioc4HB->setToolTip("IVC MPS Heart Beat");

	CAGraphic *ioc5HB = new CAGraphic(this);
	ioc5HB->setLineWidth(2);
	ioc5HB->setMinimumSize(QSize(20,20));
	ioc5HB->setMaximumSize(QSize(20,20));
	ioc5HB->setFillColor(QColor("white"));
	ioc5HB->setLineColor(QColor("black"));
	ioc5HB->setPvname("MPS_PF1_IO_BI_00");
	ioc5HB->setFillColor(QColor("gray"));
	ioc5HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc5HB->setObjectName("CAGraphic_PF1MPSHB");
	ioc5HB->setToolTip("PF1 MPS Heart Beat");

	CAGraphic *ioc6HB = new CAGraphic(this);
	ioc6HB->setLineWidth(2);
	ioc6HB->setMinimumSize(QSize(20,20));
	ioc6HB->setMaximumSize(QSize(20,20));
	ioc6HB->setFillColor(QColor("white"));
	ioc6HB->setLineColor(QColor("black"));
	ioc6HB->setPvname("MPS_PF2_IO_BI_00");
	ioc6HB->setFillColor(QColor("gray"));
	ioc6HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc6HB->setObjectName("CAGraphic_PF2MPSHB");
	ioc6HB->setToolTip("PF2 MPS Heart Beat");

	CAGraphic *ioc7HB = new CAGraphic(this);
	ioc7HB->setLineWidth(2);
	ioc7HB->setMinimumSize(QSize(20,20));
	ioc7HB->setMaximumSize(QSize(20,20));
	ioc7HB->setFillColor(QColor("white"));
	ioc7HB->setLineColor(QColor("black"));
	ioc7HB->setPvname("MPS_PF3U_IO_BI_00");
	ioc7HB->setFillColor(QColor("gray"));
	ioc7HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc7HB->setObjectName("CAGraphic_PF3UMPSHB");
	ioc7HB->setToolTip("PF3U MPS Heart Beat");

	CAGraphic *ioc8HB = new CAGraphic(this);
	ioc8HB->setLineWidth(2);
	ioc8HB->setMinimumSize(QSize(20,20));
	ioc8HB->setMaximumSize(QSize(20,20));
	ioc8HB->setFillColor(QColor("white"));
	ioc8HB->setLineColor(QColor("black"));
	ioc8HB->setPvname("MPS_PF3L_IO_BI_00");
	ioc8HB->setFillColor(QColor("gray"));
	ioc8HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc8HB->setObjectName("CAGraphic_PF3LMPSHB");
	ioc8HB->setToolTip("PF3L MPS Heart Beat");

	CAGraphic *ioc9HB = new CAGraphic(this);
	ioc9HB->setLineWidth(2);
	ioc9HB->setMinimumSize(QSize(20,20));
	ioc9HB->setMaximumSize(QSize(20,20));
	ioc9HB->setFillColor(QColor("white"));
	ioc9HB->setLineColor(QColor("black"));
	ioc9HB->setPvname("MPS_PF4U_IO_BI_00");
	ioc9HB->setFillColor(QColor("gray"));
	ioc9HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc9HB->setObjectName("CAGraphic_PF4UMPSHB");
	ioc9HB->setToolTip("PF4U MPS Heart Beat");

	CAGraphic *ioc10HB = new CAGraphic(this);
	ioc10HB->setLineWidth(2);
	ioc10HB->setMinimumSize(QSize(20,20));
	ioc10HB->setMaximumSize(QSize(20,20));
	ioc10HB->setFillColor(QColor("white"));
	ioc10HB->setLineColor(QColor("black"));
	ioc10HB->setPvname("MPS_PF4L_IO_BI_00");
	ioc10HB->setFillColor(QColor("gray"));
	ioc10HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc10HB->setObjectName("CAGraphic_PF4LMPSHB");
	ioc10HB->setToolTip("PF4L MPS Heart Beat");

	CAGraphic *ioc11HB = new CAGraphic(this);
	ioc11HB->setLineWidth(2);
	ioc11HB->setMinimumSize(QSize(20,20));
	ioc11HB->setMaximumSize(QSize(20,20));
	ioc11HB->setFillColor(QColor("white"));
	ioc11HB->setLineColor(QColor("black"));
	ioc11HB->setPvname("MPS_PF5U_IO_BI_00");
	ioc11HB->setFillColor(QColor("gray"));
	ioc11HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc11HB->setObjectName("CAGraphic_PF5UMPSHB");
	ioc11HB->setToolTip("PF5U MPS Heart Beat");

	CAGraphic *ioc12HB = new CAGraphic(this);
	ioc12HB->setLineWidth(2);
	ioc12HB->setMinimumSize(QSize(20,20));
	ioc12HB->setMaximumSize(QSize(20,20));
	ioc12HB->setFillColor(QColor("white"));
	ioc12HB->setLineColor(QColor("black"));
	ioc12HB->setPvname("MPS_PF5L_IO_BI_00");
	ioc12HB->setFillColor(QColor("gray"));
	ioc12HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc12HB->setObjectName("CAGraphic_PF5LMPSHB");
	ioc12HB->setToolTip("PF5L MPS Heart Beat");

	CAGraphic *ioc13HB = new CAGraphic(this);
	ioc13HB->setLineWidth(2);
	ioc13HB->setMinimumSize(QSize(20,20));
	ioc13HB->setMaximumSize(QSize(20,20));
	ioc13HB->setFillColor(QColor("white"));
	ioc13HB->setLineColor(QColor("black"));
	ioc13HB->setPvname("MPS_PF6U_IO_BI_00");
	ioc13HB->setFillColor(QColor("gray"));
	ioc13HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc13HB->setObjectName("CAGraphic_PF6UMPSHB");
	ioc13HB->setToolTip("PF6U MPS Heart Beat");

	CAGraphic *ioc14HB = new CAGraphic(this);
	ioc14HB->setLineWidth(2);
	ioc14HB->setMinimumSize(QSize(20,20));
	ioc14HB->setMaximumSize(QSize(20,20));
	ioc14HB->setFillColor(QColor("white"));
	ioc14HB->setLineColor(QColor("black"));
	ioc14HB->setPvname("MPS_PF6L_IO_BI_00");
	ioc14HB->setFillColor(QColor("gray"));
	ioc14HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc14HB->setObjectName("CAGraphic_PF6LMPSHB");
	ioc14HB->setToolTip("PF6L MPS Heart Beat");

	CAGraphic *ioc15HB = new CAGraphic(this);
	ioc15HB->setLineWidth(2);
	ioc15HB->setMinimumSize(QSize(20,20));
	ioc15HB->setMaximumSize(QSize(20,20));
	ioc15HB->setFillColor(QColor("white"));
	ioc15HB->setLineColor(QColor("black"));
	ioc15HB->setPvname("MPS_PF7_IO_BI_00");
	ioc15HB->setFillColor(QColor("gray"));
	ioc15HB->setFillDisplayMode(CAGraphic::ActInact);
	ioc15HB->setObjectName("CAGraphic_PF7MPSHB");
	ioc15HB->setToolTip("PF7 MPS Heart Beat");

    QFont clockfont;
    clockfont.setPointSize(12);
    CAWclock *wclock1 = new CAWclock(this);
	wclock1->setMinimumSize(QSize(160,20));
	wclock1->setMaximumSize(QSize(160,20));
	wclock1->setPvname("CCS_SYS_TIME.RVAL");
	wclock1->setFont(clockfont);
	wclock1->setObjectName("CAWclock_wclock1");
	
    QLabel *logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":/images/kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addWidget(wclock1);
    tblayout->addItem(tbspacer2);
	tblayout->addWidget(ioc1HB);
	tblayout->addWidget(ioc2HB);
	tblayout->addWidget(ioc3HB);
	tblayout->addWidget(ioc4HB);
	tblayout->addWidget(ioc5HB);
	tblayout->addWidget(ioc6HB);
	tblayout->addWidget(ioc7HB);
	tblayout->addWidget(ioc8HB);
	tblayout->addWidget(ioc9HB);
	tblayout->addWidget(ioc10HB);
	tblayout->addWidget(ioc11HB);
	tblayout->addWidget(ioc12HB);
	tblayout->addWidget(ioc13HB);
	tblayout->addWidget(ioc14HB);
	tblayout->addWidget(ioc15HB);
    tblayout->addWidget(logo);
	AttachChannelAccess *pattachTB = new AttachChannelAccess(tbframe);

    QSize size(1280, 1024);
    size = size.expandedTo(minimumSizeHint());
    resize(size);
    tabWidget->setCurrentIndex(0);
    QMetaObject::connectSlotsByName(this);

    msgframe = new QFrame(centralwidget);
    msgframe->setObjectName(QString::fromUtf8("msgframe"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    //sizePolicy3.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
    //msgframe->setSizePolicy(sizePolicy3);
    msgframe->setGeometry(QRect(19, 820, 1255, 90));
    //msgframe->setMinimumSize(QSize(1164, 90));
    //msgframe->setFrameShape(QFrame::StyledPanel);
    //msgframe->setFrameShadow(QFrame::Raised);


	vboxLayout3 = new QVBoxLayout(msgframe);
	vboxLayout3->setSpacing(0);
	vboxLayout3->setMargin(0);

	QHBoxLayout *vhLayout = new QHBoxLayout();
	vhLayout->setSpacing(0);
	vhLayout->setMargin(0);

	CADisplayer *ioc1_interlock = new CADisplayer();
	ioc1_interlock->setMinimumSize(QSize(160,20));
	ioc1_interlock->setMaximumSize(QSize(160,20));
	ioc1_interlock->setPvname("VMS_IOC1_INTERLOCK");
	ioc1_interlock->setObjectName("CADisplayer_ioc1_interlock");
	ioc1_interlock->setVisible(false);
	vhLayout->addWidget(ioc1_interlock);

	textEdit = new QTextEdit(this);
	textEdit->setObjectName(QString::fromUtf8("textEdit"));
	//textEdit->setGeometry(QRect(0, 0, 1000, 50)); 
	//textEdit->setGeometry(QRect(16, 900, 1000, 70)); 
	textEdit->setFontPointSize(12);
	textEdit->setAutoFormatting(QTextEdit::AutoAll);
	textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	vboxLayout3->addWidget(textEdit);

	AttachChannelAccess *pattach_msg = new AttachChannelAccess(msgframe);
    	setCentralWidget(centralwidget);

	QObject::connect(ioc1_interlock, SIGNAL(valueChanged(QString)), this,  SLOT(changeText(QString))); 

	QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setDefaultIndex(int))); 


	// Set Object Text.
	setWindowTitle(QApplication::translate("MainWindow", "KSTAR Supervisory Control Panel", 0, QApplication::UnicodeUTF8));
	setWindowIcon(QIcon(QString::fromUtf8("/usr/local/opi/images/MPS.xpm")));
    //action_Print->setText(QApplication::translate("MainWindow", "&Print", 0, QApplication::UnicodeUTF8));
    action_Exit->setText(QApplication::translate("MainWindow", "e&Xit", 0, QApplication::UnicodeUTF8));
    action_Multiplot->setText(QApplication::translate("MainWindow", "&Multiplot", 0, QApplication::UnicodeUTF8));
    action_Archiverviewer->setText(QApplication::translate("MainWindow", "a&Rchiveviewer", 0, QApplication::UnicodeUTF8));
    action_SignalDB->setText(QApplication::translate("MainWindow", "&SignalDB", 0, QApplication::UnicodeUTF8));
#if 0
    action_Manual->setText(QApplication::translate("MainWindow", "ma&Nual", 0, QApplication::UnicodeUTF8));
#endif
    action_AboutMPS->setText(QApplication::translate("MainWindow", "About &MPS", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_0), QApplication::translate("MainWindow", "Main panels", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    menu_Util->setTitle(QApplication::translate("MainWindow", "&Util", 0, QApplication::UnicodeUTF8));
#if 0
    menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
#endif
    menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));

} // setupUi

void MainWindow::registerPushButtonsEvent()
{
	QList<QPushButton *> allPButtons = findChildren<QPushButton *>();
	QString objName;

	for (int i = 0; i < allPButtons.size(); ++i) 
	{
		QPushButton *pushbutton = (QPushButton*)allPButtons.at(i);
		objName = pushbutton->objectName();
		if(objName.contains("pushButton"))
		{
			pushbutton -> installEventFilter(this);
			qDebug("ObjectName:%s", objName.toStdString().c_str());
		}
	};
}

void MainWindow::attachAllWidget()
{
	qDebug("-------------------------------MainWindow::attachAllWidget");
	QString displayname;
	for(int i = 0; i < displayDocklist.size();i++)
	{
		displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(i)+".ui";
		pattach = new AttachChannelAccess(displayname.toStdString().c_str(), i);
		vecACHAcc.push_back(pattach);
		if (!pattach->GetWidget())
		{
			QWidget *page = new QWidget();
			QPushButton *pbut = new QPushButton(page);
			pbut -> setGeometry(0,0,180,40);
			char display[30];
			sprintf(display,"%s: %d",displayname.toStdString().c_str(), i);
			pbut -> setText(display);
			stackedWidget->addWidget(page);

		}
		else
		{
			QWidget *w = pattach->GetWidget();
			w->setAutoFillBackground (true);
			stackedWidget->addWidget(pattach->GetWidget());
		}
	}
}

bool MainWindow::displayDockWidget(const int index, QPushButton *pobj)
{
	if ( index >= DISPLAYTOT || index < DISPLAYTYPE ) return false;

	QString displayname = QString("/usr/local/opi/ui/")+displayDocklist.at(index)+".ui";
    dockWidget->setWindowTitle(displayDocklist.at(index));
	setEnableButtons(pobj);

#if 1
	// for stackwidget
	if(stackedWidget->widget(index) != 0)
	{
		//stackedWidget->setCurrentIndex(index);
		//return true;
		setIndex(index);
	}; 
#else
	//for DockWidget
	AttachChannelAccess *pattach = new AttachChannelAccess(displayname.toStdString().c_str());
	qDebug("DisplayUI:%s",displayname.toStdString().c_str());
	if (!pattach->GetWidget())	return false;
	dockWidget->setWidget(pattach->GetWidget());
#endif
	return true;
}

void MainWindow::setIndex(const int index)
{
    stackedWidget->setCurrentIndex(index);
    AttachChannelAccess *pattach = 0;
    for (size_t i = 0; i < vecACHAcc.size(); i++)
    {
        pattach = vecACHAcc.at(i);
        if (!pattach->GetWidget()) continue;
        if ( i == index ) pattach->SetUiCurIndex(index);
        else pattach->SetUiCurIndex(-1);
    }
}


void MainWindow::setEnableButtons(QPushButton *pobj)
{
	QList<QPushButton *> allPButtons = findChildren<QPushButton *>();
	QString objName;

	for (int i = 0; i < allPButtons.size(); ++i) 
	{
		QPushButton *pushbutton = (QPushButton*)allPButtons.at(i);
		objName = pushbutton->objectName();
		if ( objName.compare(pobj->objectName()) == 0 ) pushbutton -> setEnabled(false);
		else pushbutton -> setEnabled(true);
	};

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	qDebug("---------------------------------------------MainWindow::eventFilter");
	QPushButton *pBut = (QPushButton*)obj;
	QString objName = pBut->objectName();
	int index = 0;
	if ( objName.compare("pushButton_0") == 0 ) index = DISPLAYTYPE;
	else if( objName.compare("pushButton_1") == 0 ) index = DISPLAYTYPE1;
	else if( objName.compare("pushButton_2") == 0 ) index = DISPLAYTYPE2;
	else if( objName.compare("pushButton_3") == 0 ) index = DISPLAYTYPE3;
	else if( objName.compare("pushButton_4") == 0 ) index = DISPLAYTYPE4;
	else if( objName.compare("pushButton_5") == 0 ) index = DISPLAYTYPE5;
	else if( objName.compare("pushButton_6") == 0 ) index = DISPLAYTYPE6;
	else if( objName.compare("pushButton_7") == 0 ) index = DISPLAYTYPE7;
	else if( objName.compare("pushButton_8") == 0 ) index = DISPLAYTYPE8;
	else if( objName.compare("pushButton_9") == 0 ) index = DISPLAYTYPE9;
	else if( objName.compare("pushButton_10") == 0 ) index = DISPLAYTYPE10;
	else if( objName.compare("pushButton_11") == 0 ) index = DISPLAYTYPE11;
	else if( objName.compare("pushButton_12") == 0 ) index = DISPLAYTYPE12;
	else if( objName.compare("pushButton_13") == 0 ) index = DISPLAYTYPE13;
	else if( objName.compare("pushButton_14") == 0 ) index = DISPLAYTYPE14;
	else if( objName.compare("pushButton_15") == 0 ) index = DISPLAYTYPE15;
	else if( objName.compare("pushButton_16") == 0 ) index = DISPLAYTYPE16;
	else index = -1;

	QString passwd = passWord.at(index);
	if(event->type()==QEvent::MouseButtonPress && pBut -> isEnabled() && (index < DISPLAYTOT || index >= DISPLAYTYPE))
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (mouseEvent->buttons() & Qt::LeftButton) 
		{
			proc = new QProcess(this);
			if(passwd.compare("") != 0)
			{
				//qDebug("Password: %s", passwd.toStdString().c_str()); 
				Password pword(passwd, new QWidget());
				if(pword.getStatus()== true) 
				{
					if (index == DISPLAYTYPE)
					{
						//proc = new QProcess(this);
						proc->start("edm -x /usr/local/opi/adl/CCS_Charging_mon.edl");
					} else
					{
						displayDockWidget(index, pBut);
					}
				}
			}
			else
			{
				if (index == DISPLAYTYPE)
				{
					if (proc->pid() > 0) 
					{
						qDebug("Process ID > 0?: %d", proc->pid());
					} else
					{
						qDebug("Process ID == 0?: %d", proc->pid());
					}

					proc = new QProcess();
					proc->start("edm -x /usr/local/opi/adl/CCS_Charging_mon.edl");
				} else
				{
					displayDockWidget(index, pBut);
				}
			}
			return true;
		}
		return false;
	} 

	return QMainWindow::eventFilter(obj, event);
}

void
MainWindow::showMultiplot()
{
	MultiplotMainWindow *pmainWindow = new MultiplotMainWindow;
	pmainWindow->resize(1280,900);
}
void
MainWindow::closeEvent(QCloseEvent *)
{
	archiver.kill();
	archiver2.kill();
	archiver3.kill();
	archiver4.kill();
	signaldb.kill();
	proc->kill();
	kill(getpid(), SIGTERM);
}

void
MainWindow::showSignalDB()
{
	QString program = "konqueror";
	QStringList arguments;
	arguments << "http://172.17.100.204/signalDB/";
	if (signaldb.state() != QProcess::Running )
	{
		signaldb.start(program, arguments);
	};
}

void
MainWindow::showArchiverviewer()
{
	QString program = "java";
	QStringList arguments;
	arguments << "-jar" << "/usr/local/opi/bin/archiveviewer.jar";
	qDebug("Commands: %s %s %s", program.toStdString().c_str(), arguments[0].toStdString().c_str(), arguments[1].toStdString().c_str());
#if 1
	if(archiver.state() != QProcess::Running)
	{
		archiver.start(program, arguments);
	}
	else if(archiver2.state() != QProcess::Running)
	{
		archiver2.start(program, arguments);
	}
	else if(archiver3.state() != QProcess::Running)
	{
		archiver3.start(program, arguments);
	}
	else if(archiver4.state() != QProcess::Running)
	{
		archiver4.start(program, arguments);
	}
#else
	if (myProcess->state() != QProcess::Running )
	{
		myProcess = new QProcess(this);
		myProcess->start(program, arguments);
	};
#endif

}

void
MainWindow::showManual()
{
	QString path = QApplication::applicationDirPath() + "/doc/index.html";  
	if (!assistant)
			assistant = new QAssistantClient("");
	assistant->showPage(path);
}

void
MainWindow::showAboutMPS()
{
	QMessageBox *mbox = new QMessageBox();
	mbox->setIconPixmap(QPixmap("/usr/local/opi/images/MPS.xpm"));
	mbox->setWindowTitle("About KSTAR MPS");
	mbox->setText("KSTAR MPS Operator Interface");
	mbox->setDetailedText("It is developed to control the KSTAR Integrated Control System.");
	mbox->exec();

}
void
MainWindow::setDefaultIndex(int index) 
{
#if 0
	if (index == 0)
	{
		setEnableButtons(pushButton[1]);
		setIndex(1);
	};
#else
	setEnableButtons(pushButton[index]);
	setIndex(index);
#endif
}
void
MainWindow::changeText(const QString &str) 
{
	QString newstr="Interlock Event Number: ";
	newstr.append(str);
	newstr.append("\n");
	textEdit->insertPlainText(newstr);
	textEdit->moveCursor( QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
	//textEdit->ensureCursorVisible();
}

void
MainWindow::keyPressEvent(QKeyEvent *event )
{
	int key = event->key();
	quint32 modifier = event->nativeModifiers();
	if ((modifier==0x14 || modifier == 0x4)&& key == Qt::Key_P)
	{
		QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
		QString filename = "/home/kstar/screenshot/"+curtime+".jpg";
		QString format = "jpg";
		mpixmap = QPixmap::grabWidget(this);
		mpixmap.save(filename, format.toAscii());
	};
}

