#include "splinetwobody.h"

double SplineTwoBody::Potential(double r, double l)
{
    //return Potential_Equation_Expression.value();
    Potential_Equation_R = r;
    double Value = Potential_Equation_Expression.value();
    return Value;
    //return -7.5 * r * r * exp(-r) + l*(l+1.0)/(r*r);
}

void SplineTwoBody::Initialize_Matrix_AH()
{
    double* A_1f = new double [matrix_size*matrix_size];
    double* E = new double[matrix_size];

    for (int i = 0; i < matrix_size*matrix_size; i++)
    {
        AH[i] = 0;
    }
    for (int i = 0; i < matrix_size - 1; i++)
    {
        AH[matrix_size*i+i] = -2 * h / 3.0;
        AH[matrix_size*(i + 1) + i] = -h / 6.0;
        AH[matrix_size*i + i + 1] = -h / 6.0;
    }
    AH[(matrix_size)*(matrix_size - 1) + matrix_size - 1] = -2 * h / 3.0;

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            if (i == j)
            {
                E[j] = 1;
            }
            else
            {
                E[j] = 0;
            }
        }
        SplineTwoBody::ProgonkaRIGHT(matrix_size, AH, E, A_1f, i);
    }

    //Умножение на трёхдиагональную матрицу H
    for (int i = 0; i < matrix_size; i++)
    {
        AH[matrix_size*i + 0] = A_1f[matrix_size*i+0] * (-2.0 / h) + A_1f[matrix_size*i + 1] * 1.0 / h;
        for (int j = 1; j < matrix_size-1; j++)
        {
            AH[matrix_size * i + j] = A_1f[matrix_size*i + j - 1]* 1.0 / h + A_1f[matrix_size * i + j]* (-2.0 / h) + A_1f[matrix_size * i + j + 1] * 1.0 / h;
        }
        AH[matrix_size * i + matrix_size - 1] = A_1f[matrix_size * i + matrix_size - 1] * (-2.0 / h) + A_1f[matrix_size * i + matrix_size - 2] * 1.0 / h;
    }
    //-----------------------------------------

    delete[] A_1f;
    delete[]E;
}

void SplineTwoBody::Calculate_Matrix_B(double* A1, double l)
{
    for (int i = 0; i < matrix_size; i++)
    {
        //A1[i][i] = A1[i][i] + k_fmMeV * mass_neutron * Nuclei::WS_and_SO_PotentialN(rmin + (i + 1.0) * h, l, jplus) + l * (l + 1) / (rmin + (i + 1) * h) / (rmin + (i + 1) * h);
        A1[matrix_size * i + i] = A1[matrix_size * i + i] + k_fmMeV * mass * Potential(rmin + 1.0*(i + 1) * h, l) + l * (l + 1) / (rmin + (i + 1) * h) / (rmin + (i + 1) * h);
    }
}

void SplineTwoBody::ProgonkaRIGHT(unsigned int n, double* a, double* b, double* x, int index)
{
    double* alfa = new double[n];
    double* beta = new double[n];
    alfa[0] = -a[1] / a[0];
    beta[0] = b[0] / a[0];
    for (int i = 1; i < n; i++)
    {
        alfa[i] = -a[n * i + i + 1] / (a[n * i + i - 1] * alfa[i - 1] + a[n * i + i]);
        beta[i] = (b[i] - a[n * i + i - 1] * beta[i - 1]) / (a[n * i + i - 1] * alfa[i - 1] + a[n * i + i]);
    }
    x[n*(n - 1) + index] = beta[n - 1];
    for (int i = n - 2; i >= 0; i--)
    {
        x[n*i + index] = alfa[i] * x[n*(i + 1)+index] + beta[i];
    }

    delete[] alfa;
    delete[] beta;
}

void SplineTwoBody::filter(double* X, double* A, int n)
{
    double l;
    int ord;
    int* Order = new int[n];
    for (int j = 0; j < n; j++)
    {
        Order[j] = j;
    }
    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j < n - i; j++)
        {
            if (A[j] > A[j + 1])
            {
                l = A[j + 1];
                A[j + 1] = A[j];
                A[j] = l;
                ord = Order[j + 1];
                Order[j + 1] = Order[j];
                Order[j] = ord;
            }
        }
    }
    double* X1 = new double[n * n];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            X1[j * n + i] = X[j * n + Order[i]];
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            X[j * n + i] = X1[j * n + i];
        }
    }
    delete[] X1;
    delete[] Order;
}

double SplineTwoBody::find_max_MOD(double* A, int n)
{
    double _max = abs(A[1]);

    //#pragma omp parallel for shared(_max)
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (_max < abs(A[i * n + j]))
            {
                _max = abs(A[i * n + j]);
            }
        }
    }
    return _max;
}

