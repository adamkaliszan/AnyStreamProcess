#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "model.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QtCharts>
#include <QVariant>
#include "algorithms/algorithmConv.h"
#include "algorithms/alg_cFIFO_convGamma.h"
#include "algorithms/alg_noFIFO.h"
#include "algorithms/alg_FAG_AnyStream.h"
#include "algorithms/algorithmConvGamma2d.h"
#include "algorithms/algorithmConvGamma3.h"
#include "algorithms/algorithmConv3d.h"
#include "algorithms/alg_cFIFO_hybrid.h"
#include "algorithms/algorithmHybridDiscr.h"
#include "algorithms/algorithmHybridDiscrDesc.h"
#include "algorithms/simulatorQeueFIFO.h"
#include "algorithms/simulatorBufferFIFO.h"
#include "algorithms/simulatorQeueSdFIFO.h"
#include "algorithms/simulatorNoQeue.h"
#include "algorithms/simulatorAllSystems.h"
#include "algorithms/simulatorNoQeueLag.h"
#include "algorithms/simulationParameters.h"
#include "algorithms/algRekLag.h"
#include "algorithms/algRekLagGS.h"

#include "results/resultsGnuplot.h"
#include "results/resultsInvestigator.h"
#include "results/resultsSystem.h"
#include "results/resultsApi.h"


using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    sceneSysModel = new QGraphicsScene(this);

    isDbWorking   = false;
    dlgAbout      = new DialogAbout();
    dlgConfig     = new DialogConfig();
    system        = new ModelSyst();
    scrGnuplot    = new GnuplotScript();

    resultsForSystem = new Results::RSystem(*system);

    ui->setupUi(this);

    ui->graphicsView->setScene(sceneSysModel);

    addAlgorithmsAndParams();

    createLanguageMenu();
    QString locale = QLocale::system().name();
    locale.truncate(locale.lastIndexOf('_'));
    loadLanguage(locale);
    updateAlgorithmsList();

    QChart *qtChart = new QChart();
    axisX           = new QtCharts::QValueAxis();
    axisYlog        = new QtCharts::QLogValueAxis();
    axisYlinear     = new QtCharts::QValueAxis();

    qtChart->addAxis(axisYlog,    Qt::AlignLeft);
    qtChart->addAxis(axisYlinear, Qt::AlignLeft);
    qtChart->addAxis(axisX,       Qt::AlignBottom);

    axisX->setLinePenColor(QColor::fromRgb(255, 0, 0));
    axisYlog->setLinePenColor(QColor::fromRgb(255, 0, 0));

    ui->widgetResultsPlot->setChart(qtChart);
    ui->widgetResultsPlot->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow()
{
    dlgConfig->close();
    delete dlgConfig;
    dlgAbout->close();
    delete dlgAbout;

    foreach (QListWidgetItem *item, ui->listWidgetAlgorithms->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard)+ui->listWidgetAlgorithmsAlternative->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        Investigator *alg = item->data(Qt::UserRole).value<Investigator *>();
        delete alg;
    }

    foreach (QListWidgetItem *item, ui->listWidgetWiazki->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard) + ui->listWidgetKolejki->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelResourcess *rsc = item->data(Qt::UserRole).value<ModelResourcess *>();
        delete rsc;
    }

    foreach (QListWidgetItem *item, ui->listWidgetKlasy->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelTrClass *rsc = item->data(Qt::UserRole).value<ModelTrClass *>();
        delete rsc;
    }

    foreach (QListWidgetItem *item, ui->listWidgetSimulationParameters->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        SimulationParameters *simPar = item->data(Qt::UserRole).value<SimulationParameters *>();
        delete simPar;
    }

    delete scrGnuplot;
    delete system;
    delete ui;
}

void MainWindow::addServer(int k, int v)
{
    QVariant variant;
    variant.setValue(new ModelResourcess(k, v));

    QListWidgetItem *newItem = new QListWidgetItem();

    QString newItemStr;
    newItem->setData(Qt::DisplayRole, newItemStr.sprintf("k=%d, v=%d", k, v));
    newItem->setData(Qt::UserRole, variant);
    ui->listWidgetWiazki->addItem(newItem);

    drawSystemModel();
}

void MainWindow::addBuffer(int k, int v)
{
    QVariant variant;
    variant.setValue(new ModelResourcess(k, v));

    QListWidgetItem *newItem = new QListWidgetItem();

    QString newItemStr;
    newItem->setData(Qt::DisplayRole, newItemStr.sprintf("k=%d, v=%d", k, v));
    newItem->setData(Qt::UserRole, variant);
    ui->listWidgetKolejki->addItem(newItem);

    drawSystemModel();
}

void MainWindow::addErlangTrClass(int t, int at, double mu)
{
    ModelTrClass *trClass = new ModelTrClass();
    trClass->setPropAt(at);
    trClass->setT(t);
    trClass->setMu(mu);
    trClass->setNewCallStrType(ModelTrClass::StreamType::Poisson, ModelTrClass::SourceType::Independent);
    trClass->setCallServStrType(ModelTrClass::StreamType::Poisson);
    QString classDescription = trClass->shortName();

    QListWidgetItem *newClassItem = new QListWidgetItem();
    QVariant variant;
    variant.setValue(trClass);

    newClassItem->setData(Qt::DisplayRole, classDescription);
    newClassItem->setData(Qt::UserRole, variant);
    ui->listWidgetKlasy->addItem(newClassItem);

    updateClassWidgets();
    updateAlgorithmsList();
}

void MainWindow::setMin_a(double value)
{
    ui->doubleSpinBox_aMin->setValue(value);
}

void MainWindow::setMax_a(double value)
{
    ui->doubleSpinBox_aMax->setValue(value);
}


void MainWindow::addAlgorithmForCurentSystem(Investigator *newAlg)
{
    QVariant variant;
    variant.setValue(newAlg);

    QListWidgetItem *newListItem = new QListWidgetItem();
    newListItem->setData(Qt::DisplayRole, variant.value<Investigator *>()->shortName());
    newListItem->setData(Qt::UserRole, variant);
    ui->listWidgetAlgorithms->addItem(newListItem);
}

void MainWindow::addAlternativeAlgorithmForCurentSystem(Investigator *newAlg)
{
    QVariant variant;
    variant.setValue(newAlg);

    QListWidgetItem *newListItem = new QListWidgetItem();

    newListItem->setData(Qt::DisplayRole, variant.value<Investigator *>()->shortName());
    newListItem->setData(Qt::UserRole, variant);

    ui->listWidgetAlgorithmsAlternative->addItem(newListItem);
}

void MainWindow::addSimulationParams(SimulationParameters *par)
{
    QVariant variant;
    variant.setValue(par);

    QListWidgetItem *newListItem = new QListWidgetItem();
    simulationParameters.append(par);

    newListItem->setData(Qt::DisplayRole, variant.value<SimulationParameters *>()->showDescription());
    newListItem->setData(Qt::UserRole, variant);

    ui->listWidgetSimulationParameters->addItem(newListItem);
}


void MainWindow::addTestedAlgorithm(Investigator *algorithm)
{
    algorithms.append(algorithm);
    //QObject::connect(algorithm, SIGNAL(sigCalculationDone()), this, SLOT(on_ResultsQtChartRefresh()));
}

void MainWindow::addExperimentalAlgorithm(Investigator *algorithm)
{
    algorithms.append(algorithm);
    //QObject::connect(algorithm, SIGNAL(sigCalculationDone()), this, SLOT(on_ResultsQtChartRefresh()));
}


