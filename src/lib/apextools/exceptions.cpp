#include "exceptions.h"

    ApexStringException::ApexStringException (const QString &message) :
        message (message.toLocal8Bit())
        {
        }

        /*QString ApexStringException::GetMessage() const
        {
            return QString::fromLocal8Bit (message);
        }*/

        const char* ApexStringException::what() const throw()
        {
            return message;
        }

        ApexStringException::~ApexStringException() throw ()
        {
        }




        const char* TrialDataNotFoundException::what() const throw()
        {
            return "TrialDataNotFoundException";
        }



    ApexConnectionBetweenDifferentDevicesException::ApexConnectionBetweenDifferentDevicesException (const QString& message):
        ApexStringException (message)
        {
        }



        const char * XercesException::what() const throw()
        {
            return "XercesException";
        }



        const char * ParseException::what() const throw()
        {
            return "ParseException";
        }


