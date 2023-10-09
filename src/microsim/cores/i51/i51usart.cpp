/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "i51usart.h"
#include "usarttx.h"
#include "usartrx.h"
#include "mcutimer.h"
#include "e_mcu.h"
#include "i51interrupt.h"
#include "datautils.h"

#define SCON *m_scon

I51Usart::I51Usart( eMcu* mcu, QString name, int number )
        : McuUsart( mcu, name, number )
{
    m_stopBits = 1;
    m_dataMask = 0xFF;
    m_parity   = parNONE;

    m_timer1 = mcu->getTimer( "TIMER1" );

    m_scon = mcu->getReg( "SCON" );

    m_SM     = getRegBits( "SM1,SM0", mcu );
    m_bit9Tx = getRegBits( "TB8", mcu );
    m_bit9Rx = getRegBits( "RB8", mcu );
    m_SM2    = getRegBits( "SM2", mcu );

    m_SMOD = getRegBits( "SMOD", mcu );
}
I51Usart::~I51Usart(){}

void I51Usart::reset()
{
    m_sender->enable( true ); // Sender always enabled
    m_mode = 0xFF;
    m_smodDiv = false;
    m_smodVal = 0;
}

void I51Usart::configureA( uint8_t newSCON ) //SCON
{
    uint8_t mode = getRegBitsVal( newSCON, m_SM );
    bool sm2 = getRegBitsBool( newSCON, m_SM2 );

    if( mode == m_mode ){
        if( mode == 2 || mode == 3 ) m_receiver->ignoreData( sm2 );
        return;
    }
    m_mode = mode;

    m_useTimer = false;

    switch( mode )
    {
        case 0:             // Synchronous 8 bit
            /// TODO //setPeriod(  m_mcu->psInst() );// Fixed baudrate 32 or 64
            sm2 = 0;
            m_dataBits = 8;
            break;
        case 1:             // Asynchronous Timer1 8 bits
            m_useTimer = true;
            m_dataBits = 8;
            sm2 = 0;
            /// TODO: Ignore frame if wrong Stop bit:
            ///     override frameError() // Frame Error: wrong stop bit
            ///
            /// also parityError() and overrunError() not inplemented in 8051
            ///
            break;
        case 2:             // Asynchronous MCU Clock 9 bits
            setPeriod(  m_mcu->psInst() );// Fixed baudrate 32 or 64
            m_dataBits = 9;
            break;
        case 3:             // Asynchronous Timer1 9 bits
            m_useTimer = true;
            m_dataBits = 9;
            break;
    }

    m_receiver->ignoreData( sm2 );

    I51T1Int* t1Int = static_cast<I51T1Int*>( m_timer1->getInterrupt() );
    if( m_useTimer )
    {
        t1Int->setUsart( this );
        setPeriod( 0 );
    }
    else t1Int->setUsart( NULL );
}

void I51Usart::configureB( uint8_t newPCON )
{
    m_smodVal = getRegBitsVal( newPCON, m_SMOD );
}

void I51Usart::sendByte( uint8_t data )
{
    if( m_mcu->state() != mcuStopped ) m_sender->processData( data );
}

void I51Usart::step()
{
    if( !m_useTimer ) return;

    if( (m_smodVal & 1) == 0 )
    {
        m_smodDiv = !m_smodDiv;
        if( m_smodDiv ) return;
    }
    m_sender->runEvent();
    m_receiver->runEvent();
}

void I51Usart::setRxFlags( uint16_t frame )
{
    //writeRegBits( m_FE, frame & frameError );   // frameError
    //writeRegBits( m_DOR, frame & dataOverrun ); // overrun error
    //writeRegBits( m_UPE, frame & parityError ); // parityError
}
