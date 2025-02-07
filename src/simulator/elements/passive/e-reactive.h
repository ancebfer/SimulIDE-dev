/***************************************************************************
 *   Copyright (C) 2022 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "e-resistor.h"

class eReactive : public eResistor
{
    public:
        eReactive( QString id );
        ~eReactive();

        virtual void stamp() override;
        virtual void voltChanged() override;
        virtual void runEvent() override;

        double initVolt() { return m_InitVolt; }
        void setInitVolt( double v ) { m_InitVolt = v; }

        double initCurr() { return -m_InitCurr; }
        void setInitCurr( double c ) { m_InitCurr = -c; }

    protected:
        void updtReactStep();

        virtual double updtRes(){ return 0.0;}
        virtual double updtCurr(){ return 0.0;}

        double m_value; // Capacitance or Inductance

        double m_InitCurr;
        double m_curSource;

        double m_InitVolt;
        double m_volt;

        double m_tStep;

        uint64_t m_reacStep;
        uint64_t m_timeStep;

        bool m_running;
};
