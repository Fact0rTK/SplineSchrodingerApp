#include "splinetwobody_eigen.h"

double SplineTwoBody_Eigen::Potential(double r, double l)
{
    //return Potential_Equation_Expression.value();
    Potential_Equation_R = r;
    double Value = Potential_Equation_Expression.value();
    return Value;
    //return -7.5 * r * r * exp(-r) + l*(l+1.0)/(r*r);
}

void SplineTwoBody_Eigen::Initialize_Matrix_B_AHA(int l)
{
    //Calculate A matrix
    Matrix_AH.setZero();
    for (int i = 0; i < matrix_size - 1; i++)
    {
        Matrix_AH(i,i) = -2 * h / 3.0;
        Matrix_AH(i + 1, i) = -h / 6.0;
        Matrix_AH(i, i + 1) = -h / 6.0;
    }
    Matrix_AH(matrix_size - 1,matrix_size - 1) = -2 * h / 3.0;
    //-----------------------------------------

    //Calculate inverse matrix of A
    Eigen::MatrixXd Matrix_AH_Inverse = Matrix_AH.inverse();
    //-----------------------------------------

    //Multiple A by H
    for (int i = 0; i < matrix_size; i++)
    {
        Matrix_AH(i, 0) = Matrix_AH_Inverse(i, 0) * (-2.0 / h) + Matrix_AH_Inverse(i, 1) * 1.0 / h;
        for (int j = 1; j < matrix_size-1; j++)
        {
            Matrix_AH(i, j) = Matrix_AH_Inverse(i, j - 1) * 1.0 / h + Matrix_AH_Inverse(i, j) * (-2.0 / h) + Matrix_AH_Inverse(i, j + 1) * 1.0 / h;
        }
        Matrix_AH(i, matrix_size - 1) = Matrix_AH_Inverse(i, matrix_size - 1) * (-2.0 / h) + Matrix_AH_Inverse(i, matrix_size - 2) * 1.0 / h;
    }
    //-----------------------------------------

    //Add matrix B
    for (int i = 0; i < matrix_size; i++)
    {
        Matrix_AH(i, i) = Matrix_AH(i, i) + k_units * mass * Potential(rmin + 1.0*(i + 1) * h, l) + l * (l + 1) / (rmin + (i + 1) * h) / (rmin + (i + 1) * h);
    }
    //-----------------------------------------
}

bool SplineTwoBody_Eigen::CalculateEigenValuesAndVectors()
{
    bool BoundStatesExist;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> EigenResult(Matrix_AH);
    TheLowestEnergy = EigenResult.eigenvalues()[0];
    if(EigenResult.eigenvalues()[0] < 0)
    {
        BoundStatesExist = true;
    }
    else
    {
        BoundStatesExist = false;
        return BoundStatesExist;
    }
    for(int i = 0; i < matrix_size; i++)
    {
        if(EigenResult.eigenvalues()[i] > 0)
        {
            bound_states_size = i;
            break;
        }
        else if(i == matrix_size-1)
        {
            bound_states_size = matrix_size;
        }
    }
    Matrix_Energies.resize(bound_states_size);
    Matrix_AH.resize(matrix_size, bound_states_size);
    Matrix_AH.setZero();
    for(int i = 0; i < bound_states_size; i++)
    {
        Matrix_Energies(i) = EigenResult.eigenvalues()[i];
        Matrix_AH.col(i) = EigenResult.eigenvectors().col(i);
    }
    return BoundStatesExist;
}

void SplineTwoBody_Eigen::MethodSplainNatural(double* X, double* Y, int size, double esp, bool IsRadial, double *Data, int p)
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

double SplineTwoBody_Eigen::MethodSplainNatural_Radius(double* X, double* Y, int size)
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

void SplineTwoBody_Eigen::ProgonkaRIGHT_SPLAINMOD(unsigned int n, double* a, double* b, double* x)
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

// for (int i = 0; i < matrix_size; i++)
// {
//     QString stroka = "";
//     for (int j = 0; j < matrix_size; j++)
//     {
//         stroka = stroka + " " + QString::number(Result(i, j));
//     }
//     qDebug() << stroka;
// }
// qDebug() << " ";
