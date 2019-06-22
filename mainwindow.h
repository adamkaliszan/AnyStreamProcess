#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <cmath>

#include <QMainWindow>
#include <QMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <qstring.h>
#include <qlistwidget.h>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "config.h"
#include "model.h"
#include "algorithms/simulationParameters.h"

#include "results/resultsSystem.h"
#include "results/resultsApi.h"
#include "results/resultsGnuplot.h"


#include "utils/decimal.h"

#include "dialogAbout.h"
#include "dialogconfig.h"


namespace Ui {
class MainWindow;
enum class terminalType;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

    void addAlgorithmForCurentSystem(Investigator *newAlg);
    void addAlternativeAlgorithmForCurentSystem(Investigator *newAlg);
    void addSimulationParams(SimulationParameters *par);
    void addAlgorithmsAndParams(void);
    ~MainWindow();

    void addServer(int k, int v);
    void addBuffer(int k, int v);
    void addErlangTrClass(int t, int at, double mu);

    void setMin_a(double value);
    void setMax_a(double value);

    void NewTitle();
    bool fillSystem();
    void readDataBase();

    static ModelTrClass::StreamType DBstrToStreamType(QString str);
    static ModelTrClass::SourceType DBstrToSourceType(QString str);

    void updateAlgorithmsList();

    QSet<Results::Type> getPossibleQoS_Types();

    void updateQoS_ComboBox(QSet<Results::Type> &qos);

    void prepareQoS_ComboBoxex();

    void updateGnuplotActions(QSet<Results::Type> &qos);

private:
    QList<SimulationParameters*> simulationParameters;
    SimulationParameters         *currentSimulationParameter;

    QList<Investigator*>         algorithms;

    Results::RSystem             *resultsForSystem;

    QtCharts::QValueAxis         *axisX;
    QtCharts::QLogValueAxis      *axisYlog;
    QtCharts::QValueAxis         *axisYlinear;

public slots:
    void on_gnuplotSave();

private slots:
    void on_pushButton_wiazkiDodaj_clicked();

    void on_pushButton_wiazkiUsun_clicked();

    void on_pushButton_wiazkiZmien_clicked();

    void on_listWidgetWiazki_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pushButton_kolejkidodaj_clicked();

    void on_listWidgetKolejki_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pushButton_kolejkiUsun_clicked();

    void on_pushButton_kolejkiZmien_clicked();

    void on_pushButtonKlasaDodaj_clicked();

    void on_pushButtonKlasaZmien_clicked();

    void on_pushButtonKlasaUsun_clicked();

    void groupUiParChanged();

    void queueUiParChanged();

    void classUiParChanged();

    void on_listWidgetKlasy_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pushButtonStart_clicked();

    void saveResultsGnuplotE();

    void saveResultsGnuplotLoss();

    void saveResultsGnuplotAvgQeueLen();

    void saveResultsGnuplotAvnNbrCallsInQeue();

    void saveResultsGnuplotEffectiveWaitingTime();

    void saveResultsGnuplotAbsoluteWaitingTime();

    void saveResultsGnuplotYtInQeue();

    void on_listWidgetSimulationParameters_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_Pages_currentChanged(int index);

    void on_listWidgetAlgorithms_itemClicked(QListWidgetItem *item);

    void on_listWidgetAlgorithmsAlternative_itemClicked(QListWidgetItem *item);

    void on_actionAbout_triggered();

    void on_comboBoxPredefinedSystems_currentIndexChanged(int index);

    void on_comboBox_NewCallStrType_currentIndexChanged(int index);

    void on_comboBox_CallServStrType_currentIndexChanged(int index);

    void on_ResultsQtChartRefresh();

    void on_comboBoxResultsQtType_currentIndexChanged(int index);

    void on_comboBoxResultsQtX_axis_currentIndexChanged(int index);



    void on_actionConfigure_triggered();

    void on_comboBoxServerGroupSchedulerAlgorithm_currentIndexChanged(int index);

    void on_comboBoxBufferGroupSchedulerAlgorithm_currentIndexChanged(int index);

    void on_comboBoxSystemSchedulerAlgorithm_currentIndexChanged(int index);

private:
    QVector<ModelCreator *>  vectPredefinedSystems;

    int getNoOfPoints(double aMin, double aMax, double aDelta) const;

    bool dbReadSystems();


    QSqlDatabase   db;            //SQL database
    bool           isDbWorking;

    Ui::MainWindow *ui;

    QTranslator m_translator; // contains the translations for this application
    QTranslator m_translatorQt; // contains the translations for qt
    QString m_currLang; // contains the currently loaded language
    QString m_langPath; // Path of language files. This is always fixed to /languages.

    DialogAbout    *dlgAbout;
    DialogConfig   *dlgConfig;

    ModelCreator      *system;
    GnuplotScript  *scrGnuplot;

    QGraphicsScene *sceneSysModel;

    void drawSystemModel();

    void updateGroupWidgets(void);
    void updateQeueWidgets(void);
    void updateClassWidgets(void);
    QString configureTrClass(ModelTrClass *trClass);


    void saveTheResults(QString &filename, Results::Type QoStype);


    QString ReadFilename(QString extension="ods");


    bool areCalculationsRequired();
protected:
    void changeEvent(QEvent*);

protected slots:
     // this slot is called by the language menu actions
     void slotLanguageChanged(QAction* action);

private:
    // loads a language by the given language shortcur (e.g. de, en)
    void loadLanguage(const QString& rLanguage);

    // creates the language menu dynamically from the content of m_langPath
    void createLanguageMenu(void);
    void addTestedAlgorithm(Investigator *algorithm);
    void addExperimentalAlgorithm(Investigator *algorithm);
    void fillListWidgetWithParams(QListWidget *outList, QLabel *outLabel, Results::ParameterType paramType);
    void clearParameters(Results::ParametersSet &outParameters);
};

#endif // MAINWINDOW_H
