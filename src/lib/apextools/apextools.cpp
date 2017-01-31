/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexrandom.h"
#include "apextools.h"
#include "exceptions.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QMetaMethod>
#include <QScriptEngine>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

namespace apex {

// copied and modified from qobject.cpp
void ApexTools::connectSlotsByNameToPrivate(QObject *publicObject, QObject *privateObject)
{
    if (!publicObject)
        return;
    const QMetaObject *mo = privateObject->metaObject();
    Q_ASSERT(mo);
    const QObjectList list = qFindChildren<QObject*>(publicObject, QString());
    for (int i = 0; i < mo->methodCount(); ++i) {
        const char *slot = mo->method(i).signature();
        Q_ASSERT(slot);
        if (slot[0] != 'o' || slot[1] != 'n' || slot[2] != '_')
            continue;
        bool foundIt = false;
        for(int j = 0; j < list.count(); ++j) {
            const QObject *co = list.at(j);
            QByteArray objName = co->objectName().toAscii();
            int len = objName.length();
            if (!len || qstrncmp(slot + 3, objName.data(), len) ||
                    slot[len+3] != '_')
                continue;
            const QMetaObject *smo = co->metaObject();
            int sigIndex = smo->indexOfMethod(slot + len + 4);
            if (sigIndex < 0) { // search for compatible signals
                int slotlen = qstrlen(slot + len + 4) - 1;
                for (int k = 0; k < co->metaObject()->methodCount(); ++k) {
                    if (smo->method(k).methodType() != QMetaMethod::Signal)
                        continue;

                    if (!qstrncmp(smo->method(k).signature(), slot + len + 4,
                                slotlen)) {
                        sigIndex = k;
                        break;
                    }
                }
            }
            if (sigIndex < 0)
                continue;
            if (QMetaObject::connect(co, sigIndex, privateObject, i)) {
                foundIt = true;
                break;
            }
        }
        if (foundIt) {
            // we found our slot, now skip all overloads
            while (mo->method(i + 1).attributes() & QMetaMethod::Cloned)
                  ++i;
        } else if (!(mo->method(i).attributes() & QMetaMethod::Cloned)) {
            qWarning("connectSlotsByName: No matching signal for %s", slot);
        }
    }
}

/**
 * Make sure the given directory name ends in '/'
 * @param
 * @return
 */
QString ApexTools::MakeDirEnd(QString p_path) {
	if (p_path.isEmpty())
		return p_path;

    if (p_path.right(1) != "/") {
            p_path = p_path + "/";
    }

    return p_path;

}

QString ApexTools::addPrefix(const QString& base, const QString& prefix)
{
    QString result;

	if (prefix.isEmpty())
		return base;

    QUrl theUrl(base);
    if (theUrl.isRelative())
        result = ApexTools::MakeDirEnd(prefix) + base;
    else
        result = base;

    return result;
}


QString ApexTools::absolutePath(const QString& path) {
	QFileInfo fi(path);
    return fi.absoluteFilePath();
}


QString ApexTools::boolToString(bool b)
{
    return b ? "true" : "false";
}

QString ApexTools::removeXmlTags(const QString& str)
{
    QString result = str;
    return result.remove(QRegExp("<[^<]+>"));
}

/**
 * Generate a random number between lowest_number and highest_number.
 *
 * Generate a random number between lowest_number and highest_number using the standard random() function. srand is performed once automatically
 * @param lowest_number
 * @param highest_number
 * @return lowest_number<=int<=highest_number
 */
int ApexTools::RandomRange(int lowest_number, int highest_number)
{
        InitRand();

        if(lowest_number > highest_number){
                std::swap(lowest_number, highest_number);
        }

        int range = highest_number - lowest_number;
        //return lowest_number + int(range * rand()/(RAND_MAX + 1.0));
        /*int r=rand();
        return lowest_number + r%range;*/

        int result=(int) (ran1()*(range+1) + lowest_number);
        Q_ASSERT(result>=lowest_number);
        Q_ASSERT(result<=highest_number);

        return result;
}

/**
 * Generate a uniform float random number between lowest_number and highest_number.
 *
 * Generate a random number between lowest_number and highest_number using the standard random() function. srand is performed once automatically
 * @param lowest_number
 * @param highest_number
 * @return
 */
float ApexTools::RandomRange(double lowest_number, double highest_number) {
        // FIXME: meer bits gebruiken?

        InitRand();

        if(lowest_number > highest_number){
                std::swap(lowest_number, highest_number);
        }

        float result, rand_zero_to_one;

        //rand_zero_to_one = (double) rand()/RAND_MAX; //random number between zero and one
        rand_zero_to_one=ran1();

        result = (highest_number - lowest_number)*rand_zero_to_one + lowest_number;

        return result;
}


/**
 * Generate a gaussian double random number with sigma and mu
 *
 * Generate a random number between lowest_number and highest_number using the standard random() function. srand is performed once automatically
 * @param lowest_number
 * @param highest_number
 * @return
 */
/*double ApexTools::GaussRandomRange(double mu, double sigma) {
        // FIXME: meer bits gebruiken?

        if (!srandDone) {
                InitRand();
        }

        if(lowest_number > highest_number){
                std::swap(lowest_number, highest_number);               // FIXME geen pointers->kan dit werken?
        }

        double result, rand_zero_to_one;

        rand_zero_to_one = (double) rand()/RAND_MAX; //random number between zero and one

        result = (highest_number - lowest_number)*rand_zero_to_one + lowest_number;

        return result;
} */


//bool ApexTools::srandDone = false;

void ApexTools::InitRand() {
    static bool srandDone=false;
        if (!srandDone) {
                srand(static_cast<unsigned>(time(0)));
                ran1(-static_cast<long>(time(0)));
                srandDone=true;
        }
}


unsigned int RandomNumber::operator() (int count)
{
        return unsigned(count*(rand()/(RAND_MAX + 1.0)));
}

/**
 * Return random number, normally distributed
 * Numbers are generated in pairs, but returned one by one
 * Cfr Numerical Recipes in C
 * @return
 */
double ApexTools::Randn( )
{
    static int iset=0;
    static double gset;
    double fac,rsq,v1,v2;

    if (iset==0) {
        do {
            v1=2.0*ran1()-1.0;
            v2=2.0*ran1()-1.0;
            rsq=v1*v1+v2*v2;
        } while (rsq>=1.0 || rsq ==0.0);
        fac=sqrt(-2.0*log(rsq)/rsq);
        gset=v1*fac;
        iset=1;
        return v2*fac;
    } else {
        iset=0;
        return gset;
    }
}


/*std::string ApexTools::Q2StdString(const QString & p )
{
  return p.toStdString();
}*/

void ApexTools::ReplaceCharInString( QString& a_StringToSearch, const QChar& ac_ToReplace, const QString& ac_ReplaceWith )
{
  const unsigned nLen = a_StringToSearch.length();
  for( unsigned i = 0 ; i < nLen ; ++i )
  {
    if( a_StringToSearch.at( i ) == ac_ToReplace )
    {
      a_StringToSearch.remove( i, 1 );
      a_StringToSearch.insert( i, ac_ReplaceWith );
      i += ac_ReplaceWith.length();
    }
  }
}

void ApexTools::ReplaceWhiteSpaceWithNBSP( QString& a_StringToSearch )
{
//  const unsigned nLen = a_StringToSearch.length();
  const QString s( " " );
  const QChar p( *s.toAscii() );
  const QString sNBSP( "%20" );

  ReplaceCharInString( a_StringToSearch, p, sNBSP );
}

QString ApexTools::escapeJavascriptString (const QString &text)
{
    QString result = text;
    result.replace ("\\", "\\\\");
    result.replace ("\"", "\\\"");
    result.replace ("'", "\\'");
    result.replace ("\n", "\\n");
    result.replace ("\t", "\\t");
    result.replace ("\r", "\\r");
    result.replace ("\f", "\\f");
    return result;
}

bool ApexTools::bQStringToBoolean( const QString& ac_sBoolean ) {
    if( ac_sBoolean == "true" || ac_sBoolean == "1" )
        return true;
    else if( ac_sBoolean == "false" || ac_sBoolean == "0" )
        return false;
    else
        throw( ApexStringException("cannot convert string \"" +
                ac_sBoolean + "\" to boolean") );
}

int ApexTools::maximumFontPointSize (const QString &text, const QSize &box,
        const QFont &originalFont)
{
    const int width = box.width();
    const int height = box.height();
    QFont font (originalFont);

    Q_FOREVER {
        const int pointSize = font.pointSize();
        QSize result = QFontMetrics (font).size (0, text);
        if (result.width() < width && result.height() < height)
            return pointSize;
        if (pointSize <= 1)
            return -1;
        font.setPointSize (pointSize - 1);
    }
}

void ApexTools::shrinkTillItFits (QWidget *widget, const QString &text,
        const QSize &border)
{
    QFont font = widget->font();

    const int fontSize = maximumFontPointSize (text, widget->size() - border,
            widget->font());

    if (fontSize > 0) {
        font.setPointSize (fontSize);
        widget->setFont (font);
    }
}


QString ApexTools::findReadableFile (const QString &baseName,
                                     const QStringList &directories, const QStringList &extensions,
                                     const QStringList &prefixes)
{
    QFileInfo fileInfo;
    Q_FOREACH (const QString &directory,
               QStringList (directories) << QString()) {
        Q_FOREACH (const QString &extension,
                   QStringList (extensions) << QString()) {
            Q_FOREACH (const QString &prefix,
                       QStringList (prefixes) << QString()) {
                fileInfo.setFile (QDir (directory
                                       ), prefix + baseName + extension);
                /*qDebug("Trying %s",
                                                 qPrintable(directory+"/"+prefix + baseName + extension));*/
                if (fileInfo.exists() && fileInfo.isFile() &&
                        fileInfo.isReadable())
                    return fileInfo.filePath();
            }
        }
    }
    return QString();
}

QScriptValue ApexTools::QVariant2ScriptValue(QScriptEngine *e, QVariant var)
{
    if(var.isNull())
        return e->nullValue();

    switch(var.type())
    {
        case QVariant::Invalid:
            return e->nullValue();
        case QVariant::Bool:
            return QScriptValue(e, var.toBool());
        case QVariant::Date:
            return e->newDate(var.toDateTime());
        case QVariant::DateTime:
            return e->newDate(var.toDateTime());
        case QVariant::Double:
            return QScriptValue(e, var.toDouble());
        case QVariant::Int:
        case QVariant::LongLong:
            return QScriptValue(e, var.toInt());
        case QVariant::String:
            return QScriptValue(e, var.toString());
        case QVariant::Time:
            return e->newDate(var.toDateTime());
        case QVariant::UInt:
            return QScriptValue(e, var.toUInt());
        default:
            qFatal("Type conversion not implemented");
    }

    // bad but better then nothing
    return QScriptValue(e, var.toString());
}




QStringList ApexTools::unique(const QStringList& list)
{
     QStringList n;
     Q_FOREACH(const QString s, list) {
          if (!n.contains(s))
              n.push_back(s);
     }
     return n;
}

QStringList ApexTools::toStringList(const QList<int> intList)
{
    QStringList result;

    Q_FOREACH (int i, intList)
        result << QString::number(i);

    return result;
}

}
