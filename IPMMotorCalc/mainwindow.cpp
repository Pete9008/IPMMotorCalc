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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QtMath>

//Most graphs
#define IQ 1
#define ID 2
#define VQ 3
#define VD 4
#define FRQ 5

//Results graph
#define LD 1
#define LQ 2
#define RS 3
#define FL 4
#define KG 5


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("OpenInverter", "IPMMotorCalc");
    restoreGeometry(settings.value("mainwin/geometry").toByteArray());
    restoreState(settings.value("mainwin/windowState").toByteArray());

    if(settings.contains(ui->le_filename->objectName())) ui->le_filename->setText(settings.value(ui->le_filename->objectName(),QString()).toString());

    if(settings.contains(ui->vehicleWeight->objectName())) ui->vehicleWeight->setText(settings.value(ui->vehicleWeight->objectName(),QString()).toString());
    if(settings.contains(ui->wheelSize->objectName())) ui->wheelSize->setText(settings.value(ui->wheelSize->objectName(),QString()).toString());
    if(settings.contains(ui->gearRatio->objectName())) ui->gearRatio->setText(settings.value(ui->gearRatio->objectName(),QString()).toString());
    if(settings.contains(ui->Poles->objectName())) ui->Poles->setText(settings.value(ui->Poles->objectName(),QString()).toString());

    if(settings.contains(ui->Lq->objectName())) ui->Lq->setText(settings.value(ui->Lq->objectName(),QString()).toString());
    if(settings.contains(ui->Ld->objectName())) ui->Ld->setText(settings.value(ui->Ld->objectName(),QString()).toString());
    if(settings.contains(ui->Rs->objectName())) ui->Rs->setText(settings.value(ui->Rs->objectName(),QString()).toString());
    if(settings.contains(ui->FluxLinkage->objectName())) ui->FluxLinkage->setText(settings.value(ui->FluxLinkage->objectName(),QString()).toString());

    inputGraph = new DataGraph("input", this);
    inputGraph->setWindowTitle("Input Data");
    inputGraph->setAxisText("", "Amps (A)/Volts (V)", "Hz");
    inputGraph->addSeries("Iq (A)", axis_left, IQ);
    inputGraph->addSeries("Id (A)", axis_left, ID);
    inputGraph->addSeries("Vq (V)", axis_left, VQ);
    inputGraph->addSeries("Vd (V)", axis_left, VD);
    inputGraph->addSeries("Freq (Hz)", axis_right, FRQ);
    inputGraph->setColour(Qt::blue, VQ);
    inputGraph->setColour(Qt::red, VD);
    inputGraph->setColour(Qt::cyan, IQ);
    inputGraph->setColour(Qt::magenta, ID);
    inputGraph->setColour(Qt::darkGreen, FRQ);
    inputGraph->updateGraph();
    inputGraph->show();

    modelGraph = new DataGraph("model", this);
    modelGraph->setWindowTitle("Model Data");
    modelGraph->setAxisText("", "Volts", "Hz");
    modelGraph->addSeries("Vq (V)", axis_left, VQ);
    modelGraph->addSeries("Vd (V)", axis_left, VD);
    modelGraph->addSeries("Frq (Hz)", axis_right, FRQ);
    modelGraph->setColour(Qt::blue, VQ);
    modelGraph->setColour(Qt::red, VD);
    modelGraph->setColour(Qt::darkGreen, FRQ);
    modelGraph->updateGraph();
    modelGraph->show();

    errorGraph = new DataGraph("error", this);
    errorGraph->setWindowTitle("Error Data");
    errorGraph->setAxisText("", "Volts", "Hz");
    errorGraph->addSeries("Vq (V)", axis_left, VQ);
    errorGraph->addSeries("Vd (V)", axis_left, VD);
    errorGraph->addSeries("Frq (Hz)", axis_right, FRQ);
    errorGraph->setColour(Qt::blue, VQ);
    errorGraph->setColour(Qt::red, VD);
    errorGraph->setColour(Qt::darkGreen, FRQ);
    errorGraph->updateGraph();
    errorGraph->show();

    resultsGraph = new DataGraph("results", this);
    resultsGraph->setWindowTitle("Results");
    resultsGraph->setAxisText("", "Error", "");
    resultsGraph->addSeries("Ld (mH)", axis_left, LD);
    resultsGraph->addSeries("Lq (mH)", axis_left, LQ);
    resultsGraph->addSeries("Rs (mR)", axis_left, RS);
    resultsGraph->addSeries("λ (mWb)", axis_left, FL);
    resultsGraph->updateGraph();
    resultsGraph->show();

    m_wheelSize = ui->wheelSize->text().toDouble();
    m_vehicleWeight = ui->vehicleWeight->text().toDouble();
    m_gearRatio = ui->gearRatio->text().toDouble();
    m_Lq = ui->Lq->text().toDouble()/1000; //entered in mH
    m_Ld = ui->Ld->text().toDouble()/1000; //entered in mH
    m_Rs = ui->Rs->text().toDouble()/1000;
    m_Poles = ui->Poles->text().toDouble();
    m_fluxLinkage = ui->FluxLinkage->text().toDouble()/1000; //entered in mWeber

    motor = new MotorModel(m_wheelSize,m_gearRatio,0,m_vehicleWeight,m_Lq,m_Ld,m_Rs,m_Poles,m_fluxLinkage,0.001,0,1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("OpenInverter", "IPMMotorCalc");
    settings.setValue("mainwin/geometry", saveGeometry());
    settings.setValue("mainwin/windowState", saveState());

    settings.setValue(ui->le_filename->objectName(), ui->le_filename->text());

    settings.setValue(ui->vehicleWeight->objectName(), ui->vehicleWeight->text());
    settings.setValue(ui->wheelSize->objectName(), ui->wheelSize->text());
    settings.setValue(ui->gearRatio->objectName(), ui->gearRatio->text());
    settings.setValue(ui->Poles->objectName(), ui->Poles->text());

    settings.setValue(ui->Lq->objectName(), ui->Lq->text());
    settings.setValue(ui->Ld->objectName(), ui->Ld->text());
    settings.setValue(ui->Rs->objectName(), ui->Rs->text());
    settings.setValue(ui->FluxLinkage->objectName(), ui->FluxLinkage->text());

    inputGraph->saveWinState();
    modelGraph->saveWinState();
    errorGraph->saveWinState();
    resultsGraph->saveWinState();

    QWidget::closeEvent(event);
}


