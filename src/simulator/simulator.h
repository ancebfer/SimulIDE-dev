/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

enum simState_t{
    SIM_STOPPED=0,
    SIM_ERROR,
    SIM_STARTING,
    SIM_PAUSED,
    SIM_WAITING,
    SIM_RUNNING,
    SIM_DEBUG,
};

#include <QElapsedTimer>
#include <QFuture>

class BaseProcessor;
class Updatable;
class eElement;
class eNode;
class CircMatrix;

class MAINMODULE_EXPORT Simulator : public QObject
{
    Q_OBJECT
    public:
        Simulator( QObject* parent=0 );
        ~Simulator();

 static Simulator* self() { return m_pSelf; }

         void addEvent( uint64_t time, eElement* el );
         void cancelEvents( eElement* el );

        void startSim( bool paused=false );
        void resumeSim();

        void pauseSim();
        void stopSim();

        void setWarning( int warning ) { m_warning = warning; }

        uint64_t stepSize() { return m_stepSize; }
        void setStepSize( uint64_t stepSize ) { m_stepSize = stepSize; }
        
        uint64_t fps() { return m_fps; }
        void setFps( uint64_t fps );
        uint64_t stepsPerFrame() { return m_stepsPF; }
        uint64_t realSPF() { return m_realStepsPF; }

        uint64_t stepsPerSec() { return m_stepsPS; }
        void setStepsPerSec( uint64_t sps );

        double NLaccuracy();

        void  setSlopeSteps( int steps ) { m_slopeSteps = steps; }
        int slopeSteps( ) { return m_slopeSteps; }

        void  setMaxNlSteps( uint32_t steps ) { m_maxNlstp = steps; }
        uint32_t maxNlSteps( ) { return m_maxNlstp; }
        
        bool isRunning() { return (m_state >= SIM_STARTING); }
        bool isPaused()  { return (m_state == SIM_PAUSED); }

        uint64_t circTime() { return m_circTime; }
        //void setCircTime( uint64_t time );

        void timerEvent( QTimerEvent* e );
        //uint64_t mS(){ return m_RefTimer.elapsed(); }

        double realSpeed() { return m_realSpeed; } // 0 to 10000 => 0 to 100%

        simState_t simState() { return m_state; }

        void notCorverged() { m_converged = false; }

        void addToEnodeList( eNode* nod );
        void remFromEnodeList( eNode* nod, bool del );

        void addToChangedNodes( eNode* nod );
        
        void addToElementList( eElement* el );
        void remFromElementList( eElement* el );
        
        void addToUpdateList( Updatable* el );
        void remFromUpdateList( Updatable* el );

        void addToChangedFast( eElement* el );
        void addToNoLinList( eElement* el );
        
    private:
 static Simulator* m_pSelf;

        void createNodes();
        void resetSim();
        void runCircuit();
        void solveMatrix();
        void solveCircuit();

        inline void clearEventList();

        inline void stopTimer();
        inline void initTimer();

        eElement* m_firstEvent;

        QFuture<void> m_CircuitFuture;

        CircMatrix* m_matrix;

        QHash<int, QString> m_errors;
        QHash<int, QString> m_warnings;

        QList<eNode*> m_eNodeList;

        eNode*    m_changedNode;
        eElement* m_voltChanged;
        eElement* m_nonLin;

        QList<eElement*> m_elementList;
        QList<Updatable*> m_updateList;

        simState_t m_state;
        simState_t m_oldState;

        bool m_converged;

        //int m_numEvents;

        int m_error;
        int m_warning;
        int m_timerId;
        int m_timerTick;
        int m_noLinAcc;
        int m_slopeSteps;

        uint64_t m_fps;
        uint32_t m_NLstep;
        uint32_t m_maxNlstp;

        uint64_t m_stepSize;
        uint64_t m_stepsPS;
        uint64_t m_stepsPF;
        uint64_t m_realStepsPF;
        double   m_realSpeed;

        uint64_t m_circTime;
        uint64_t m_tStep;
        uint64_t m_lastStep;
        uint64_t m_refTime;
        uint64_t m_lastRefT;
        uint64_t m_loopTime;
        double   m_load;

        uint64_t m_kkTime;

        QElapsedTimer m_RefTimer;
};
 #endif


