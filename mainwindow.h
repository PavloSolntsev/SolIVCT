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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "about.h"
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openfile();
    void clearalldata();
    void savefile();
    void calculate();
    void about();


private:
    Ui::MainWindow *ui;

    void printresults();

    double ivct_energy;
    double ivct_epsilon;
    double ivct_fwhm;
    double ivct_temp;
    double rab;
    double de;
    double temperature;
    double Hab;
    double fwhmtheory;
    double ffunction;
    double gammatest;
    double dispconst;
    double dispgibbs;
    double oscstrength;
    double ivctdipolmoment;
    double elchemtemp;

    bool iscalculated;
    About aboutdialog;
    QString gammatestresults;
    QString ftest;

};

#endif // MAINWINDOW_H