void MainWindow::on_pb_selectFile_clicked()
{
    struct file_data ipline;
    int timepos, idpos, iqpos, udpos, uqpos, frqpos, udcpos;
    QStringList fieldList;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CSV"), ui->le_filename->text(), tr("CSV Files (*.csv)"));
    ui->le_filename->setText(fileName);

    fdata.clear();
    inputGraph->clearData();
    modelGraph->clearData();
    errorGraph->clearData();
    resultsGraph->clearData();
    listLd.clear();
    listLq.clear();
    listRs.clear();
    listFL.clear();

    QFile inFile(fileName);
    inFile.open(QIODevice::ReadOnly);
    QTextStream sIn(&inFile);
    QString s=sIn.readLine(); //get column defs
    fieldList = s.split(u',');
    timepos = fieldList.indexOf("Timestamp");
    udcpos = fieldList.indexOf("udc");
    idpos = fieldList.indexOf("id");
    iqpos = fieldList.indexOf("iq");
    udpos = fieldList.indexOf("ud");
    uqpos = fieldList.indexOf("uq");
    frqpos = fieldList.indexOf("fstat");
    if((timepos>=0) && (udcpos>=0) && (idpos>=0) && (iqpos>=0) && (udpos>=0) && (uqpos>=0) && (frqpos>=0))
    {//have all required fields so check order
        QList<QPointF> listIq, listId, listVq, listVd, listFrq;
        bool firstTime = true;
        qint64 startTime = 0;
        while (!sIn.atEnd())
        {
          QString s=sIn.readLine();
          fieldList = s.split(u',');
          QString format = "yyyy-MM-ddTHH:mm:ss.zzz";
          QDateTime dt = QDateTime::fromString (fieldList[timepos], format);
          if(firstTime)
          {
            ipline.time = 0;
            startTime = dt.toMSecsSinceEpoch();
            firstTime = false;
          }
          else
            ipline.time = dt.toMSecsSinceEpoch() - startTime;

          double voltageDiv2 = fieldList[udcpos].toDouble()/2.0;
          ipline.id = fieldList[idpos].toDouble();
          ipline.iq = fieldList[iqpos].toDouble();
          ipline.ud = (voltageDiv2/32768)*fieldList[udpos].toDouble();
          ipline.uq = (voltageDiv2/32768)*fieldList[uqpos].toDouble();
          ipline.frq = fieldList[frqpos].toDouble();
          double secTime = ipline.time/1000.0;
          listId.append(QPointF(secTime, ipline.id));
          listIq.append(QPointF(secTime, ipline.iq));
          listVd.append(QPointF(secTime, ipline.ud));
          listVq.append(QPointF(secTime, ipline.uq));
          listFrq.append(QPointF(secTime, ipline.frq));

          fdata.push_back(ipline);
        }
        inputGraph->addDataPoints(listId, ID);
        inputGraph->addDataPoints(listIq, IQ);
        inputGraph->addDataPoints(listVd, VD);
        inputGraph->addDataPoints(listVq, VQ);
        inputGraph->addDataPoints(listFrq, FRQ);
        inputGraph->updateGraph();
        modelGraph->updateGraph();
        errorGraph->updateGraph();
        resultsGraph->updateGraph();
        ui->pb_Run->setEnabled(true);
        ui->pb_AutoTune->setEnabled(true);
        ui->pb_TuneFL->setEnabled(true);
        ui->pb_TuneLd->setEnabled(true);
        ui->pb_TuneLq->setEnabled(true);
        ui->pb_TuneRs->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this, tr("IPMMotorCalc"),
                                       tr("File does not contain required data fields.\n"
                                          "Minimum contents:Timestamp,udc,id,iq,ud,uq,fstat"));
        ui->le_filename->setText("");
        ui->pb_Run->setEnabled(false);
        ui->pb_AutoTune->setEnabled(false);
        ui->pb_TuneFL->setEnabled(false);
        ui->pb_TuneLd->setEnabled(false);
        ui->pb_TuneLq->setEnabled(false);
        ui->pb_TuneRs->setEnabled(false);
        ui->pb_CopyFL->setEnabled(false);
        ui->pb_CopyLd->setEnabled(false);
        ui->pb_CopyLq->setEnabled(false);
        ui->pb_CopyRs->setEnabled(false);
    }
    inFile.close();
}

