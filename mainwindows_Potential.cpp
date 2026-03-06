#include "mainwindow.h"
#include "./ui_mainwindow.h"


void MainWindow::Potential_Equation_Parse()
{
    if(Potential_settings_success[0] && Potential_settings_success[1] && Potential_settings_success[2])
    {
        double r;
        int N = (Potential_R_max_d - Potential_R_min_d)/Potential_Step_d + 1;
        std::string expression = ui->Potential_Equation_Box->text().toStdString();
        exprtk::symbol_table<double> Potential_Equation_Table;
        exprtk::expression<double> Potential_Equation_Expression;
        exprtk::parser<double> Potential_Equation_Parser;

        Potential_Equation_Parser.settings().disable_all_control_structures();
        Potential_Equation_Parser.settings().disable_all_logic_ops();
        Potential_Equation_Table.add_variable("r", r);
        Potential_Equation_Expression.register_symbol_table(Potential_Equation_Table);
        if(Potential_Equation_Parser.compile(expression, Potential_Equation_Expression))
        {
            Potential_success = true;
            Potential_Equation_s = ui->Potential_Equation_Box->text();
            //Potential_Equation_Table.add_constants();
            ui->PlotPotential->graph(0)->data()->clear();
            DataPotentialPoints_x.clear();
            DataPotentialPoints_y.clear();
            ui->Potential_Equation_Box->setStyleSheet("QLineEdit { border: 1px solid; border-color: rgb(0, 0, 0); border-radius: 2px; background-color: white; color: black;}");
            for(int i = 0; i < N; i++)
            {
                DataPotentialPoints_x.append(Potential_R_min_d + i*Potential_Step_d);
            }
            for(int i = 0; i < DataPotentialPoints_x.size(); i++)
            {
                r = DataPotentialPoints_x[i];
                DataPotentialPoints_y.append(Potential_Equation_Expression.value());
            }
            ui->PlotPotential->graph(0)->setData(DataPotentialPoints_x, DataPotentialPoints_y);
            ui->PlotPotential->replot();
        }
        else
        {
            Potential_success = false;
            ui->Potential_Equation_Box->setStyleSheet("QLineEdit { border: 1px solid; border-color: rgb(0, 0, 0); border-radius: 2px; background-color: red; color: black;}");
        }
        Set_button_activity(ui->Calculate_Button, Potential_success);
    }
}
