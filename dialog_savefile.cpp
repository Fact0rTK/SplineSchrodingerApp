#include "dialog_savefile.h"
#include "ui_dialog_savefile.h"
#include <QFile>
#include <fstream>
#include <QMessageBox>


Dialog_SaveFile::Dialog_SaveFile(QVector<double> &Exported_x_axis , QVector<QVector<double>> &Exported_Wave_functions, QVector<QVector<double>> &Exported_Radial_functions, QVector<double> &Exported_Energies, int &Exported_size_splined, int &Exported_size_states, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_SaveFile)
{
    ui->setupUi(this);

    FileSavingDirectory = qApp->applicationDirPath();
    UpdateNameAndFolderOfFiles();

    this->Wave_functions = Exported_Wave_functions;
    this->Radial_functions = Exported_Radial_functions;
    this->Energies = Exported_Energies;
    this->Plot_data_x = Exported_x_axis;
    this->Size_splined = Exported_size_splined;
    this->Size_states = Exported_size_states;

    for(int i = 0; i < Exported_size_states; i++)
    {
        ui->ListEnergies_Widget->addItem("E"+ QString::number(i) + " = " + QString::number(Exported_Energies[i]));
        ui->ListEnergies_Widget->item(i)->setFlags(ui->ListEnergies_Widget->item(i)->flags() | Qt::ItemIsUserCheckable);
        ui->ListEnergies_Widget->item(i)->setCheckState(Qt::Unchecked);
    }

    ui->FunctionType_Select->addItem("Wave and Radial Functions");
    ui->FunctionType_Select->addItem("Only Wave Functions");
    ui->FunctionType_Select->addItem("Only Radial Functions");
}

Dialog_SaveFile::~Dialog_SaveFile()
{
    delete ui;
    for(int i = 0; i < Size_states; i++)
    {
        Wave_functions[i].clear();
        Radial_functions[i].clear();
    }
    Wave_functions.clear();
    Radial_functions.clear();
    Energies.clear();
    Plot_data_x.clear();
}

void Dialog_SaveFile::UpdateNameAndFolderOfFiles()
{
    ui->FileNameShowLabel->setText("The selected files will be saved in\n" + FileSavingDirectory + "\n" +  "as \"" + "_Wave_E0.txt" + "\"");
    ui->FileNameShowLabel->setAlignment(Qt::AlignCenter);
}

void Dialog_SaveFile::on_PrefixNameEdit_textChanged(const QString &arg1)
{
    UpdateNameAndFolderOfFiles();
}

void Dialog_SaveFile::SaveFunctionsToFile()
{
    switch(ui->FunctionType_Select->currentIndex())
    {
    case(0):
        for(int i = 0; i < ui->ListEnergies_Widget->count(); i++)
        {
            if(ui->ListEnergies_Widget->item(i)->checkState())
            {
                std::ofstream SaveFileWave(FileSavingDirectory.toStdString() + ui->PrefixNameEdit->text().toStdString() + "_Wave_" + "E" + std::to_string(i) + ".txt");
                SaveFileWave << "E = " << Energies[i] << std::endl;
                SaveFileWave << "r " << "Psi(r)" << std::endl;
                for(int j = 0; j < Size_splined; j++)
                {
                    SaveFileWave << Plot_data_x[j] << " " << Wave_functions[i][j] << std::endl;
                }
                SaveFileWave.close();
                std::ofstream SaveFileRadial(FileSavingDirectory.toStdString() + ui->PrefixNameEdit->text().toStdString() + "_Radial_" + "E" + std::to_string(i) + ".txt");
                SaveFileRadial << "E = " << Energies[i] << std::endl;
                SaveFileRadial << "r " << "Psi(r)/r" << std::endl;
                for(int j = 0; j < Size_splined; j++)
                {
                    SaveFileRadial << Plot_data_x[j] << " " << Wave_functions[i][j] << std::endl;
                }
                SaveFileRadial.close();
            }
        }
        break;
    case(1):
        for(int i = 0; i < ui->FunctionType_Select->count(); i++)
        {
            if(ui->ListEnergies_Widget->item(i)->checkState())
            {
                std::ofstream SaveFileWave(FileSavingDirectory.toStdString() + ui->PrefixNameEdit->text().toStdString() + "_Wave_" + "E" + std::to_string(i) + ".txt");
                SaveFileWave << "E = " << Energies[i] << std::endl;
                SaveFileWave << "r " << "Psi(r)" << std::endl;
                for(int j = 0; j < Size_splined; j++)
                {
                    SaveFileWave << Plot_data_x[j] << " " << Wave_functions[i][j] << std::endl;
                }
                SaveFileWave.close();
            }
        }
        break;
    case(2):
        for(int i = 0; i < ui->FunctionType_Select->count(); i++)
        {
            if(ui->ListEnergies_Widget->item(i)->checkState())
            {
                std::ofstream SaveFileRadial(FileSavingDirectory.toStdString() + ui->PrefixNameEdit->text().toStdString() + "_Radial_" + "E" + std::to_string(i) + ".txt");
                SaveFileRadial << "E = " << Energies[i] << std::endl;
                SaveFileRadial << "r " << "Psi(r)/r" << std::endl;
                for(int j = 0; j < Size_splined; j++)
                {
                    SaveFileRadial << Plot_data_x[j] << " " << Wave_functions[i][j] << std::endl;
                }
                SaveFileRadial.close();
            }
        }
        break;
    default:
        break;
    }
}

bool Dialog_SaveFile::CheckExistingFiles()
{
    bool FilesExist = false;
    for(int i = 0; i < ui->FunctionType_Select->count(); i++)
    {
        if(std::filesystem::exists(FileSavingDirectory.toStdString() + ui->PrefixNameEdit->text().toStdString() + "_Wave_" + "E" + std::to_string(i) + ".txt"))
        {
            FilesExist = true;
        }
    }
    return FilesExist;
}

void Dialog_SaveFile::on_buttonBox_accepted()
{
    if(ui->FunctionType_Select->count() == 0)
    {
        QMessageBox::warning(this, "Error", tr("None of the bound states are selected to save\n"), QMessageBox::Ok);
    }
    else
    {
        if(!CheckExistingFiles())
        {
            SaveFunctionsToFile();
        }
        else
        {
            QMessageBox::StandardButton RewriteQustionFile = QMessageBox::question( this, "Confirm the action", tr("Files already exist with such name \n Do you want to overwrite them?"),
                                                                                   QMessageBox::No | QMessageBox::Yes,
                                                                                   QMessageBox::Yes);
            switch (RewriteQustionFile){
            case(QMessageBox::Yes):
                SaveFunctionsToFile();
                break;
            case(QMessageBox::No):
                break;
            default:
                break;
            }
        }
    }
}

void Dialog_SaveFile::on_pushButton_clicked()
{
    QString DirectoryPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Choose Directory"), qApp->applicationDirPath()));
    if(!DirectoryPath.isEmpty())
    {
        FileSavingDirectory = DirectoryPath + QDir::separator();
    }
    UpdateNameAndFolderOfFiles();
}

