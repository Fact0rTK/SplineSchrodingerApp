#include "dialog_settings.h"
#include "ui_dialog_settings.h"

Dialog_settings::Dialog_settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_settings)
{
    ui->setupUi(this);

    ui->DecimalPoint_Label->setToolTip("How many numbers will be displayed\n"
                                       "after the decimal point");
    ui->Potential_Rmin_Label->setToolTip("The lower limit of the displaying potential");
    ui->Potential_Rmax_Label->setToolTip("The upper limit of the displaying potential");
    ui->Potential_Step_Label->setToolTip("The step between points of the displaying potential");




}

QString Dialog_settings::get_Potential_R_min_Box()
{
    return ui->Potential_R_min_Box->text();
}

QString Dialog_settings::get_Potential_R_max_Box()
{
    return ui->Potential_R_max_Box->text();
}

QString Dialog_settings::get_Potential_Step_Box()
{
    return ui->Potential_Step_Box->text();
}

int Dialog_settings::get_Decimal_point_Box()
{
    return ui->Decimal_point_Box->value();
}



Dialog_settings::~Dialog_settings()
{
    delete ui;
}

void Dialog_settings::on_Potential_R_min_Box_textChanged(const QString &arg1)
{
    emit transfer_Potential_R_min_Box_textChanged(arg1, ui->Potential_R_min_Box);
}


void Dialog_settings::on_Potential_R_max_Box_textChanged(const QString &arg1)
{
    emit transfer_Potential_R_max_Box_textChanged(arg1, ui->Potential_R_max_Box);

}


void Dialog_settings::on_Potential_Step_Box_textChanged(const QString &arg1)
{
    emit transfer_Potential_Step_Box_textChanged(arg1, ui->Potential_Step_Box);
}


void Dialog_settings::on_Decimal_point_Box_valueChanged(int arg1)
{
    emit transfer_Decimal_point_Box_valueChanged(arg1);
}

