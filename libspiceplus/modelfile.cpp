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

#include <qdom.h>
#include <qregexp.h>

#include <klocale.h>
#include <kurl.h>

#include "modelfile.h"
#include "file.h"
#include "settings.h"

using namespace Spiceplus;

bool ModelFile::load(const QString &modelPath)
{
    KURL url = Settings::self()->modelPathToURL(modelPath);
    if (url.isEmpty())
    {
        m_errorString = i18n("Unknown model path: %1").arg(modelPath);
        return false;
    }

    return load(url);
}

bool ModelFile::load(const KURL &url)
{
    File file(url);

    if (!file.open(IO_ReadOnly))
    {
        m_errorString = file.errorString();
        return false;
    }

    QDomDocument doc;

    if (!doc.setContent(&file))
    {
        m_errorString = i18n("Invalid document structure");
        return false;
    }

    QDomElement root = doc.documentElement();

    if (root.tagName() != "model")
    {
        m_errorString = i18n("Invalid Model File");
        return false;
    }

    m_name = url.fileName().remove(QRegExp("\\.model$"));
    m_alias = root.attribute("alias");
    setType(root.attribute("type").lower());
    setDeviceType(root.attribute("device-type").lower());

    for (QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        QDomElement elem = node.toElement();
        if (elem.tagName() == "parameters")
        {
            for (QDomNode paramNode = node.firstChild(); !paramNode.isNull(); paramNode = paramNode.nextSibling())
            {
                if (!paramNode.isElement())
                    continue;

                QDomElement paramElem = paramNode.toElement();
                if (paramElem.tagName() == "parameter")
                {
                    QDomNode textNode = paramElem.firstChild();
                    if (!textNode.isNull() && textNode.isText())
                        setParameter(paramElem.attribute("name"), textNode.nodeValue());
                }
            }
        }
        else if (elem.tagName() == "symbol")
        {
            for (QDomNode symbolNode = node.firstChild(); !symbolNode.isNull(); symbolNode = symbolNode.nextSibling())
            {
                if (!symbolNode.isElement())
                    continue;

                QDomElement symbolElem = symbolNode.toElement();
                if (symbolElem.tagName() == "path")
                {
                    QDomNode textNode = symbolElem.firstChild();
                    if (!textNode.isNull() && textNode.isText())
                        m_symbolPath = textNode.nodeValue();
                }
            }
        }
    }

    return true;
}

bool ModelFile::save(const KURL &url)
{
    File file(url);

    if (!file.open(IO_WriteOnly))
    {
        m_errorString = file.errorString();
        return false;
    }

    QDomDocument doc("model");
    QDomElement root = doc.createElement("model");

    if (!m_alias.isEmpty())
        root.setAttribute("alias", m_alias);

    root.setAttribute("type", type());
    root.setAttribute("device-type", deviceType());
    doc.appendChild(root);

    QDomElement paramsElem = doc.createElement("parameters");
    root.appendChild(paramsElem);

    QMap<QString, QString> params = parameters();
    for (QMap<QString, QString>::Iterator it = params.begin(); it != params.end(); ++it)
    {
        QDomElement paramElem = doc.createElement("parameter");
        paramElem.setAttribute("name", it.key());
        paramElem.appendChild(doc.createTextNode(it.data()));
        paramsElem.appendChild(paramElem);
    }

    if (!m_symbolPath.isEmpty())
    {
        QDomElement pathElem = doc.createElement("path");
        pathElem.appendChild(doc.createTextNode(m_symbolPath));

        QDomElement symbolElem = doc.createElement("symbol");
        symbolElem.appendChild(pathElem);
        root.appendChild(symbolElem);
    }

    QTextStream stream(&file);
    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << doc.toString();

    if (!file.closeWithStatus())
    {
        m_errorString = file.errorString();
        return false;
    }

    return true;
}

// vim: ts=4 sw=4 et