void MainWindow::on_pb_Run_clicked()
{
    QList<QPointF> listVq, listVd, listFrq;
    QList<QPointF> listEVq, listEVd, listEFrq;

    motor->Restart();
    m_timenow = 0;
    modelGraph->clearData();
    errorGraph->clearData();

    double xmin, xmax;
    inputGraph->queryXaxis(&xmin, &xmax);
    for(int i=0; i<fdata.size()-1; i++)
    {
        if((fdata[i].time >= (1000*xmin)) && (fdata[i].time <= (1000*xmax)))
        {
            do
            {
                motor->setSpeedFromElecFreq(fdata[i].frq);//prevent cumulative drift
                motor->Step(fdata[i].iq, fdata[i].id);
                m_timenow++;
            }
            while(m_timenow < fdata[i+1].time);

            double error_vq = motor->getVq() - fdata[i].uq;
            double error_vd = motor->getVd() - fdata[i].ud;
            double error_frq = motor->getElecFreq() - fdata[i+1].frq;

            double secTime = m_timenow/1000.0;
            listEVd.append(QPointF(secTime, error_vd));
            listEVq.append(QPointF(secTime, error_vq));
            listEFrq.append(QPointF(secTime, error_frq));
            listVd.append(QPointF(secTime, motor->getVd()));
            listVq.append(QPointF(secTime, motor->getVq()));
            listFrq.append(QPointF(secTime, motor->getElecFreq()));
        }
    }

    errorGraph->addDataPoints(listEVd, VD);
    errorGraph->addDataPoints(listEVq, VQ);
    errorGraph->addDataPoints(listEFrq, FRQ);
    errorGraph->updateGraph();

    modelGraph->addDataPoints(listVd, VD);
    modelGraph->addDataPoints(listVq, VQ);
    modelGraph->addDataPoints(listFrq, FRQ);
    modelGraph->updateGraph();

}

