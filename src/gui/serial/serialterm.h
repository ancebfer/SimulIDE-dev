/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#ifndef SERIALTERM_H
#define SERIALTERM_H

#include "component.h"
#include "e-element.h"
#include "usartmodule.h"

class LibraryItem;
class CustomButton;
class QGraphicsProxyWidget;

class MAINMODULE_EXPORT SerialTerm : public Component, public UsartModule, public eElement
{
    public:
        SerialTerm( QString type, QString id );
        ~SerialTerm();
        
 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void runEvent() override;

        int dataBits(){ return m_dataBits; }
        void setDataBits( int db ){ m_dataBits = db; }

        int parity() { return (int)m_parity; }
        void setParity( int par ) { m_parity = (parity_t)par; }

        int stopBits() { return m_stopBits; }
        void setStopBits( int sb ) { m_stopBits = sb; }

        void setSerialMon( bool s );

        virtual void sendByte( uint8_t data ) override;

        virtual void byteReceived( uint8_t byte ) override;
        virtual void frameSent( uint8_t data ) override;

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );

    public slots:
        void slotOpenTerm();

    protected:
        //virtual void setflip() override;
        virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu );

    private:
        bool m_receiving;
        bool m_sending;

        //QByteArray m_serData;
        QByteArray m_uartData;
};

#endif

