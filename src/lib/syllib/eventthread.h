/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#ifndef _SYL_SRC_LIB_EVENTTHREAD_H_
#define _SYL_SRC_LIB_EVENTTHREAD_H_

/** @file
 * Thread with event loop declaration.
 */

#include "global.h"

#include <QThread>

namespace syl
{

/** Thread with an event loop. It calls #exec() in its #run() method.
 * Allows the use of
 *
 * @code
 * // create the producer and consumer and plug them together
 * Producer producer;
 * Consumer consumer;
 * producer.connect (&consumer, SIGNAL (consumed()), SLOT (produce()));
 * consumer.connect (&producer, SIGNAL (produced(QByteArray*)),
 *     SLOT (consume(QByteArray*)));
 *
 * // they both get their own thread
 * EventThread producerThread;
 * producer.moveToThread (&producerThread);
 * EventThread consumerThread;
 * consumer.moveToThread (&consumerThread);
 *
 * // go!
 * producerThread.start();
 * consumerThread.start();
 * @endcode
 *
 * @todo This class can go away with Qt 4.4.
 *
 * @see
 * http://labs.trolltech.com/blogs/2006/12/04/threading-without-the-headache/
 */
class EventThread : public QThread
{
public:
    /** Creates a new thread with an event loop. Use #start() to start the event
     * loop.
     *
     * @param parent parent object that should take ownership or @c NULL
     */
    EventThread (QObject *parent = NULL) :
        QThread (parent)
    {
    }

protected:
    /** Calls exec(). */
    virtual void run()
    {
        exec();
    }
};

} // namespace syl

#endif