void MainWindow::on_pb_TuneLq_clicked()
{
    double minError = std::numeric_limits<double>::max();
    double bestLq = 0;

    motor->Restart();
    m_timenow = 0;
    resultsGraph->clearData();
    listLq.clear();
    double scale = ui->Lq_Delta->text().toDouble()/10000.0;
    for(int percent=-100;percent<=100;percent++)
    {
        double Lq = m_Lq + (m_Lq * ((percent * scale)));
        motor->setLq(Lq);
        double totalError = 0;        
        double xmin, xmax;
        inputGraph->queryXaxis(&xmin, &xmax);
        for(int i=0; i<fdata.size()-1; i++)
        {
            if((fdata[i].time >= (1000*xmin)) && (fdata[i].time <= (1000*xmax)))
            {
                do
                {
                    motor->setSpeedFromElecFreq(fdata[i].frq);//prevent cumulative drift
                    motor->Step(fdata[i].iq, fdata[i].id);
                    m_timenow++;
                }
                while(m_timenow < fdata[i+1].time);
                double error_vd = motor->getVd() - fdata[i].ud;
                totalError += qFabs(error_vd);
            }
        }
        listLq.append(QPointF(Lq*1000, totalError));
        if(totalError < minError)
        {
            minError = totalError;
            bestLq = Lq;
        }
    }
    resultsGraph->addDataPoints(listLd, LD);
    resultsGraph->addDataPoints(listLq, LQ);
    resultsGraph->addDataPoints(listRs, RS);
    resultsGraph->addDataPoints(listFL, FL);
    resultsGraph->updateGraph();
    ui->Lq_BF->setText(QString::number(bestLq*1000));
    motor->setLq(m_Lq);
    ui->pb_CopyLq->setEnabled(true);
}

void MainWindow::on_pb_TuneLd_clicked()
{
    double minError = std::numeric_limits<double>::max();
    double bestLd = 0;

    motor->Restart();
    m_timenow = 0;
    resultsGraph->clearData();
    listLd.clear();
    double scale = ui->Ld_Delta->text().toDouble()/10000.0;
    for(int percent=-100;percent<=100;percent++)
    {
        double Ld = m_Ld + (m_Ld * ((percent * scale)));
        motor->setLd(Ld);
        double totalError = 0;
        double xmin, xmax;
        inputGraph->queryXaxis(&xmin, &xmax);
        for(int i=0; i<fdata.size()-1; i++)
        {
            if((fdata[i].time >= (1000*xmin)) && (fdata[i].time <= (1000*xmax)))
            {
                do
                {
                    motor->setSpeedFromElecFreq(fdata[i].frq);//prevent cumulative drift
                    motor->Step(fdata[i].iq, fdata[i].id);
                    m_timenow++;
                }
                while(m_timenow < fdata[i+1].time);
                double error_vq = motor->getVq() - fdata[i].uq;
                totalError += qFabs(error_vq);
            }
        }
        listLd.append(QPointF(Ld*1000, totalError));
        if(totalError < minError)
        {
            minError = totalError;
            bestLd = Ld;
        }
    }
    resultsGraph->addDataPoints(listLd, LD);
    resultsGraph->addDataPoints(listLq, LQ);
    resultsGraph->addDataPoints(listRs, RS);
    resultsGraph->addDataPoints(listFL, FL);
    resultsGraph->updateGraph();
    ui->Ld_BF->setText(QString::number(bestLd*1000));
    motor->setLd(m_Ld);
    ui->pb_CopyLd->setEnabled(true);
}

void MainWindow::on_pb_TuneRs_clicked()
{
    double minError = std::numeric_limits<double>::max();
    double bestRs = 0;

    motor->Restart();
    m_timenow = 0;
    resultsGraph->clearData();
    listRs.clear();
    double scale = ui->Rs_Delta->text().toDouble()/10000.0;
    for(int percent=-100;percent<=100;percent++)
    {
        double Rs = m_Rs + (m_Rs * ((percent * scale)));
        motor->setRs(Rs);
        double totalError = 0;
        double xmin, xmax;
        inputGraph->queryXaxis(&xmin, &xmax);
        for(int i=0; i<fdata.size()-1; i++)
        {
            if((fdata[i].time >= (1000*xmin)) && (fdata[i].time <= (1000*xmax)))
            {
                do
                {
                    motor->setSpeedFromElecFreq(fdata[i].frq);//prevent cumulative drift
                    motor->Step(fdata[i].iq, fdata[i].id);
                    m_timenow++;
                }
                while(m_timenow < fdata[i+1].time);
                double error_v = (qFabs(motor->getVq() - fdata[i].uq) + qFabs(motor->getVd() - fdata[i].ud))/2.0;
                totalError += error_v;
            }
        }
        listRs.append(QPointF(Rs*1000, totalError));
        if(totalError < minError)
        {
            minError = totalError;
            bestRs = Rs;
        }
    }
    resultsGraph->addDataPoints(listLd, LD);
    resultsGraph->addDataPoints(listLq, LQ);
    resultsGraph->addDataPoints(listRs, RS);
    resultsGraph->addDataPoints(listFL, FL);
    resultsGraph->updateGraph();
    ui->Rs_BF->setText(QString::number(bestRs*1000));
    motor->setRs(m_Rs);
    ui->pb_CopyRs->setEnabled(true);
}