void SplineTwoBody::RotateMethodMultipleALeft(double* A, int n, int i_r, int j_r, double phi, double* TEMP)
{
    for (int i = 0; i < n; i++)
    {
        TEMP[i] = A[i * n + i_r] * cos(phi) + A[i * n + j_r] * sin(phi);
        TEMP[i + n] = -A[i * n + i_r] * sin(phi) + A[i * n + j_r] * cos(phi);
    }
    for (int i = 0; i < n; i++)
    {
        A[i * n + i_r] = TEMP[i];
        A[i * n + j_r] = TEMP[i + n];
    }
}

void SplineTwoBody::RotateMethodMultipleARight(double* A, int n, int i_r, int j_r, double phi, double* TEMP)
{
    for (int i = 0; i < n; i++)
    {
        TEMP[i] = A[i_r * n + i] * cos(phi) + A[j_r * n + i] * sin(phi);
        TEMP[i + n] = -A[i_r * n + i] * sin(phi) + A[j_r * n + i] * cos(phi);
    }
    for (int i = 0; i < n; i++)
    {
        A[i_r * n + i] = TEMP[i];
        A[j_r * n + i] = TEMP[i + n];
    }
}



void SplineTwoBody::RotateMethod(double* A_1, double* Energy, int n, double eps)
{
    double phi;
    double norm;
    double maxelement;
    double* TEMP = new double [2*n];
    double* TEMP_X = new double[n * n];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                TEMP_X[i*n+j] = 1;
            }
            else {
                TEMP_X[i*n+j] = 0;
            }
        }
    }
    do
    {
        for (int i = 0; i < n - 1; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                phi = 0.5 * atan(2 * A_1[i * n + j] / (A_1[i * n + i] - A_1[j * n + j]));
                RotateMethodMultipleALeft(TEMP_X, n, i, j, phi, TEMP);
                RotateMethodMultipleALeft(A_1, n, i, j, phi, TEMP);
                RotateMethodMultipleARight(A_1, n, i, j, phi, TEMP);
            }
        }
        maxelement = find_max_MOD(A_1, n);
    } while (maxelement > eps);
    norm = 0;
    for (int i = 0; i < n; i++)
    {
        norm = 0;
        for (int j = 0; j < n; j++)
        {
            norm += TEMP_X[j*n+i] * TEMP_X[j*n+i] * h;
        }
        norm = sqrt(norm);
        for (int j = 0; j < n; j++)
        {
            TEMP_X[j*n+i] = TEMP_X[j*n+i] / norm;
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (TEMP_X[(n - 1)*n+i] < 0)
        {
            for (int j = 0; j < n; j++)
            {
                TEMP_X[j*n+i] = TEMP_X[j*n+i] * (-1);
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        Energy[i] = A_1[i * n + i];
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A_1[i * n + j] = TEMP_X[i * n + j];
        }
    }
    delete[] TEMP_X;
    delete[] TEMP;
}

void SplineTwoBody::MethodSplainNatural(double* X, double* Y, int size, double esp, bool IsRadial, double *Data, int p)
{
    double S;
    int matrixsize = size - 2;
    double* B = new double[matrixsize];
    double* A = new double [matrixsize*3];
    double* m = new double[matrixsize + 2];
    for (int i = 1; i < matrixsize - 1; i++)
    {
        A[i] = (X[i + 1] - X[i]) / 6.0;
        A[i + matrixsize] = (-X[i] + X[i + 2]) / 3.0;
        A[i+ 2*matrixsize] = (X[i + 2] - X[i + 1]) / 6.0;
    }
    A[0] = (-X[0] + X[2]) / 3.0;
    A[0+ matrixsize] = (X[2] - X[1]) / 6.0;
    A[0+2* matrixsize] = 0;
    A[matrixsize - 1] = (X[matrixsize] - X[matrixsize - 1]) / 6.0;
    A[matrixsize - 1+ matrixsize] = (-X[matrixsize - 1] + X[matrixsize + 1]) / 3.0;
    A[matrixsize - 1+2* matrixsize] = 0;
    for (int i = 0; i < matrixsize; i++)
    {
        B[i] = (Y[i + 2] - Y[i + 1]) / (X[i + 2] - X[i + 1]) - (Y[i + 1] - Y[i]) / (X[i + 1] - X[i]);
    }
    ProgonkaRIGHT_SPLAINMOD(matrixsize, A, B, m);


    for (int i = matrixsize + 1; i > 0; i--)
    {
        m[i] = m[i-1];
    }
    m[0] = 0;
    m[matrixsize + 1] = 0;


    //Normalization
    double* Xr = new double[size];
    double* Yr = new double[size];
    double Norm;
    for (int i = 0; i < size; i++)
    {
        Xr[i] = X[i];
        Yr[i] = Y[i] * Y[i];
    }
    Norm = MethodSplainNatural_Radius(Xr, Yr, size);
    delete[] Xr;
    delete[] Yr;


    //double t = X[0];

    int N_spl;
    N_spl = h / esp;

    for (int i = 0; i < size-1; i++)
    {
        for (int k = 0; k < N_spl; k++)
        {
            S = m[i] * (X[i + 1] - (k * esp + i * h)) * (X[i + 1] - (k * esp + i * h)) * (X[i + 1] - (k * esp + i * h)) / 6.0 / (X[i + 1] - X[i]) +
                m[i + 1] * ((k * esp + i *h) - X[i]) * ((k * esp + i * h) - X[i]) * ((k * esp + i * h) - X[i]) / 6.0 / (X[i + 1] - X[i]) +
                (Y[i] - m[i] * (X[i + 1] - X[i]) * (X[i + 1] - X[i]) / 6.0) * (X[i + 1] - (k * esp + i * h)) / (X[i + 1] - X[i]) +
                (Y[i+1] - m[i + 1] * (X[i + 1] - X[i]) * (X[i + 1] - X[i]) / 6.0) * ((k * esp + i * h) - X[i]) / (X[i + 1] - X[i]);
            if (IsRadial == true)
            {

                Data[p * Wave_and_Radial_size + k + i * N_spl] = S / ((k + i * N_spl)*esp);
            }
            else if (IsRadial == false)
            {
                Data[p * Wave_and_Radial_size + k + i * N_spl] = S/Norm;
            }
        }
    }
    if (IsRadial == true)
    {
        Data[p * Wave_and_Radial_size] = Data[p * Wave_and_Radial_size + 1];
        Data[p * Wave_and_Radial_size + N_spl + matrix_size * N_spl] = 0;
    }
    else
    {
        Data[p * Wave_and_Radial_size] = 0;
        Data[p * Wave_and_Radial_size + N_spl + matrix_size * N_spl] = 0;
    }

    delete[] A;
    delete[] B;
    delete[] m;
}

double SplineTwoBody::MethodSplainNatural_Radius(double* X, double* Y, int size)
{
    int matrixsize = size - 2;
    double* B = new double[matrixsize];
    double* A = new double [matrixsize*3];
    double* m = new double[matrixsize + 2];
    for (int i = 1; i < matrixsize - 1; i++)
    {
        A[i] = (X[i + 1] - X[i]) / 6.0;
        A[i + matrixsize] = (-X[i] + X[i + 2]) / 3.0;
        A[i+ 2*matrixsize] = (X[i + 2] - X[i + 1]) / 6.0;
    }
    A[0] = (-X[0] + X[2]) / 3.0;
    A[0+ matrixsize] = (X[2] - X[1]) / 6.0;
    A[0+2* matrixsize] = 0;
    A[matrixsize - 1] = (X[matrixsize] - X[matrixsize - 1]) / 6.0;
    A[matrixsize - 1+ matrixsize] = (-X[matrixsize - 1] + X[matrixsize + 1]) / 3.0;
    A[matrixsize - 1+2* matrixsize] = 0;
    for (int i = 0; i < matrixsize; i++)
    {
        B[i] = (Y[i + 2] - Y[i + 1]) / (X[i + 2] - X[i + 1]) - (Y[i + 1] - Y[i]) / (X[i + 1] - X[i]);
    }
    ProgonkaRIGHT_SPLAINMOD(matrixsize, A, B, m);


    for (int i = matrixsize + 1; i > 0; i--)
    {
        m[i] = m[i-1];
    }
    m[0] = 0;
    m[matrixsize + 1] = 0;


    double C;
    double IntegralOfSpline = 0;
    for (int j = 0; j < size - 1; j++)
    {
        C = (1.0 / (X[j + 1] - X[j]) * (Y[j + 1] - Y[j]) - (X[j + 1] - X[j]) * 0.5 * m[j] - (X[j + 1] - X[j]) / 6.0 * (m[j + 1] - m[j]));
        IntegralOfSpline +=
            Y[j] * (X[j + 1] - X[j]) +
            C / 2 * (X[j + 1] - X[j]) * (X[j + 1] - X[j]) +
            m[j] / 6 * (X[j + 1] - X[j]) * (X[j + 1] - X[j]) * (X[j + 1] - X[j]) +
            1.0 / (24 * (X[j + 1] - X[j])) * (m[j + 1] - m[j]) * (X[j + 1] - X[j]) * (X[j + 1] - X[j]) * (X[j + 1] - X[j]) * (X[j + 1] - X[j]);
    }
    delete[] A;
    delete[] B;
    delete[] m;
    return IntegralOfSpline;
}

void SplineTwoBody::ProgonkaRIGHT_SPLAINMOD(unsigned int n, double* a, double* b, double* x)
{
    double* alfa = new double[n];
    double* beta = new double[n];
    alfa[0] = -a[0+n] / a[0];
    beta[0] = b[0] / a[0];
    for (int i = 1; i < n; i++)
    {
        alfa[i] = -a[i+2* n] / (a[i] * alfa[i - 1] + a[i+n]);
        beta[i] = (b[i] - a[i] * beta[i - 1]) / (a[i] * alfa[i - 1] + a[i+n]);
    }
    x[n - 1] = beta[n - 1];
    for (int i = n - 2; i >= 0; i--)
    {
        x[i] = alfa[i] * x[i + 1] + beta[i];
    }
    delete[] alfa;
    delete[] beta;
}




// void SplineTwoBody::MethodSplainNatural(double* X, double* Y, int size, double esp, bool IsRadial, double *Data, int p)
// {


// if (isnan(S / (k * esp)) || isinf(S / (k * esp)))
// {
//     Data[p * Wave_and_Radial_size + k + i * N_spl] = S / (k*esp)/Norm;
// }
// else
// {
//     Data[p * Wave_and_Radial_size + k + i * N_spl] = S/Norm;
// }


    // for (int j = 0; j < size - 1; j++)
    // {
    //     do
    //     {
    //         S = Y[j] + (1.0 / (X[j + 1] - X[j]) * (Y[j + 1] - Y[j]) - (X[j + 1] - X[j]) * 0.5 * m[j] - (X[j + 1] - X[j]) / 6.0 * (m[j + 1] - m[j])) * (t - X[j]);
    //         S += m[j] * 0.5 * (t - X[j]) * (t - X[j]);
    //         S += 1.0 / (6 * (X[j + 1] - X[j])) * (m[j + 1] - m[j]) * (t - X[j]) * (t - X[j]) * (t - X[j]);
    //         if (IsRadial == true)
    //         {
    //             if (isnan(S / t) || isinf(S / t))// Ноль дополнить
    //             {
    //                 Data[data_count + p * Wave_and_Radial_size] = S/Norm;
    //                 data_count++;
    //             }
    //             else {
    //                 Data[data_count + p * Wave_and_Radial_size] = S / t/Norm;
    //                 data_count++;
    //             }
    //         }
    //         else if (IsRadial == false) {
    //             Data[data_count + p * Wave_and_Radial_size] = S/Norm;
    //             data_count++;
    //         }
    //         t += esp;
    //     } while (t < X[j + 1] - esp * 0.01);
    //     if (j == size - 2)
    //     {
    //         S = Y[j] + (1.0 / (X[j + 1] - X[j]) * (Y[j + 1] - Y[j]) - (X[j + 1] - X[j]) * 0.5 * m[j] - (X[j + 1] - X[j]) / 6.0 * (m[j + 1] - m[j])) * (t - X[j]);
    //         S += m[j] * 0.5 * (t - X[j]) * (t - X[j]);
    //         S += 1.0 / (6 * (X[j + 1] - X[j])) * (m[j + 1] - m[j]) * (t - X[j]) * (t - X[j]) * (t - X[j]);
    //         if (IsRadial == true)
    //         {
    //             Data[data_count + p * Wave_and_Radial_size] = S / t / Norm;
    //             data_count++;
    //         }
    //         else if (IsRadial == false)
    //         {
    //             Data[data_count + p * Wave_and_Radial_size] = S / Norm;
    //             data_count++;
    //         }
    //     }
    // }
// }









// void Nuclei::WriteToFile_P()
// {
//     double* VectorToSplain = new double[n + 2];
//     double* r = new double[n + 2];
//     VectorToSplain[0] = 0;
//     VectorToSplain[n + 1] = 0;
//     for (int j = 0; j < (n + 2); j++)
//     {
//         r[j] = rmin + j * h;
//     }
//     for (int i = 0; i < E_Xi_l_p_size; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             VectorToSplain[j + 1] = Xi_l_p[i][j];
//         }
//         rms[i] = Radius(r, VectorToSplain, n + 2);
//         std::cout << "Radius (rms) " << rms[i] << "  Energy " << E_l_p[i] << std::endl;
//         MethodSplainNatural(r, VectorToSplain, n + 2, h_spl, "Xi_" + std::to_string(i) + "Proton.txt", false, Wave_Proton[i]);
//         MethodSplainNatural(r, VectorToSplain, n + 2, h_spl, "R_" + std::to_string(i) + "Proton.txt", true, Radial_Proton[i]);
//     }
//     delete[] VectorToSplain;
//     delete[] r;
// }