void MainWindow::addAlgorithmsAndParams()
{
    //addExperimentalAlgorithm(new convolutionAlgorithm1());
    //addExperimentalAlgorithm(new convolutionAlgorithm2());
    //addExperimentalAlgorithm(new convolutionAlgorithmGamma3());
    //addExperimentalAlgorithm(new convolutionAlgorithm3d_v1());
    //addExperimentalAlgorithm(new Algorithm2Pass());
    //addExperimentalAlgorithm(new SimulatorBufferFifo());
    //addExperimentalAlgorithm(new AlgorithmHybridDiscr());

    algorithms.append(new Algorithms::AlgorithmHybridNoFifo());
//    algorithms.append(new AlgorithmAnyStreamFAG());
//    algorithms.append(new convolutionAlgorithmGamma());
//    algorithms.append(new AlgorithmHybrid(AlgorithmHybrid::algVariant::yProp));
//    algorithms.append(new AlgorithmHybrid(AlgorithmHybrid::algVariant::yPropPlus));
//    algorithms.append(new AlgorithmHybrid(AlgorithmHybrid::algVariant::yAprox));
//    algorithms.append(new AlgorithmHybridDiscrDesc());

//    addTestedAlgorithm(new Algorithms::SimulatorQeueFifo());
//    addTestedAlgorithm(new Algorithms::SimulatorQeueSdFifo());
//    addTestedAlgorithm(new Algorithms::simulatorNoQeue());
//    addTestedAlgorithm(new Algorithms::simulatorNoQeueLag());
    addTestedAlgorithm(new Algorithms::SimulatorAll());
    addTestedAlgorithm(new Algorithms::algRekLagGS());
    addTestedAlgorithm(new Algorithms::algRekLagGS2());
    addTestedAlgorithm(new Algorithms::algRekLAG());

    addTestedAlgorithm(new Algorithms::AlgorithmHybrid(Algorithms::AlgorithmHybrid::AlgVariant::yFAG));

/*
    addExperimentalAlgorithm(new convolutionAlgorithm1());
    addExperimentalAlgorithm(new convolutionAlgorithm2());
    addExperimentalAlgorithm(new convolutionAlgorithmGamma3());
    //addExperimentalAlgorithm(new convolutionAlgorithm3d_v1());
    //addExperimentalAlgorithm(new Algorithm2Pass());
    //addExperimentalAlgorithm(new SimulatorBufferFifo());
    addExperimentalAlgorithm(new AlgorithmHybridDiscr());

    addAlgorithm(new AlgorithmHybridNoFifo());
    addAlgorithm(new convolutionAlgorithmGamma());
    //addAlgorithm(new AlgorithmHybrid());
    addAlgorithm(new AlgorithmHybrid(AlgorithmHybrid::algVariant::yProp));
    addAlgorithm(new AlgorithmHybrid(AlgorithmHybrid::algVariant::yPropPlus));
    //addAlgorithm(new AlgorithmHybrid(AlgorithmHybrid::algVariant::yAprox));
    addAlgorithm(new AlgorithmHybridDiscrDesc());
    addAlgorithm(new SimulatorQeueFifo(queueServDiscipline::cFIFO));
    addAlgorithm(new SimulatorQeueFifo(queueServDiscipline::dFIFO));
    addAlgorithm(new SimulatorQeueFifo(queueServDiscipline::qFIFO));
    addAlgorithm(new SimulatorQeueSdFifo());
*/
    //foreach (Investigator *tmpAlg, algorithms)
    //{
        //connect(tmpAlg, tmpAlg->si)
    //}

    addSimulationParams(new SimulationParameters(3,  0,       100, 10, 10000));
    addSimulationParams(new SimulationParameters(3,  1000,    100, 10, 0));
    addSimulationParams(new SimulationParameters(10, 1000,    100, 10, 1000000));
    addSimulationParams(new SimulationParameters(10, 10000,   100, 10, 10000000));

    currentSimulationParameter = simulationParameters[0];

    ui->listWidgetSimulationParameters->item(0)->setSelected(true);
    QVariant selVariant = ui->listWidgetSimulationParameters->selectedItems()[0]->data(Qt::UserRole);
    //resultsDeprecated->simParameters = selVariant.value<simulationParameters *>();
}

void MainWindow::on_pushButton_wiazkiDodaj_clicked()
{
    int k = static_cast<int>(ui->spinBoxPodgrupaLiczba->value());
    int v = static_cast<int>(ui->spinBoxPodgrupaPojemnosc->value());

    addServer(k, v);
    fillSystem();
}

void MainWindow::on_pushButton_wiazkiUsun_clicked()
{
    if (ui->listWidgetWiazki->selectedItems().length() > 0)
    {
        QListWidgetItem *toDelete = ui->listWidgetWiazki->selectedItems()[0];
        ui->listWidgetWiazki->removeItemWidget(toDelete);
        delete toDelete;
    }
    fillSystem();
    updateGroupWidgets();
}

void MainWindow::on_pushButton_wiazkiZmien_clicked()
{
    int k = ui->spinBoxPodgrupaLiczba->value();
    int v = ui->spinBoxPodgrupaPojemnosc->value();

    for (int idx=0; idx<ui->listWidgetWiazki->selectedItems().length(); idx++)
    {
        QListWidgetItem *toModify = ui->listWidgetWiazki->selectedItems()[idx];
        ModelResourcess *group = toModify->data(Qt::UserRole).value<ModelResourcess*>();
        group->set_k(k);
        group->set_v(v);
        QString modifiedItemStr;
        toModify->setData(Qt::DisplayRole, modifiedItemStr.sprintf("k=%d, v=%d", k, v));
    }

    fillSystem();
    updateGroupWidgets();
}

void MainWindow::on_listWidgetWiazki_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == previous)
        return;

    if (current != nullptr)
    {
        ModelResourcess *group = current->data(Qt::UserRole).value<ModelResourcess*>();
        ui->spinBoxPodgrupaLiczba->   setValue(static_cast<int>(group->k()));
        ui->spinBoxPodgrupaPojemnosc->setValue(static_cast<int>(group->v()));
    }

    updateGroupWidgets();
}

void MainWindow::on_pushButton_kolejkidodaj_clicked()
{
    int k = static_cast<int>(ui->spinBoxKolejkaLiczba->value());
    int v = static_cast<int>(ui->spinBoxKolejkaPojemnosc->value());

    addBuffer(k, v);
    fillSystem();
    updateAlgorithmsList();
}


void MainWindow::on_pushButton_kolejkiUsun_clicked()
{
    if (ui->listWidgetKolejki->selectedItems().length() > 0)
    {
        QListWidgetItem *toDelete = ui->listWidgetKolejki->selectedItems()[0];
        ModelResourcess *queue = toDelete->data(Qt::UserRole).value<ModelResourcess*>();
        delete queue;
        ui->listWidgetKolejki->removeItemWidget(toDelete);
        delete toDelete;
    }
    fillSystem();
    updateAlgorithmsList();
}

void MainWindow::on_pushButton_kolejkiZmien_clicked()
{
    int k = ui->spinBoxKolejkaLiczba->value();
    int v = ui->spinBoxKolejkaPojemnosc->value();

    for (int idx=0; idx<ui->listWidgetKolejki->selectedItems().length(); idx++)
    {
        QListWidgetItem *toModify = ui->listWidgetKolejki->selectedItems()[idx];
        ModelResourcess *queue = toModify->data(Qt::UserRole).value<ModelResourcess*>();
        queue->set_k(k);
        queue->set_v(v);
        QString modifiedItemStr;
        toModify->setData(Qt::DisplayRole, modifiedItemStr.sprintf("k=%d, v=%d", k, v));
    }
    fillSystem();
    updateAlgorithmsList();
}

void MainWindow::on_listWidgetKolejki_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == previous)
        return;

    updateQeueWidgets();

    if (current != nullptr)
    {
        ModelResourcess *queue = current->data(Qt::UserRole).value<ModelResourcess*>();
        ui->spinBoxKolejkaLiczba->   setValue(static_cast<int>(queue->k()));
        ui->spinBoxKolejkaPojemnosc->setValue(static_cast<int>(queue->v()));
    }
}

QString MainWindow::configureTrClass(ModelTrClass *trClass)
{
    int propAt = static_cast<int>(ui->spinBoxClassesAT->value());
    int t      = static_cast<int>(ui->spinBoxClassesT->value());
    double mu   = ui->doubleSpinBoxClassesMu->value();

    trClass->setPropAt(propAt);
    trClass->setT(t);
    trClass->setMu(mu);

    ModelTrClass::StreamType newCallStr  = ui->comboBox_NewCallStrType->currentData().value<ModelTrClass::StreamType>();
    ModelTrClass::StreamType callServStr = ui->comboBox_CallServStrType->currentData().value<ModelTrClass::StreamType>();
    ModelTrClass::SourceType srcType     = ui->comboBox_SourceType->currentData().value<ModelTrClass::SourceType>();

    trClass->setNewCallStrType(newCallStr, srcType);
    trClass->setCallServStrType(callServStr);
    if (newCallStr == ModelTrClass::StreamType::Poisson)
        ;
    else
        trClass->setIncommingExPerDx(ui->doubleSpinBoxIncomingExDx->value());

    if (callServStr == ModelTrClass::StreamType::Poisson)
        ;
    else
        trClass->setServiceExPerDx(ui->doubleSpinBoxServiceExDx->value());

    if (srcType == ModelTrClass::SourceType::Independent)
        ;
    else
        trClass->setNoOfSourcess(ui->spinBoxClassesSourcess->value());

    return trClass->shortName();
}

void MainWindow::on_pushButtonKlasaDodaj_clicked()
{
    ModelTrClass *trClass = new ModelTrClass();

    QString classDescription = configureTrClass(trClass);

    QListWidgetItem *newClassItem = new QListWidgetItem();
    QVariant variant;
    variant.setValue(trClass);

    newClassItem->setData(Qt::DisplayRole, classDescription);
    newClassItem->setData(Qt::UserRole, variant);
    ui->listWidgetKlasy->addItem(newClassItem);

    fillSystem();
    updateClassWidgets();
    updateAlgorithmsList();
}

void MainWindow::on_pushButtonKlasaZmien_clicked()
{
    QListWidgetItem *tmpItem = ui->listWidgetKlasy->currentItem();
    if (tmpItem == nullptr)
    {
        QMessageBox::information(nullptr, "Not fully implemented GUI", "The class is not selected");
    }
    ModelTrClass *modClass = tmpItem->data(Qt::UserRole).value<ModelTrClass *>();

    QString classDescription = configureTrClass(modClass);
    tmpItem->setData(Qt::DisplayRole, classDescription);

    fillSystem();
    updateClassWidgets();
    updateAlgorithmsList();
}

