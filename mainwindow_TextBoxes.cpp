#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::on_Potential_Equation_Box_textChanged(const QString &arg1)
{
    Potential_Equation_Parse();
}


void MainWindow::on_Calc_settings_R_min_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[0], ui->Calc_settings_R_min_Box, arg1, Calc_settings_R_min_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
    CalculateRAMNeeded();
}

void MainWindow::on_Calc_settings_R_max_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[1], ui->Calc_settings_R_max_Box, arg1, Calc_settings_R_max_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
    CalculateRAMNeeded();
}

void MainWindow::on_Calc_settings_Step_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[2], ui->Calc_settings_Step_Box, arg1, Calc_settings_h_step_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
    CalculateRAMNeeded();
}

void MainWindow::on_Calc_settings_SplineStep_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[3], ui->Calc_settings_SplineStep_Box, arg1, Calc_settings_h_spline_step_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
    CalculateRAMNeeded();
}

void MainWindow::on_Calc_settings_l_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[4], ui->Calc_settings_l_Box, arg1, Calc_settings_l_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
}

void MainWindow::on_Calc_settings_Mass1_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[5], ui->Calc_settings_Mass1_Box, arg1, Calc_settings_Mass1_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
}

void MainWindow::on_Calc_settings_Mass2_Box_textChanged(const QString &arg1)
{
    Check_container(Calc_settings_success[6], ui->Calc_settings_Mass2_Box, arg1, Calc_settings_Mass2_d);
    Set_button_activity(ui->Calculate_Button, Calc_settings_success);
}



void MainWindow::receive_Potential_R_max_Box_textChanged(QString arg1, QLineEdit *_LineEdit)
{
    Check_container(Potential_settings_success[0], _LineEdit, arg1, Potential_R_max_d);
    Set_button_activity(ui->Calculate_Button, Potential_settings_success);
    Potential_Equation_Parse();
}

void MainWindow::receive_Potential_R_min_Box_textChanged(QString arg1, QLineEdit *_LineEdit)
{
    Check_container(Potential_settings_success[1], _LineEdit, arg1, Potential_R_min_d);
    Set_button_activity(ui->Calculate_Button, Potential_settings_success);
    Potential_Equation_Parse();
}

void MainWindow::receive_Potential_Step_Box_textChanged(QString arg1, QLineEdit *_LineEdit)
{
    Check_container(Potential_settings_success[2], _LineEdit, arg1, Potential_Step_d);
    Set_button_activity(ui->Calculate_Button, Potential_settings_success);
    Potential_Equation_Parse();
}

void MainWindow::receive_Decimal_point_Box_valueChanged(int arg1)
{
    Decimal_points_numbers = arg1;
}

