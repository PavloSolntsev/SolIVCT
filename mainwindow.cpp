/*
 *  This file is a part of SolIVCT software, which is a tool to analyze
 *  experimental data for mixed-valence compounds, e.g. IVCT band and
 *  electrochemical data
 *
 *  Copyright (C) 2015  Pavlo Solntsev <pavlo.solntsev@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include <cmath>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMap>

#define GASCONSTCM 0.6946616
#define GASCONSTJ 8.314462
#define FARADAY 96485.34
#define VERSION 1

#define START_INPUT "*** INPUT PARAMETERS ***"
#define END_INPUT "*** END INPUT PARAMETERS ***"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen_File,SIGNAL(activated()),this,SLOT(openfile()));
    connect(ui->actionClearAll,SIGNAL(activated()),this,SLOT(clearalldata()));
    connect(ui->actionSaveFile,SIGNAL(activated()),this,SLOT(savefile()));
    connect(ui->actionCalculate,SIGNAL(activated()),this,SLOT(calculate()));
    connect(ui->actionAbout,SIGNAL(activated()),this,SLOT(about()));

    iscalculated = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openfile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open File"), QDir::homePath(), tr("Text files (*.txt)"));

    QFile ifile(fileName);

    if(!ifile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Can't open file " << fileName;
    }
    else
    {
        QString buffer;
        QTextStream input(&ifile);
        while (!input.atEnd()) {
            buffer = input.readLine();

//            qDebug() << buffer;
//            qDebug() << buffer.size();

            if(buffer.size() == 0) // Ignore empty lines
                continue;

            if(buffer.at(0) == '#') // This is a comment
                continue;

            if (buffer.contains("*** INPUT PARAMETERS ***")) {
                continue;
            }

            if (buffer.contains("=== Data for IVCT analysis ===")) {
                continue;
            }

            if (buffer.contains("*** END INPUT PARAMETERS ***")) {
                break;
            }
//            IVCT_energy      15000       cm-1
//               IVCT_eps       2000 L/mol*cm-1
//              IVCT_FWHM       1000       cm-1
//                    Rab          3          A
//              IVCT_Temp        300          K
//               T_elchem        300          K
//                     dE        0.1          V

            if(buffer.contains("IVCT_energy"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> ivct_energy >> buffer2;
                continue;
            }

            if(buffer.contains("IVCT_eps"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> ivct_epsilon >> buffer2;
                continue;
            }

            if(buffer.contains("IVCT_FWHM"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> ivct_fwhm >> buffer2;
                continue;
            }

            if(buffer.contains("Rab"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> rab >> buffer2;
                continue;
            }

            if(buffer.contains("IVCT_Temp"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> temperature >> buffer2;
                continue;
            }

            if(buffer.contains("T_elchem"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> elchemtemp >> buffer2;
                continue;
            }

            if(buffer.contains("dE"))
            {
                QString buffer2;
                QTextStream in(&buffer,QIODevice::ReadOnly | QIODevice::Text);
                in >> buffer2 >> de >> buffer2;
                continue;
            }

        } //end while
    }// end else

    ui->lineEdit_ivct_energy->setText(QString::number(ivct_energy));
    ui->lineEdit_epsilon->setText(QString::number(ivct_epsilon));
    ui->lineEdit_deltae->setText(QString::number(de));
    ui->lineEdit_elchemtemp->setText(QString::number(elchemtemp));
    ui->lineEdit_fwhm->setText(QString::number(ivct_fwhm));
    ui->lineEdit_rab->setText(QString::number(rab));
    ui->lineEdit_temp->setText(QString::number(temperature));
}

void MainWindow::clearalldata()
{
    ui->lineEdit_ivct_energy->clear();
    ui->lineEdit_epsilon->clear();
    ui->lineEdit_deltae->clear();
    ui->lineEdit_fwhm->clear();
    ui->lineEdit_rab->clear();
    ui->plainTextEdit_output->clear();
}

void MainWindow::savefile()
{
    if (!iscalculated) {
        calculate();
    }

    QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save File"), QDir::homePath(), tr("Text files (*.txt)"));

    QFile ofile(fileName);

    if(!ofile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Can't open file " << fileName;
    }
    else
    {
        const int fieldwidth = 11;
        QTextStream output(&ofile);
        output << "=== Data for IVCT analysis ===\n" << endl;
        output << "# You can modify parameters between" << endl;
        output << "# *** INPUT PARAMETERS ***" << endl;
        output << "#       and" << endl;
        output << "# *** END INPUT PARAMETERS ***" << endl;
        output << "#" << endl;

        output << START_INPUT << endl;
        output.setFieldWidth(fieldwidth);
        output << "IVCT_energy" << ivct_energy << "cm-1"<< qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "IVCT_eps" << ivct_epsilon << "L/mol cm-1" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "IVCT_FWHM" << ivct_fwhm << "cm-1" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "Rab" << rab << 'A' << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "IVCT_Temp" << temperature << "K" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "T_elchem" << elchemtemp << "K" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "dE" << de << "V" << qSetFieldWidth(0) << endl;
        output << END_INPUT << endl << qSetFieldWidth(fieldwidth);
        output << "Hab" << Hab << "cm-1" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "FWHM(theor)" << fwhmtheory << "cm-1" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "f(osc.str)" << oscstrength << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "M(dipol)" << ivctdipolmoment << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << "F-test" << ffunction << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
        output << QString::fromUtf8("\u0393-test") << gammatest << qSetFieldWidth(0) << endl;

        output << QString(39,'*') << endl;
        output << "*  Reorganization energy estimation:  *" << endl;
        output << '*' << QString(37,' ') << '*' << endl;
        output.setRealNumberPrecision(1);
        output.setRealNumberNotation(QTextStream::FixedNotation);
        output << "*  Class  II" << qSetFieldWidth(fieldwidth) << ivct_energy << qSetFieldWidth(0)
               << " cm-1" << QString(10,' ') << '*' << qSetFieldWidth(0) << endl;
        output << "*  Class III" << qSetFieldWidth(fieldwidth) << ivct_energy/2 << qSetFieldWidth(0)
               << " cm-1" << QString(10,' ') << '*' << qSetFieldWidth(0) << endl;
        output << '*' << QString(37,' ') << '*' << endl;
        output << QString(39,'*') << endl;

        output.setFieldWidth(fieldwidth);
        output << QString("Kc(%1K)").arg(elchemtemp);
        output.setRealNumberNotation(QTextStream::SmartNotation);
        output.setRealNumberPrecision(3);
        output << dispconst << endl;
        output << QString("Gc(%1K)").arg(elchemtemp);
        output.setFieldWidth(fieldwidth);
        output.setRealNumberPrecision(3);
        output << dispgibbs  << qSetFieldWidth(0) << '\n' << endl;
        output << gammatestresults << endl;
        output << ftest << endl;
        output << QString(57,'*') << endl;
        output << "            = = = COMMENTS = = =" << endl;
        output << QString(57,'-') << endl;
        output << QString::fromUtf8("           *  \u0393-test conditions  *") << endl;
        output << QString(57,'-') << endl;
        output << QString::fromUtf8("0   < \u0393 < 0.1 for weakly-coupled Class II systems") << endl;
        output << QString::fromUtf8("0.1 < \u0393 < 0.5 for moderately-coupled Class II systems") << endl;
        output << QString::fromUtf8("\u0393 ~ 0.5 at the transition between Class II and Class III") << endl;
        output << QString::fromUtf8("\u0393 > 0.5 for Class III") << endl;
        output << QString(57,'-') << endl;
        output << "           *  F-test conditions  *" << endl;
        output << QString(57,'-') << endl;
        output << "F = 1 : complete localization" << endl;
        output << "F = 0 : borderline between Class II and Class III" << endl;
        output << QString(57,'-') << endl;
        output << QString(57,'*') << endl;
        output << "=== END OF FILE ===" << endl;
        ofile.close();
    }
}

void MainWindow::calculate()
{

    ivct_energy = ui->lineEdit_ivct_energy->text().toDouble();
    ivct_epsilon = ui->lineEdit_epsilon->text().toDouble();
    ivct_fwhm = ui->lineEdit_fwhm->text().toDouble();
    ivct_temp = ui->lineEdit_temp->text().toDouble();
    rab = ui->lineEdit_rab->text().toDouble();
    de = ui->lineEdit_deltae->text().toDouble();
    temperature = ui->lineEdit_temp->text().toDouble();
    elchemtemp = ui->lineEdit_elchemtemp->text().toDouble();

// Calculate Hab
    Hab = sqrt(ivct_energy*ivct_epsilon*ivct_fwhm)*0.0206/rab;
// Theoretical 	value for FWHM for symmetric compounds
    fwhmtheory = sqrt(16*GASCONSTCM*temperature*M_LN2*ivct_energy);
// F function analysis
    ffunction = pow(1-2*Hab/ivct_energy,2);
// Gama analysis
    gammatest = fabs(1 - ivct_fwhm/fwhmtheory);
// Disproportional constant
    dispconst = pow(M_E, de*FARADAY/(elchemtemp*GASCONSTJ)) ;
// Disproportional Gibbs energy
    dispgibbs = -1*de*FARADAY;
// Osc. strength
    oscstrength = 4.6e-9*ivct_epsilon*ivct_fwhm;
// Dipole moment of IVCT
    ivctdipolmoment = sqrt(oscstrength/(1.085e-5*ivct_energy));
    iscalculated = true;

    if (gammatest > 0 && gammatest <= 0.1)
        gammatestresults = "This is weekly-coupled Class II of mixed-valence compounds";
    else
        if (gammatest > 0.1 && gammatest < 0.47)
            gammatestresults = "This is moderately-coupled Class II of mixed-valence compounds";
        else
            if (fabs(gammatest - 0.5) <= 0.03)
                gammatestresults = "This is transition between Class II and Class II of mixed-valence compounds";
            else
                if (gammatest > 0.5)
                    gammatestresults = "This is Class III for mixed-valence compounds";
                else
                    gammatestresults = QString::fromUtf8("\u0393-test is not valid. Ignor it.");

    if ((1 - ffunction) <= 0.2)
        ftest = "Check carefully F-test value. Class I or week-coupled Class II";
    else
        if (ffunction < 0.8 && ffunction > 0.2)
            ftest = "Check carefully F-test value. Moderately-coupled Class II";
        else
            if (ffunction <= 0.2)
                ftest = "Check carefully F-test value. High-coupled Class II or Class II";
            else
                ftest = "F-test doesn't make sence. Ignore it.";

    printresults();
}

void MainWindow::about()
{
    aboutdialog.show();
}

void MainWindow::printresults()
{
    QString buffer;

    QTextStream output(&buffer,QIODevice::WriteOnly | QIODevice::Text);

    const int fieldwidth = 11;

    output << "=== Data for IVCT analysis ===\n" << endl;
    output.setFieldWidth(fieldwidth);
    output << "Hab" << Hab << "cm-1" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
    output << "FWHM(theor)" << fwhmtheory << "cm-1" << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
    output << "f(osc.str)" << oscstrength << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
    output << "M(dipol)" << ivctdipolmoment << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
    output << "F-test" << ffunction << qSetFieldWidth(0) << endl << qSetFieldWidth(fieldwidth);
    output << QString::fromUtf8("\u0393-test") << gammatest << qSetFieldWidth(0) << endl;

    output << QString(39,'*') << endl;
    output << "Reorganization energy estimation:" << endl;
    output.setRealNumberPrecision(1);
    output.setRealNumberNotation(QTextStream::FixedNotation);
    output << "Class  II" << qSetFieldWidth(fieldwidth) << ivct_energy << qSetFieldWidth(0)
           << " cm-1" << qSetFieldWidth(0) << endl;
    output << "Class III" << qSetFieldWidth(fieldwidth) << ivct_energy/2 << qSetFieldWidth(0)
           << " cm-1" << qSetFieldWidth(0) << endl;
    output << QString(39,'*') << endl;

    output.setFieldWidth(fieldwidth);
    output << QString("Kc(%1K)").arg(elchemtemp);
    output.setRealNumberNotation(QTextStream::SmartNotation);
    output.setRealNumberPrecision(3);
    output << dispconst << endl;
    output << QString("Gc(%1K)").arg(elchemtemp);
    output.setFieldWidth(fieldwidth);
    output.setRealNumberPrecision(3);
    output << dispgibbs  << qSetFieldWidth(0) << '\n' << endl;
    output << gammatestresults << endl;
    output << ftest << endl;
    output << QString(57,'*') << endl;
    output << "            = = = COMMENTS = = =" << endl;
    output << QString(57,'-') << endl;
    output << QString::fromUtf8("           *  \u0393-test conditions  *") << endl;
    output << QString(57,'-') << endl;
    output << QString::fromUtf8("0   < \u0393 < 0.1 for weakly-coupled Class II systems") << endl;
    output << QString::fromUtf8("0.1 < \u0393 < 0.5 for moderately-coupled Class II systems") << endl;
    output << QString::fromUtf8("\u0393 ~ 0.5 at the transition between Class II and Class III") << endl;
    output << QString::fromUtf8("\u0393 > 0.5 for Class III") << endl;
    output << QString(57,'-') << endl;
    output << "           *  F-test conditions  *" << endl;
    output << QString(57,'-') << endl;
    output << "F = 1 : complete localization" << endl;
    output << "F = 0 : borderline between Class II and Class III" << endl;
    output << QString(57,'-') << endl;
    output << QString(57,'*') << endl;
    output << "=== END OF FILE ===" << endl;

    ui->plainTextEdit_output->appendPlainText(buffer);
}