void MainWindow::on_pushButtonKlasaUsun_clicked()
{
    if (ui->listWidgetKlasy->selectedItems().length() > 0)
    {
        QListWidgetItem *toDelete = ui->listWidgetKlasy->selectedItems()[0];
        ui->listWidgetKlasy->removeItemWidget(toDelete);
        delete toDelete;
    }

    fillSystem();
    updateClassWidgets();
    updateAlgorithmsList();
}

void MainWindow::NewTitle()
{
    QString title;
    QTextStream titleStr(&title);
    titleStr<<"Queue analyzer: ";
    titleStr<<*system;
    setWindowTitle(title);
}

void MainWindow::groupUiParChanged()
{
    updateGroupWidgets();
}

void MainWindow::queueUiParChanged()
{
    updateQeueWidgets();
}

void MainWindow::classUiParChanged()
{
    updateClassWidgets();
}

void MainWindow::updateGroupWidgets(void)
{
    QListWidgetItem *tmp = ui->listWidgetWiazki->currentItem();
    ui->pushButton_wiazkiUsun->setEnabled(tmp);

    if (tmp != nullptr)
    {
        ModelResourcess *group = tmp->data(Qt::UserRole).value<ModelResourcess*>();
        if (group->k() != static_cast<int>(ui->spinBoxPodgrupaLiczba->value())
                || group->v() != static_cast<int>(ui->spinBoxPodgrupaPojemnosc->value()))
            ui->pushButton_wiazkiZmien->setEnabled(true);
        else
            ui->pushButton_wiazkiZmien->setEnabled(false);
    }
    else
        ui->pushButton_wiazkiZmien->setEnabled(false);
}

void MainWindow::updateQeueWidgets(void)
{
    QListWidgetItem *tmp = ui->listWidgetKolejki->currentItem();
    ui->pushButton_kolejkiUsun->setEnabled(tmp);
    if (tmp != nullptr)
    {
        ModelResourcess *qeue = tmp->data(Qt::UserRole).value<ModelResourcess*>();
        if (qeue->k() != static_cast<int>(ui->spinBoxKolejkaLiczba->value())
                || qeue->v() != static_cast<int>(ui->spinBoxKolejkaPojemnosc->value()))
            ui->pushButton_kolejkiZmien->setEnabled(true);
        else
            ui->pushButton_kolejkiZmien->setEnabled(false);
    }
    else
        ui->pushButton_kolejkiZmien->setEnabled(false);
}

void MainWindow::updateClassWidgets(void)
{
    int propAt = static_cast<int>(ui->spinBoxClassesAT->value());
    int t      = static_cast<int>(ui->spinBoxClassesT->value());
    double mu   = ui->doubleSpinBoxClassesMu->value();

    QListWidgetItem *tmp = ui->listWidgetKlasy->currentItem();



    ui->pushButtonKlasaUsun->setEnabled(tmp);
    ui->pushButtonKlasaZmien->setEnabled(false);    

    if (tmp != nullptr)
    {
        ModelTrClass *trClass = tmp->data(Qt::UserRole).value<ModelTrClass*>();


        if (ui->comboBox_SourceType->currentData().value<ModelTrClass::SourceType>() != trClass->srcType())
            ui->pushButtonKlasaZmien->setEnabled(true);
        if (trClass->srcType() != ModelTrClass::SourceType::Independent)
            if (ui->spinBoxClassesSourcess->value() != trClass->s())
                ui->pushButtonKlasaZmien->setEnabled(true);

        if (ui->comboBox_NewCallStrType->currentData().value<ModelTrClass::StreamType>() != trClass->newCallStr())
            ui->pushButtonKlasaZmien->setEnabled(true);
        if ( !qFuzzyCompare(ui->doubleSpinBoxIncomingExDx->value(), trClass->getIncommingExPerDx()))
            ui->pushButtonKlasaZmien->setEnabled(true);


        if (ui->comboBox_CallServStrType->currentData().value<ModelTrClass::StreamType>() != trClass->callServStr())
            ui->pushButtonKlasaZmien->setEnabled(true);
        if ( !qFuzzyCompare(ui->doubleSpinBoxServiceExDx->value(), trClass->getServiceExPerDx()))
            ui->pushButtonKlasaZmien->setEnabled(true);

        if (trClass->propAt() != propAt)
            ui->pushButtonKlasaZmien->setEnabled(true);
        if (trClass->t() != t)
            ui->pushButtonKlasaZmien->setEnabled(true);
        if ( !qFuzzyCompare(trClass->getMu(), mu))
            ui->pushButtonKlasaZmien->setEnabled(true);
    }
/*    else
    {
        switch (ui->comboBox_SourceType->currentData().value<modelTrClass::sourceType>())
        {
        case modelTrClass::sourceType::independent:
            ui->spinBoxClassesSourcess->setEnabled(false);
            break;
        case modelTrClass::sourceType::dependentMinus:
        case modelTrClass::sourceType::dependentPlus:
            ui->spinBoxClassesSourcess->setEnabled(true);
            break;
        }

        switch (ui->comboBox_NewCallStrType->currentData().value<modelTrClass::streamType>())
        {
        case modelTrClass::streamType::poisson:
            ui->labelClassesIncommingExDx->setEnabled(false);
            ui->doubleSpinBoxIncomingExDx->setEnabled(false);
            break;
        case modelTrClass::streamType::uniform:
        case modelTrClass::streamType::normal:
        case modelTrClass::streamType::gamma:
            ui->labelClassesIncommingExDx->setEnabled(true);
            ui->doubleSpinBoxIncomingExDx->setEnabled(true);
            break;
        }

        switch (ui->comboBox_CallServStrType->currentData().value<modelTrClass::streamType>())
        {
        case modelTrClass::streamType::poisson:
            ui->labelClassesServiceExDx->setEnabled(false);
            ui->doubleSpinBoxServiceExDx->setEnabled(false);
            break;
        case modelTrClass::streamType::uniform:
        case modelTrClass::streamType::normal:
        case modelTrClass::streamType::gamma:
            ui->labelClassesServiceExDx->setEnabled(true);
            ui->doubleSpinBoxServiceExDx->setEnabled(true);
            break;
        }

    }
*/
    switch (ui->comboBox_SourceType->currentData().value<ModelTrClass::SourceType>())
    {
    case ModelTrClass::SourceType::Independent:
        ui->spinBoxClassesSourcess->setEnabled(false);
        break;
    case ModelTrClass::SourceType::DependentMinus:
    case ModelTrClass::SourceType::DependentPlus:
        ui->spinBoxClassesSourcess->setEnabled(true);
        break;
    }

    switch(ui->comboBox_NewCallStrType->currentData().value<ModelTrClass::StreamType>())
    {
    case ModelTrClass::StreamType::Poisson:
        ui->labelClassesIncommingExDx->setEnabled(false);
        ui->doubleSpinBoxIncomingExDx->setEnabled(false);
        break;
    case ModelTrClass::StreamType::Uniform:
    case ModelTrClass::StreamType::Normal:
    case ModelTrClass::StreamType::Gamma:
    case ModelTrClass::StreamType::Pareto:
        ui->labelClassesIncommingExDx->setEnabled(true);
        ui->doubleSpinBoxIncomingExDx->setEnabled(true);
        break;
    }

    switch (ui->comboBox_CallServStrType->currentData().value<ModelTrClass::StreamType>())
    {
    case ModelTrClass::StreamType::Poisson:
        ui->labelClassesServiceExDx->setEnabled(false);
        ui->doubleSpinBoxServiceExDx->setEnabled(false);
        break;
    case ModelTrClass::StreamType::Uniform:
    case ModelTrClass::StreamType::Normal:
    case ModelTrClass::StreamType::Gamma:
    case ModelTrClass::StreamType::Pareto:
        ui->labelClassesServiceExDx->setEnabled(true);
        ui->doubleSpinBoxServiceExDx->setEnabled(true);
        break;
    }
    drawSystemModel();
}

void MainWindow::on_comboBoxResultsQtType_currentIndexChanged(int index)
{
    (void) index;
    Results::Type type = ui->comboBoxResultsQtType->currentData().value<Results::Type>();

    const Settings *settings = Results::TypesAndSettings::getSettingConst(type);

    ui->comboBoxResultsQtX_axis->clear();
    int currentIndex = -1;
    int tmpIndex = 0;

    if (settings)
    {
       foreach (Results::ParameterType param, settings->dependencyParameters)
       {

           ui->comboBoxResultsQtX_axis->addItem(Results::TypesAndSettings::parameterToString(param), QVariant::fromValue(param));
           if (param == settings->getFunctionalParameter())
               currentIndex = tmpIndex;
           tmpIndex++;
       }
    }
    if (currentIndex >= 0)
        ui->comboBoxResultsQtX_axis->setCurrentIndex(currentIndex);
}

void MainWindow::on_comboBoxResultsQtX_axis_currentIndexChanged(int index)
{
    (void) index;
    Results::Type type = ui->comboBoxResultsQtType->currentData().value<Results::Type>();
    Settings *settings = Results::TypesAndSettings::getSetting(type);

    Results::ParameterType paramType = ui->comboBoxResultsQtX_axis->currentData().value<Results::ParameterType>();
    settings->setFunctionalParameter(paramType);

    fillListWidgetWithParams(ui->listWidgetResultsQtAdditionalParameters1, ui->labelResultsQtAdditionalParameters1, settings->getAdditionalParameter1());
    fillListWidgetWithParams(ui->listWidgetResultsQtAdditionalParameters2, ui->labelResultsQtAdditionalParameters2, settings->getAdditionalParameter2());
}

