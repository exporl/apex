#ifndef _EXPORL_SRC_PLUGINS_LEDFEEDBACKPLUGIN_LEDCONTROLLER_H_
#define _EXPORL_SRC_PLUGINS_LEDFEEDBACKPLUGIN_LEDCONTROLLER_H_

#include <QLoggingCategory>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(APEX_LEDFEEDBACKPLUGIN)

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
