/*
 * SPICE+
 * Copyright (C) 2004 Andreas Unger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SYMBOLDIRSEDIT_H
#define SYMBOLDIRSEDIT_H

#include <qwidget.h>
#include <qstring.h>

#include <kdialogbase.h>

class QLineEdit;
class QPushButton;
class KURL;
class KListView;
class KURLRequester;

namespace Spiceplus {

class SymbolDirs;

class ParameterLineEdit;

class SymbolDirsEditInputDialog : public KDialogBase
{
    Q_OBJECT

public:
    SymbolDirsEditInputDialog(const QString &caption, QWidget *parent = 0);
    SymbolDirsEditInputDialog(const QString &caption, const QString &id, const QString &name, const QString &path, QWidget *parent = 0);

    QString id() const;
    QString name() const;
    QString path() const;

protected slots:
    void slotOk();

private:
    void init();

    ParameterLineEdit *m_id;
    QLineEdit *m_name;
    KURLRequester *m_path;
};

class SymbolDirsEdit : public QWidget
{
    Q_OBJECT

public:
    SymbolDirsEdit(QWidget *parent = 0);
    bool isEmpty() const;

    SymbolDirs dirs() const;
    void setDirs(const SymbolDirs &symbolDirs);

public slots:
    void clear();

signals:
    void modified();

private slots:
    void addDir();
    void modifyDir();
    void removeDir();

    void updateButtons();

private:
    KListView *m_dirs;
    QPushButton *m_modifyButton;
    QPushButton *m_removeButton;
};

} // namespace Spiceplus

#endif // SYMBOLDIRSEDIT_H

// vim: ts=4 sw=4 et