void MainWindow::on_listWidgetKlasy_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current == previous)
        return;

    if (current == nullptr)
    {
        return;
    }
    ModelTrClass *selectedClass = current->data(Qt::UserRole).value<ModelTrClass *>();
    if (selectedClass == nullptr)
    {
        QMessageBox::warning(nullptr, "Somethink is not fully implemented", "Selected class = NULL");
        return;
    }

    for(int idx=0; idx< ui->comboBox_SourceType->count(); idx++)
        if (ui->comboBox_SourceType->itemData(idx).value<ModelTrClass::SourceType>() == selectedClass->srcType())
        {
            ui->comboBox_SourceType->setCurrentIndex(idx);
            break;
        }

    for(int idx=0; idx< ui->comboBox_NewCallStrType->count(); idx++)
        if (ui->comboBox_NewCallStrType->itemData(idx).value<ModelTrClass::StreamType>() == selectedClass->newCallStr())
        {
            ui->comboBox_NewCallStrType->setCurrentIndex(idx);
            break;
        }

    for(int idx=0; idx< ui->comboBox_CallServStrType->count(); idx++)
        if (ui->comboBox_CallServStrType->itemData(idx).value<ModelTrClass::StreamType>() == selectedClass->callServStr())
        {
            ui->comboBox_CallServStrType->setCurrentIndex(idx);
            break;
        }


    ui->spinBoxClassesAT->setValue(static_cast<int>(selectedClass->propAt()));
    ui->spinBoxClassesT-> setValue(static_cast<int>(selectedClass->t()));
    ui->doubleSpinBoxClassesMu->setValue(selectedClass->getMu());
    updateClassWidgets();
}

void MainWindow::readDataBase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName(appConfig.value("dbURL").value<QString>());
    db.setDatabaseName(appConfig.value("dbName").value<QString>());
    db.setUserName(appConfig.value("dbUserName").value<QString>());
    db.setPassword(appConfig.value("dbPassword").value<QString>());


    isDbWorking = db.open();
    QSqlError tmp = db.lastError();
    if (!isDbWorking)
    {
        qDebug("Błąd otwarcia bazy danych\n");
        qDebug()<<tmp;
    }

    ui->horizontalLayoutPredefinedSystems->setEnabled(isDbWorking);

    dbReadSystems();
    foreach (ModelSyst *tmpSyst, vectPredefinedSystems)
    {
        QVariant tmpVar;

        tmpVar.setValue<ModelSyst*>(tmpSyst);
        QString tmpStr;
        QTextStream tmpStream(&tmpStr);
//      tmpStream.setString(tmpStr);
        tmpStream<<(*tmpSyst);
        ui->comboBoxPredefinedSystems->addItem(tmpStr, tmpVar);
    }
    updateAlgorithmsList();
}

ModelTrClass::StreamType MainWindow::DBstrToStreamType(QString str)
{
    if (str == QString("exp"))
        return ModelTrClass::StreamType::Poisson;
    if (str == QString("norm"))
        return ModelTrClass::StreamType::Normal;
    if (str == QString("uni"))
        return ModelTrClass::StreamType::Uniform;
    if (str == QString("gamma"))
        return ModelTrClass::StreamType::Gamma;
    if (str == QString("pareto"))
        return ModelTrClass::StreamType::Pareto;
    else
        qFatal("unrecognized stream type");
}

ModelTrClass::SourceType MainWindow::DBstrToSourceType(QString str)
{
    if (str == QString("const"))
        return ModelTrClass::SourceType::Independent;

    if (str == QString("descending"))
        return ModelTrClass::SourceType::DependentMinus;

    if (str == QString("ascending"))
        return ModelTrClass::SourceType::DependentPlus;
    else
        qFatal("unrecognized source type");
}

void MainWindow::updateAlgorithmsList()
{
    ui->listWidgetAlgorithms->clear();
    ui->listWidgetAlgorithmsAlternative->clear();
    foreach (Investigator *alg, this->algorithms)
    {
        if (alg->possible(this->system))
             addAlgorithmForCurentSystem(alg);
        else if (alg->possibleAlternative(this->system))
             addAlternativeAlgorithmForCurentSystem(alg);
    }

    ui->listWidgetAlgorithms->update();
    ui->listWidgetAlgorithmsAlternative->update();

    update(); //Update Window form
}

QSet<Results::Type> MainWindow::getPossibleQoS_Types()
{
    QSet<Results::Type> possibleQoS_Types;
    foreach (Investigator *alg, this->algorithms)
    {
        if (alg->possible(this->system) && alg->calculationDone && alg->isSelected)
            possibleQoS_Types += alg->getQoS_Set();
    }
    return possibleQoS_Types;
}


void MainWindow::updateQoS_ComboBox(QSet<Results::Type> &qos)
{
    ui->comboBoxResultsQtType->clear();
    ui->comboBoxResultsQtX_axis->clear();
    ui->listWidgetResultsQtAdditionalParameters1->setVisible(false);
    ui->listWidgetResultsQtAdditionalParameters2->setVisible(false);
    ui->labelResultsQtAdditionalParameters1->setVisible(false);
    ui->labelResultsQtAdditionalParameters2->setVisible(false);

    foreach (Results::Type type, qos)
        ui->comboBoxResultsQtType->addItem(Results::TypesAndSettings::typeToString(type), QVariant::fromValue(type));

    update(); //Update Window form
}

void MainWindow::updateGnuplotActions(QSet<Type> &qos)
{
//    menuSave_results_to_gnuplot
    QMenu *gnuplotMenu = this->menuBar()->findChild<QMenu *>("menuFileGnuplot");
    gnuplotMenu->clear();
    foreach (Results::Type type, qos)
    {
        QAction *newAction = gnuplotMenu->addAction(Results::TypesAndSettings::typeToString(type), this, SLOT(on_gnuplotSave()));
        QVariant tmpVariant;
        tmpVariant.setValue<Results::Type>(type);
        newAction->setData(tmpVariant);
    }
}

void MainWindow::on_gnuplotSave()
{
    QAction *qAction = qobject_cast<QAction *>(sender());
    Results::Type type = qAction->data().value<Results::Type>();

    QString defaultFileName;
    QTextStream textStream(&defaultFileName);
    textStream<<"~/";
    textStream<<TypesAndSettings::typeToString(type);
    textStream<<*system;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save the results"), defaultFileName, tr("Gnuplot Files (*.gp)"));

    saveTheResults(fileName, type);
}

bool MainWindow::fillSystem()
{
    system->clearAll();

    int progress = 0;


    foreach (QListWidgetItem *tmpItem, ui->listWidgetKlasy->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelTrClass *tmpClass = tmpItem->data(Qt::UserRole).value<ModelTrClass *>();
        if (tmpClass == nullptr)
            continue;
        system->addClass(tmpClass);
        progress = 1;
    }

    foreach (QListWidgetItem *tmpItem, ui->listWidgetWiazki->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelResourcess tmpGroup = *tmpItem->data(Qt::UserRole).value<ModelResourcess*>();
        system->addGroups(tmpGroup);
        if (progress > 0)
            progress++;
    }

    foreach (QListWidgetItem *tmpItem, ui->listWidgetKolejki->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelResourcess tmpGroup = *tmpItem->data(Qt::UserRole).value<ModelResourcess*>();
        system->addQeues(tmpGroup);
    }

    NewTitle();
    qDebug()<<"Created new system: "<<(*system);

    system->id = -1;
    return (progress > 1) ? true : false;
}

int MainWindow::getNoOfPoints(double aMin, double aMax, double aDelta) const
{
    return static_cast<int>((aMax - aMin + aDelta / 2) / aDelta + 1);
}

