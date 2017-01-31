#ifndef _FEEDBACK_DEFINED_
#define _FEEDBACK_DEFINED_

#include <QString>
#include "feedback/pluginfeedbackinterface.h"

namespace apex {

class FeedbackPrivate;
namespace data {
class ScreensData;
}
    
class Feedback
{
public:
    Feedback(const data::ScreensData* data);
    ~Feedback();
    
    void loadPlugin(const QString& name,
                    const FeedbackPluginParameters& params);

    //! initialize all plugins
    void initialize();
    
    void highLight(const QString& element);

    //! Indicate that the response was correct (thumbs up)
    void showPositive();

    //! Indicate that the response was false (thumbs down)
    void showNegative();

    //! Remove all feedback (highlight & positive/negative)
    void clear();

    

private:
    FeedbackPrivate* d;
};



}       // ns apex


#endif
