#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFrame>
#include <math.h>
#include <cmath>
#include <complex>
#include <QDebug>
#include <QCloseEvent>
#include <cstdio>
#include "qcustomplot.h"
#include <iostream>
#include <format>
#include "exprtk_complex_adaptor.hpp"
#include "dialog_settings.h"
#include "dialog_result.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    void closeEvent(QCloseEvent *event);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void receive_Potential_R_min_Box_textChanged(QString arg1, QLineEdit *_LineEdit);

    void receive_Potential_R_max_Box_textChanged(QString arg1, QLineEdit *_LineEdit);

    void receive_Potential_Step_Box_textChanged(QString arg1, QLineEdit *_LineEdit);

    void receive_Decimal_point_Box_valueChanged(int arg1);

private slots:
    void on_Potential_Equation_Box_textChanged(const QString &arg1);

    void on_actionSettings_triggered();

    void on_Calc_settings_R_min_Box_textChanged(const QString &arg1);

    void on_Calc_settings_R_max_Box_textChanged(const QString &arg1);

    void on_Calc_settings_Step_Box_textChanged(const QString &arg1);

    void on_Calc_settings_SplineStep_Box_textChanged(const QString &arg1);

    void on_Calc_settings_Mass1_Box_textChanged(const QString &arg1);

    void on_Calc_settings_Mass2_Box_textChanged(const QString &arg1);

    void on_Calc_settings_l_Box_textChanged(const QString &arg1);

    void on_Calculate_Button_clicked();

    void MouseMovePlotPotential(QMouseEvent *event);

    void MouseWheelPlotPotential(QWheelEvent *event);
private:
    Ui::MainWindow *ui;
    Dialog_settings *Settings_Window;
    Dialog_result *Result_Window;
    QCPItemText *CoordText;

    //Calc settings/////////////////////////////////
    double Calc_settings_R_max_d;
    double Calc_settings_R_min_d;
    double Calc_settings_h_step_d;
    double Calc_settings_h_spline_step_d;
    double Calc_settings_l_d;
    double Calc_settings_Mass1_d;
    double Calc_settings_Mass2_d;

    QVector<bool> Calc_settings_success;
    //Calc_settings_R_min_d[0];
    //Calc_settings_R_max_d[1];
    //Calc_settings_h_step_d[2];
    //Calc_settings_h_spline_step_d[3];
    //Calc_settings_l_d[4];
    //Calc_settings_mass1_d[5];
    //Calc_settings_mass2_d[6];



    //Potential settings////////////////////////////
    double Potential_R_max_d;
    double Potential_R_min_d;
    double Potential_Step_d;

    QVector<double> DataPotentialPoints_x, DataPotentialPoints_y;
    QVector<bool> Potential_settings_success;
    //Potential_Rmax_d_success = [0]
    //Potential_Rmin_d_success = [1]
    //Potential_Step_d_success = [2]


    //Potential/////////////////////////////////////
    QString Potential_Equation_s;
    bool Potential_success;



    //Misc/////////////////////////////////////////
    int Decimal_points_numbers;
    int Sci_times_notation;

    ///////////////////////////////////////////////
    void Check_container(bool &Success, QLineEdit *_LineEdit, const QString &Container, double &Value);
    void Potential_Equation_Parse();
    void Set_Graph_Style(QCustomPlot *Graph);
    void Set_button_activity(QPushButton *Button, QVector<bool> Success);
    void Set_button_activity(QPushButton *Button, bool Success);
    void CalculateRAMNeeded();
    void DrawCrossAndCoord(QPoint Cursor);
    void GetFormattedNumber(double Value, std::string &FormattedNumber);

};
#endif // MAINWINDOW_H