bool MainWindow::dbReadSystems()
{
    const int noOfTypes = 3;


    QSqlQuery dbSystems;
    dbSystems.exec("SELECT * FROM Systems");

    while (dbSystems.next())
    {   
        ModelSyst *tmpSyst = new ModelSyst();
        int id = dbSystems.value("Id").toInt();

        int idClassSet = dbSystems.value("IdClassSet").toInt();
        int idServerSet = dbSystems.value("IdServerSet").toInt();
        int idBufferSet = dbSystems.value("IdBufferSet").toInt();


        qDebug("Adding system %d (%d %d %d)", id, idClassSet, idServerSet, idBufferSet);

        QSqlQuery singleSystem;
        singleSystem.exec(QString("SELECT * FROM TrClasses JOIN TrClassesConn WHERE TrClasses.Id = TrClassesConn.IdClass AND TrClassesConn.IdClassSet = %1\r\n").arg(idClassSet));

        tmpSyst->id = id;

        while (singleSystem.next())
        {
            ModelTrClass *trClass = new ModelTrClass();
            trClass->setT(singleSystem.value("t").toInt());
            trClass->setPropAt(singleSystem.value("propAT").toInt());
            trClass->setMu(singleSystem.value("u").toDouble());

            ModelTrClass::StreamType streamType = DBstrToStreamType(singleSystem.value("arrivalPDF").toString());
            ModelTrClass::SourceType sourceType = DBstrToSourceType(singleSystem.value("sourceType").toString());
            trClass->setNewCallStrType(streamType, sourceType);
            trClass->setNoOfSourcess(singleSystem.value("S").toInt());
            trClass->setIncommingExPerDx(singleSystem.value("arrivalED").toDouble());


            streamType = DBstrToStreamType(singleSystem.value("servicePDF").toString());
            trClass->setCallServStrType(streamType);
            trClass->setServiceExPerDx(singleSystem.value("serviceED").toDouble());

            tmpSyst->addClass(trClass);

            QString classDescription;
            QTextStream classDescriptionStream(&classDescription);
            classDescriptionStream<<*trClass;

            qDebug("Adding class %s", classDescription.toStdString().data());
        }

        if (idServerSet > 0)
        {
            singleSystem.exec(QString("SELECT * FROM ResServer JOIN ResServerConn WHERE ResServer.Id = ResServerConn.IdServer AND ResServerConn.IdServerSet = %1\r\n").arg(idServerSet));
            while (singleSystem.next())
            {
                int k = singleSystem.value("k").toInt();
                int v = singleSystem.value("v").toInt();
                ModelResourcess tmpGroup(k, v);
                tmpSyst->addGroups(tmpGroup);
            }
        }

        if (idBufferSet > 0)
        {
            singleSystem.exec(QString("SELECT * FROM ResBuffer JOIN ResBufferConn WHERE ResBuffer.Id = ResBufferConn.IdServer AND ResBufferConn.IdBufferSet = %1\r\n").arg(idServerSet));
            while (singleSystem.next())
            {
                int k = singleSystem.value("k").toInt();
                int v = singleSystem.value("v").toInt();
                ModelResourcess tmpGroup(k, v);
                tmpSyst->addQeues(tmpGroup);
            }
        }
        QString systemDescription;
        QTextStream systemDescriptionStream(&systemDescription);
        systemDescriptionStream<<*tmpSyst;
        qDebug("Adding system %s", systemDescription.toStdString().data());
        vectPredefinedSystems.append(tmpSyst);
    }

/*    QSqlQuery systemStructure;
    systemStructure.exec("SELECT * FROM systemy");

    while (systemStructure.next())
    {
        ModelSyst *tmpSyst = new ModelSyst();

        tmpSyst->id = pobieranieSystemu.value("id").toInt();

        int m = pobieranieSystemu.value("m").toInt();
        int collIdx = 2;
        int vs[noOfTypes];
        int ks[noOfTypes];
        int vb[noOfTypes];
        int kb[noOfTypes];

        for (int x=0; x<noOfTypes; x++)
            ks[x] = pobieranieSystemu.value(collIdx++).toInt();
        for (int x=0; x<noOfTypes; x++)
        {
            vs[x] = pobieranieSystemu.value(collIdx++).toInt();
            if (ks[x] == 0)
                continue;
            tmpSyst->addGroups(ModelResourcess(ks[x], vs[x]), false);
        }
        for (int x=0; x<noOfTypes; x++)
            kb[x] = pobieranieSystemu.value(collIdx++).toInt();
        for (int x=0; x<noOfTypes; x++)
        {
            vb[x] = pobieranieSystemu.value(collIdx++).toInt();
            if (kb[x] == 0)
                continue;
            tmpSyst->addQeues(ModelResourcess(kb[x], vb[x]), false);
        }

        QSqlQuery pobieranieKlasSystemu;
        QString strQuerry = QString("SELECT * FROM klasySystemu WHERE sysId = %1 ORDER BY \"idx\"").arg(tmpSyst->id);

        pobieranieKlasSystemu.exec(strQuerry);
        for (int i=0; i<m; i++)
        {
            if (pobieranieKlasSystemu.next() == false)
            {
                qDebug()<<"Can't get class no "<<i<<" for system "<<tmpSyst->id;
                break;
            }
            ModelTrClass *tmpTrClass = new ModelTrClass();
            tmpTrClass->setT(pobieranieKlasSystemu.value("t").toInt());
            tmpTrClass->setMu(pobieranieKlasSystemu.value("servEx").toDouble());

            tmpTrClass->setIncommingExPerDx(pobieranieKlasSystemu.value("newCallEx2VsD").toDouble());
            tmpTrClass->setServiceExPerDx(pobieranieKlasSystemu.value("servEx2VsD").toDouble());

            QString newCallStrTypeStr = pobieranieKlasSystemu.value("newCallStrType").toString();
            QString servStrTypeStr = pobieranieKlasSystemu.value("servStrType").toString();

            int S = pobieranieKlasSystemu.value("S").toInt();


            ModelTrClass::SourceType sourceType = ModelTrClass::SourceType::Independent;
            if (S > 0)
                sourceType = ModelTrClass::SourceType::DependentMinus;
            if (S < 0)
                sourceType = ModelTrClass::SourceType::DependentPlus;

            ModelTrClass::StreamType newCallStrType  = DBstrTostreamType(newCallStrTypeStr);
            ModelTrClass::StreamType servStrType     = DBstrTostreamType(servStrTypeStr);

            tmpTrClass->setNewCallStrType(newCallStrType, sourceType);
            tmpTrClass->setCallServStrType(servStrType);

            if (S < 0)
                S *= -1;
            tmpTrClass->setNoOfSourcess(S);


            tmpSyst->addClass(tmpTrClass);
        }
        vectPredefinedSystems.append(tmpSyst);
    }
    */
    return true;
}

void MainWindow::on_pushButtonStart_clicked()
{
    foreach (QListWidgetItem *tmpItem, ui->listWidgetAlgorithms->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        Investigator *tmpAlg = tmpItem->data(Qt::UserRole).value<Investigator *>();
        tmpAlg->show = false;
    }

    int noOfpoints = getNoOfPoints(ui->doubleSpinBox_aMin->value(), ui->doubleSpinBox_aMax->value(), ui->doubleSpinBox_aDelta->value());

    QString title;
    QTextStream tmpStream(&title);
    tmpStream<<"Qeue analyzer: calculating ";
    tmpStream<<*system;
    setWindowTitle(title);
    ui->progressBar->setValue(0);
    int total = 0;

    foreach (QListWidgetItem *tmpItem, ui->listWidgetAlgorithms->selectedItems()+ui->listWidgetAlgorithmsAlternative->selectedItems())
    {
        Investigator *tmpAlg = tmpItem->data(Qt::UserRole).value<Investigator *>();
        if (tmpAlg != nullptr)
        {
            tmpAlg->show = true;

            tmpAlg->calculationDone = false;
            total += tmpAlg->complexity();
        }
    }

    int progress = 0;
    double a=0;
    for (int x=0; x<noOfpoints; x++)
    {
        a = ui->doubleSpinBox_aMin->value();
        a += (x*ui->doubleSpinBox_aDelta->value());

        if ( qFuzzyIsNull(a))
            qFatal("a = 0");

        foreach (QListWidgetItem *tmpItem, ui->listWidgetAlgorithms->selectedItems()+ui->listWidgetAlgorithmsAlternative->selectedItems())
        {
            Investigator *tmpAlg = tmpItem->data(Qt::UserRole).value<Investigator *>();
            if (tmpAlg != nullptr)
            {
                if (tmpAlg->calculationDone)
                    continue;

                Results::RInvestigator& investigatorResults = resultsForSystem->createNewInvestigation(tmpAlg, a);

                investigatorResults.init((tmpAlg->hasConfIntervall()) ? currentSimulationParameter->noOfSeries : 0);

                progress += tmpAlg->complexity();
                if (!tmpAlg->correctSystemParameters(system, a))
                {
                    qDebug("Incorrect input data... skiping");
                    continue;
                }

                tmpAlg->calculateSystem(system, a, &investigatorResults, currentSimulationParameter);
                int valProgBar = static_cast<int>((100.0*progress/(total * noOfpoints)));
                ui->progressBar->setValue(valProgBar);
            }
        }
    }
    ui->progressBar->setValue(100);

    title.clear();
    tmpStream<<"Qeue analyzer: ";
    tmpStream<<*system;
    tmpStream<<" ready";
    setWindowTitle(title);

    foreach (QListWidgetItem *tmpItem, ui->listWidgetAlgorithms->selectedItems()+ui->listWidgetAlgorithmsAlternative->selectedItems())
    {
        Investigator *tmpAlg = tmpItem->data(Qt::UserRole).value<Investigator *>();
        tmpAlg->calculationDone = true;
    }
    QSet<Results::Type> qosPar = getPossibleQoS_Types();
    updateQoS_ComboBox(qosPar);
    updateGnuplotActions(qosPar);
}


void MainWindow::clearParameters(ParametersSet &outParameters)
{
    outParameters.a = -1;
    outParameters.classIndex = -1;
    outParameters.systemState = -1;
    outParameters.serverState = -1;
    outParameters.bufferState = -1;
    outParameters.combinationNumber = -1;
    outParameters.numberOfGroups = -1;
}

