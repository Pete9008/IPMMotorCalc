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

#ifndef MOTORMODEL_H
#define MOTORMODEL_H

#include <QtMath>

class MotorModel
{
public:
    MotorModel(double wheelSize,double ratio,double roadGradient,double mass,double Lq,double Ld,double Rs,double poles,double fluxLink,double timestep, double syncDelay, double sampPoint);
    void Step(double Iq, double Id);
    void Restart(void);
    void setWheelSize(double val) {m_WheelSize = val;}
    void setGboxRatio(double val) {m_Ratio = val;}
    void setVehicleMass(double val) {m_Mass = val;}
    void setLq(double val) {m_Lq = val;}
    void setLd(double val) {m_Ld = val;}
    void setRs(double val) {m_Rs = val;}
    void setPoles(double val) {m_Poles = val;}
    void setFluxLinkage(double val) {m_FluxLink = val;}
    void setSyncDelay(double val) {m_syncdelay = val;}
    void setTimestep(double val) {m_Timestep = val;}
    void setPosition(double val) {m_Position = (val * m_Poles);}
    void setSamplingPoint(double val) {m_samplingPoint = val;}
    void setRoadGradient(double val) {m_RoadGradient = val;}
    double getMotorPosition(void);
    double getElecPosition(void);
    double getMotorFreq(void) {return m_Frequency;}
    double getElecFreq(void) {return m_Frequency*m_Poles;}
    void setSpeedFromElecFreq(double val);
    bool getMotorDirection(void) {return (m_Speed>=0);}
    double getIq(void) {return m_Iq;} //model output
    double getId(void) {return m_Id;}
    double getVd(void) {return m_Vd;}
    double getVq(void) {return m_Vq;}
    double getVq_bemf(void) {return m_Vq_bemf;}
    double getVq_dueto_id(void) {return m_Vq_dueto_id;}
    double getVd_dueto_iq(void) {return m_Vd_dueto_iq;}
    double getVq_dueto_Rq(void) {return m_Vq_dueto_Rq;}
    double getVd_dueto_Rd(void) {return m_Vd_dueto_Rd;}
    double getVLd(void) {return m_VLd;}
    double getVLq(void) {return m_VLq;}
    double getPower(void) {return m_Power;}
    double getTorque(void) {return m_Torque;}


private:
    double m_WheelSize;
    double m_Ratio;
    double m_RoadGradient;
    double m_Mass;
    double m_Lq;
    double m_Ld;
    double m_Rs;
    double m_Poles;
    double m_FluxLink; //Hz
    double m_syncdelay;
    double m_samplingPoint; //sampling position as fraction of period, 0=start, 1=end

    double m_Position; //degrees
    double m_Frequency; // Hz motor speed (NOT electrical)
    double m_Timestep;
    double m_Id, m_Iq;
    double m_Speed; // m/s
    double m_Power;
    double m_Torque; //motor torque

    double m_Vd;
    double m_Vq;
    double m_Vq_bemf;
    double m_Vq_dueto_id;
    double m_Vd_dueto_iq;
    double m_Vq_dueto_Rq;
    double m_Vd_dueto_Rd;
    double m_VLd;
    double m_VLq;
};

#endif // MOTORMODEL_H
