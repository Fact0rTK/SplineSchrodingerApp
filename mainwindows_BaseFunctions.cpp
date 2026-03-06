#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::Set_Graph_Style(QCustomPlot *Graph)
{
    Graph->axisRect()->setupFullAxesBox();
    Graph->xAxis->setRange(-20, 20);
    Graph->yAxis->setRange(-20, 20);
    Graph->xAxis->setLabel("Re(k)");
    Graph->yAxis->setLabel("Im(k)");
    Graph->xAxis->setVisible(true);
    Graph->yAxis->setVisible(true);
    Graph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    Graph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    Graph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    Graph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    Graph->xAxis->grid()->setSubGridVisible(true);
    Graph->yAxis->grid()->setSubGridVisible(true);
    Graph->xAxis->grid()->setZeroLinePen(Qt::SolidLine);
    Graph->yAxis->grid()->setZeroLinePen(Qt::SolidLine);
}

void MainWindow::Check_container(bool &Success, QLineEdit *_LineEdit, const QString &Container, double &Value)
{
    Container.toDouble(&Success);
    if(!Success || Container.isEmpty())
    {
        _LineEdit->setStyleSheet("QLineEdit { border: 1px solid; border-color: rgb(0, 0, 0); border-radius: 2px; background-color: red; color: black;}");;
    }
    else
    {
        Value = Container.toDouble(&Success);
        _LineEdit->setStyleSheet("QLineEdit { border: 1px solid; border-color: rgb(0, 0, 0); border-radius: 2px; background-color: white; color: black;}");;
    }
}

void MainWindow::Set_button_activity(QPushButton *Button, QVector<bool> Success)
{
    bool active = true;
    for(int i =0; i < Success.size(); i++)
    {
        active = active && Success[i];
    }
    if(active)
    {
        Button->setEnabled(true);
    } else
    {
        Button->setEnabled(false);
    }
}

void MainWindow::Set_button_activity(QPushButton *Button, bool Success)
{
    bool active = true;
    active = active && Success;
    if(active)
    {
        Button->setEnabled(true);
    } else
    {
        Button->setEnabled(false);
    }
}

void MainWindow::GetFormattedNumber(double Value, std::string &FormattedNumber)
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

void MainWindow::CalculateRAMNeeded()
{
    double RAM = ((Calc_settings_R_max_d - Calc_settings_R_min_d)/Calc_settings_h_step_d)*((Calc_settings_R_max_d - Calc_settings_R_min_d)/Calc_settings_h_step_d)*2
                 + (Calc_settings_R_max_d - Calc_settings_R_min_d)/Calc_settings_h_spline_step_d*(Calc_settings_R_max_d - Calc_settings_R_min_d)/Calc_settings_h_step_d*2;
    RAM = RAM*8/1024/1024;
    RAM = 1.0*int(RAM * 1000)/1000;
    ui->SizeOfRAMNeeded_Label->setText("Estimated (max)\n" + QString::number(RAM) + " MB of RAM");
}

void MainWindow::on_actionSettings_triggered()
{
    if(Settings_Window->isHidden())
    {
        Settings_Window->show();
    }
}

void MainWindow::on_Calculate_Button_clicked()
{
    ui->Calculate_Button->setEnabled(false);
    bool SuccessCalc = false;
    SplineTwoBody_Eigen Task;
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat("Init memory");
    Task.Initialize_Param(Calc_settings_R_min_d, Calc_settings_R_max_d, Calc_settings_h_step_d, Calc_settings_h_spline_step_d, ui->UnitsType_Select->currentIndex(), Calc_settings_Mass1_d, Calc_settings_Mass2_d, ui->Potential_Equation_Box->text().toStdString());

    SuccessCalc = Task.Calculate_on_CPU(Calc_settings_l_d, ui->progressBar);
    if(SuccessCalc)
    {
        Result_Window = new Dialog_result(Task, Decimal_points_numbers, Sci_times_notation, ui->UnitsType_Select->currentIndex(), this);
        Result_Window->setAttribute(Qt::WA_DeleteOnClose);
        Result_Window->show();
    }
    else
    {
        std::string message = "There is no bound states or the parameters are wrong.\n"
                              "The lowest bound state is E = " + std::to_string(Task.GetGroundState());
        QMessageBox::warning(this, "Error", tr(message.c_str()), QMessageBox::Ok);
    }
    ui->Calculate_Button->setEnabled(true);
}

void MainWindow::DrawCrossAndCoord(QPoint Cursor)
{
    ui->PlotPotential->graph(1)->data()->clear();
    ui->PlotPotential->graph(2)->data()->clear();
    QVector<double> xData;
    QVector<double> yData;
    xData.append(ui->PlotPotential->xAxis->pixelToCoord(Cursor.x()));
    yData.append(ui->PlotPotential->yAxis->pixelToCoord(0));

    xData.append(ui->PlotPotential->xAxis->pixelToCoord(Cursor.x()));
    yData.append(ui->PlotPotential->yAxis->pixelToCoord(ui->PlotPotential->height()));
    ui->PlotPotential->graph(1)->setData(xData, yData);

    xData.clear();
    yData.clear();

    xData.append(ui->PlotPotential->xAxis->pixelToCoord(0));
    yData.append(ui->PlotPotential->yAxis->pixelToCoord(Cursor.y()));

    xData.append(ui->PlotPotential->xAxis->pixelToCoord(ui->PlotPotential->width()));
    yData.append(ui->PlotPotential->yAxis->pixelToCoord(Cursor.y()));

    ui->PlotPotential->graph(2)->addData(xData, yData);

    double coord_x = ui->PlotPotential->xAxis->pixelToCoord(Cursor.x()+70);
    double coord_y = ui->PlotPotential->yAxis->pixelToCoord(Cursor.y()+10);
    std::string coord_x_formatted;
    std::string coord_y_formatted;
    GetFormattedNumber(coord_x, coord_x_formatted);
    GetFormattedNumber(coord_y, coord_y_formatted);
    CoordText->setText(QString("(%1, %2)").arg(coord_x_formatted).arg(coord_y_formatted));
    CoordText->position->setCoords(QPointF( ui->PlotPotential->xAxis->pixelToCoord(Cursor.x() + 50), ui->PlotPotential->yAxis->pixelToCoord(Cursor.y()-10)));
    CoordText->setFont(QFont(font().family(), 10));
    ui->PlotPotential->replot();
}

void MainWindow::MouseMovePlotPotential(QMouseEvent *event)
{
    DrawCrossAndCoord(event->pos());
}

void MainWindow::MouseWheelPlotPotential(QWheelEvent *event)
{
    DrawCrossAndCoord(event->position().toPoint());
}