void MainWindow::saveTheResults(QString &fileName, Results::Type qosType)
{
    QString scriptFileName = fileName + QString(".gp");
    QString dataFileName = fileName + QString(".dat");
    QString graphBaseFileName = fileName;

    Results::Settings *setting = Results::TypesAndSettings::getSetting(qosType);
    scrGnuplot->systemResults = this->resultsForSystem;
    scrGnuplot->WriteDataAndScript(fileName, this->system, setting, qosType);
}

void MainWindow::saveResultsGnuplotE()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_E";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Blocking probability"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::E);
}

void MainWindow::saveResultsGnuplotLoss()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_B";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Loss probability"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::B);
}

void MainWindow::saveResultsGnuplotAvgQeueLen()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_AvgQeueLen";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Avg qeue length"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::Qlen);
}

void MainWindow::saveResultsGnuplotAvnNbrCallsInQeue()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_AvgNbrOfCallsInQeue";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Avg nbr of calls in qeue"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::yQ);
}

void MainWindow::saveResultsGnuplotEffectiveWaitingTime()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_EfWaitingTime";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Effective waiting time"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::tWait);
}

void MainWindow::saveResultsGnuplotAbsoluteWaitingTime()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_AbsWaitingTime";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Absolute waiting time"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::tQeue);
}

void MainWindow::saveResultsGnuplotYtInQeue()
{
    QString suggestedFilename;
    QTextStream sugestedFilenameStr(&suggestedFilename);
    sugestedFilenameStr<<*system;
    sugestedFilenameStr<<"_QtInQeue";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save QoS: Occupied resourcess in Qeue"), suggestedFilename, tr("Data files (*.dat)"));
    //saveTheResults(fileName, resultsType::ytQ);
}

void MainWindow::on_listWidgetSimulationParameters_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == previous)
        return;
    QVariant selVariant = current->data(Qt::UserRole);
    currentSimulationParameter = selVariant.value<SimulationParameters *>();
}

QString MainWindow::ReadFilename(QString extension)
{
    QString suggestedFilename;
    QTextStream asd(&suggestedFilename);
    asd<<*system;
    return QFileDialog::getSaveFileName(this, tr("Save the results"), suggestedFilename, tr("Spreadsheet")+QString(" (%1)").arg(extension));
}

bool MainWindow::areCalculationsRequired()
{
    //TODO sprawdzić czy są wyniki dla wszystkich a
    bool result = false;


    foreach (Investigator *alg, algorithms)
        if ((alg->isSelected) && (alg->calculationDone))
                result = true;

    return result;
}

void MainWindow::changeEvent(QEvent* event)
{
 if(nullptr != event)
 {
    QString locale = QLocale::system().name();

    switch(event->type())
    {
    // this event is send if a translator is loaded    

    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

   // this event is send, if the system, language changes
    case QEvent::LocaleChange:
        locale.truncate(locale.lastIndexOf('_'));
        loadLanguage(locale);
        ui->retranslateUi(this);
        break;

    default:
        break;
    }
 }

 QMainWindow::changeEvent(event);
}

void MainWindow::slotLanguageChanged(QAction *action)
{
    if(nullptr != action)
    {
        // load the language dependant on the action content
        loadLanguage(action->data().toString());
        setWindowIcon(action->icon());
        ui->retranslateUi(this);
    }
}

void switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    if(translator.load(filename))
     qApp->installTranslator(&translator);
    else
        qDebug()<<"Can't load translator: "<<filename;
}

void MainWindow::loadLanguage(const QString &rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        QString languageName = QLocale::languageToString(locale.language());
        switchTranslator(m_translator, QString("languages/kolejki_%1.qm").arg(rLanguage));
        switchTranslator(m_translatorQt, QString("languages/qt_%1.qm").arg(rLanguage));
        qDebug()<<tr("Current Language changed to %1 (%2)").arg(languageName, rLanguage);

        QVariant variant;

        drawSystemModel();

        ui->comboBox_SourceType->addItem("Poisson", variant);// (newItem);

        ui->comboBox_SourceType->clear();

        variant.setValue(ModelTrClass::SourceType::Independent);
        ui->comboBox_SourceType->addItem("Independent", variant);
        variant.setValue(ModelTrClass::SourceType::DependentMinus);
        ui->comboBox_SourceType->addItem("Dependent -", variant);
        variant.setValue(ModelTrClass::SourceType::DependentPlus);
        ui->comboBox_SourceType->addItem("Dependent +", variant);

        ui->comboBox_NewCallStrType->clear();
        variant.setValue(ModelTrClass::StreamType::Poisson);
        ui->comboBox_NewCallStrType->addItem("Poisson", variant);
        variant.setValue(ModelTrClass::StreamType::Uniform);
        ui->comboBox_NewCallStrType->addItem("Uniform", variant);
        variant.setValue(ModelTrClass::StreamType::Normal);
        ui->comboBox_NewCallStrType->addItem("Normal", variant);
        variant.setValue(ModelTrClass::StreamType::Gamma);
        ui->comboBox_NewCallStrType->addItem("Gamma", variant);
        variant.setValue(ModelTrClass::StreamType::Pareto);
        ui->comboBox_NewCallStrType->addItem("Pareto", variant);
        ui->comboBox_CallServStrType->clear();
        variant.setValue(ModelTrClass::StreamType::Poisson);
        ui->comboBox_CallServStrType->addItem("Poisson", variant);
        variant.setValue(ModelTrClass::StreamType::Uniform);
        ui->comboBox_CallServStrType->addItem("Uniform", variant);
        variant.setValue(ModelTrClass::StreamType::Normal);
        ui->comboBox_CallServStrType->addItem("Normal", variant);
        variant.setValue(ModelTrClass::StreamType::Gamma);
        ui->comboBox_CallServStrType->addItem("Gamma", variant);
        variant.setValue(ModelTrClass::StreamType::Pareto);
        ui->comboBox_CallServStrType->addItem("Pareto", variant);

        QVector<ServerResourcessScheduler> tmpOptions = { ServerResourcessScheduler::Random, ServerResourcessScheduler::Sequencial };
        foreach (ServerResourcessScheduler tmp, tmpOptions)
        {
            variant.setValue(tmp);
            ui->comboBoxServerSchedulerAlgorithm->addItem(serverResourcessSchedulerToString(tmp), variant);
        }

        QVector<BufferResourcessScheduler> tmpOptionsBuffer = {
            BufferResourcessScheduler::dFIFO_Seq,
            BufferResourcessScheduler::Continuos,
            BufferResourcessScheduler::qFIFO_Seq,
            BufferResourcessScheduler::SD_FIFO
        };
        foreach (BufferResourcessScheduler tmp, tmpOptionsBuffer)
        {
            variant.setValue(tmp);
            ui->comboBoxBufferSchedulerAlgorithm->addItem(bufferResourcessSchedulerToString(tmp), variant);
        }
    }
}

void MainWindow::createLanguageMenu()
{
    QActionGroup* langGroup = new QActionGroup(ui->menuLanguage);
    langGroup->setExclusive(true);

    connect(langGroup, SIGNAL (triggered(QAction *)), this, SLOT (slotLanguageChanged(QAction *)));

    // format systems language
    QString defaultLocale = QLocale::system().name(); // e.g. "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("kolejki_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i)
    {
         // get locale extracted by filename
         QString locale;
         locale = fileNames[i]; // "TranslationExample_de.qm"
         locale.truncate(locale.lastIndexOf('.')); // "TranslationExample_de"
         locale.remove(0, locale.indexOf('_') + 1); // "de"

        QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        QAction *action = new QAction(ico, lang, this);
        action->setCheckable(true);
        action->setData(locale);

        ui->menuLanguage->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
        }
    }
}

void MainWindow::on_Pages_currentChanged(int index)
{
    if (index == 1)
    {
        for(int idx=0; idx<ui->listWidgetAlgorithms->count(); idx++)
        {
            QListWidgetItem *tmp = ui->listWidgetAlgorithms->item(idx);
            Investigator *alg = tmp->data(Qt::UserRole).value<Investigator *>();
            tmp->setSelected(alg->isSelected);
        }
        for(int idx=0; idx<ui->listWidgetAlgorithmsAlternative->count(); idx++)
        {
            QListWidgetItem *tmp = ui->listWidgetAlgorithmsAlternative->item(idx);
            Investigator *alg = tmp->data(Qt::UserRole).value<Investigator *>();
            tmp->setSelected(alg->isSelected);
        }
    }
}

void MainWindow::on_listWidgetAlgorithms_itemClicked(QListWidgetItem *item)
{
    Investigator *selAlg = item->data(Qt::UserRole).value<Investigator *>();
    selAlg->isSelected = item->isSelected();

    if ((selAlg->isSelected) && (!selAlg->calculationDone))
        ui->pushButtonStart->setEnabled(true);
    else
        ui->pushButtonStart->setEnabled(areCalculationsRequired());
}

void MainWindow::on_listWidgetAlgorithmsAlternative_itemClicked(QListWidgetItem *item)
{
    Investigator *selAlg = item->data(Qt::UserRole).value<Investigator *>();
    selAlg->isSelected = item->isSelected();
}

void MainWindow::on_actionAbout_triggered()
{

    dlgAbout->show();
}

