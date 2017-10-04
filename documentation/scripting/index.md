Scripting APEX
==============

APEX is designed to allow setting up experiments without programming.
However, in some cases it can be convenient to script some tasks, to
avoid repetitive typing work and in case special behaviour is desired
which is not built into APEX. In the following sections, we will
describe how to implement custom procedures using Javascript
(see [Plugin procedures](#pluginprocedures)), and how to automatically generate
trials, datablocks and stimuli (see [XML Plugin Api](#xmlpluginapi)).

The programming language used for these scripts is JavaScript, as
implemented by Qt. Abundant online documentation of the Javascript
language is available , and in most cases modifying examples provided
with APEX will suffice to achieve the desired results.

Note that these are more advanced APEX features, and while they are
convenient, in most cases it is possible to avoid using them.

Plugin procedures <a name="pluginprocedures"/>
-----------------

As an example of a plugin procedure, we will discuss the adjustment
procedure that is included with the APEX examples
(examples/procedure/adjustment-pluginprocedure.apx).

### The experiment file

In the experiment file, the plugin procedure is specified as follows:

```xml
    <procedure xsi:type="apex:pluginProcedure">
            <parameters>
                <presentations>1</presentations>
                <order>sequential</order>
                <script>adjustmentprocedure.js</script>
                <parameter name="screen">screen</parameter>
                <parameter name="startvalue">-5</parameter>
                <parameter name="parameter">gain_ac</parameter>
                <parameter name="maxvalue">10</parameter>
            </parameters>
            <trials>
                <trial id="trial">
                    <screen id="screen"/>
                    <stimulus id="stimulus"/>
                </trial>
            </trials>
        </procedure>
```

The key here is the use of `xsi:type="apex:pluginProcedure`, and to refer to a javascript file in the
element. In this case, we refer to *testprocedure.js*. APEX will try to find the
javascript in the following folders (order as indicated):

1.  the folder where the experiment file resides

2.  the pluginprocedures folder in the main APEX folder

In this case, APEX will search for *adjustmentprocedure.js* in the
experiment folder (*examples/procedure*), will not find it there, then
look in the *apex/pluginprocedures* and find it there.

Next to the default procedure parameters (`<presentations/>`, `<order/>`, etc.), extra parameters
can be passed on to the pluginprocedure as indicated in the example
above. They will be made available to the Javascript code in a variable
`params`. In Javascript syntax, the parameters above would be specified as:

```javascript
    params = { "presentations": 1,
               "order": "sequential",
               "screen": "screen",
               "startvalue": -5,
               "parameter": "gain_ac",
               "maxvalue": 10 };
```

### The javascript file

In the javascript file, a class needs to be implemented, inheriting from
ScriptProcedureInterface, as follows:

```javascript
    adjustmentProcedure.prototype = new ScriptProcedureInterface();
```

Function should be implemented, and return an instance of the desired
procedure:

```javascript
    function getProcedure()
    {
        return new adjustmentProcedure();
    }
```

A constructor can be added to initialise some variables:

```javascript
    function adjustmentProcedure()
    {
        gain = parseFloat(params["startvalue"]);
        screen = params["screen"];
        stim = "stimulus";
        hasMax = !(params["maxvalue"] === null);
        this.button='';
        if (hasMax) {
            maxValue = parseFloat(params["maxvalue"]);
        }
    }
```

Here the params variable is used to get access to parameters defined in
the experiment file.

The following methods of ScriptProcedureInterface can be reimplemented:
processResult, setupNextTrial, firstScreen, resultXml, progress,
checkParameters. These methods are documented in detail in
[Script procedure interface](#scriptprocedureinterface). They will be called by APEX in
the following order:

checkParameters

:   checks whether all parameter values make sense. If so, return an
    empty string, otherwise return an error message:

```javascript
        adjustmentProcedure.prototype.checkParameters = function()
        {
            if (params['parameter'] === null)
                return "adjustmentProcedure: parameter not found";
            if (params['startvalue'] === null)
                return "adjustmentProcedure: parameter startvalue not found";
            if (params['screen'] === null)
                return "adjustmentProcedure: parameter screen not found";
            return "";
        }
```

firstScreen

:   return the first screen to be shown, before the user clicks the
    start button to start the experiment

```javascript
        adjustmentProcedure.prototype.firstScreen = function()
        {
            return screen;
        }
```

setupNextTrial

:   returns the next trial to be presented. This is where most of the
    work happens. This function should return an object of class . Class
    Trial is specified in detail in [Trial](#trial).

```javascript
        adjustmentProcedure.prototype.setupNextTrial = function ()
        {
            // some code removed for brevity, cf adjustmentprocedure.js
            var t = new Trial();
            t.addScreen(screen, true, 0);
            var parameters = {};
            parameters[params["parameter"]] = gain;
            t.addStimulus(stim, parameters, "", 0);
            t.setId("trial");
            return t;
        }
```

processResult

:   After the user has given a response, and therefore the trial has
    finished, is called with argument screenresult. This is where the
    plugin procedure decides whether the user’s response was correct,
    and stores the response if desired. It should return a class of type
    ResultHighLight specifying with element should be highlighted on the
    screen to give feedback. is documented in
    [Screenresult](#screenresult) and in [Resulthighlight](#resulthighlight).
    In this case, we store the name of the button that was clicked and
    return an empty ResultHighlight element, indicating that nothing
    should be highlighted.

```javascript
        adjustmentProcedure.prototype.processResult = function(screenresult)
        {
            this.button = screenresult["buttongroup"];
            return new ResultHighlight;
        }
```

progress

:   can be implemented to indicate the value to be shown on the
    progress bar. It can return a value between 0 and 100.

To further aid writing plugin procedures, 2 extra facilities are
available: a javascript library with convenient functions and an API to
APEX. The javascript library resides in . The APEX API is described in
[script api](#scriptapi).

### ScriptProcedureInterface <a name="scriptprocedureinterface"/>

ProcedureInterface

### ScriptAPI <a name="scriptapi"/>

```c++
/** API for script procedure
 * Is a proxy to ScriptProcedure, but only exposes relevant methods
 */
class ScriptProcedureApi {
public:
    ScriptProcedureApi(ProcedureApi*, ScriptProcedure* p);

public slots:
    // Functions to change something to Apex

    QStringList parameterNames(const QString stimulusID) const;

    QVariantMap parameterMap(const QString stimulusID) const;

    QVariant parameterValue (const QString& parameter_id);

    /**
     * Register a new parameter with the manager.
     */
    void registerParameter(const QString& name); //TODO more parameters?

    // Functions to get something from Apex
    /** Get list of trials for this procedure
     * will return a QList of ApexTrial objects
     */
    QObjectList trials() const;

    QStringList stimuli() const;

    QVariant fixedParameterValue(const QString stimulusID,
                                    const QString parameterID);

    QString answerElement(const QString trialid);

    // Misc functions
    /** Shows the user a messagebox with the specified messagebox
     */
    void messageBox(const QString message) const;

    void showMessage(const QString& message) const;

    void abort(const QString& message) const;

    /**
      * Returns full filepath for relative path
      */
    QString absoluteFilePath(const QString& path);

signals:
    void showMessageBox(const QString title, const QString text) const;
};
```

### Trial <a name="trial"/>

```c++
class Trial {
public:
        Trial(QObject* parent = 0);
        Trial(const Trial& other);
        virtual ~Trial();

        virtual Trial& operator=(const Trial& other);

        typedef QList<Trial> List;

public slots:

        /** Add a screen to the list of screens to be shown
        * Every subsequently added stimulus will be associated with this screen
        * @param id the id of the screen
        * @param acceptresponse if true, the response of this screen will be
        * reported in the next processResult call
        * @param timeout if timeout >0, the screen wil disappear after timeout s.
        * If acceptresponse is true, timeout must be <=0
        */
        virtual void addScreen(const QString& id, bool acceptResponse, double timeout);

        /**
         * Add a stimulus to be played during the last added screen
         *
         * @param id id of the stimulus to be added
         * @param parameters parameters to be set before the stimulus is played
         * @param highlightElement id of screenelement to be highlighted during
         *                         presentation of this stimulus
         * @param waitafter time in s to wait after the stimulus was played
         */
        virtual void addStimulus(const QString& id, const QMap<QString, QVariant>& parameters,
                         const QString& highlightedElement, double waitAfter);

        virtual void setId(const QString& id);
        virtual void setAnswer(const QString& answer);

        virtual bool isValid() const;


        virtual int screenCount() const;
        virtual QString screen(int screenIndex) const;
        virtual bool acceptResponse(int screenIndex) const;
        virtual double timeout(int screenIndex) const;

        virtual int stimulusCount(int screenIndex) const;
        virtual QString stimulus(int screenIndex, int stimulusIndex) const;
        virtual QMap<QString, QVariant> parameters(int screenIndex, int stimulusIndex) const;
        virtual QString highlightedElement(int screenIndex, int stimulusIndex) const;
        virtual bool doWaitAfter(int screenIndex, int stimulusIndex) const;
        virtual double waitAfter(int screenIndex, int stimulusIndex);
        virtual QString id() const;
        virtual QString answer() const;

        /**
         * Clear all data
         */
        virtual void reset();

private:

        QSharedDataPointer<TrialPrivate> d;
};
```

### Screenresult <a name="screenresult"/>

```c++
class APEXDATA_EXPORT ScreenResult
{
public:
    ScreenResult();
    virtual ~ScreenResult();

    const QString toXML() const;

    virtual void clear();

    void SetStimulusParameter(const QString& parameter, const QString& value);
    const stimulusParamsT& GetStimulusParameters() const { return stimulusparams;};

    void setLastClickPosition(const QPointF& point);
    const QPointF& lastClickPosition() const;

    private:
        stimulusParamsT stimulusparams;     //! used to keep parameters set on the screen of the last trial (eg using a spinbox)
        QPointF mLastClickPosition;

        typedef QString KeyType;
        typedef QString ValueType;
        typedef QMap<KeyType, ValueType> Parent;
        Parent map;

        friend QDebug operator<< (QDebug dbg, const ScreenResult& screenResult);
    public:
        typedef Parent::const_iterator const_iterator;

        Parent::const_iterator begin() const;
        Parent::const_iterator end() const;
        const ValueType value (const KeyType& key, const ValueType& defaultValue = QString()) const;
        ValueType& operator[](const KeyType& key);
        const ValueType operator[](const KeyType& key) const;
        bool contains(const KeyType& key) const ;
        const Parent get() const;
};
```

### ResultHighlight <a name="resultshighlight"/>

```c++
class ResultHighlight {
    Q_OBJECT
    Q_PROPERTY(bool correct READ getCorrect WRITE setCorrect)
    Q_PROPERTY(QString highlightElement READ getHighlightElement WRITE setHighlightElement)
    Q_PROPERTY(bool overrideCorrectFalse READ getOverrideCorrectFalse WRITE setOverrideCorrectFalse)
    Q_PROPERTY(bool showCorrectFalse  READ getShowCorrectFalse WRITE setShowCorrectFalse)

public:
    ScriptResultHighlight(QObject* parent=0): QObject(parent) {}

    bool getCorrect() const {return correct;}
    void setCorrect(bool p) {correct=p;}

    QString getHighlightElement() const {return highlightElement;}
    void setHighlightElement(const QString& p) {highlightElement = p;}

    bool getOverrideCorrectFalse () {return overrideCorrectFalse;}
    void setOverrideCorrectFalse(bool p) {overrideCorrectFalse=p;}

    bool getShowCorrectFalse () {return showCorrectFalse; }
    void setShowCorrectFalse(bool p) {showCorrectFalse=p;}
};
```

Plugin XML <a name="pluginxml"/>
----------

### Trials, datablocks and stimuli

When many trials/datablocks/stimuli need to be specified in the
experiment file, or you want to parametrize them, it can be useful to
generate the corresponding XML code programmatically rather than typing
it by hand. This can be achieved by the use of
plugintrials/plugindatablocks/pluginstimuli. We will illustrate this
according to the example . The experiment file looks like this:

```xml
      <procedure xsi:type="apex:constantProcedure">
        ...
        <trials>
          <plugintrials>
            <parameter name="path">../stimuli/wd*.wav</parameter>
            <parameter name="screen">screen1</parameter>
          </plugintrials>
        </trials>
      </procedure>
      ...
      <datablocks>
        <plugindatablocks>
          <parameter name="path">../stimuli/*.wav</parameter>
          <parameter name="device">wavdevice</parameter>
        </plugindatablocks>
      </datablocks>
      ...
      <stimuli>
        <pluginstimuli>
          <parameter name="path">../stimuli/*.wav</parameter>
          <parameter name="prefix">stimulus_</parameter>
        </pluginstimuli>
      </stimuli>
      <general>
        <scriptlibrary>autostimulus.js</scriptlibrary>
      </general>
    </apex:apex>
```

At the beginning of the trials/datablocks/stimuli section, the element
plugintrials/plugindatablocks/pluginstimuli can be specified, referring
to javascript code. There are three options to specify the javascript
code:

1.  Specify the code directly in the experiment file

2.  Refer to a javavascript file from the
    plugintrials/plugindatablocks/pluginstimuli element

3.  Specify a scriptlibrary in that contains all functions

The latter approach was chosen for this example.

In element plugintrials/plugindatablocks/pluginstimuli, parameters can
be specified that will be made available to the javascript code. In this
case the parameters are and , which specify the path in which to search
for wav files, and the prefix that will be prepended to the
trial/datablock/stimulus ID.

The content of autostimulus.js is:

```javascript
    function getStimuli () {
        misc.checkParams( Array("path", "prefix"));
        files = api.files( params["path"] );
        r="";
        for(i=0; i<files.length; ++i) {
            print( files[i] );
            r=r+xml.stimulus( params["prefix"]+files[i], "datablock_" + files[i]);
        }
        return r;
    }
    function getDatablocks() {
        ...
    }
    function getTrials() {
        ...
    }
```

Functions getStimuli, getDatablocks and getTrilsll be called by
APEX, and should return the XML code for stimuli, datablocks and trials.
A javascript library of convenience function is made available (), and
an API exposed by APEX, see [XML Plugin API](#xmlpluginapi) for a complete
description. In this case the function api.files is used to get a list
of wav files in a specified path. For each of these files, a
stimulus/datablock/trial is then generated. Note the use of convenience
function xml.stimulus etc. to generate the XML code.

### Pluginscreens

Similarly to trials, datablocks and stimuli, screens can also be
generated using JavaScript code. In this case the element is used. See
examples/xmlplugin/screenplugin.apx.

### XML Plugin API <a name="xmlpluginapi"/>

```c++

public slots:
    QString version();

    /**
     * Get all files in the filesystem matching wildcard
     */
    QStringList files(const QString& path);

    /**
      * Read file content into string
      */
    QString readAll(const QString& path);

    /**
      * Get the duration of a WAV file in seconds
      */
    double stimulusDuration(const QString& path);

    /**
     * Get path without filename for relative path
     */
    QString path(const QString &s);

    // Deprecated: do not use!
    QString stripPath(const QString &s);

    /**
      * Returns full filepath for relative path
      */
    QString absoluteFilePath(const QString& path);

    /**
     * Add warning message to message window
     */
    void addWarning(const QString& warning);

    /**
     * Add error message to message window
     */
    void addError(const QString& warning);
```

### Debugging

To debug generating XML from JavaScript code, the usual mechanisms are
available, such as the use of the statement in JavaScript, which will
open a graphical debugger. You can also save the experiment file in its
expanded form after opening it in APEX, using the “Experiment|Save
experiment as...” option in the menu.

Screens <a name="screens"/>
-------

To make screens more interactive, can be used.

```xml
    <screen id="screen1">
      <gridLayout w" height="2">
        <html row="1" col="1" id="htmlelement">
        <page>page.html</page>
      </html>
    </screen>
```

can contain HTML and Javascript, allowing for aded user interaction.

An API is available.

APEX will call the following functions:

When the screen is enabled:

```javascript
    reset(); enabled();
```

When the trial is finished, and the screenresult is needed:

```javascript
    getResult();    // returns string
```

When the user has finished answering, should be called.

### HtmlAPI <a name="htmlapi"/>

```c++
public:
    explicit HtmlAPI(QObject *parent = 0);

signals:
    void answered();
```
