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

#ifndef MODEL_H
#define MODEL_H

#include <qstring.h>
#include <qmap.h>
#include <qwidget.h>

#include <klibloader.h>

namespace Spiceplus {

class Model
{
public:
    Model();
    virtual ~Model() {}

    QString type() const { return m_type; }
    void setType(const QString &type) { m_type = type; }
    QString deviceType() const { return m_deviceType; }
    void setDeviceType(const QString &deviceType) { m_deviceType = deviceType; }

    QString parameter(const QString &name) const;
    void setParameter(const QString &name, const QString &value) { m_parameters[name] = value; }
    void removeParameter(const QString &name) { m_parameters.remove(name); }

    QString createCommand(const QString &modelName);

    bool operator==(const Model &model) const;
    bool operator!=(const Model &model) const;

    bool isNull() const { return m_type.isNull() || m_deviceType.isNull(); }

protected:
    QMap<QString, QString> parameters() const { return m_parameters; }

private:
    QString m_type;
    QString m_deviceType;
    QMap<QString, QString> m_parameters;
};

class ModelView : public QWidget
{
    Q_OBJECT

public:
    ModelView(QWidget *parent = 0) : QWidget(parent) {}

    virtual void update() = 0;
    virtual bool apply() = 0;
    QString errorString() const { return m_errorString; }

signals:
    void modified(bool yes);

protected:
    void setErrorString(const QString &str) { m_errorString = str; }

private:
    QString m_errorString;
};

class ModelViewFactory : public KLibFactory
{
    Q_OBJECT

public:
    virtual ModelView *createModelView(Model &model, QWidget *parent = 0) = 0;
    QString errorString() const { return m_errorString; }

protected:
    void setErrorString(const QString &str) { m_errorString = str; }
    QObject *createObject(QObject *, const char *, const char *, const QStringList &) { return 0; }

private:
    QString m_errorString;
};

} // namespace Spiceplus

#endif // MODEL_H

// vim: ts=4 sw=4 et