void MainWindow::drawSystemModel()
{
    if (sceneSysModel == nullptr)
        return;
    sceneSysModel->items().clear();
    sceneSysModel->clear();

    //sceneSysModel->addText("Hello, world!");

    ModelSyst *sys = new ModelSyst();

    foreach (QListWidgetItem *tmpItem, ui->listWidgetKlasy->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelTrClass *tmpClass = tmpItem->data(Qt::UserRole).value<ModelTrClass *>();
        sys->addClass(tmpClass);
    }

    foreach (QListWidgetItem *tmpItem, ui->listWidgetWiazki->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelResourcess tmpGroup = *tmpItem->data(Qt::UserRole).value<ModelResourcess*>();
        sys->addGroups(tmpGroup);
    }

    foreach (QListWidgetItem *tmpItem, ui->listWidgetKolejki->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        ModelResourcess tmpGroup = *tmpItem->data(Qt::UserRole).value<ModelResourcess*>();
        sys->addQeues(tmpGroup);
    }

    QBrush qeueBrush(Qt::lightGray);
    QBrush serverBrush(Qt::yellow);
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);

    int kQtype = sys->k_bType();
    int kStype = sys->k_sType();

    int width  = static_cast<int>(ui->graphicsView->width());
    int height = static_cast<int>(ui->graphicsView->height());

    int wClasses=static_cast<int>(width*0.4);
    //int wserver=width*0.2;
    int wQeue= static_cast<int>(width*0.4);

    qreal xClasses = 0;
    qreal xQeue = xClasses + wClasses;
    qreal xServer = xQeue + wClasses;

    qreal classSep =  static_cast<qreal>((height) / (sys->m()+2));
    for (int i=0; i < sys->m(); i++)
    {
        qreal y = 2*classSep + i * classSep - static_cast<qreal>((height)/2);
        /*QGraphicsLineItem *line  = */sceneSysModel->addLine(xClasses, y, xClasses+0.9*wClasses, y, blackPen);
        /*QGraphicsLineItem *line1 = */sceneSysModel->addLine(xClasses+0.9*wClasses, y, xClasses+0.9*wClasses - 5, y - 5, blackPen);
        /*QGraphicsLineItem *line2 = */sceneSysModel->addLine(xClasses+0.9*wClasses, y, xClasses+0.9*wClasses - 5, y + 5, blackPen);

        QGraphicsTextItem  *text = sceneSysModel->addText(sys->getClass(i)->shortName(), QFont("Arial", 12));
        text->setPos(QPoint(static_cast<int>(xClasses), static_cast<int>(y-22)));
    }

    for (int qIdx=0; qIdx<kQtype; qIdx++)
    {
        int sglQeueWidth = (wQeue/kQtype);
        qreal x, y;
        x = qIdx*sglQeueWidth + xQeue;
        int k = sys->k_b(qIdx);
        for (int qIdx2=0; qIdx2<k; qIdx2++)
        {
            y = qIdx2*25-10;
            /*QGraphicsItem *itm = */sceneSysModel->addRect(x, y, sglQeueWidth*0.9, 20, blackPen, qeueBrush);
            QGraphicsTextItem  *text = sceneSysModel->addText(QString("%1").arg(sys->v_b(qIdx)), QFont("Arial", 16));
            text->setPos(QPoint(static_cast<int>(x), static_cast<int>(y-8)));
        }
    }

    for (int sIdx=0; sIdx<kStype; sIdx++)
    {
        int sglServerHeight = (height/kStype);
        qreal x, y;

        y = height * (-0.5);
        y += sIdx*sglServerHeight;
        int k = sys->k_s(sIdx);
        for (int sIdx2=0; sIdx2<k; sIdx2++)
        {
            x = xServer + sIdx2*35;
            /*QGraphicsItem *itm = */sceneSysModel->addRect(x, y, 30, sglServerHeight*0.9, blackPen, serverBrush);
            QGraphicsTextItem  *text = sceneSysModel->addText(QString("%1").arg(sys->v_s(sIdx)), QFont("Arial", 16));
            text->setPos(QPoint(static_cast<int>(x), static_cast<int>(y+sglServerHeight*0.45-8)));
        }
    }
    delete sys;
}

void MainWindow::on_comboBoxPredefinedSystems_currentIndexChanged(int index)
{
    (void) index;
    ModelSyst *system =ui->comboBoxPredefinedSystems->currentData().value<ModelSyst *>();

    this->system->id = system->id;

    ui->listWidgetWiazki->clear();
    for(int grId=0; grId<system->k_s(); grId++)
        addServer(system->k_s(grId), system->v_s(grId));

    ui->listWidgetKolejki->clear();
    for(int grId=0; grId<system->k_b(); grId++)
        addBuffer(system->k_b(grId), system->v_b(grId));

    ui->listWidgetKlasy->clear();
    for (int i=0; i<system->m(); i++)
    {
        const ModelTrClass *trClass = system->getClass(i);

        QString classDescription = trClass->shortName();

        QListWidgetItem *newClassItem = new QListWidgetItem();
        QVariant variant;
        variant.setValue(trClass);

        newClassItem->setData(Qt::DisplayRole, classDescription);
        newClassItem->setData(Qt::UserRole, variant);
        ui->listWidgetKlasy->addItem(newClassItem);
    }    
    fillSystem();
}

void MainWindow::on_comboBox_NewCallStrType_currentIndexChanged(int index)
{
    (void) index;
    switch(ui->comboBox_NewCallStrType->currentData().value<ModelTrClass::StreamType>())
    {
    case ModelTrClass::StreamType::Poisson:
        ui->doubleSpinBoxIncomingExDx->setMinimum(1);
        ui->doubleSpinBoxIncomingExDx->setEnabled(false);
        break;

    case ModelTrClass::StreamType::Uniform:
        ui->doubleSpinBoxIncomingExDx->setMinimum(3);
        ui->doubleSpinBoxIncomingExDx->setEnabled(true);
        break;

    case ModelTrClass::StreamType::Normal:
        ui->doubleSpinBoxIncomingExDx->setMinimum(1);
        ui->doubleSpinBoxIncomingExDx->setEnabled(true);
        break;

    case ModelTrClass::StreamType::Gamma:
        ui->doubleSpinBoxIncomingExDx->setMinimum(1);
        ui->doubleSpinBoxIncomingExDx->setEnabled(true);
        break;

    case ModelTrClass::StreamType::Pareto:
        ui->doubleSpinBoxIncomingExDx->setMinimum(0.000001);
        ui->doubleSpinBoxIncomingExDx->setEnabled(true);
        break;
    }
}

void MainWindow::on_comboBox_CallServStrType_currentIndexChanged(int index)
{
    (void) index;
    switch(ui->comboBox_CallServStrType->currentData().value<ModelTrClass::StreamType>())
    {
    case ModelTrClass::StreamType::Poisson:
        ui->doubleSpinBoxServiceExDx->setMinimum(1);
        ui->doubleSpinBoxServiceExDx->setEnabled(false);
        break;

    case ModelTrClass::StreamType::Uniform:
        ui->doubleSpinBoxServiceExDx->setMinimum(3);
        ui->doubleSpinBoxServiceExDx->setEnabled(true);
        break;

    case ModelTrClass::StreamType::Normal:
        ui->doubleSpinBoxServiceExDx->setMinimum(1);
        ui->doubleSpinBoxServiceExDx->setEnabled(true);
        break;

    case ModelTrClass::StreamType::Gamma:
        ui->doubleSpinBoxServiceExDx->setMinimum(1);
        ui->doubleSpinBoxServiceExDx->setEnabled(true);
        break;

    case ModelTrClass::StreamType::Pareto:
        ui->doubleSpinBoxServiceExDx->setMinimum(0.000001);
        ui->doubleSpinBoxServiceExDx->setEnabled(true);
        break;
    }
}

void MainWindow::on_comboBoxServerSchedulerAlgorithm_currentIndexChanged(int index)
{
    (void) index;
    system->setServerSchedulerAlgorithm(ui->comboBoxServerSchedulerAlgorithm->currentData().value<ServerResourcessScheduler>());
    fillSystem();
}

void MainWindow::on_comboBoxBufferSchedulerAlgorithm_currentIndexChanged(int index)
{
    (void) index;
    system->setBufferSchedulerAlgorithm(ui->comboBoxBufferSchedulerAlgorithm->currentData().value<BufferResourcessScheduler>());
    fillSystem();
}


