/***************************************************************************
 *   Copyright (C) 2020 by Benoit ZERR                                     *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified 2020 by Santiago González                                    *
 *   santigoro@gmail.com                                                   *
 *                                                                         */

#pragma once

#include "thermistorbase.h"

class LibraryItem;

class RTD : public ThermistorBase
{
     public:
        RTD( QString type, QString id );
        ~RTD();

        static Component* construct( QString type, QString id );
        static LibraryItem* libraryItem();

        double getR0() { return m_r0; }
        void setR0( double r0 ) { m_r0 = r0; }

        virtual void updateStep() override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

     private:
        double sensorFunction ( double temp );

        uint64_t m_lastTime;

        double m_tau = 0.3;  // 0.3 in water 4.0 in air
        double m_t0_tau = 0.0;
        double m_r0 = 100.0;
        //double coef_temp = 0.00385;  // linear approximation
        //here we use a more realist third order polynomial approximation
        double coef_temp_a = 3.9083e-3;   // a * T
        double coef_temp_b = -5.775e-7;   // b*T2
        double coef_temp_c = -4.183e-12;  // c*(T-100)*T3 (T < 0 only)
};
