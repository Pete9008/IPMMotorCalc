/*
 * This file is part of the IPMMotorCalc project
 *
 * Copyright (C) 2023 Pete9008 <openinverter.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datagraph.h"
#include "motormodel.h"

struct file_data {
    qint64 time;
    double id;
    double iq;
    double ud;
    double uq;
    double frq;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    DataGraph *inputGraph;
    DataGraph *errorGraph;
    DataGraph *modelGraph;
    DataGraph *resultsGraph;
    QVector<file_data> fdata;
    MotorModel *motor;
    qint64 m_timenow;
    QList<QPointF> listLd;
    QList<QPointF> listLq;
    QList<QPointF> listRs;
    QList<QPointF> listFL;

    double m_wheelSize;
    double m_vehicleWeight;
    double m_gearRatio;
    double m_Lq;
    double m_Ld;
    double m_Rs;
    double m_Poles;
    double m_fluxLinkage;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_selectFile_clicked();

    void on_pb_Run_clicked();

    void on_pb_TuneLq_clicked();

    void on_pb_TuneLd_clicked();

    void on_pb_TuneRs_clicked();

    void on_pb_TuneFL_clicked();

    void on_vehicleWeight_editingFinished();

    void on_wheelSize_editingFinished();

    void on_gearRatio_editingFinished();

    void on_Poles_editingFinished();

    void on_Lq_editingFinished();

    void on_Ld_editingFinished();

    void on_Rs_editingFinished();

    void on_FluxLinkage_editingFinished();

    void on_pb_CopyLq_clicked();

    void on_pb_CopyLd_clicked();

    void on_pb_CopyRs_clicked();

    void on_pb_CopyFL_clicked();

    void on_pb_AutoTune_clicked();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *bar);

};

#endif // MAINWINDOW_H