void MainWindow::fillListWidgetWithParams(QListWidget *outList, QLabel *outLabel, Results::ParameterType paramType)
{
    outLabel->setText(Results::TypesAndSettings::parameterToString(paramType));
    outList->clear();

    outLabel->setVisible(paramType != Results::ParameterType::None);
    outList->setVisible(paramType != Results::ParameterType::None);

    QListWidgetItem *tmpItem;
    QVariant tmpVariant;


    int noOfGroupCombinations;
    QString menuItemStr;

    switch (paramType)
    {
    case Results::ParameterType::OfferedTrafficPerAS:
        foreach (decimal a, resultsForSystem->getAvailableAperAU())
        {
            double dblA = static_cast<double>(a);
            tmpVariant.setValue(dblA);
            tmpItem = new QListWidgetItem(QString::number(static_cast<double>(a)));
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::TrafficClass:
        for(int i = 0; i < resultsForSystem->getModel().m(); i++)
        {
            tmpVariant.setValue(i);
            tmpItem = new QListWidgetItem(resultsForSystem->getModel().getClass(i)->shortName());
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::SystemState:
        for(int n = 0; n <= resultsForSystem->getModel().V(); n++)
        {
            tmpVariant.setValue(n);
            tmpItem = new QListWidgetItem(QString::number(n));
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::ServerState:
        for(int n = 0; n <= resultsForSystem->getModel().vk_s(); n++)
        {
            tmpVariant.setValue(n);
            tmpItem = new QListWidgetItem(QString::number(n));
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::BufferState:
        for(int n = 0; n <= resultsForSystem->getModel().vk_b(); n++)
        {
            tmpVariant.setValue(n);
            tmpItem = new QListWidgetItem(QString::number(n));
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::CombinationNumber:
        noOfGroupCombinations = resultsForSystem->getNoOfGroupsCombinations();
        for (int k = 0; k < noOfGroupCombinations; k++)
        {
            tmpVariant.setValue(k);
            menuItemStr = resultsForSystem->getGroupCombinationStr(k);
            tmpItem = new QListWidgetItem(menuItemStr);
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::NumberOfGroups:
        for (int k = 0; k<=resultsForSystem->getModel().k_s(); k++)
        {
            tmpVariant.setValue(k);
            tmpItem = new QListWidgetItem(QString::number(k));
            tmpItem->setData(Qt::UserRole, tmpVariant);

            outList->addItem(tmpItem);
        }
        break;

    case Results::ParameterType::None:
        break;
    }
}

void MainWindow::on_ResultsQtChartRefresh()
{
    struct Results::ParametersSet parameters;
    QString serDescription;

    Results::Type type = ui->comboBoxResultsQtType->currentData().value<Results::Type>();
    Results::Settings *setting = Results::TypesAndSettings::getSetting(type);

    Results::ParameterType xAxisParam = ui->comboBoxResultsQtX_axis->currentData(Qt::UserRole).value<Results::ParameterType>();
    setting->setFunctionalParameter(xAxisParam);

    int noOfAlgorithms = 0;

    foreach (QListWidgetItem *itm, ui->listWidgetAlgorithms->selectedItems() + ui->listWidgetAlgorithmsAlternative->selectedItems())
    {
        Investigator *algorithm = itm->data(Qt::UserRole).value<Investigator *>();
        if (algorithm->calculationDone)
            noOfAlgorithms++;
    }

    foreach (QListWidgetItem *itm,
             ui->listWidgetAlgorithms->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard) +
             ui->listWidgetAlgorithmsAlternative->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        if (itm->data(Qt::UserRole).value<Investigator *>()->calculationDone)
            itm->setBackground(QBrush(QColor(0, 255, 0, 32)));
        else if (itm->data(Qt::UserRole).value<Investigator *>()->getQoS_Set().contains(type))
            itm->setBackground(QBrush(QColor(255, 255, 0, 32)));
        else
            itm->setBackground(QBrush(QColor(255, 0, 0, 32)));
    }


    if (noOfAlgorithms == 0)
        return;


    QChart *chart = ui->widgetResultsPlot->chart();
    chart->removeAllSeries();

    QAbstractAxis *axisY;


    axisX->setVisible();
    axisX->setLinePenColor(QColor::fromRgb(0, 0, 0));
    axisX->setTickCount(1);

    if (chart->geometry().width() > 800)
    {
        chart->legend()->setAlignment(Qt::AlignRight);
    }
    else
        chart->legend()->setAlignment(Qt::AlignBottom);

    static QVector<QColor> algColors =
    {
        QColor::fromRgb(255,   0, 0), QColor::fromRgb(  0, 255,   0), QColor::fromRgb(  0,   0, 255)
      , QColor::fromRgb(128, 128, 0), QColor::fromRgb(128,   0, 128), QColor::fromRgb(  0, 128, 128), QColor::fromRgb(128, 128, 128)
      , QColor::fromRgb(128,   0, 0), QColor::fromRgb(  0, 128,   0), QColor::fromRgb(  0,   0, 128)
    };

    static QVector<Qt::PenStyle> par1lineStyle =
    {
        Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashLine, Qt::DashDotDotLine, Qt::CustomDashLine
    };

    static QVector<int> par2Lw =
    {
        3, 5, 1, 2, 4
    };

    QPair<double, double> yMinAndMax(DBL_MAX, -DBL_MAX);

    if (ui->checkBoxResultsQtLogScaleOnAxisY->isChecked())
    {
        axisYlinear->setVisible(false);
        axisY = axisYlog;

        axisYlog->setLabelFormat("%f");
    }
    else
    {
        axisYlog->setVisible(false);
        axisY = axisYlinear;

        axisYlinear->setLabelFormat("%f");
    }


    int algColIdx = 0;
    foreach (QListWidgetItem *itm, ui->listWidgetAlgorithms->selectedItems() + ui->listWidgetAlgorithmsAlternative->selectedItems())
    {
        int par1BrushIdx = 0;
        Investigator *algorithm = itm->data(Qt::UserRole).value<Investigator *>();
        serDescription = (noOfAlgorithms<=1) ? "" : algorithm->shortName() + " ";


        if (!algorithm->calculationDone)
            continue;

        clearParameters(parameters);

        if (setting->getAdditionalParameter1() != Results::ParameterType::None)
        {
            foreach (const QListWidgetItem *tmpItem1, ui->listWidgetResultsQtAdditionalParameters1->selectedItems())
            {
                QString name = Settings::updateParameters(parameters, tmpItem1->data(Qt::UserRole), setting->getAdditionalParameter1(), system, resultsForSystem);
                if (setting->getAdditionalParameter2() != Results::ParameterType::None)
                {
                    int par2LwIdx = 0;
                    foreach (const QListWidgetItem *tmpItem2, ui->listWidgetResultsQtAdditionalParameters2->selectedItems())
                    {
                        QString name2 = Settings::updateParameters(parameters, tmpItem2->data(Qt::UserRole), setting->getAdditionalParameter2(), system, resultsForSystem);
                        QLineSeries *series = new QLineSeries();
                        setting->getSinglePlot(series, yMinAndMax, *resultsForSystem, algorithm, parameters, !ui->checkBoxResultsQtLogScaleOnAxisY->isChecked());

                        series->setName(algorithm->shortName() + " " + name + " " + name2);

                        QPen tmpPen = QPen(algColors[algColIdx]);
                        tmpPen.setStyle(par1lineStyle[par1BrushIdx]);
                        tmpPen.setWidth(par2Lw[par2LwIdx]);
                        tmpPen.setColor(algColors[algColIdx]);

                        series->setPen(tmpPen);

                        chart->addSeries(series);
                        series->attachAxis(axisX);
                        series->attachAxis(axisY);

                        par2LwIdx++;
                        if (par2LwIdx == par2Lw.length())
                            par2LwIdx = 0;
                    }
                }
                else
                {
                    QLineSeries *series = new QLineSeries();
                    setting->getSinglePlot(series, yMinAndMax, *resultsForSystem, algorithm, parameters, !ui->checkBoxResultsQtLogScaleOnAxisY->isChecked());

                    series->setName(algorithm->shortName() + " " + name);
                    QPen tmpPen = QPen(algColors[algColIdx]);
                    tmpPen.setStyle(par1lineStyle[par1BrushIdx]);
                    series->setPen(tmpPen);

                    chart->addSeries(series);
                    series->attachAxis(axisX);
                    series->attachAxis(axisY);
                }
                par1BrushIdx++;
                if (par1BrushIdx == par1lineStyle.length())
                    par1BrushIdx = 0;
            }
        }
        else
        {
            QLineSeries *series = new QLineSeries();
            setting->getSinglePlot(series, yMinAndMax, *resultsForSystem, algorithm, parameters, !ui->checkBoxResultsQtLogScaleOnAxisY->isChecked());

            series->setName(algorithm->shortName());
            series->setColor(algColors[algColIdx]);

            chart->addSeries(series);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
        algColIdx++;
        if (algColIdx == algColors.size())
            algColIdx = 0;
    }

    if (ui->checkBoxResultsQtLogScaleOnAxisY->isChecked())
    {
        if (yMinAndMax.first <= 0)
            axisYlog->setMin(0.0001);
        else
            axisYlog->setMin(yMinAndMax.first);
    }
    else
    {
        axisYlinear->setMin(yMinAndMax.first);
    }
    axisYlog->setMax(yMinAndMax.second);


    axisY->setVisible();
    axisY->setLinePenColor(QColor::fromRgb(0, 0, 0));
    axisY->setLabelsAngle(315);
    axisY->setLineVisible();
    axisY->setTitleText(Results::TypesAndSettings::typeToString(type));


    axisX->setMin(setting->getXmin(*resultsForSystem));
    axisX->setMax(setting->getXmax(*resultsForSystem));


    if (ui->checkBoxResultsQtShowKey->isEnabled())
        chart->legend()->show();
    else
        chart->legend()->hide();
}

void MainWindow::on_actionConfigure_triggered()
{
    dlgConfig->show();
}
