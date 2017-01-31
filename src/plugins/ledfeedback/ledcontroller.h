#ifndef _LEDCONTROLLER_H
#define _LEDCONTROLLER_H

#include <QString>
class LedControllerPrivate;

class LedController
{
public:
    LedController();
    ~LedController();

    /**
     * Turn led number [1-N] on or off
     */
    void setLed(int number);

    /**
     * turn off all leds
     */
    void clear();

    /**
     * Write bit pattern
     */
    bool write(unsigned long value);

    QString errorMessage();

private:
    LedControllerPrivate* d;    
};


#endif
