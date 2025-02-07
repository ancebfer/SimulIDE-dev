/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "ui_boolval.h"
#include "propval.h"

class Component;
class PropDialog;

class BoolVal : public PropVal, private Ui::BoolVal
{
    Q_OBJECT
    
    public:
        BoolVal( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~BoolVal();
        
        virtual void setup( bool ) override;
        virtual void updtValues() override;

    public slots:
        void on_trueVal_toggled( bool checked );
};
