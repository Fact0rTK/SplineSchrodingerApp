#include "dialog_result.h"
#include "ui_dialog_result.h"

Dialog_result::Dialog_result(SplineTwoBody_Eigen &Result, int Decimal_points_numbers, int Sci_times_notation, int Units_selected, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_result)
{
    ui->setupUi(this);
    Result.Export_Wave_Functions(Plot_data_x, Wave_functions, Radial_functions, Energies, Size_splined, Size_states);
    this->Decimal_points_numbers = Decimal_points_numbers;
    this->Sci_times_notation = Sci_times_notation;


    SaveFile_Window = new Dialog_SaveFile(Plot_data_x, Wave_functions, Radial_functions, Energies, Size_splined, Size_states, this);

    connect(ui->PlotWavesAndRadial, SIGNAL(mouseMove(QMouseEvent*)), SLOT(MouseMovePlotWaveAndRadial(QMouseEvent*)));
    connect(ui->PlotWavesAndRadial, SIGNAL(mouseWheel(QWheelEvent*)),  SLOT(MouseWheelPlotWaveAndRadial(QWheelEvent*)));


    CoordText = new QCPItemText(ui->PlotWavesAndRadial);
    CoordText->setText(QString("(%1, %2)").arg(0).arg(0));
    CoordText->position->setCoords(QPointF(ui->PlotWavesAndRadial->xAxis->pixelToCoord(50), ui->PlotWavesAndRadial->yAxis->pixelToCoord(-10)));
    CoordText->setFont(QFont(font().family(), 10));


    //Settings for PlotRootsFunction
    ui->PlotWavesAndRadial->axisRect()->setupFullAxesBox();
    ui->PlotWavesAndRadial->xAxis->setRange(-20, 20);
    ui->PlotWavesAndRadial->yAxis->setRange(-1, 1);
    ui->PlotWavesAndRadial->xAxis->setVisible(true);
    ui->PlotWavesAndRadial->yAxis->setVisible(true);

    ui->PlotWavesAndRadial->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->PlotWavesAndRadial->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->PlotWavesAndRadial->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->PlotWavesAndRadial->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->PlotWavesAndRadial->xAxis->grid()->setSubGridVisible(true);
    ui->PlotWavesAndRadial->yAxis->grid()->setSubGridVisible(true);
    ui->PlotWavesAndRadial->xAxis->grid()->setZeroLinePen(Qt::SolidLine);
    ui->PlotWavesAndRadial->yAxis->grid()->setZeroLinePen(Qt::SolidLine);

    ui->PlotWavesAndRadial->addGraph();
    ui->PlotWavesAndRadial->addGraph();
    ui->PlotWavesAndRadial->addGraph();
    ui->PlotWavesAndRadial->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->PlotWavesAndRadial->graph(0)->setLineStyle(QCPGraph::LineStyle(QCPCurve::lsLine));
    ui->PlotWavesAndRadial->graph(0)->setPen(QPen(Qt::blue));
    ui->PlotWavesAndRadial->graph(1)->setLineStyle(QCPGraph::LineStyle(QCPCurve::lsLine));
    ui->PlotWavesAndRadial->graph(1)->setPen(QPen(Qt::blue, 0.5, Qt::DashLine));
    ui->PlotWavesAndRadial->graph(2)->setLineStyle(QCPGraph::LineStyle(QCPCurve::lsLine));
    ui->PlotWavesAndRadial->graph(2)->setPen(QPen(Qt::blue, 0.5, Qt::DashLine));

    ui->PlotWavesAndRadial->xAxis->setLabelFont(QFont("Segoe UI", 12));
    ui->PlotWavesAndRadial->yAxis->setLabelFont(QFont("Segoe UI", 12));
    ui->PlotWavesAndRadial->xAxis->setLabel("r");
    ui->PlotWavesAndRadial->xAxis->setLabelColor(Qt::black);
    ui->PlotWavesAndRadial->yAxis->setLabel("Psi(r)");
    ui->PlotWavesAndRadial->yAxis->setLabelColor(Qt::black);
    ui->PlotWavesAndRadial->rescaleAxes();



    ui->Energy_Select->clear();


    switch(Units_selected)
    {
    case(0):
        ui->PlotWavesAndRadial->xAxis->setLabel("r, fm");
        for(int i = 0; i < Energies.size(); i++)
        {
            ui->Energy_Select->addItem(QString::number(Energies[i]) + " MeV");
        }
        break;
    case(1):
        ui->PlotWavesAndRadial->xAxis->setLabel("r");
        for(int i = 0; i < Energies.size(); i++)
        {
            ui->Energy_Select->addItem(QString::number(Energies[i]));
        }
        break;
    default:
        ui->PlotWavesAndRadial->xAxis->setLabel("r");
        for(int i = 0; i < Energies.size(); i++)
        {
            ui->Energy_Select->addItem(QString::number(Energies[i]));
        }
        break;
    }

    ui->PlotWavesAndRadial->graph(0)->setData(Plot_data_x, Wave_functions[0]);

    ui->PlotWavesAndRadial->replot();
}

