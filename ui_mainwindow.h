/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionZapisz_system;
    QAction *actionWczytaj_system;
    QAction *actionSaveToSpreadsheetBlProb;
    QAction *actionSaveToSpreadsheetLossProb;
    QAction *actionSaveToSpreadsheetAll;
    QAction *actionSaveToSpreadsheetAvgQeue;
    QAction *actionSaveToSpreadsheetTime;
    QAction *actionSaveToSpreadsheeTrDistrib;
    QAction *actionSaveToSpreadsheetAUs;
    QAction *actionSaveToSpreadsheeTrDistribNQ;
    QAction *actionGnuplotBlocking_probability;
    QAction *actionGnuplotLoss_probability;
    QAction *actionGnuplotAvarage_qeue_length;
    QAction *actionAbout;
    QAction *actionGnuplotAvarage_number_of_calls_in_qeue;
    QAction *actionGnuplotEffectiveWaitingTime;
    QAction *actionGnuplotAbsoluteWaitingTime;
    QAction *actionGnuplotYtInQeue;
    QAction *actionZapiszZajetoscPpodgrup;
    QAction *actionSaveXSLX_QoSforClasses;
    QAction *actionSaveXLSX_subroupAvailability;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *Pages;
    QWidget *tabWidgetPage_SystemModel;
    QHBoxLayout *verticalLayoutModel_2;
    QFrame *verticalLayoutModel;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayoutPredefinedSystems;
    QComboBox *comboBoxPredefinedSystems;
    QPushButton *pushButtonPredSystemsAdd;
    QFrame *line_DdSeparator;
    QGridLayout *gridLayoutModelResourcess;
    QLabel *labelModelServer;
    QGridLayout *gridLayoutModelQeue;
    QSpinBox *spinBoxKolejkaPojemnosc;
    QLabel *labelModelQeueNumber;
    QSpinBox *spinBoxKolejkaLiczba;
    QLabel *labelModelQeueCapacity;
    QListWidget *listWidgetKolejki;
    QHBoxLayout *horizontalLayoutModelQeueListButtons;
    QPushButton *pushButton_kolejkidodaj;
    QPushButton *pushButton_kolejkiZmien;
    QPushButton *pushButton_kolejkiUsun;
    QComboBox *comboBoxSubBufferScheduler;
    QLabel *label_subbufferScheduler;
    QLabel *labelModelQeue;
    QGridLayout *gridLayoutModelServer;
    QSpinBox *spinBoxPodgrupaPojemnosc;
    QSpinBox *spinBoxPodgrupaLiczba;
    QLabel *labelPodgrupaPojemnosc;
    QLabel *labelPodgrupaLiczba;
    QListWidget *listWidgetWiazki;
    QHBoxLayout *horizontalLayoutModelServerListButtons;
    QPushButton *pushButton_wiazkiDodaj;
    QPushButton *pushButton_wiazkiZmien;
    QPushButton *pushButton_wiazkiUsun;
    QComboBox *comboBoxSubgoupSchedulerAlgorithm;
    QLabel *label_subgroupSchedulerAlgorithm;
    QHBoxLayout *horizontalLayoutGroupChooseAlg;
    QFrame *line_ModelSeparator;
    QLabel *labelClasses;
    QGridLayout *gridLayoutClasses;
    QLabel *labelKlasyPropAT;
    QLabel *labelKlasyT;
    QSpinBox *spinBoxClassesAT;
    QListWidget *listWidgetKlasy;
    QDoubleSpinBox *doubleSpinBoxClassesMu;
    QComboBox *comboBox_NewCallStrType;
    QSpinBox *spinBoxClassesSourcess;
    QLabel *label_trSrcType;
    QPushButton *pushButtonKlasaDodaj;
    QDoubleSpinBox *doubleSpinBoxIncomingExDx;
    QSpinBox *spinBoxClassesT;
    QPushButton *pushButtonKlasaZmien;
    QLabel *labelClassesNumberOfSourcess;
    QDoubleSpinBox *doubleSpinBoxServiceExDx;
    QPushButton *pushButtonKlasaUsun;
    QComboBox *comboBox_SourceType;
    QLabel *labelKlasyMu;
    QLabel *labelClassesServiceExDx;
    QLabel *labelClassesIncommingExDx;
    QLabel *label_NewCallStrType;
    QLabel *label_CallServStreamType;
    QComboBox *comboBox_CallServStrType;
    QGraphicsView *graphicsView;
    QWidget *tabWidgetQoS;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayoutQoS_Algorithms;
    QLabel *label_Amin;
    QListWidget *listWidgetAlgorithmsAlternative;
    QLabel *labelAlgorithmsAlternative;
    QDoubleSpinBox *doubleSpinBox_aMax;
    QLabel *label;
    QLabel *label_Amax;
    QLabel *labelAlgorithms;
    QLabel *label_Adelta;
    QDoubleSpinBox *doubleSpinBox_aDelta;
    QListWidget *listWidgetAlgorithms;
    QListWidget *listWidgetSimulationParameters;
    QPushButton *pushButtonStart;
    QDoubleSpinBox *doubleSpinBox_aMin;
    QGridLayout *gridLayoutCharts;
    QCheckBox *checkBoxResultsQtShowKey;
    QListWidget *listWidgetResultsQtAdditionalParameters1;
    QListWidget *listWidgetResultsQtAdditionalParameters2;
    QLabel *labelResultsQtAdditionalParameters2;
    QLabel *labelResultsQtAdditionalParameters1;
    QFrame *line;
    QChartView *widgetResultsPlot;
    QLabel *labelResultsQtType;
    QLabel *labelResultsQtX_axis;
    QCheckBox *checkBoxResultsQtLogScaleOnAxisY;
    QComboBox *comboBoxResultsQtType;
    QComboBox *comboBoxResultsQtX_axis;
    QProgressBar *progressBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSave_results_to_gnuplot;
    QMenu *menuZapisz_do_arkusza_XSLX;
    QMenu *menuHelp;
    QMenu *menuLanguage;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(735, 900);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setBaseSize(QSize(0, 50));
        QIcon icon;
        icon.addFile(QStringLiteral(":/graphics/Img/ikona.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setIconSize(QSize(24, 24));
        actionZapisz_system = new QAction(MainWindow);
        actionZapisz_system->setObjectName(QStringLiteral("actionZapisz_system"));
        actionWczytaj_system = new QAction(MainWindow);
        actionWczytaj_system->setObjectName(QStringLiteral("actionWczytaj_system"));
        actionSaveToSpreadsheetBlProb = new QAction(MainWindow);
        actionSaveToSpreadsheetBlProb->setObjectName(QStringLiteral("actionSaveToSpreadsheetBlProb"));
        actionSaveToSpreadsheetLossProb = new QAction(MainWindow);
        actionSaveToSpreadsheetLossProb->setObjectName(QStringLiteral("actionSaveToSpreadsheetLossProb"));
        actionSaveToSpreadsheetAll = new QAction(MainWindow);
        actionSaveToSpreadsheetAll->setObjectName(QStringLiteral("actionSaveToSpreadsheetAll"));
        actionSaveToSpreadsheetAvgQeue = new QAction(MainWindow);
        actionSaveToSpreadsheetAvgQeue->setObjectName(QStringLiteral("actionSaveToSpreadsheetAvgQeue"));
        actionSaveToSpreadsheetTime = new QAction(MainWindow);
        actionSaveToSpreadsheetTime->setObjectName(QStringLiteral("actionSaveToSpreadsheetTime"));
        actionSaveToSpreadsheeTrDistrib = new QAction(MainWindow);
        actionSaveToSpreadsheeTrDistrib->setObjectName(QStringLiteral("actionSaveToSpreadsheeTrDistrib"));
        actionSaveToSpreadsheetAUs = new QAction(MainWindow);
        actionSaveToSpreadsheetAUs->setObjectName(QStringLiteral("actionSaveToSpreadsheetAUs"));
        actionSaveToSpreadsheeTrDistribNQ = new QAction(MainWindow);
        actionSaveToSpreadsheeTrDistribNQ->setObjectName(QStringLiteral("actionSaveToSpreadsheeTrDistribNQ"));
        actionGnuplotBlocking_probability = new QAction(MainWindow);
        actionGnuplotBlocking_probability->setObjectName(QStringLiteral("actionGnuplotBlocking_probability"));
        actionGnuplotLoss_probability = new QAction(MainWindow);
        actionGnuplotLoss_probability->setObjectName(QStringLiteral("actionGnuplotLoss_probability"));
        actionGnuplotAvarage_qeue_length = new QAction(MainWindow);
        actionGnuplotAvarage_qeue_length->setObjectName(QStringLiteral("actionGnuplotAvarage_qeue_length"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionGnuplotAvarage_number_of_calls_in_qeue = new QAction(MainWindow);
        actionGnuplotAvarage_number_of_calls_in_qeue->setObjectName(QStringLiteral("actionGnuplotAvarage_number_of_calls_in_qeue"));
        actionGnuplotEffectiveWaitingTime = new QAction(MainWindow);
        actionGnuplotEffectiveWaitingTime->setObjectName(QStringLiteral("actionGnuplotEffectiveWaitingTime"));
        actionGnuplotAbsoluteWaitingTime = new QAction(MainWindow);
        actionGnuplotAbsoluteWaitingTime->setObjectName(QStringLiteral("actionGnuplotAbsoluteWaitingTime"));
        actionGnuplotYtInQeue = new QAction(MainWindow);
        actionGnuplotYtInQeue->setObjectName(QStringLiteral("actionGnuplotYtInQeue"));
        actionZapiszZajetoscPpodgrup = new QAction(MainWindow);
        actionZapiszZajetoscPpodgrup->setObjectName(QStringLiteral("actionZapiszZajetoscPpodgrup"));
        actionSaveXSLX_QoSforClasses = new QAction(MainWindow);
        actionSaveXSLX_QoSforClasses->setObjectName(QStringLiteral("actionSaveXSLX_QoSforClasses"));
        actionSaveXLSX_subroupAvailability = new QAction(MainWindow);
        actionSaveXLSX_subroupAvailability->setObjectName(QStringLiteral("actionSaveXLSX_subroupAvailability"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        centralWidget->setMinimumSize(QSize(400, 0));
        centralWidget->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        Pages = new QTabWidget(centralWidget);
        Pages->setObjectName(QStringLiteral("Pages"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Pages->sizePolicy().hasHeightForWidth());
        Pages->setSizePolicy(sizePolicy2);
        Pages->setTabPosition(QTabWidget::North);
        Pages->setTabShape(QTabWidget::Rounded);
        tabWidgetPage_SystemModel = new QWidget();
        tabWidgetPage_SystemModel->setObjectName(QStringLiteral("tabWidgetPage_SystemModel"));
        tabWidgetPage_SystemModel->setEnabled(true);
        verticalLayoutModel_2 = new QHBoxLayout(tabWidgetPage_SystemModel);
        verticalLayoutModel_2->setSpacing(6);
        verticalLayoutModel_2->setContentsMargins(11, 11, 11, 11);
        verticalLayoutModel_2->setObjectName(QStringLiteral("verticalLayoutModel_2"));
        verticalLayoutModel = new QFrame(tabWidgetPage_SystemModel);
        verticalLayoutModel->setObjectName(QStringLiteral("verticalLayoutModel"));
        verticalLayoutModel->setEnabled(true);
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(verticalLayoutModel->sizePolicy().hasHeightForWidth());
        verticalLayoutModel->setSizePolicy(sizePolicy3);
        verticalLayout_5 = new QVBoxLayout(verticalLayoutModel);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        horizontalLayoutPredefinedSystems = new QHBoxLayout();
        horizontalLayoutPredefinedSystems->setSpacing(6);
        horizontalLayoutPredefinedSystems->setObjectName(QStringLiteral("horizontalLayoutPredefinedSystems"));
        comboBoxPredefinedSystems = new QComboBox(verticalLayoutModel);
        comboBoxPredefinedSystems->setObjectName(QStringLiteral("comboBoxPredefinedSystems"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(comboBoxPredefinedSystems->sizePolicy().hasHeightForWidth());
        comboBoxPredefinedSystems->setSizePolicy(sizePolicy4);

        horizontalLayoutPredefinedSystems->addWidget(comboBoxPredefinedSystems);

        pushButtonPredSystemsAdd = new QPushButton(verticalLayoutModel);
        pushButtonPredSystemsAdd->setObjectName(QStringLiteral("pushButtonPredSystemsAdd"));
        QSizePolicy sizePolicy5(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(pushButtonPredSystemsAdd->sizePolicy().hasHeightForWidth());
        pushButtonPredSystemsAdd->setSizePolicy(sizePolicy5);

        horizontalLayoutPredefinedSystems->addWidget(pushButtonPredSystemsAdd);


        verticalLayout_5->addLayout(horizontalLayoutPredefinedSystems);

        line_DdSeparator = new QFrame(verticalLayoutModel);
        line_DdSeparator->setObjectName(QStringLiteral("line_DdSeparator"));
        line_DdSeparator->setFrameShape(QFrame::HLine);
        line_DdSeparator->setFrameShadow(QFrame::Sunken);

        verticalLayout_5->addWidget(line_DdSeparator);

        gridLayoutModelResourcess = new QGridLayout();
        gridLayoutModelResourcess->setSpacing(6);
        gridLayoutModelResourcess->setObjectName(QStringLiteral("gridLayoutModelResourcess"));
        labelModelServer = new QLabel(verticalLayoutModel);
        labelModelServer->setObjectName(QStringLiteral("labelModelServer"));
        QSizePolicy sizePolicy6(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(labelModelServer->sizePolicy().hasHeightForWidth());
        labelModelServer->setSizePolicy(sizePolicy6);

        gridLayoutModelResourcess->addWidget(labelModelServer, 0, 0, 1, 1);

        gridLayoutModelQeue = new QGridLayout();
        gridLayoutModelQeue->setSpacing(6);
        gridLayoutModelQeue->setObjectName(QStringLiteral("gridLayoutModelQeue"));
        gridLayoutModelQeue->setSizeConstraint(QLayout::SetMinAndMaxSize);
        spinBoxKolejkaPojemnosc = new QSpinBox(verticalLayoutModel);
        spinBoxKolejkaPojemnosc->setObjectName(QStringLiteral("spinBoxKolejkaPojemnosc"));
        QSizePolicy sizePolicy7(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(spinBoxKolejkaPojemnosc->sizePolicy().hasHeightForWidth());
        spinBoxKolejkaPojemnosc->setSizePolicy(sizePolicy7);
        spinBoxKolejkaPojemnosc->setMinimum(1);
        spinBoxKolejkaPojemnosc->setMaximum(300);
        spinBoxKolejkaPojemnosc->setValue(10);

        gridLayoutModelQeue->addWidget(spinBoxKolejkaPojemnosc, 1, 1, 1, 1);

        labelModelQeueNumber = new QLabel(verticalLayoutModel);
        labelModelQeueNumber->setObjectName(QStringLiteral("labelModelQeueNumber"));
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(labelModelQeueNumber->sizePolicy().hasHeightForWidth());
        labelModelQeueNumber->setSizePolicy(sizePolicy8);

        gridLayoutModelQeue->addWidget(labelModelQeueNumber, 2, 0, 1, 1);

        spinBoxKolejkaLiczba = new QSpinBox(verticalLayoutModel);
        spinBoxKolejkaLiczba->setObjectName(QStringLiteral("spinBoxKolejkaLiczba"));
        sizePolicy7.setHeightForWidth(spinBoxKolejkaLiczba->sizePolicy().hasHeightForWidth());
        spinBoxKolejkaLiczba->setSizePolicy(sizePolicy7);
        spinBoxKolejkaLiczba->setMinimum(1);
        spinBoxKolejkaLiczba->setMaximum(100);

        gridLayoutModelQeue->addWidget(spinBoxKolejkaLiczba, 2, 1, 1, 1);

        labelModelQeueCapacity = new QLabel(verticalLayoutModel);
        labelModelQeueCapacity->setObjectName(QStringLiteral("labelModelQeueCapacity"));
        sizePolicy8.setHeightForWidth(labelModelQeueCapacity->sizePolicy().hasHeightForWidth());
        labelModelQeueCapacity->setSizePolicy(sizePolicy8);

        gridLayoutModelQeue->addWidget(labelModelQeueCapacity, 1, 0, 1, 1);

        listWidgetKolejki = new QListWidget(verticalLayoutModel);
        listWidgetKolejki->setObjectName(QStringLiteral("listWidgetKolejki"));
        QSizePolicy sizePolicy9(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy9.setHorizontalStretch(0);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(listWidgetKolejki->sizePolicy().hasHeightForWidth());
        listWidgetKolejki->setSizePolicy(sizePolicy9);
        listWidgetKolejki->setMinimumSize(QSize(100, 0));
        listWidgetKolejki->setBaseSize(QSize(0, 50));
        listWidgetKolejki->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

        gridLayoutModelQeue->addWidget(listWidgetKolejki, 1, 2, 2, 1);

        horizontalLayoutModelQeueListButtons = new QHBoxLayout();
        horizontalLayoutModelQeueListButtons->setSpacing(6);
        horizontalLayoutModelQeueListButtons->setObjectName(QStringLiteral("horizontalLayoutModelQeueListButtons"));
        horizontalLayoutModelQeueListButtons->setSizeConstraint(QLayout::SetDefaultConstraint);
        pushButton_kolejkidodaj = new QPushButton(verticalLayoutModel);
        pushButton_kolejkidodaj->setObjectName(QStringLiteral("pushButton_kolejkidodaj"));
        pushButton_kolejkidodaj->setEnabled(true);

        horizontalLayoutModelQeueListButtons->addWidget(pushButton_kolejkidodaj);

        pushButton_kolejkiZmien = new QPushButton(verticalLayoutModel);
        pushButton_kolejkiZmien->setObjectName(QStringLiteral("pushButton_kolejkiZmien"));
        pushButton_kolejkiZmien->setEnabled(false);

        horizontalLayoutModelQeueListButtons->addWidget(pushButton_kolejkiZmien);

        pushButton_kolejkiUsun = new QPushButton(verticalLayoutModel);
        pushButton_kolejkiUsun->setObjectName(QStringLiteral("pushButton_kolejkiUsun"));
        pushButton_kolejkiUsun->setEnabled(false);
        sizePolicy5.setHeightForWidth(pushButton_kolejkiUsun->sizePolicy().hasHeightForWidth());
        pushButton_kolejkiUsun->setSizePolicy(sizePolicy5);
        pushButton_kolejkiUsun->setLayoutDirection(Qt::RightToLeft);

        horizontalLayoutModelQeueListButtons->addWidget(pushButton_kolejkiUsun);


        gridLayoutModelQeue->addLayout(horizontalLayoutModelQeueListButtons, 3, 0, 1, 3);

        comboBoxSubBufferScheduler = new QComboBox(verticalLayoutModel);
        comboBoxSubBufferScheduler->setObjectName(QStringLiteral("comboBoxSubBufferScheduler"));

        gridLayoutModelQeue->addWidget(comboBoxSubBufferScheduler, 0, 2, 1, 1);

        label_subbufferScheduler = new QLabel(verticalLayoutModel);
        label_subbufferScheduler->setObjectName(QStringLiteral("label_subbufferScheduler"));

        gridLayoutModelQeue->addWidget(label_subbufferScheduler, 0, 0, 1, 2);

        gridLayoutModelQeue->setColumnStretch(2, 1);

        gridLayoutModelResourcess->addLayout(gridLayoutModelQeue, 2, 2, 1, 1);

        labelModelQeue = new QLabel(verticalLayoutModel);
        labelModelQeue->setObjectName(QStringLiteral("labelModelQeue"));
        labelModelQeue->setEnabled(true);
        sizePolicy6.setHeightForWidth(labelModelQeue->sizePolicy().hasHeightForWidth());
        labelModelQeue->setSizePolicy(sizePolicy6);

        gridLayoutModelResourcess->addWidget(labelModelQeue, 0, 2, 1, 1);

        gridLayoutModelServer = new QGridLayout();
        gridLayoutModelServer->setSpacing(6);
        gridLayoutModelServer->setObjectName(QStringLiteral("gridLayoutModelServer"));
        gridLayoutModelServer->setSizeConstraint(QLayout::SetMinAndMaxSize);
        spinBoxPodgrupaPojemnosc = new QSpinBox(verticalLayoutModel);
        spinBoxPodgrupaPojemnosc->setObjectName(QStringLiteral("spinBoxPodgrupaPojemnosc"));
        sizePolicy7.setHeightForWidth(spinBoxPodgrupaPojemnosc->sizePolicy().hasHeightForWidth());
        spinBoxPodgrupaPojemnosc->setSizePolicy(sizePolicy7);
        spinBoxPodgrupaPojemnosc->setMinimum(1);
        spinBoxPodgrupaPojemnosc->setMaximum(300);
        spinBoxPodgrupaPojemnosc->setValue(20);

        gridLayoutModelServer->addWidget(spinBoxPodgrupaPojemnosc, 1, 1, 1, 1);

        spinBoxPodgrupaLiczba = new QSpinBox(verticalLayoutModel);
        spinBoxPodgrupaLiczba->setObjectName(QStringLiteral("spinBoxPodgrupaLiczba"));
        sizePolicy7.setHeightForWidth(spinBoxPodgrupaLiczba->sizePolicy().hasHeightForWidth());
        spinBoxPodgrupaLiczba->setSizePolicy(sizePolicy7);
        spinBoxPodgrupaLiczba->setMinimum(1);
        spinBoxPodgrupaLiczba->setMaximum(100);

        gridLayoutModelServer->addWidget(spinBoxPodgrupaLiczba, 2, 1, 1, 1);

        labelPodgrupaPojemnosc = new QLabel(verticalLayoutModel);
        labelPodgrupaPojemnosc->setObjectName(QStringLiteral("labelPodgrupaPojemnosc"));
        sizePolicy8.setHeightForWidth(labelPodgrupaPojemnosc->sizePolicy().hasHeightForWidth());
        labelPodgrupaPojemnosc->setSizePolicy(sizePolicy8);

        gridLayoutModelServer->addWidget(labelPodgrupaPojemnosc, 1, 0, 1, 1);

        labelPodgrupaLiczba = new QLabel(verticalLayoutModel);
        labelPodgrupaLiczba->setObjectName(QStringLiteral("labelPodgrupaLiczba"));
        sizePolicy8.setHeightForWidth(labelPodgrupaLiczba->sizePolicy().hasHeightForWidth());
        labelPodgrupaLiczba->setSizePolicy(sizePolicy8);

        gridLayoutModelServer->addWidget(labelPodgrupaLiczba, 2, 0, 1, 1);

        listWidgetWiazki = new QListWidget(verticalLayoutModel);
        listWidgetWiazki->setObjectName(QStringLiteral("listWidgetWiazki"));
        sizePolicy9.setHeightForWidth(listWidgetWiazki->sizePolicy().hasHeightForWidth());
        listWidgetWiazki->setSizePolicy(sizePolicy9);
        listWidgetWiazki->setMinimumSize(QSize(100, 0));
        listWidgetWiazki->setBaseSize(QSize(0, 50));
        listWidgetWiazki->setFrameShape(QFrame::StyledPanel);
        listWidgetWiazki->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

        gridLayoutModelServer->addWidget(listWidgetWiazki, 1, 2, 2, 1);

        horizontalLayoutModelServerListButtons = new QHBoxLayout();
        horizontalLayoutModelServerListButtons->setSpacing(6);
        horizontalLayoutModelServerListButtons->setObjectName(QStringLiteral("horizontalLayoutModelServerListButtons"));
        horizontalLayoutModelServerListButtons->setSizeConstraint(QLayout::SetDefaultConstraint);
        pushButton_wiazkiDodaj = new QPushButton(verticalLayoutModel);
        pushButton_wiazkiDodaj->setObjectName(QStringLiteral("pushButton_wiazkiDodaj"));

        horizontalLayoutModelServerListButtons->addWidget(pushButton_wiazkiDodaj);

        pushButton_wiazkiZmien = new QPushButton(verticalLayoutModel);
        pushButton_wiazkiZmien->setObjectName(QStringLiteral("pushButton_wiazkiZmien"));
        pushButton_wiazkiZmien->setEnabled(false);

        horizontalLayoutModelServerListButtons->addWidget(pushButton_wiazkiZmien);

        pushButton_wiazkiUsun = new QPushButton(verticalLayoutModel);
        pushButton_wiazkiUsun->setObjectName(QStringLiteral("pushButton_wiazkiUsun"));
        pushButton_wiazkiUsun->setEnabled(false);
        sizePolicy5.setHeightForWidth(pushButton_wiazkiUsun->sizePolicy().hasHeightForWidth());
        pushButton_wiazkiUsun->setSizePolicy(sizePolicy5);
        pushButton_wiazkiUsun->setLayoutDirection(Qt::RightToLeft);

        horizontalLayoutModelServerListButtons->addWidget(pushButton_wiazkiUsun);


        gridLayoutModelServer->addLayout(horizontalLayoutModelServerListButtons, 3, 0, 1, 3);

        comboBoxSubgoupSchedulerAlgorithm = new QComboBox(verticalLayoutModel);
        comboBoxSubgoupSchedulerAlgorithm->setObjectName(QStringLiteral("comboBoxSubgoupSchedulerAlgorithm"));

        gridLayoutModelServer->addWidget(comboBoxSubgoupSchedulerAlgorithm, 0, 2, 1, 1);

        label_subgroupSchedulerAlgorithm = new QLabel(verticalLayoutModel);
        label_subgroupSchedulerAlgorithm->setObjectName(QStringLiteral("label_subgroupSchedulerAlgorithm"));

        gridLayoutModelServer->addWidget(label_subgroupSchedulerAlgorithm, 0, 0, 1, 2);


        gridLayoutModelResourcess->addLayout(gridLayoutModelServer, 2, 0, 1, 1);

        horizontalLayoutGroupChooseAlg = new QHBoxLayout();
        horizontalLayoutGroupChooseAlg->setSpacing(6);
        horizontalLayoutGroupChooseAlg->setObjectName(QStringLiteral("horizontalLayoutGroupChooseAlg"));

        gridLayoutModelResourcess->addLayout(horizontalLayoutGroupChooseAlg, 3, 0, 1, 1);


        verticalLayout_5->addLayout(gridLayoutModelResourcess);

        line_ModelSeparator = new QFrame(verticalLayoutModel);
        line_ModelSeparator->setObjectName(QStringLiteral("line_ModelSeparator"));
        line_ModelSeparator->setFrameShape(QFrame::HLine);
        line_ModelSeparator->setFrameShadow(QFrame::Sunken);

        verticalLayout_5->addWidget(line_ModelSeparator);

        labelClasses = new QLabel(verticalLayoutModel);
        labelClasses->setObjectName(QStringLiteral("labelClasses"));

        verticalLayout_5->addWidget(labelClasses);

        gridLayoutClasses = new QGridLayout();
        gridLayoutClasses->setSpacing(6);
        gridLayoutClasses->setObjectName(QStringLiteral("gridLayoutClasses"));
        gridLayoutClasses->setVerticalSpacing(6);
        labelKlasyPropAT = new QLabel(verticalLayoutModel);
        labelKlasyPropAT->setObjectName(QStringLiteral("labelKlasyPropAT"));
        sizePolicy7.setHeightForWidth(labelKlasyPropAT->sizePolicy().hasHeightForWidth());
        labelKlasyPropAT->setSizePolicy(sizePolicy7);
        labelKlasyPropAT->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutClasses->addWidget(labelKlasyPropAT, 1, 2, 1, 1);

        labelKlasyT = new QLabel(verticalLayoutModel);
        labelKlasyT->setObjectName(QStringLiteral("labelKlasyT"));
        sizePolicy7.setHeightForWidth(labelKlasyT->sizePolicy().hasHeightForWidth());
        labelKlasyT->setSizePolicy(sizePolicy7);
        labelKlasyT->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutClasses->addWidget(labelKlasyT, 2, 2, 1, 1);

        spinBoxClassesAT = new QSpinBox(verticalLayoutModel);
        spinBoxClassesAT->setObjectName(QStringLiteral("spinBoxClassesAT"));
        sizePolicy7.setHeightForWidth(spinBoxClassesAT->sizePolicy().hasHeightForWidth());
        spinBoxClassesAT->setSizePolicy(sizePolicy7);
        spinBoxClassesAT->setMinimum(1);

        gridLayoutClasses->addWidget(spinBoxClassesAT, 1, 3, 1, 1);

        listWidgetKlasy = new QListWidget(verticalLayoutModel);
        listWidgetKlasy->setObjectName(QStringLiteral("listWidgetKlasy"));
        sizePolicy2.setHeightForWidth(listWidgetKlasy->sizePolicy().hasHeightForWidth());
        listWidgetKlasy->setSizePolicy(sizePolicy2);
        listWidgetKlasy->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);

        gridLayoutClasses->addWidget(listWidgetKlasy, 1, 4, 3, 2);

        doubleSpinBoxClassesMu = new QDoubleSpinBox(verticalLayoutModel);
        doubleSpinBoxClassesMu->setObjectName(QStringLiteral("doubleSpinBoxClassesMu"));
        sizePolicy7.setHeightForWidth(doubleSpinBoxClassesMu->sizePolicy().hasHeightForWidth());
        doubleSpinBoxClassesMu->setSizePolicy(sizePolicy7);
        doubleSpinBoxClassesMu->setValue(1);

        gridLayoutClasses->addWidget(doubleSpinBoxClassesMu, 3, 3, 1, 1);

        comboBox_NewCallStrType = new QComboBox(verticalLayoutModel);
        comboBox_NewCallStrType->setObjectName(QStringLiteral("comboBox_NewCallStrType"));

        gridLayoutClasses->addWidget(comboBox_NewCallStrType, 6, 4, 1, 1);

        spinBoxClassesSourcess = new QSpinBox(verticalLayoutModel);
        spinBoxClassesSourcess->setObjectName(QStringLiteral("spinBoxClassesSourcess"));
        spinBoxClassesSourcess->setEnabled(false);
        sizePolicy7.setHeightForWidth(spinBoxClassesSourcess->sizePolicy().hasHeightForWidth());
        spinBoxClassesSourcess->setSizePolicy(sizePolicy7);
        spinBoxClassesSourcess->setMinimum(1);
        spinBoxClassesSourcess->setMaximum(1000);
        spinBoxClassesSourcess->setSingleStep(10);
        spinBoxClassesSourcess->setValue(20);

        gridLayoutClasses->addWidget(spinBoxClassesSourcess, 4, 6, 1, 1);

        label_trSrcType = new QLabel(verticalLayoutModel);
        label_trSrcType->setObjectName(QStringLiteral("label_trSrcType"));

        gridLayoutClasses->addWidget(label_trSrcType, 4, 2, 1, 2);

        pushButtonKlasaDodaj = new QPushButton(verticalLayoutModel);
        pushButtonKlasaDodaj->setObjectName(QStringLiteral("pushButtonKlasaDodaj"));
        QSizePolicy sizePolicy10(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy10.setHorizontalStretch(0);
        sizePolicy10.setVerticalStretch(0);
        sizePolicy10.setHeightForWidth(pushButtonKlasaDodaj->sizePolicy().hasHeightForWidth());
        pushButtonKlasaDodaj->setSizePolicy(sizePolicy10);

        gridLayoutClasses->addWidget(pushButtonKlasaDodaj, 1, 6, 1, 1);

        doubleSpinBoxIncomingExDx = new QDoubleSpinBox(verticalLayoutModel);
        doubleSpinBoxIncomingExDx->setObjectName(QStringLiteral("doubleSpinBoxIncomingExDx"));
        doubleSpinBoxIncomingExDx->setEnabled(false);
        sizePolicy7.setHeightForWidth(doubleSpinBoxIncomingExDx->sizePolicy().hasHeightForWidth());
        doubleSpinBoxIncomingExDx->setSizePolicy(sizePolicy7);
        doubleSpinBoxIncomingExDx->setMinimum(3);
        doubleSpinBoxIncomingExDx->setMaximum(1000);
        doubleSpinBoxIncomingExDx->setValue(3);

        gridLayoutClasses->addWidget(doubleSpinBoxIncomingExDx, 6, 6, 1, 1);

        spinBoxClassesT = new QSpinBox(verticalLayoutModel);
        spinBoxClassesT->setObjectName(QStringLiteral("spinBoxClassesT"));
        sizePolicy7.setHeightForWidth(spinBoxClassesT->sizePolicy().hasHeightForWidth());
        spinBoxClassesT->setSizePolicy(sizePolicy7);
        spinBoxClassesT->setMinimum(1);

        gridLayoutClasses->addWidget(spinBoxClassesT, 2, 3, 1, 1);

        pushButtonKlasaZmien = new QPushButton(verticalLayoutModel);
        pushButtonKlasaZmien->setObjectName(QStringLiteral("pushButtonKlasaZmien"));
        pushButtonKlasaZmien->setEnabled(false);
        sizePolicy10.setHeightForWidth(pushButtonKlasaZmien->sizePolicy().hasHeightForWidth());
        pushButtonKlasaZmien->setSizePolicy(sizePolicy10);

        gridLayoutClasses->addWidget(pushButtonKlasaZmien, 2, 6, 1, 1);

        labelClassesNumberOfSourcess = new QLabel(verticalLayoutModel);
        labelClassesNumberOfSourcess->setObjectName(QStringLiteral("labelClassesNumberOfSourcess"));
        labelClassesNumberOfSourcess->setEnabled(false);
        labelClassesNumberOfSourcess->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutClasses->addWidget(labelClassesNumberOfSourcess, 4, 5, 1, 1);

        doubleSpinBoxServiceExDx = new QDoubleSpinBox(verticalLayoutModel);
        doubleSpinBoxServiceExDx->setObjectName(QStringLiteral("doubleSpinBoxServiceExDx"));
        doubleSpinBoxServiceExDx->setEnabled(false);
        sizePolicy7.setHeightForWidth(doubleSpinBoxServiceExDx->sizePolicy().hasHeightForWidth());
        doubleSpinBoxServiceExDx->setSizePolicy(sizePolicy7);
        doubleSpinBoxServiceExDx->setMinimum(3);
        doubleSpinBoxServiceExDx->setMaximum(1000);

        gridLayoutClasses->addWidget(doubleSpinBoxServiceExDx, 8, 6, 1, 1);

        pushButtonKlasaUsun = new QPushButton(verticalLayoutModel);
        pushButtonKlasaUsun->setObjectName(QStringLiteral("pushButtonKlasaUsun"));
        pushButtonKlasaUsun->setEnabled(false);
        sizePolicy10.setHeightForWidth(pushButtonKlasaUsun->sizePolicy().hasHeightForWidth());
        pushButtonKlasaUsun->setSizePolicy(sizePolicy10);
        pushButtonKlasaUsun->setLayoutDirection(Qt::RightToLeft);

        gridLayoutClasses->addWidget(pushButtonKlasaUsun, 3, 6, 1, 1);

        comboBox_SourceType = new QComboBox(verticalLayoutModel);
        comboBox_SourceType->setObjectName(QStringLiteral("comboBox_SourceType"));

        gridLayoutClasses->addWidget(comboBox_SourceType, 4, 4, 1, 1);

        labelKlasyMu = new QLabel(verticalLayoutModel);
        labelKlasyMu->setObjectName(QStringLiteral("labelKlasyMu"));
        sizePolicy7.setHeightForWidth(labelKlasyMu->sizePolicy().hasHeightForWidth());
        labelKlasyMu->setSizePolicy(sizePolicy7);
        labelKlasyMu->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutClasses->addWidget(labelKlasyMu, 3, 2, 1, 1);

        labelClassesServiceExDx = new QLabel(verticalLayoutModel);
        labelClassesServiceExDx->setObjectName(QStringLiteral("labelClassesServiceExDx"));
        labelClassesServiceExDx->setEnabled(false);
        sizePolicy7.setHeightForWidth(labelClassesServiceExDx->sizePolicy().hasHeightForWidth());
        labelClassesServiceExDx->setSizePolicy(sizePolicy7);
        labelClassesServiceExDx->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutClasses->addWidget(labelClassesServiceExDx, 8, 5, 1, 1);

        labelClassesIncommingExDx = new QLabel(verticalLayoutModel);
        labelClassesIncommingExDx->setObjectName(QStringLiteral("labelClassesIncommingExDx"));
        labelClassesIncommingExDx->setEnabled(false);
        sizePolicy7.setHeightForWidth(labelClassesIncommingExDx->sizePolicy().hasHeightForWidth());
        labelClassesIncommingExDx->setSizePolicy(sizePolicy7);
        labelClassesIncommingExDx->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutClasses->addWidget(labelClassesIncommingExDx, 6, 5, 1, 1);

        label_NewCallStrType = new QLabel(verticalLayoutModel);
        label_NewCallStrType->setObjectName(QStringLiteral("label_NewCallStrType"));

        gridLayoutClasses->addWidget(label_NewCallStrType, 6, 2, 1, 2);

        label_CallServStreamType = new QLabel(verticalLayoutModel);
        label_CallServStreamType->setObjectName(QStringLiteral("label_CallServStreamType"));

        gridLayoutClasses->addWidget(label_CallServStreamType, 8, 2, 1, 2);

        comboBox_CallServStrType = new QComboBox(verticalLayoutModel);
        comboBox_CallServStrType->setObjectName(QStringLiteral("comboBox_CallServStrType"));

        gridLayoutClasses->addWidget(comboBox_CallServStrType, 8, 4, 1, 1);


        verticalLayout_5->addLayout(gridLayoutClasses);

        graphicsView = new QGraphicsView(verticalLayoutModel);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));

        verticalLayout_5->addWidget(graphicsView);


        verticalLayoutModel_2->addWidget(verticalLayoutModel);

        Pages->addTab(tabWidgetPage_SystemModel, QString());
        tabWidgetQoS = new QWidget();
        tabWidgetQoS->setObjectName(QStringLiteral("tabWidgetQoS"));
        verticalLayout = new QVBoxLayout(tabWidgetQoS);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayoutQoS_Algorithms = new QGridLayout();
        gridLayoutQoS_Algorithms->setSpacing(6);
        gridLayoutQoS_Algorithms->setObjectName(QStringLiteral("gridLayoutQoS_Algorithms"));
        gridLayoutQoS_Algorithms->setSizeConstraint(QLayout::SetMinimumSize);
        label_Amin = new QLabel(tabWidgetQoS);
        label_Amin->setObjectName(QStringLiteral("label_Amin"));
        QSizePolicy sizePolicy11(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
        sizePolicy11.setHorizontalStretch(0);
        sizePolicy11.setVerticalStretch(0);
        sizePolicy11.setHeightForWidth(label_Amin->sizePolicy().hasHeightForWidth());
        label_Amin->setSizePolicy(sizePolicy11);
        label_Amin->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutQoS_Algorithms->addWidget(label_Amin, 0, 3, 1, 1);

        listWidgetAlgorithmsAlternative = new QListWidget(tabWidgetQoS);
        listWidgetAlgorithmsAlternative->setObjectName(QStringLiteral("listWidgetAlgorithmsAlternative"));
        sizePolicy11.setHeightForWidth(listWidgetAlgorithmsAlternative->sizePolicy().hasHeightForWidth());
        listWidgetAlgorithmsAlternative->setSizePolicy(sizePolicy11);
        listWidgetAlgorithmsAlternative->setBaseSize(QSize(0, 0));
        listWidgetAlgorithmsAlternative->setSelectionMode(QAbstractItemView::MultiSelection);

        gridLayoutQoS_Algorithms->addWidget(listWidgetAlgorithmsAlternative, 1, 1, 3, 1);

        labelAlgorithmsAlternative = new QLabel(tabWidgetQoS);
        labelAlgorithmsAlternative->setObjectName(QStringLiteral("labelAlgorithmsAlternative"));
        QSizePolicy sizePolicy12(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
        sizePolicy12.setHorizontalStretch(1);
        sizePolicy12.setVerticalStretch(0);
        sizePolicy12.setHeightForWidth(labelAlgorithmsAlternative->sizePolicy().hasHeightForWidth());
        labelAlgorithmsAlternative->setSizePolicy(sizePolicy12);

        gridLayoutQoS_Algorithms->addWidget(labelAlgorithmsAlternative, 0, 1, 1, 1);

        doubleSpinBox_aMax = new QDoubleSpinBox(tabWidgetQoS);
        doubleSpinBox_aMax->setObjectName(QStringLiteral("doubleSpinBox_aMax"));
        sizePolicy11.setHeightForWidth(doubleSpinBox_aMax->sizePolicy().hasHeightForWidth());
        doubleSpinBox_aMax->setSizePolicy(sizePolicy11);
        doubleSpinBox_aMax->setDecimals(3);
        doubleSpinBox_aMax->setMaximum(15);
        doubleSpinBox_aMax->setSingleStep(0.1);
        doubleSpinBox_aMax->setValue(1.5);

        gridLayoutQoS_Algorithms->addWidget(doubleSpinBox_aMax, 1, 4, 1, 1);

        label = new QLabel(tabWidgetQoS);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy13(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
        sizePolicy13.setHorizontalStretch(2);
        sizePolicy13.setVerticalStretch(0);
        sizePolicy13.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy13);
        label->setBaseSize(QSize(300, 0));

        gridLayoutQoS_Algorithms->addWidget(label, 0, 2, 1, 1);

        label_Amax = new QLabel(tabWidgetQoS);
        label_Amax->setObjectName(QStringLiteral("label_Amax"));
        sizePolicy11.setHeightForWidth(label_Amax->sizePolicy().hasHeightForWidth());
        label_Amax->setSizePolicy(sizePolicy11);
        label_Amax->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutQoS_Algorithms->addWidget(label_Amax, 1, 3, 1, 1);

        labelAlgorithms = new QLabel(tabWidgetQoS);
        labelAlgorithms->setObjectName(QStringLiteral("labelAlgorithms"));
        sizePolicy13.setHeightForWidth(labelAlgorithms->sizePolicy().hasHeightForWidth());
        labelAlgorithms->setSizePolicy(sizePolicy13);
        labelAlgorithms->setBaseSize(QSize(300, 0));

        gridLayoutQoS_Algorithms->addWidget(labelAlgorithms, 0, 0, 1, 1);

        label_Adelta = new QLabel(tabWidgetQoS);
        label_Adelta->setObjectName(QStringLiteral("label_Adelta"));
        sizePolicy11.setHeightForWidth(label_Adelta->sizePolicy().hasHeightForWidth());
        label_Adelta->setSizePolicy(sizePolicy11);
        label_Adelta->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutQoS_Algorithms->addWidget(label_Adelta, 2, 3, 1, 1);

        doubleSpinBox_aDelta = new QDoubleSpinBox(tabWidgetQoS);
        doubleSpinBox_aDelta->setObjectName(QStringLiteral("doubleSpinBox_aDelta"));
        sizePolicy11.setHeightForWidth(doubleSpinBox_aDelta->sizePolicy().hasHeightForWidth());
        doubleSpinBox_aDelta->setSizePolicy(sizePolicy11);
        doubleSpinBox_aDelta->setDecimals(3);
        doubleSpinBox_aDelta->setMinimum(0.001);
        doubleSpinBox_aDelta->setMaximum(1);
        doubleSpinBox_aDelta->setSingleStep(0.05);
        doubleSpinBox_aDelta->setValue(0.1);

        gridLayoutQoS_Algorithms->addWidget(doubleSpinBox_aDelta, 2, 4, 1, 1);

        listWidgetAlgorithms = new QListWidget(tabWidgetQoS);
        listWidgetAlgorithms->setObjectName(QStringLiteral("listWidgetAlgorithms"));
        sizePolicy11.setHeightForWidth(listWidgetAlgorithms->sizePolicy().hasHeightForWidth());
        listWidgetAlgorithms->setSizePolicy(sizePolicy11);
        listWidgetAlgorithms->setBaseSize(QSize(0, 0));
        listWidgetAlgorithms->setSelectionMode(QAbstractItemView::MultiSelection);
        listWidgetAlgorithms->setSelectionBehavior(QAbstractItemView::SelectItems);
        listWidgetAlgorithms->setLayoutMode(QListView::SinglePass);

        gridLayoutQoS_Algorithms->addWidget(listWidgetAlgorithms, 1, 0, 3, 1);

        listWidgetSimulationParameters = new QListWidget(tabWidgetQoS);
        listWidgetSimulationParameters->setObjectName(QStringLiteral("listWidgetSimulationParameters"));
        sizePolicy11.setHeightForWidth(listWidgetSimulationParameters->sizePolicy().hasHeightForWidth());
        listWidgetSimulationParameters->setSizePolicy(sizePolicy11);
        listWidgetSimulationParameters->setBaseSize(QSize(300, 0));

        gridLayoutQoS_Algorithms->addWidget(listWidgetSimulationParameters, 1, 2, 3, 1);

        pushButtonStart = new QPushButton(tabWidgetQoS);
        pushButtonStart->setObjectName(QStringLiteral("pushButtonStart"));
        sizePolicy11.setHeightForWidth(pushButtonStart->sizePolicy().hasHeightForWidth());
        pushButtonStart->setSizePolicy(sizePolicy11);

        gridLayoutQoS_Algorithms->addWidget(pushButtonStart, 3, 3, 1, 2);

        doubleSpinBox_aMin = new QDoubleSpinBox(tabWidgetQoS);
        doubleSpinBox_aMin->setObjectName(QStringLiteral("doubleSpinBox_aMin"));
        sizePolicy11.setHeightForWidth(doubleSpinBox_aMin->sizePolicy().hasHeightForWidth());
        doubleSpinBox_aMin->setSizePolicy(sizePolicy11);
        doubleSpinBox_aMin->setDecimals(3);
        doubleSpinBox_aMin->setMaximum(10);
        doubleSpinBox_aMin->setSingleStep(0.1);
        doubleSpinBox_aMin->setValue(0.5);

        gridLayoutQoS_Algorithms->addWidget(doubleSpinBox_aMin, 0, 4, 1, 1);


        verticalLayout->addLayout(gridLayoutQoS_Algorithms);

        gridLayoutCharts = new QGridLayout();
        gridLayoutCharts->setSpacing(6);
        gridLayoutCharts->setObjectName(QStringLiteral("gridLayoutCharts"));
        gridLayoutCharts->setSizeConstraint(QLayout::SetMaximumSize);
        checkBoxResultsQtShowKey = new QCheckBox(tabWidgetQoS);
        checkBoxResultsQtShowKey->setObjectName(QStringLiteral("checkBoxResultsQtShowKey"));
        sizePolicy11.setHeightForWidth(checkBoxResultsQtShowKey->sizePolicy().hasHeightForWidth());
        checkBoxResultsQtShowKey->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(checkBoxResultsQtShowKey, 23, 0, 1, 3);

        listWidgetResultsQtAdditionalParameters1 = new QListWidget(tabWidgetQoS);
        listWidgetResultsQtAdditionalParameters1->setObjectName(QStringLiteral("listWidgetResultsQtAdditionalParameters1"));
        sizePolicy11.setHeightForWidth(listWidgetResultsQtAdditionalParameters1->sizePolicy().hasHeightForWidth());
        listWidgetResultsQtAdditionalParameters1->setSizePolicy(sizePolicy11);
        listWidgetResultsQtAdditionalParameters1->setSelectionMode(QAbstractItemView::MultiSelection);

        gridLayoutCharts->addWidget(listWidgetResultsQtAdditionalParameters1, 22, 3, 3, 1);

        listWidgetResultsQtAdditionalParameters2 = new QListWidget(tabWidgetQoS);
        listWidgetResultsQtAdditionalParameters2->setObjectName(QStringLiteral("listWidgetResultsQtAdditionalParameters2"));
        listWidgetResultsQtAdditionalParameters2->setEnabled(true);
        sizePolicy11.setHeightForWidth(listWidgetResultsQtAdditionalParameters2->sizePolicy().hasHeightForWidth());
        listWidgetResultsQtAdditionalParameters2->setSizePolicy(sizePolicy11);
        listWidgetResultsQtAdditionalParameters2->setMinimumSize(QSize(0, 0));
        listWidgetResultsQtAdditionalParameters2->setSelectionMode(QAbstractItemView::MultiSelection);

        gridLayoutCharts->addWidget(listWidgetResultsQtAdditionalParameters2, 22, 4, 3, 1);

        labelResultsQtAdditionalParameters2 = new QLabel(tabWidgetQoS);
        labelResultsQtAdditionalParameters2->setObjectName(QStringLiteral("labelResultsQtAdditionalParameters2"));
        sizePolicy11.setHeightForWidth(labelResultsQtAdditionalParameters2->sizePolicy().hasHeightForWidth());
        labelResultsQtAdditionalParameters2->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(labelResultsQtAdditionalParameters2, 20, 4, 1, 1);

        labelResultsQtAdditionalParameters1 = new QLabel(tabWidgetQoS);
        labelResultsQtAdditionalParameters1->setObjectName(QStringLiteral("labelResultsQtAdditionalParameters1"));
        sizePolicy11.setHeightForWidth(labelResultsQtAdditionalParameters1->sizePolicy().hasHeightForWidth());
        labelResultsQtAdditionalParameters1->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(labelResultsQtAdditionalParameters1, 20, 3, 1, 1);

        line = new QFrame(tabWidgetQoS);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayoutCharts->addWidget(line, 17, 0, 1, 5);

        widgetResultsPlot = new QChartView(tabWidgetQoS);
        widgetResultsPlot->setObjectName(QStringLiteral("widgetResultsPlot"));
        sizePolicy2.setHeightForWidth(widgetResultsPlot->sizePolicy().hasHeightForWidth());
        widgetResultsPlot->setSizePolicy(sizePolicy2);
        widgetResultsPlot->setMinimumSize(QSize(4, 0));

        gridLayoutCharts->addWidget(widgetResultsPlot, 3, 0, 3, 5);

        labelResultsQtType = new QLabel(tabWidgetQoS);
        labelResultsQtType->setObjectName(QStringLiteral("labelResultsQtType"));
        sizePolicy11.setHeightForWidth(labelResultsQtType->sizePolicy().hasHeightForWidth());
        labelResultsQtType->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(labelResultsQtType, 20, 0, 1, 1);

        labelResultsQtX_axis = new QLabel(tabWidgetQoS);
        labelResultsQtX_axis->setObjectName(QStringLiteral("labelResultsQtX_axis"));
        sizePolicy11.setHeightForWidth(labelResultsQtX_axis->sizePolicy().hasHeightForWidth());
        labelResultsQtX_axis->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(labelResultsQtX_axis, 22, 0, 1, 1);

        checkBoxResultsQtLogScaleOnAxisY = new QCheckBox(tabWidgetQoS);
        checkBoxResultsQtLogScaleOnAxisY->setObjectName(QStringLiteral("checkBoxResultsQtLogScaleOnAxisY"));
        sizePolicy11.setHeightForWidth(checkBoxResultsQtLogScaleOnAxisY->sizePolicy().hasHeightForWidth());
        checkBoxResultsQtLogScaleOnAxisY->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(checkBoxResultsQtLogScaleOnAxisY, 24, 0, 1, 3);

        comboBoxResultsQtType = new QComboBox(tabWidgetQoS);
        comboBoxResultsQtType->setObjectName(QStringLiteral("comboBoxResultsQtType"));
        sizePolicy11.setHeightForWidth(comboBoxResultsQtType->sizePolicy().hasHeightForWidth());
        comboBoxResultsQtType->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(comboBoxResultsQtType, 20, 2, 1, 1);

        comboBoxResultsQtX_axis = new QComboBox(tabWidgetQoS);
        comboBoxResultsQtX_axis->setObjectName(QStringLiteral("comboBoxResultsQtX_axis"));
        sizePolicy11.setHeightForWidth(comboBoxResultsQtX_axis->sizePolicy().hasHeightForWidth());
        comboBoxResultsQtX_axis->setSizePolicy(sizePolicy11);

        gridLayoutCharts->addWidget(comboBoxResultsQtX_axis, 22, 2, 1, 1);


        verticalLayout->addLayout(gridLayoutCharts);

        progressBar = new QProgressBar(tabWidgetQoS);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        sizePolicy11.setHeightForWidth(progressBar->sizePolicy().hasHeightForWidth());
        progressBar->setSizePolicy(sizePolicy11);
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);

        Pages->addTab(tabWidgetQoS, QString());

        horizontalLayout->addWidget(Pages);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 735, 28));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuSave_results_to_gnuplot = new QMenu(menuFile);
        menuSave_results_to_gnuplot->setObjectName(QStringLiteral("menuSave_results_to_gnuplot"));
        menuZapisz_do_arkusza_XSLX = new QMenu(menuFile);
        menuZapisz_do_arkusza_XSLX->setObjectName(QStringLiteral("menuZapisz_do_arkusza_XSLX"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuLanguage = new QMenu(menuBar);
        menuLanguage->setObjectName(QStringLiteral("menuLanguage"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuLanguage->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionWczytaj_system);
        menuFile->addAction(actionZapisz_system);
        menuFile->addSeparator();
        menuFile->addAction(menuSave_results_to_gnuplot->menuAction());
        menuFile->addAction(menuZapisz_do_arkusza_XSLX->menuAction());
        menuSave_results_to_gnuplot->addAction(actionGnuplotBlocking_probability);
        menuSave_results_to_gnuplot->addAction(actionGnuplotLoss_probability);
        menuSave_results_to_gnuplot->addAction(actionGnuplotAvarage_qeue_length);
        menuSave_results_to_gnuplot->addAction(actionGnuplotYtInQeue);
        menuSave_results_to_gnuplot->addSeparator();
        menuSave_results_to_gnuplot->addAction(actionGnuplotAvarage_number_of_calls_in_qeue);
        menuSave_results_to_gnuplot->addAction(actionGnuplotAbsoluteWaitingTime);
        menuSave_results_to_gnuplot->addAction(actionGnuplotEffectiveWaitingTime);
        menuZapisz_do_arkusza_XSLX->addAction(actionSaveXSLX_QoSforClasses);
        menuZapisz_do_arkusza_XSLX->addAction(actionSaveXLSX_subroupAvailability);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);
        QObject::connect(actionGnuplotBlocking_probability, SIGNAL(triggered()), MainWindow, SLOT(saveResultsGnuplotE()));
        QObject::connect(actionGnuplotLoss_probability, SIGNAL(triggered()), MainWindow, SLOT(saveResultsGnuplotLoss()));
        QObject::connect(actionGnuplotAvarage_qeue_length, SIGNAL(triggered()), MainWindow, SLOT(saveResultsGnuplotAvgQeueLen()));
        QObject::connect(actionGnuplotAvarage_number_of_calls_in_qeue, SIGNAL(triggered()), MainWindow, SLOT(saveResultsGnuplotAvnNbrCallsInQeue()));
        QObject::connect(actionGnuplotEffectiveWaitingTime, SIGNAL(triggered()), MainWindow, SLOT(saveResultsGnuplotEffectiveWaitingTime()));
        QObject::connect(actionGnuplotAbsoluteWaitingTime, SIGNAL(triggered()), MainWindow, SLOT(saveResultsGnuplotAbsoluteWaitingTime()));
        QObject::connect(spinBoxClassesSourcess, SIGNAL(valueChanged(int)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(doubleSpinBoxServiceExDx, SIGNAL(valueChanged(double)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(doubleSpinBoxIncomingExDx, SIGNAL(valueChanged(double)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(spinBoxClassesT, SIGNAL(valueChanged(int)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(comboBox_NewCallStrType, SIGNAL(currentIndexChanged(int)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(spinBoxKolejkaLiczba, SIGNAL(valueChanged(int)), MainWindow, SLOT(queueUiParChanged()));
        QObject::connect(comboBox_SourceType, SIGNAL(currentIndexChanged(int)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(checkBoxResultsQtShowKey, SIGNAL(toggled(bool)), MainWindow, SLOT(on_ResultsQtChartRefresh()));
        QObject::connect(checkBoxResultsQtLogScaleOnAxisY, SIGNAL(toggled(bool)), MainWindow, SLOT(on_ResultsQtChartRefresh()));
        QObject::connect(spinBoxPodgrupaPojemnosc, SIGNAL(valueChanged(int)), MainWindow, SLOT(groupUiParChanged()));
        QObject::connect(spinBoxPodgrupaLiczba, SIGNAL(valueChanged(int)), MainWindow, SLOT(groupUiParChanged()));
        QObject::connect(spinBoxKolejkaPojemnosc, SIGNAL(valueChanged(int)), MainWindow, SLOT(queueUiParChanged()));
        QObject::connect(comboBox_CallServStrType, SIGNAL(currentIndexChanged(int)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(doubleSpinBoxClassesMu, SIGNAL(valueChanged(double)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(spinBoxClassesAT, SIGNAL(valueChanged(int)), MainWindow, SLOT(classUiParChanged()));
        QObject::connect(listWidgetResultsQtAdditionalParameters2, SIGNAL(pressed(QModelIndex)), MainWindow, SLOT(on_ResultsQtChartRefresh()));
        QObject::connect(listWidgetResultsQtAdditionalParameters1, SIGNAL(itemSelectionChanged()), MainWindow, SLOT(on_ResultsQtChartRefresh()));
        QObject::connect(listWidgetAlgorithms, SIGNAL(itemSelectionChanged()), MainWindow, SLOT(on_ResultsQtChartRefresh()));
        QObject::connect(listWidgetAlgorithmsAlternative, SIGNAL(itemSelectionChanged()), MainWindow, SLOT(on_ResultsQtChartRefresh()));
        QObject::connect(listWidgetSimulationParameters, SIGNAL(itemSelectionChanged()), MainWindow, SLOT(on_ResultsQtChartRefresh()));

        Pages->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Queue Analyzer", nullptr));
        actionZapisz_system->setText(QApplication::translate("MainWindow", "Zapisz system", nullptr));
        actionWczytaj_system->setText(QApplication::translate("MainWindow", "Wczytaj system", nullptr));
        actionSaveToSpreadsheetBlProb->setText(QApplication::translate("MainWindow", "Prawdopodobie\305\204stwo blokady", nullptr));
        actionSaveToSpreadsheetLossProb->setText(QApplication::translate("MainWindow", "Prawdopodobie\305\204stwo strat", nullptr));
        actionSaveToSpreadsheetAll->setText(QApplication::translate("MainWindow", "Wszystko", nullptr));
        actionSaveToSpreadsheetAvgQeue->setText(QApplication::translate("MainWindow", "\305\232rednia d\305\202ugo\305\233\304\207 kolejki", nullptr));
        actionSaveToSpreadsheetTime->setText(QApplication::translate("MainWindow", "Czasy", nullptr));
        actionSaveToSpreadsheeTrDistrib->setText(QApplication::translate("MainWindow", "Rozk\305\202ady zaj\304\231to\305\233ci n", nullptr));
        actionSaveToSpreadsheetAUs->setText(QApplication::translate("MainWindow", "Zaj\304\231te zasoby w stanie n", nullptr));
        actionSaveToSpreadsheeTrDistribNQ->setText(QApplication::translate("MainWindow", "Rozk\305\202ad zaj\304\231to\305\233ci n q", nullptr));
        actionGnuplotBlocking_probability->setText(QApplication::translate("MainWindow", "Blocking probability", nullptr));
        actionGnuplotLoss_probability->setText(QApplication::translate("MainWindow", "Loss probability", nullptr));
        actionGnuplotAvarage_qeue_length->setText(QApplication::translate("MainWindow", "Avarage qeue length", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        actionGnuplotAvarage_number_of_calls_in_qeue->setText(QApplication::translate("MainWindow", "Avarage number of calls in qeue", nullptr));
        actionGnuplotEffectiveWaitingTime->setText(QApplication::translate("MainWindow", "Effective waiting time", nullptr));
        actionGnuplotAbsoluteWaitingTime->setText(QApplication::translate("MainWindow", "Absolute waiting time", nullptr));
        actionGnuplotYtInQeue->setText(QApplication::translate("MainWindow", "Number of resourcess of qeue occupied by calls", nullptr));
        actionZapiszZajetoscPpodgrup->setText(QApplication::translate("MainWindow", "Zaj\304\231to\305\233\304\207 podgrup", nullptr));
#ifndef QT_NO_TOOLTIP
        actionZapiszZajetoscPpodgrup->setToolTip(QApplication::translate("MainWindow", "Zapisz Zaj\304\231to\305\233\304\207 podgrup", nullptr));
#endif // QT_NO_TOOLTIP
        actionSaveXSLX_QoSforClasses->setText(QApplication::translate("MainWindow", "QoS dla klas", nullptr));
        actionSaveXLSX_subroupAvailability->setText(QApplication::translate("MainWindow", "Zaj\304\231to\305\233\304\207 podgrup", nullptr));
#ifndef QT_NO_TOOLTIP
        tabWidgetPage_SystemModel->setToolTip(QApplication::translate("MainWindow", "Model systemu", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tabWidgetPage_SystemModel->setStatusTip(QApplication::translate("MainWindow", "Model systemu", nullptr));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        tabWidgetPage_SystemModel->setWhatsThis(QApplication::translate("MainWindow", "Model systemu", nullptr));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_ACCESSIBILITY
        tabWidgetPage_SystemModel->setAccessibleName(QApplication::translate("MainWindow", "Model Systemu", nullptr));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        tabWidgetPage_SystemModel->setAccessibleDescription(QApplication::translate("MainWindow", "Model Systemu", nullptr));
#endif // QT_NO_ACCESSIBILITY
        pushButtonPredSystemsAdd->setText(QApplication::translate("MainWindow", "Add", nullptr));
        labelModelServer->setText(QApplication::translate("MainWindow", "Servers", nullptr));
        labelModelQeueNumber->setText(QApplication::translate("MainWindow", "No of subgroups", nullptr));
        labelModelQeueCapacity->setText(QApplication::translate("MainWindow", "Subgroup capacity", nullptr));
        pushButton_kolejkidodaj->setText(QApplication::translate("MainWindow", "Add", nullptr));
        pushButton_kolejkiZmien->setText(QApplication::translate("MainWindow", "Change", nullptr));
        pushButton_kolejkiUsun->setText(QApplication::translate("MainWindow", "Remove", nullptr));
        label_subbufferScheduler->setText(QApplication::translate("MainWindow", "Subbuffer scheduler", nullptr));
        labelModelQeue->setText(QApplication::translate("MainWindow", "Buffers", nullptr));
        labelPodgrupaPojemnosc->setText(QApplication::translate("MainWindow", "Subgroup capacity", nullptr));
        labelPodgrupaLiczba->setText(QApplication::translate("MainWindow", "No of subgroups", nullptr));
        pushButton_wiazkiDodaj->setText(QApplication::translate("MainWindow", "Add", nullptr));
        pushButton_wiazkiZmien->setText(QApplication::translate("MainWindow", "Change", nullptr));
        pushButton_wiazkiUsun->setText(QApplication::translate("MainWindow", "Remove", nullptr));
        label_subgroupSchedulerAlgorithm->setText(QApplication::translate("MainWindow", "Subgroup scheduler", nullptr));
        labelClasses->setText(QApplication::translate("MainWindow", "Trafic classes", nullptr));
        labelKlasyPropAT->setText(QApplication::translate("MainWindow", "at", nullptr));
        labelKlasyT->setText(QApplication::translate("MainWindow", "t", nullptr));
        label_trSrcType->setText(QApplication::translate("MainWindow", "<html><head/><body><p>Class source type</p></body></html>", nullptr));
        pushButtonKlasaDodaj->setText(QApplication::translate("MainWindow", "Add", nullptr));
        pushButtonKlasaZmien->setText(QApplication::translate("MainWindow", "Change", nullptr));
        labelClassesNumberOfSourcess->setText(QApplication::translate("MainWindow", "Number of sourcess", nullptr));
        pushButtonKlasaUsun->setText(QApplication::translate("MainWindow", "Remove", nullptr));
        labelKlasyMu->setText(QApplication::translate("MainWindow", "\302\265", nullptr));
        labelClassesServiceExDx->setText(QApplication::translate("MainWindow", "Service E\302\262/\303\260\302\262", nullptr));
        labelClassesIncommingExDx->setText(QApplication::translate("MainWindow", "Incomming E\302\262/\303\260\302\262", nullptr));
        label_NewCallStrType->setText(QApplication::translate("MainWindow", "<html><head/><body><p>New call stram type</p></body></html>", nullptr));
        label_CallServStreamType->setText(QApplication::translate("MainWindow", "Service stream type", nullptr));
        Pages->setTabText(Pages->indexOf(tabWidgetPage_SystemModel), QApplication::translate("MainWindow", "Model of the system", nullptr));
        label_Amin->setText(QApplication::translate("MainWindow", "<html><head/><body><p>a<span style=\" vertical-align:sub;\">min</span></p></body></html>", nullptr));
        labelAlgorithmsAlternative->setText(QApplication::translate("MainWindow", "Alternative algorithms", nullptr));
        label->setText(QApplication::translate("MainWindow", "Simulation's exp param", nullptr));
        label_Amax->setText(QApplication::translate("MainWindow", "<html><head/><body><p>a<span style=\" vertical-align:sub;\">max</span></p></body></html>", nullptr));
        labelAlgorithms->setText(QApplication::translate("MainWindow", "Working algorithms", nullptr));
        label_Adelta->setText(QApplication::translate("MainWindow", "<html><head/><body><p>\303\260 a</p></body></html>", nullptr));
        pushButtonStart->setText(QApplication::translate("MainWindow", "Start", nullptr));
        checkBoxResultsQtShowKey->setText(QApplication::translate("MainWindow", "Show key", nullptr));
        labelResultsQtAdditionalParameters2->setText(QApplication::translate("MainWindow", "Trafic classes", nullptr));
        labelResultsQtAdditionalParameters1->setText(QApplication::translate("MainWindow", "Additional parameters", nullptr));
        labelResultsQtType->setText(QApplication::translate("MainWindow", "Result type", nullptr));
        labelResultsQtX_axis->setText(QApplication::translate("MainWindow", "X axis", nullptr));
        checkBoxResultsQtLogScaleOnAxisY->setText(QApplication::translate("MainWindow", "Log scale on axis Y", nullptr));
        Pages->setTabText(Pages->indexOf(tabWidgetQoS), QApplication::translate("MainWindow", "QoS", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuSave_results_to_gnuplot->setTitle(QApplication::translate("MainWindow", "Save results to gnuplot", nullptr));
        menuZapisz_do_arkusza_XSLX->setTitle(QApplication::translate("MainWindow", "Zapisz do arkusza XSLX", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
        menuLanguage->setTitle(QApplication::translate("MainWindow", "Language", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