void MainWindow::on_pb_TuneFL_clicked()
{
    double minError = std::numeric_limits<double>::max();
    double bestFL = 0;

    motor->Restart();
    m_timenow = 0;
    resultsGraph->clearData();
    listFL.clear();
    double scale = ui->FluxLinkage_Delta->text().toDouble()/10000.0;
    for(int percent=-100;percent<=100;percent++)
    {
        double FLink = m_fluxLinkage + (m_fluxLinkage * ((percent * scale)));
        motor->setFluxLinkage(FLink);
        double totalError = 0;
        double xmin, xmax;
        inputGraph->queryXaxis(&xmin, &xmax);
        for(int i=0; i<fdata.size()-1; i++)
        {
            if((fdata[i].time >= (1000*xmin)) && (fdata[i].time <= (1000*xmax)))
            {
                do
                {
                    motor->setSpeedFromElecFreq(fdata[i].frq);//prevent cumulative drift
                    motor->Step(fdata[i].iq, fdata[i].id);
                    m_timenow++;
                }
                while(m_timenow < fdata[i+1].time);
                double error_v = qFabs(motor->getVq() - fdata[i].uq);
                totalError += error_v;
            }
        }
        listFL.append(QPointF(FLink*1000, totalError));
        if(totalError < minError)
        {
            minError = totalError;
            bestFL = FLink;
        }
    }
    resultsGraph->addDataPoints(listLd, LD);
    resultsGraph->addDataPoints(listLq, LQ);
    resultsGraph->addDataPoints(listRs, RS);
    resultsGraph->addDataPoints(listFL, FL);
    resultsGraph->updateGraph();
    ui->FluxLinkage_BF->setText(QString::number(bestFL*1000));
    motor->setFluxLinkage(m_fluxLinkage);
    ui->pb_CopyFL->setEnabled(true);
}

void MainWindow::on_vehicleWeight_editingFinished()
{
    m_vehicleWeight = ui->vehicleWeight->text().toDouble();
    motor->setVehicleMass(m_vehicleWeight);
}

void MainWindow::on_wheelSize_editingFinished()
{
    m_wheelSize = ui->wheelSize->text().toDouble();
    motor->setWheelSize(m_wheelSize);
}

void MainWindow::on_gearRatio_editingFinished()
{
    m_gearRatio = ui->gearRatio->text().toDouble();
    motor->setGboxRatio(m_gearRatio);
}

void MainWindow::on_Poles_editingFinished()
{
    m_Poles = ui->Poles->text().toDouble();
    motor->setPoles(m_Poles);
}

void MainWindow::on_Lq_editingFinished()
{
    m_Lq = ui->Lq->text().toDouble()/1000; //entered in mH
    motor->setLq(m_Lq);
}

void MainWindow::on_Ld_editingFinished()
{
    m_Ld = ui->Ld->text().toDouble()/1000; //entered in mH
    motor->setLd(m_Ld);
}

void MainWindow::on_Rs_editingFinished()
{
    m_Rs = ui->Rs->text().toDouble()/1000;
    motor->setRs(m_Rs);
}

void MainWindow::on_FluxLinkage_editingFinished()
{
    m_fluxLinkage = ui->FluxLinkage->text().toDouble()/1000; //entered in mWeber
    motor->setFluxLinkage(m_fluxLinkage);
}

void MainWindow::on_pb_CopyLq_clicked()
{
    ui->Lq->setText(ui->Lq_BF->text());
    on_Lq_editingFinished();
}

void MainWindow::on_pb_CopyLd_clicked()
{
    ui->Ld->setText(ui->Ld_BF->text());
    on_Ld_editingFinished();
}

void MainWindow::on_pb_CopyRs_clicked()
{
    ui->Rs->setText(ui->Rs_BF->text());
    on_Rs_editingFinished();
}

void MainWindow::on_pb_CopyFL_clicked()
{
    ui->FluxLinkage->setText(ui->FluxLinkage_BF->text());
    on_FluxLinkage_editingFinished();
}

void MainWindow::on_pb_AutoTune_clicked()
{   //run each tune 4 times, FL first as it impacts on the others more than they impact on it
    for(int i=0;i<4;i++)
    {
        on_pb_TuneFL_clicked();
        on_pb_CopyFL_clicked();
        on_pb_TuneLd_clicked();
        on_pb_CopyLd_clicked();
        on_pb_TuneLq_clicked();
        on_pb_CopyLq_clicked();
    }
}
