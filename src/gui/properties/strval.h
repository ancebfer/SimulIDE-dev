/***************************************************************************
 *   Copyright (C) 2021 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "ui_strval.h"
#include "propval.h"

class Component;
class PropDialog;

class StrVal : public PropVal, private Ui::StrVal
{
    Q_OBJECT
    
    public:
        StrVal( PropDialog* parent, CompBase* comp, ComProperty* prop );
        ~StrVal();

        virtual void setup( bool ) override;
        virtual void updtValues() override;

    public slots:
        void on_value_editingFinished();
};