void Dialog_result::Replot_Graph()
{
    ui->PlotWavesAndRadial->graph(0)->data()->clear();
    if(ui->Switch_Wave_Radial->isChecked())
    {
        ui->PlotWavesAndRadial->graph(0)->setData(Plot_data_x, Wave_functions[ui->Energy_Select->currentIndex()]);
        ui->PlotWavesAndRadial->yAxis->setLabel("Psi(r)");
    }
    else
    {
        ui->PlotWavesAndRadial->graph(0)->setData(Plot_data_x, Radial_functions[ui->Energy_Select->currentIndex()]);
        ui->PlotWavesAndRadial->yAxis->setLabel("Psi(r)/r");
    }

    ui->PlotWavesAndRadial->replot();
}


Dialog_result::~Dialog_result()
{
    delete ui;
    delete SaveFile_Window;
    for(int i = 0; i < Size_states; i++)
    {
        Wave_functions[i].clear();
        Radial_functions[i].clear();
    }
    Wave_functions.clear();
    Radial_functions.clear();
    Energies.clear();
    Plot_data_x.clear();
    Plot_data_y.clear();
}

void Dialog_result::DrawCrossAndCoord(QPoint Cursor)
{
    ui->PlotWavesAndRadial->graph(1)->data()->clear();
    ui->PlotWavesAndRadial->graph(2)->data()->clear();
    QVector<double> xData;
    QVector<double> yData;
    xData.append(ui->PlotWavesAndRadial->xAxis->pixelToCoord(Cursor.x()));
    yData.append(ui->PlotWavesAndRadial->yAxis->pixelToCoord(0));

    xData.append(ui->PlotWavesAndRadial->xAxis->pixelToCoord(Cursor.x()));
    yData.append(ui->PlotWavesAndRadial->yAxis->pixelToCoord(ui->PlotWavesAndRadial->height()));
    ui->PlotWavesAndRadial->graph(1)->setData(xData, yData);

    xData.clear();
    yData.clear();

    xData.append(ui->PlotWavesAndRadial->xAxis->pixelToCoord(0));
    yData.append(ui->PlotWavesAndRadial->yAxis->pixelToCoord(Cursor.y()));

    xData.append(ui->PlotWavesAndRadial->xAxis->pixelToCoord(ui->PlotWavesAndRadial->width()));
    yData.append(ui->PlotWavesAndRadial->yAxis->pixelToCoord(Cursor.y()));

    ui->PlotWavesAndRadial->graph(2)->addData(xData, yData);

    double coord_x = ui->PlotWavesAndRadial->xAxis->pixelToCoord(Cursor.x());
    double coord_y = ui->PlotWavesAndRadial->yAxis->pixelToCoord(Cursor.y());
    std::string coord_x_formatted;
    std::string coord_y_formatted;
    GetFormattedNumber(coord_x, coord_x_formatted);
    GetFormattedNumber(coord_y, coord_y_formatted);
    CoordText->setText(QString("(%1, %2)").arg(coord_x_formatted).arg(coord_y_formatted));
    CoordText->position->setCoords(QPointF( ui->PlotWavesAndRadial->xAxis->pixelToCoord(Cursor.x() + 50),  ui->PlotWavesAndRadial->yAxis->pixelToCoord(Cursor.y()-10)));
    CoordText->setFont(QFont(font().family(), 10));
    ui->PlotWavesAndRadial->replot();
}



void Dialog_result::MouseMovePlotWaveAndRadial(QMouseEvent *event)
{
    DrawCrossAndCoord(event->pos());
}

void Dialog_result::MouseWheelPlotWaveAndRadial(QWheelEvent *event)
{
    DrawCrossAndCoord(event->position().toPoint());
}


void Dialog_result::on_Switch_Wave_Radial_stateChanged(int arg1)
{
    Replot_Graph();
}


void Dialog_result::on_Energy_Select_currentIndexChanged(int index)
{
    Replot_Graph();
}

void Dialog_result::on_FlipFunctionsButton_clicked()
{
    for(int i = 0; i < Size_states; i++)
    {
        for(int j = 0; j < Size_splined; j++)
        {
            Wave_functions[i][j] = Wave_functions[i][j]*(-1);
            Radial_functions[i][j] = Radial_functions[i][j]*(-1);
        }
    }
    Replot_Graph();
}

void Dialog_result::on_SaveFileButton_clicked()
{
    if(SaveFile_Window->isHidden())
    {
        SaveFile_Window->show();
    }
}

void Dialog_result::GetFormattedNumber(double Value, std::string &FormattedNumber)
{
    if(abs(Value) > Sci_times_notation)
    {
        FormattedNumber = std::string(std::format("{:.{}E}", Value, Decimal_points_numbers));
    }
    else
    {
        FormattedNumber = std::string(std::format("{:.{}f}", Value, Decimal_points_numbers));
    }
}
