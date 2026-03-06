#ifndef DIALOG_RESULT_H
#define DIALOG_RESULT_H

#include <QDialog>

#include "qcustomplot.h"
#include "splinetwobody_eigen.h"
#include "dialog_savefile.h"

namespace Ui {
class Dialog_result;
}

class Dialog_result : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_result(SplineTwoBody_Eigen &Result, int Decimal_points_numbers, int Sci_times_notation, int Units_selected, QWidget *parent = nullptr);
    ~Dialog_result();

private slots:
    void on_Switch_Wave_Radial_stateChanged(int arg1);

    void on_Energy_Select_currentIndexChanged(int index);

    void MouseMovePlotWaveAndRadial(QMouseEvent *event);

    void MouseWheelPlotWaveAndRadial(QWheelEvent *event);

    void on_FlipFunctionsButton_clicked();

    void on_SaveFileButton_clicked();

private:
    Ui::Dialog_result *ui;
    Dialog_SaveFile *SaveFile_Window;

    QCPItemText *CoordText;
    QVector<QVector<double>> Wave_functions;
    QVector<QVector<double>> Radial_functions;
    QVector<double> Energies;
    QVector<double> Plot_data_x;
    QVector<double> Plot_data_y;
    int Size_splined;
    int Size_states;

    int Decimal_points_numbers;
    int Sci_times_notation;

    void Replot_Graph();

    void DrawCrossAndCoord(QPoint Cursor);
    void GetFormattedNumber(double Value, std::string &FormattedNumber);

};

#endif // DIALOG_RESULT_H
