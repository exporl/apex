/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the ActiveQt framework of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "axobject.h"

#ifndef QT_NO_WIN_ACTIVEQT

#include <quuid.h>
#include <qmetaobject.h>
#include <qstringlist.h>

#include <windows.h>

QT_BEGIN_NAMESPACE

/*!
    \class AxObject
    \brief The AxObject class provides a QObject that wraps a COM object.

    \inmodule AxContainer

    A AxObject can be instantiated as an empty object, with the name
    of the COM object it should wrap, or with a pointer to the
    IUnknown that represents an existing COM object. If the COM object
    implements the \c IDispatch interface, the properties, methods and
    events of that object become available as Qt properties, slots and
    signals. The base class, AxBase, provides an API to access the
    COM object directly through the IUnknown pointer.

    AxObject is a QObject and can be used as such, e.g. it can be
    organized in an object hierarchy, receive events and connect to
    signals and slots.

    AxObject also inherits most of its ActiveX-related functionality
    from AxBase, notably dynamicCall() and querySubObject().

    \warning
    You can subclass AxObject, but you cannot use the Q_OBJECT macro
    in the subclass (the generated moc-file will not compile), so you
    cannot add further signals, slots or properties. This limitation is 
    due to the metaobject information generated in runtime. 
    To work around this problem, aggregate the AxObject as a member of
    the QObject subclass.

    \sa AxBase, AxWidget, AxScript, {ActiveQt Framework}
*/

/*!
    Creates an empty COM object and propagates \a parent to the
    QObject constructor. To initialize the object, call \link
    AxBase::setControl() setControl \endlink.
*/
AxObject::AxObject(QObject *parent)
: QObject(parent)
{
}

/*!
    Creates a AxObject that wraps the COM object \a c. \a parent is
    propagated to the QObject constructor.

    \sa setControl()
*/
AxObject::AxObject(const QString &c, QObject *parent)
: QObject(parent)
{
    setControl(c);
}

/*!
    Creates a AxObject that wraps the COM object referenced by \a
    iface. \a parent is propagated to the QObject constructor.
*/
AxObject::AxObject(IUnknown *iface, QObject *parent)
: QObject(parent), AxBase(iface)
{
}

/*!
    Releases the COM object and destroys the AxObject,
    cleaning up all allocated resources.
*/
AxObject::~AxObject()
{
    clear();
}

/*!
    \internal
*/
const QMetaObject *AxObject::metaObject() const
{
    return AxBase::metaObject();
}

/*!
    \internal
*/
const QMetaObject *AxObject::parentMetaObject() const
{
    return &QObject::staticMetaObject;
}

/*!
    \internal
*/
void *AxObject::qt_metacast(const char *cname)
{
    if (!qstrcmp(cname, "AxObject")) return (void*)this;
    if (!qstrcmp(cname, "AxBase")) return (AxBase*)this;
    return QObject::qt_metacast(cname);
}

/*!
    \internal
*/
const char *AxObject::className() const
{
    return "AxObject";
}

/*!
    \internal
*/
int AxObject::qt_metacall(QMetaObject::Call call, int id, void **v)
{
    id = QObject::qt_metacall(call, id, v);
    if (id < 0)
        return id;
    return AxBase::qt_metacall(call, id, v);
}

/*!
    \fn QObject *AxObject::qObject() const
    \internal
*/

/*!
    \reimp
*/
void AxObject::connectNotify(const char *)
{
    AxBase::connectNotify();
}

/*!
    \since 4.1

    Requests the COM object to perform the action \a verb. The
    possible verbs are returned by verbs().

    The function returns true if the object could perform the action, otherwise returns false.
*/
bool AxObject::doVerb(const QString &verb)
{
    if (!verbs().contains(verb))
        return false;
    IOleObject *ole = 0;
    queryInterface(IID_IOleObject, (void**)&ole);
    if (!ole)
        return false;

    LONG index = indexOfVerb(verb);

    HRESULT hres = ole->DoVerb(index, 0, 0, 0, 0, 0);

    ole->Release();

    return hres == S_OK;
}

QT_END_NAMESPACE
#endif // QT_NO_WIN_ACTIVEQT
