/*
 * SPICE+
 * Copyright (C) 2004 Andreas Unger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INDEPENDENTSOURCE_H
#define INDEPENDENTSOURCE_H

#include "schematicstandarddevice.h"

class QWidget;
class QWidgetStack;
class QString;
class QComboBox;

namespace Spiceplus {

class ParameterLineEdit;
class EditListView;

namespace Plugin {

class IndependentSource : public SchematicStandardDevice
{
public:
    IndependentSource(Schematic *schematic);

    bool initialize();
    void updateLabels();
    void setDefaults();

    ModelPolicy modelPolicy() const { return None; }

    bool hasCommand() const { return true; }
    QString createCommand();

    bool hasPropertiesWidget() const { return true; }
    SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent = 0);

    virtual QString description() const = 0;
    virtual QString unit() const = 0;
};

class IndependentSourceValues : public QWidget
{
public:
    IndependentSourceValues(IndependentSource *source, QWidget *parent = 0) : QWidget(parent), m_source(source) {}
    virtual ~IndependentSourceValues() {}

    virtual void loadParameters() = 0;
    virtual bool saveParameters() const = 0;
    QString errorString() const { return m_errorString; }

protected:
    IndependentSource *m_source;
    QString m_errorString;
};

class IndependentSourceValuesDC : public IndependentSourceValues
{
public:
    IndependentSourceValuesDC(IndependentSource *source, QWidget *parent = 0);

    void loadParameters();
    bool saveParameters() const;

private:
    ParameterLineEdit *m_dcvalue;
};

class IndependentSourceValuesPulse : public IndependentSourceValues
{
public:
    IndependentSourceValuesPulse(IndependentSource *source, QWidget *parent = 0);

    void loadParameters();
    bool saveParameters() const;

private:
    ParameterLineEdit *m_v1;
    ParameterLineEdit *m_v2;
    ParameterLineEdit *m_td;
    ParameterLineEdit *m_tr;
    ParameterLineEdit *m_tf;
    ParameterLineEdit *m_pw;
    ParameterLineEdit *m_per;
};

class IndependentSourceValuesSin : public IndependentSourceValues
{
public:
    IndependentSourceValuesSin(IndependentSource *source, QWidget *parent = 0);

    void loadParameters();
    bool saveParameters() const;

private:
    ParameterLineEdit *m_vo;
    ParameterLineEdit *m_va;
    ParameterLineEdit *m_freq;
    ParameterLineEdit *m_td;
    ParameterLineEdit *m_theta;
};

class IndependentSourceValuesExp : public IndependentSourceValues
{
public:
    IndependentSourceValuesExp(IndependentSource *source, QWidget *parent = 0);

    void loadParameters();
    bool saveParameters() const;

private:
    ParameterLineEdit *m_v1;
    ParameterLineEdit *m_v2;
    ParameterLineEdit *m_td1;
    ParameterLineEdit *m_tau1;
    ParameterLineEdit *m_td2;
    ParameterLineEdit *m_tau2;
};

class IndependentSourceValuesPWL : public IndependentSourceValues
{
    Q_OBJECT

public:
    IndependentSourceValuesPWL(IndependentSource *source, QWidget *parent = 0);

    void loadParameters();
    bool saveParameters() const;

private slots:
    void insertPoint();

private:
    EditListView *m_points;
    ParameterLineEdit *m_time;
    ParameterLineEdit *m_value;
};

class IndependentSourceValuesSFFM : public IndependentSourceValues
{
public:
    IndependentSourceValuesSFFM(IndependentSource *source, QWidget *parent = 0);

    void loadParameters();
    bool saveParameters() const;

private:
    ParameterLineEdit *m_vo;
    ParameterLineEdit *m_va;
    ParameterLineEdit *m_fc;
    ParameterLineEdit *m_mdi;
    ParameterLineEdit *m_fs;
};

class IndependentSourcePropertiesWidget : public SchematicDevicePropertiesWidget
{
    Q_OBJECT

public:
    IndependentSourcePropertiesWidget(IndependentSource *source, QWidget *parent = 0);

    bool apply();

private:
    enum Type
    {
        DC,
        Pulse,
        Sin,
        Exp,
        PWL,
        SFFM
    };

    IndependentSource *m_source;
    QWidgetStack *m_stack;

    ParameterLineEdit *m_name;
    QComboBox *m_type;
    ParameterLineEdit *m_acmag;
    ParameterLineEdit *m_acphase;
    ParameterLineEdit *m_distof1mag;
    ParameterLineEdit *m_distof1phase;
    ParameterLineEdit *m_distof2mag;
    ParameterLineEdit *m_distof2phase;
};

} // namespace Plugin
} // namespace Spiceplus

#endif // INDEPENDENTSOURCE_H

// vim: ts=4 sw=4 et
