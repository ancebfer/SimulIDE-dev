/***************************************************************************
 *   Copyright (C) 2021 by santiago González                               *
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

#include <QFileInfo>

#include "avrgccdebugger.h"
#include "mcuinterface.h"
#include "codeeditor.h"
#include "utils.h"

AvrGccDebugger::AvrGccDebugger( CodeEditor* parent, OutPanelText* outPane )
              : BaseDebugger( parent, outPane )
{
    m_typesList["char"]   = "int8";
    m_typesList["uchar"]  = "uint8";
    m_typesList["byte"]   = "uint8";
    m_typesList["int"]    = "int16";
    m_typesList["uint"]   = "uint16";
    m_typesList["short"]  = "int16";
    m_typesList["ushort"] = "uint16";
    m_typesList["word"]   = "uint16";
    m_typesList["long"]   = "int32";
    m_typesList["ulong"]  = "uint32";
    m_typesList["float"]  = "float32";
    m_typesList["double"] = "float32";
}
AvrGccDebugger::~AvrGccDebugger(){}

void AvrGccDebugger::getData()
{
    QStringList lines = fileToStringList( m_file, "AvrGccDebugger::getAvrGccData" );
    getDevice( lines.first() );

    if( !McuInterface::self() ) return;

    m_varList.clear();

    for( QString line : lines )          // Get Variables from file
    {
        line = line.replace( "\t", " " ).remove(";");
        QStringList wordList= line.split( " " );
        wordList.removeAll( "" );

        if( wordList.isEmpty() ) continue;

        QString type = wordList.takeFirst();
        if( type == "unsigned" ) type = "u"+wordList.takeFirst();

        if( m_typesList.contains( type ) )
            for( QString word : wordList )
        {
            for( QString varName : word.split(",") )
            {
                if( varName.isEmpty() ) continue;
                varName.remove(" ");
                if( !m_varList.contains( varName ) )
                    m_varList[ varName ] = m_typesList[ type ];
                //qDebug() << "AvrGccDebugger::getAvrGccData  variable "<<type<<varName<<m_typesList[ type ];
    }   }   }

    QString objdump = m_toolPath+"avr-objdump";
    QString elfPath = m_buildPath+m_fileName+".elf";

    objdump = addQuotes( objdump );
    elfPath = addQuotes( elfPath );

    QProcess getBss( NULL );      // Get var addresses from .bss section
    QString command  = objdump+" -t -j.bss "+elfPath;
    getBss.start( command );
    getBss.waitForFinished(-1);

    QString  p_stdout = getBss.readAllStandardOutput();
    QStringList varNames = m_varList.keys();
    m_varNames.clear();
    m_subs.clear();

    for( QString line : p_stdout.split("\n") )
    {
        if( line.isEmpty() ) continue;
        QStringList words = line.split(" ");
        if( words.size() < 9 ) continue;
        if( words.at(6) != "O" ) continue;

        QString addr   = words.at(0);
        bool ok = false;
        int address = addr.toInt( &ok, 16 );
        if( !ok ) continue;

        QString symbol = words.at(8);
        QString type;

        if( varNames.contains( symbol ) ) type = m_varList.value( symbol );
        else{
            QString size = words.at(7);
            size = size.split("\t").last();
            type = "u"+QString::number( size.toInt()*8 );
        }
        address -= 0x800000;          // 0x800000 offset

        McuInterface::self()->addWatchVar( symbol, address, type );
        m_varNames.append( symbol );
        //qDebug() << "AvrGccDebugger::getAvrGccData  variable "<<type<<symbol<<address;
    }
}

void AvrGccDebugger::mapFlashToSource()
{
    m_flashToSource.clear();
    m_sourceToFlash.clear();

    QString elfPath = m_buildPath+m_fileName+".elf";
    QString avrSize = m_toolPath+"avr-size";
    QString addr2li = m_toolPath+"avr-addr2line";

    avrSize = addQuotes( avrSize );
    addr2li = addQuotes( addr2li );
    elfPath = addQuotes( elfPath );

    QProcess getSize( this );  // Get Firmware size
    getSize.start( avrSize + " " + elfPath );
    getSize.waitForFinished();
    QString lines = getSize.readAllStandardOutput();
    getSize.close();
    bool ok = false;
    int flashSize;
    if( !lines.isEmpty() )
    {
        QString size = lines.split("\n").at(1).split("\t").takeFirst().remove(" ");
        flashSize = size.toInt( &ok );
    }
    if( !ok ) flashSize = 35000;

    QProcess flashToLine( this );
    flashToLine.start( addr2li + " -e " + elfPath );
    bool started = flashToLine.waitForStarted( 1000 );
    if( !started ) return;

    for( int flashAddr=0; flashAddr<flashSize; ++flashAddr ) // Map Flash Address to Source Line
    {
        QString addr = val2hex( flashAddr )+"\n";
        flashToLine.write( addr.toUtf8() );
    }
    flashToLine.closeWriteChannel();
    flashToLine.waitForFinished();

    m_lastLine = 0;
    for( int flashAddr=0; flashAddr<flashSize; ++flashAddr ) // Map Flash Address to Source Line
    {
        QString p_stdout = flashToLine.readLine();
        if( p_stdout.isEmpty() ) continue;
        if( p_stdout.startsWith("?") ) continue;

        int idx = p_stdout.lastIndexOf( ":" );
        if( idx == -1 ) continue;

        QString fileName = QFileInfo( p_stdout.left( idx ) ).fileName();
        if( m_fileList.contains( fileName ) )
        {
            bool ok = false;
            int line = p_stdout.mid( idx+1 ).toInt( &ok );
            if( !ok ) continue;
            int addr = flashAddr/2;
            if( !m_sourceToFlash.contains( line ) )
            {
                if( line > m_lastLine ) m_lastLine = line;
                m_flashToSource[ addr ] = line;
                m_sourceToFlash[ line ] = addr;
            }
    }   }
    flashToLine.close();
}

