#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "splinetwobody_eigen.h"
#include "dialog_settings.h"
#include "dialog_result.h"
#include "dialog_savefile.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Settings_Window = new Dialog_settings(this);

    //Description of the variables
    ui->Rmin_Label->setToolTip("The lower limit of the calculation mesh");
    ui->Rmax_Label->setToolTip("The upper limit of the calculation mesh");
    ui->Step_Label->setToolTip("The step of the calculation mesh");
    ui->StepSpline_Label->setToolTip("The step of spline interpolation.\n"
                                     "It must be less than the step of the mesh");
    ui->l_Label->setToolTip("The orbital quantum number");
    ui->m1_Label->setToolTip("The mass of the first particle");
    ui->m2_Label->setToolTip("The mass of the second particle");
    ui->Units_Label->setToolTip("Units selection.\n"
                             "For units (MeV|fm|a.m.u.), it equals to 0.4784506...\n"
                             "For atomic units (1|1|1) it equals to 2");
    ui->Potential_Label->setToolTip("Potential input.\n"
                                    "* - for multiplication.\n"
                                    "^ - for raising a number to a power.\n"
                                    "/ - for division.\n"
                                    "+/- - for addition/substraction.\n"
                                    "It also supports functions like erf(), abs(), sin(), cos() etc.");


    //Units select add types
    ui->UnitsType_Select->addItem("MeV|fm|a.m.u.");
    ui->UnitsType_Select->addItem("Atomic units");


    connect(Settings_Window, &Dialog_settings::transfer_Potential_R_min_Box_textChanged, this, &MainWindow::receive_Potential_R_min_Box_textChanged);
    connect(Settings_Window, &Dialog_settings::transfer_Potential_R_max_Box_textChanged, this, &MainWindow::receive_Potential_R_max_Box_textChanged);
    connect(Settings_Window, &Dialog_settings::transfer_Potential_Step_Box_textChanged, this, &MainWindow::receive_Potential_Step_Box_textChanged);
    connect(ui->PlotPotential, SIGNAL(mouseMove(QMouseEvent*)), SLOT(MouseMovePlotPotential(QMouseEvent*)));
    connect(ui->PlotPotential, SIGNAL(mouseWheel(QWheelEvent*)),  SLOT(MouseWheelPlotPotential(QWheelEvent*)));



    //Create and fill Success flags for LineEdit boxes
    Calc_settings_success.resize(8);
    Calc_settings_success.fill(true);
    Potential_settings_success.resize(3);
    Potential_settings_success.fill(true);
    Potential_success = true;

    //Set to zero progress bar
    ui->progressBar->setValue(0);

    //Main Window
    Calc_settings_h_step_d= ui->Calc_settings_Step_Box->text().toDouble();
    Calc_settings_R_min_d= ui->Calc_settings_R_min_Box->text().toDouble();
    Calc_settings_R_max_d =ui->Calc_settings_R_max_Box->text().toDouble();
    Calc_settings_h_spline_step_d= ui->Calc_settings_SplineStep_Box->text().toDouble();
    Calc_settings_Mass1_d= ui->Calc_settings_Mass1_Box->text().toDouble();
    Calc_settings_Mass2_d= ui->Calc_settings_Mass2_Box->text().toDouble();
    Calc_settings_l_d = ui->Calc_settings_l_Box->text().toDouble();
    Potential_Equation_s = ui->Potential_Equation_Box->text();

    //Settings Window
    Potential_R_max_d = Settings_Window->get_Potential_R_max_Box().toDouble();
    Potential_R_min_d = Settings_Window->get_Potential_R_min_Box().toDouble();
    Potential_Step_d = Settings_Window->get_Potential_Step_Box().toDouble();
    Decimal_points_numbers = Settings_Window->get_Decimal_point_Box();
    Sci_times_notation = 1e4;

    //!!!!!!!!!!!!!!!!!
    //Settings file
    //if()
    //!!!!!!!!!!!!!!!!!


    //Settings for PlotPotential
    Set_Graph_Style(ui->PlotPotential);
    ui->PlotPotential->addGraph();
    ui->PlotPotential->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->PlotPotential->graph(0)->setLineStyle(QCPGraph::LineStyle(QCPCurve::lsLine));
    ui->PlotPotential->graph(0)->setPen(QPen(Qt::blue));
    ui->PlotPotential->xAxis->setLabelFont(QFont("Segoe UI", 12));
    ui->PlotPotential->yAxis->setLabelFont(QFont("Segoe UI", 12));
    ui->PlotPotential->xAxis->setLabel("r");
    ui->PlotPotential->xAxis->setLabelColor(Qt::black);
    ui->PlotPotential->yAxis->setLabel("U(r)");
    ui->PlotPotential->yAxis->setLabelColor(Qt::black);
    ui->PlotPotential->rescaleAxes();
    Potential_Equation_Parse();

    //Settings for Display Coordinates
    ui->PlotPotential->addGraph();
    ui->PlotPotential->addGraph();
    ui->PlotPotential->graph(1)->setLineStyle(QCPGraph::LineStyle(QCPCurve::lsLine));
    ui->PlotPotential->graph(1)->setPen(QPen(Qt::blue, 0.5, Qt::DashLine));
    ui->PlotPotential->graph(2)->setLineStyle(QCPGraph::LineStyle(QCPCurve::lsLine));
    ui->PlotPotential->graph(2)->setPen(QPen(Qt::blue, 0.5, Qt::DashLine));
    CoordText = new QCPItemText(ui->PlotPotential);
    CoordText->setText(QString("(%1, %2)").arg(0).arg(0));
    CoordText->position->setCoords(QPointF(ui->PlotPotential->xAxis->pixelToCoord(50), ui->PlotPotential->yAxis->pixelToCoord(-10)));
    CoordText->setFont(QFont(font().family(), 10));

    //Calculate needed RAM (max)
    CalculateRAMNeeded();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete Settings_Window;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ExitMessageBox = QMessageBox::question( this, "Exiting the app",
                                                               tr("Are you sure?\n"),
                                                               QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    switch (ExitMessageBox){
        case(QMessageBox::Yes):
            event->accept();
        break;
        case(QMessageBox::No):
            event->ignore();
            break;
        default:
            event->accept();
            break;
        }
}
