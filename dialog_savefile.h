#ifndef DIALOG_SAVEFILE_H
#define DIALOG_SAVEFILE_H


#include <QDialog>
#include <QFileDialog>

namespace Ui {
class Dialog_SaveFile;
}

class Dialog_SaveFile : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_SaveFile(QVector<double> &Exported_x_axis , QVector<QVector<double>> &Exported_Wave_functions, QVector<QVector<double>> &Exported_Radial_functions, QVector<double> &Exported_Energies, int &Exported_size_splined, int &Exported_size_states, QWidget *parent = nullptr);
    ~Dialog_SaveFile();

private slots:
    void on_PrefixNameEdit_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_pushButton_clicked();

private:
    Ui::Dialog_SaveFile *ui;
    QString FileSavingDirectory;


    QVector<QVector<double>> Wave_functions;
    QVector<QVector<double>> Radial_functions;
    QVector<double> Energies;
    QVector<double> Plot_data_x;
    int Size_splined;
    int Size_states;

    void SaveFunctionsToFile();
    bool CheckExistingFiles();
    void UpdateNameAndFolderOfFiles();

};

#endif // DIALOG_SAVEFILE_H
