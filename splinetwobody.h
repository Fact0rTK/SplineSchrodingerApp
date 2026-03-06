#define _USE_MATH_DEFINES
#ifndef SPLINETWOBODY_H
#define SPLINETWOBODY_H

#include <iostream>
#include <cmath>
#include <fstream>
#include <QVector>
#include <QDebug>
//#include <msclr\marshal_cppstd.h>
#include "exprtk_complex_adaptor.hpp"


class SplineTwoBody
{
private:
    double* AH;
    double* Energies;
    double* m_coeff;
    double* Radial_Functions;
    double* Wave_Functions;

    double rmax, rmin, h, h_spl;
    int matrix_size;
    int bound_states_size;
    int Wave_and_Radial_size;

    double k_fmMeV = 2.0; //0.04784506682;
    double mass = 1.0;
    double mass1 = 1.0;
    double mass2 = 1.0;

    exprtk::symbol_table<double> Potential_Equation_Table;
    exprtk::expression<double> Potential_Equation_Expression;
    exprtk::parser<double> Potential_Equation_Parser;
    double Potential_Equation_R;


public:
    SplineTwoBody()
    {

    }

    void Initialize_Param(double R_min, double R_max, double accuracy, double accuracy_spl, double b0, double m1, double m2, std::string expression)
    {
        this->h = accuracy;
        this->h_spl = accuracy_spl;
        this->rmax = R_max;
        this->rmin = R_min;
        this->k_fmMeV = b0;
        this->mass1 = m1;
        this->mass2 = m2;
        this->mass = (m1*m2)/(m1+m2);

        this->matrix_size = (rmax - rmin) / h - 1;
        this->Wave_and_Radial_size = int((rmax - rmin) / h_spl + 1);
        this->AH = new double[matrix_size*matrix_size];

        Potential_Equation_Parser.settings().disable_all_control_structures();
        Potential_Equation_Parser.settings().disable_all_logic_ops();
        Potential_Equation_Table.add_variable("r", Potential_Equation_R);
        Potential_Equation_Expression.register_symbol_table(Potential_Equation_Table);
        Potential_Equation_Parser.compile(expression, Potential_Equation_Expression);


    }

    bool Calculate_on_CPU(int l)
    {
        double* TEMP_Energy = new double[matrix_size];
        std::string Stroka;
        Initialize_Matrix_AH();
        Calculate_Matrix_B(AH, l);
        RotateMethod(AH, TEMP_Energy, matrix_size, 1e-10);
        filter(AH, TEMP_Energy, matrix_size);
        bound_states_size = matrix_size;
        for(int i = 0; i < matrix_size; i++)
        {
            if(TEMP_Energy[i] > 0)
            {
                bound_states_size = i;
                break;
            }
        }
        this->Energies = new double [bound_states_size];
        this->m_coeff = new double [bound_states_size*(matrix_size+2)];
        for(int i = 0; i < bound_states_size; i++)
        {
            Energies[i] = TEMP_Energy[i];
        }
        delete[] TEMP_Energy;
        if(Energies[0] < 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void Calculate_Wave_Functions()
    {
        this->Radial_Functions = new double [bound_states_size*Wave_and_Radial_size];
        this->Wave_Functions = new double [bound_states_size*Wave_and_Radial_size];
        //     try {
        // } catch (const std::exception& e) {
        //     std::cerr << "Cannot allocate memory for splined function" << e.what() << std::endl;
        //     return false;
        // }
        double* VectorToSplain = new double[matrix_size + 2];
        double* r = new double[matrix_size + 2];
        VectorToSplain[0] = 0;
        VectorToSplain[matrix_size + 1] = 0;
        for (int j = 0; j < (matrix_size + 2); j++)
        {
            r[j] = rmin + j * h;
        }
        for (int i = 0; i < bound_states_size; i++)
        {
            for (int j = 0; j < matrix_size; j++)
            {
                VectorToSplain[j + 1] = AH[j * matrix_size + i];
            }
            MethodSplainNatural(r, VectorToSplain, matrix_size + 2, h_spl, false, Wave_Functions, i);
            MethodSplainNatural(r, VectorToSplain, matrix_size + 2, h_spl, true, Radial_Functions, i);
        }

        delete[] VectorToSplain;
        delete[] r;
    }

    void Export_Wave_Functions(QVector<double> &Exported_x_axis , QVector<QVector<double>> &Exported_Wave_functions, QVector<QVector<double>> &Exported_Radial_functions, QVector<double> &Exported_Energies, int &Exported_size_splined, int &Exported_size_states)
    {
        Exported_Radial_functions.clear();
        Exported_Wave_functions.clear();
        Exported_Energies.clear();

        Exported_size_states = bound_states_size;
        Exported_size_splined = Wave_and_Radial_size;

        for(int i = 0; i < Wave_and_Radial_size; i++)
        {
            Exported_x_axis.append(rmin + i*h_spl);
        }
        QVector<double> Wave;
        QVector<double> Radial;
        for (int i = 0; i < bound_states_size; i++)
        {
            Wave.clear();
            Radial.clear();
            for (int j = 0; j < Wave_and_Radial_size; j++)
            {
                Wave.append(Wave_Functions[j + i*Wave_and_Radial_size]);
                Radial.append(Radial_Functions[j + i*Wave_and_Radial_size]);
            }
            Exported_Wave_functions.append(Wave);
            Exported_Radial_functions.append(Radial);
        }
        for (int i = 0; i < bound_states_size; i++)
        {
            Exported_Energies.append(Energies[i]);
        }
    }

    double Get_GroundEnergy()
    {
        return Energies[0];
    }
    void Show_WaveFunctions()
    {
        for (int i = 0; i < bound_states_size; i++)
        {
            QString stroka = "";
            for (int j = 0; j < Wave_and_Radial_size; j++)
            {
                stroka = stroka + " " + QString::number(Wave_Functions[j + i*Wave_and_Radial_size]);
            }
            qDebug() << stroka;
        }
        qDebug() << " ";
    }


    ~SplineTwoBody()
    {
        delete[] AH;
        delete[] Energies;
        delete[] m_coeff;
        delete[] Radial_Functions;
        delete[] Wave_Functions;
    }



private:

    //Calculation of Matrices and Potential
    void Initialize_Matrix_AH();
    void Calculate_Matrix_B(double* A1, double l);
    double Potential(double r, double l);


    //Methods
    void ProgonkaRIGHT(unsigned int n, double* a, double* b, double* x, int index);
    void RotateMethodMultipleALeft(double* A, int n, int i_r, int j_r, double phi, double* TEMP);
    void RotateMethodMultipleARight(double* A, int n, int i_r, int j_r, double phi, double* TEMP);
    void RotateMethod(double* A_1, double* Energy, int n, double eps);
    double find_max_MOD(double* A, int n);
    void filter(double* X, double* A, int n);
    void MethodSplainNatural(double* X, double* Y, int size, double esp, bool IsRadial, double *Data, int p);
    void ProgonkaRIGHT_SPLAINMOD(unsigned int n, double* a, double* b, double* x);
    double MethodSplainNatural_Radius(double* X, double* Y, int size);
public:






};

#endif // SPLINETWOBODY_H
