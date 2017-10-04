Examples
========

In this chapter we describe four examples: 1) a closed-set
identification task, 2) an adaptive speech in noise identification task,
3) an adaptive just noticeable difference task (gap detection), and 4)
same as 3, but modified to the attention span and interest of young
children. Each example consists of 1) a general description, 2) the
concept, and 3) the implementation in XML.

It is advised to run the experiment before reading the details. The
experiment files are stored in the Apex3 directory under in the Apex3
folder, together with sound files and figures of the respective
experiments. Note again: if the experiment file has the extension “.apx”
it remains an XML file that can be edited with OxygenXML. The results
file will automatically have the extension “.results”. *(needs to be
done)*.

Overview of examples provided with APEX
---------------------------------------

### calibration

#### calibration/calibration.apx

Short

:   Simple calibration use

Description

:   The experiment shows how to calibrate using the calibration screen.
    You can calibrate the left and right channel seperately.

How

:   Use the calibration element with a calibration profile and an id for
    each channel (left and right)

#### calibration/calibration-automatic-bk2250.apx

Short

:   example of automatic calibration with B&K SLM 2250 plugin

Description

:   calibrate left and right ear at the start of the experiment

How

:   The &lt;soundlevelmeter&gt; block specifies how to use the sound
    level meter. If the sound level meter is not found, the regular
    calibration dialog is shown.

#### calibration/calibration-automatic-dummyslm.apx

Short

:   example of automatic calibration with dummy SLM plugin

Description

:   Allow to experiment with automatic calibration without having a
    sound level meter handy

How

:   The &lt;soundlevelmeter&gt; block specifies how to use the sound
    level meter.

#### calibration/calibration-automatic-localisation.apx

Short

:   Automatic calibration using interface to sound level meter

Description

:   13-loudspeaker sound source localisation experiment, with automatic
    calibration through the interface to the B&K SLM2250 Sound Level
    Meter

How

:   Each channel of the sound card has a gain. These gains are specified
    in the &lt;calibration&gt; element. Here the &lt;soundlevelmeter&gt;
    block specifies how to use the sound level meter. If the sound level
    meter is not found, the regular calibration dialog is shown.

#### calibration/calibration-connections.apx

Short

:   

Description

:   

How

:   

### childmode

#### childmode/childmode-car-full.apx

Short

:   Shows use of child mode: intro and outro movies, child panel

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the animal in one of the three cars. One car has a
    different sound than the two other cars.

How

:   Flash elements are introduced that read movie files with extension
    .swf

#### childmode/childmode-extramovies.apx

Short

:   Demonstration of extra movies (snail movie)

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three snails. One snail has
    a different sound than the two other snails. Childfriendly feedback
    is provided.

How

:   Flash elements of snails

#### childmode/childmode-full.apx

Short

:   Shows use of child mode: intro and outro movies, child panel,
    progressbar, shortcuts.

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. The progressbar and
    feedback are also childfriendly.

How

:   Flash elements are introduced that read movie files with
    extension.swf, child panel activated by &lt;panel&gt; in
    &lt;childmode&gt; (&lt;screens&gt;) + button shortcuts are
    introduced (button 1 = 1, button 2 = 2, button 3 = 3)

#### childmode/childmode-full-L34.apx

Short

:   Shows use of child mode: intro and outro movies, child panel,
    progressbar for L34

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. The progressbar and
    feedback are also childfriendly.

How

:   Flash elements, L34 implementation in &lt;devices&gt;

#### childmode/childmode-justmovies.apx

Short

:   Shows use of child mode: without intro and outro movies, but with
    the normal panel

Description

:   The child needs to find the stimulus in one of the three eggs. One
    egg has a different sound than the two other eggs. The same
    progressbar is used as in adult experiments, feedback
    is childfriendly.

How

:   Flash elements, same panel as adult experiments (no childmode
    selected in &lt;screens&gt;)

#### childmode/childmode-movies+intro.apx

Short

:   Shows use of flash movies, but without the actual childmode
    (no childpanel)

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. The same
    progressbar is used as in adult experiments. Feedback
    is childfriendly.

How

:   flash elements, same panel as adult experiments (no
    &lt;panel&gt;reinforcement.swf&lt;/panel&gt; in childmode)

#### childmode/childmode-nofeedback.apx

Short

:   Shows use of child mode without special feedback movies

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs.

How

:   Flash elements, no special feedback with flash elements provided

#### childmode/childmode-noprogress.apx

Short

:   Shows use of child mode: intro and outro movies, but without the
    actual childmode (no childpanel)

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. Feedback is childfriendly.

How

:   Flash elements, no panel or progressbar
    (&lt;showpanel&gt;false&lt;/showpanel&gt;
    and &lt;progressbar&gt;false&lt;/progressbar&gt;)

#### childmode/childmode-onlyscreen-normalpanel.apx

Short

:   Shows use of child mode: intro and outro movies, but with the normal
    panel

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. The same progressbar is
    used as in adult experiments. Feedback is childfriendly.

How

:   Flash elements, same panel as adult experiments (no childmode
    selected in &lt;screens&gt;)

#### childmode/childmode-poll.apx

Short

:   Shows use of child mode: instead of waiting a specific time, 0 is
    specified as length for intro, outro and feedback, which makes apex
    wait for the movies to finish.

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. The same progressbar is
    used as in adult experiments.

How

:   Flash elements, same panel as adult experiments (no childmode
    selected in &lt;screens&gt;), length of intro and outro is specified
    in &lt;childmode&gt;

#### childmode/childmode-shortcuts.apx

Short

:   Shows use of child mode with shortcut keys

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. The progressbar and
    feedback are also childfriendly.

How

:   Flash elements, button shortcuts are introduced (leftarrow = 1,
    downarrow = 2, rightarrow = 3)

#### childmode/childmode-skipintrooutro.apx

Short

:   Shows use of child mode: intro and outro movies, but without the
    child panel; also has button shortcuts to skip the intro or outro
    movie

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. Feedback is
    also childfriendly.

How

:   The F7 shortcut can be used to skip the intro and outro movies when
    &lt;allowskip&gt;true&lt;/allowskip&gt; in &lt;general&gt;. Flash
    elements, button shortcuts are introduced (leftarrow = 1, downarrow
    = 2, rightarrow = 3, ’s’ = skipping intro or outro movie),

#### childmode/childmode-skipintrooutro-disabled.apx

Short

:   Skip intro or outro movie by hitting ’s’ on the keyboard
    is disabled.

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. Feedback is childfriendly.

How

:   Flash elements, skipping is disabled by setting
    &lt;allowskip&gt;false&lt;/allowskip&gt; in &lt;general&gt;

#### childmode/childmode-snake-full.apx

Short

:   Shows use of child mode: intro and outro movies, child panel,
    progressbar

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three snakes. One snake has
    a different sound than the two other snakes. The progressbar and
    feedback are also childfriendly.

How

:   Flash elements of snakes, child panel activated by &lt;panel&gt; in
    &lt;childmode&gt; (&lt;screens&gt;)

#### childmode/childmode-waitforstart.apx

Short

:   Shows use of child mode, with no shortcut keys and waiting for start
    before every trial

Description

:   The experiment starts after a silent introductory movie. The child
    needs to find the stimulus in one of the three eggs. One egg has a
    different sound than the two other eggs. The next trial is only
    presented after selecting Start from the Experiment menu or
    pressing F5.

How

:   Flash elements + &lt;waitforstart&gt; function in &lt;general&gt;

### connections

#### connections/connections-all-mixed-monostereo.apx

Short

:   The use of ALL connections

Description

:   Monaural presentations of digit 1. Level is changed by clicking the
    buttons quieter and louder

How

:   Connections defines how the different datablocks and filters are
    routed to the output device

#### connections/connections-byname.apx

Short

:   Try to autoconnect a stereo wavfile to a mono wavdevice by name

Description

:   Digits are presented dichotically, you can give a verbal response

How

:   Connections are used to route different datablocks and filters to an
    output device

#### connections/connections-byregexp.apx

Short

:   Try to autoconnect a stereo wavfile to a mono wavdevice using
    mode=regexp

Description

:   Dichotic presentation of digits, you can give a verbal response

How

:   Connections are used to route different datablocks and filters to an
    output device

#### connections/connections-bywildcard.apx

Short

:   Try to autoconnect a stereo wavfile to a mono wavdevice using
    mode=wildcard

Description

:   Dichotic presentation of digits, you can give a verbal response

How

:   Connections are used to route different datablocks and filters to an
    output device

#### connections/connections-negativechannel.apx

Short

:   Test the use of channel -1

Description

:   If something is connected to channel -1 of the output device, it
    should be ignored, no output when button wrong is clicked

How

:   Connections are used to route filters to output devices

### controller

#### controller/controller-plugincontroller.apx

Short

:   Plugincontroller

Description

:   Gain of plugin controller varies according to user input

How

:   the democontroller is used here to demonstrate how a plugin
    controller can be implemented.

#### controller/osccontroller.apx

Short

:   

Description

:   

How

:   

#### controller/osccontroller\_avatar.apx

Short

:   

Description

:   

How

:   

### datablocks

#### datablocks/datablocks-combination.apx

Short

:   Combination of datablocks using sequential and simultaneous

Description

:   Play datablocks similtaneously and sequentially

How

:   Using the &lt;simultaneous&gt; and &lt;sequential&gt; tags in
    &lt;stimulus&gt;, datablocks can be organised and reused.

#### datablocks/datablocks-loop.apx

Short

:   Use the same datablock multiple times in a stimulus

Description

:   The datablock wavfile “één” is played twice within one stimulus

How

:   set the number of replications of the datablock by setting the
    number of the loop, here it is &lt;loop&gt;2&lt;/loop&gt;

#### datablocks/datablocks-nodevice.apx

Short

:   demonstration of omission of device in datablock

Description

:   When there is only one device in the experiment, it does not need to
    be explicitly mentioned in each datablock.

How

:   APEX will automatically assign the device to each datatblock

#### datablocks/datablocks-repeat.apx

Short

:   Use the same datablock multiple times in a stimulus

Description

:   The datablock wavfile “één” is played four times within one stimulus

How

:   sequential datablocks within stimulus1

#### datablocks/datablocks-silence.apx

Short

:   Combination of silence and signal for 1 stimulus presentation

Description

:   The stimulus in this example contains first silence (a delay) before
    you hear “één”

How

:   Use sequential datablocks that include silence and
    wavfile/signals, e.g. also possible to put silence datablocks after
    the wavfile

#### datablocks/datablocks-simultaneous.apx

Short

:   Play datablocks simultaneously

Description

:   Demonstrate the use of simultaneous datablocks in a stimulus

How

:   When datablocks are listed in the &lt;simultaneous&gt; element in
    &lt;stimulu&gt; they are played simultaneously

### filters

#### filters/filters-amplifier-defaultparameters.apx

Short

:   Show amplifier default parameters

Description

:   the stimulus wd1.wav is played in the left ear, when you click the
    button ’play with default parameters’, the stimulus is played again
    with the same gain (=0) (10 trials)

How

:   fixed\_parameters

#### filters/filters-amplifier-setgain.apx

Short

:   Adapting device parameters: gain of device varies according to user
    input

Description

:   the stimulus wd1.wav and noise are played simultaneously in the left
    ear, when you click the button ’higher’/’lower’, the stimulus and
    the noise are played again with a different gain for the noise (=
    gain + 2/- 2, see output box ’parameterlist’)

How

:   adaptiveProcedure: gain is adjusted with stepsize = 2

#### filters/filters-complexprocessing.apx

Short

:   More complex processing, can be used for testing skips - 2 noise
    stimuli (sinus & wivineruis.wav) with different gains (filters)

Description

:   When you click on ’wrong’ you hear in both ears two noise stimuli
    and in the left ear 2 times a sentence, the same happens when you
    click on the ’wrong’ button again

How

:   trainingProcedure, creating a sinus as noise-stimulus

#### filters/filters-dataloop-2simultaneous.apx

Short

:   Shows use of dataloop generator : uses the same datablock twice (for
    2 different dataloops)

Description

:   When you click on the button ’correct/wrong’, in both ears a noise
    stimulus is played and in the left
    ear ’one-two’(=stimulus1)/’silence-two’(=stimulus2) is presented -
    Dataloop: playing noise and according to the users
    input (correct/wrong) stimulus1 or stimulus2 is presented

How

:   2 dataloops: noise for trial1 and trial2

#### filters/filters-dataloop.apx

Short

:   Shows use of dataloop generator - wivineruis.wav should play !not!
    continuously over trials (see filters - noisegen - apex:dataloop)

Description

:   When you click on the button ’wrong/correct’, in both ears a noise
    stimulus is played and in the left
    ear ’silence-two(twee)’(=stimulus2)/’one-two(een-twee)’(=stimulus1)
    is presented

How

:   trainingProcedure - dataloop generator - continuous: false

#### filters/filters-dataloop-continuous.apx

Short

:   Shows use of dataloop generator - wivineruis.wav should play
    continuously over trials (see filters - noisegen - apex:dataloop)

Description

:   The noise is playing continuously over trials. When you click on the
    button ’dataloop - silence/dataloop - 1’, in both ears a noise
    stimulus is played and in the left
    ear ’silence-two(twee)’(=stimulus2)/’one-two(een-twee)’(=stimulus1)
    is presented

How

:   trainingProcedure - dataloop generator - continuous: true

#### filters/filters-dataloop-jump.apx

Short

:   Shows use of jump parameter of dataloop generator

Description

:   

How

:   

#### filters/filters-dataloop-randomjump.apx

Short

:   

Description

:   

How

:   

#### filters/filters-noisegen-setgain.apx

Short

:   Basegain of noise: 0 - Aapting device parameters - gain of device
    varies according to user input

Description

:   Noise and a stimulus (one-een) are presented simultaneously in the
    left ear, when you click on ’higher/lower’, you hear again the noise
    and stimulus in the left ear but the gain of the noise is adjusted
    with +2/-2 (=stepsize, see output box)

How

:   adaptiveProcedure - stepsize of gain - adapt\_parameter (Procedure)
    - continuous:true

#### filters/filters-plugin.apx

Short

:   

Description

:   example already somewhere else as well!

How

:   

#### filters/filters-plugin-matlabfilter.apx

Short

:   Example using the matlabfilter plugin.

Description

:   The plugin will use matlab to process data, one buffer at a time.
    Note that you should make sure your system can find the Matlab
    eng library.

How

:   &lt;plugin&gt;matlabfilter&lt;/plugin&gt; in &lt;filters&gt;.

#### filters/filters-plugin-scamblespectrumfilter.apx

Short

:   Demonstrate scramblespectrum filter

Description

:   The scramblespectrum filter will randomize the spectrum to reduce
    monaural spectral cues in a localisation experiment

How

:   parameters to the scramblespectrumfilter are specified in
    &lt;filter&gt;

#### filters/filters-plugin-scamblespectrumfilter\_calibration.apx

Short

:   

Description

:   

How

:   

#### filters/filters-pulsegen.apx

Short

:   Shows synchronisation for L34 using a soundcard pulse

Description

:   When you click on send, a pulse is presented in your left ear

How

:   filters: generator makes a soundcard pulse =&gt; 2 channels of
    stereo file are mixed into output ch 0, the pulse goes to ch 1
    (left ear)

#### filters/filters-sinegenerator.apx

Short

:   Test of setting frequency parameter of sine generator - frequency of
    sinus varies according to user input

Description

:   A sinus is playing in the left ear when you click on one of the
    3 buttons. If you click on 100Hz/1000Hz/defaults, the frequency of
    the sinus changes to 100Hz/1000Hz/10 000Hz.

How

:   filter: 1 generator: sinus (10 000Hz) - 3 stimuli: 3x silence with
    adjusted parameters (frequency: 100Hz/1000Hz/no change) -
    continuous: false

#### filters/filters-sinegen-setfrequency.apx

Short

:   Regression test for adapting device parameters - gain of device
    varies according to user input

Description

:   A sinus is played in the left ear (sinus is played continuously over
    trials), when you click the button ’higher’/’lower’, the sinus is
    played again with a different frequency for the sinus (= frequency +
    100Hz/- 100Hz, see output box ’parameterlist’)

How

:   adaptiveProcedure: frequency (adapt\_parameter) is adjusted with
    stepsize = 100Hz - datablock/stimulus: silence

#### filters/filters-sinegen-setgain.apx

Short

:   Regression test for adapting device parameters - gain of device
    varies according to user input

Description

:   A sinus is played in the left ear. The sinus is played continuously
    over trial but when you click on ’higher/lower’ the sinus is played
    again but the gain of the sinus is adapted with +5/-5 dB.

How

:   adaptiveProcedure - datablock/stimulus: silence - filter: generator:
    sinus - continuous: true - adapt\_parameter: gain - (!basegain has
    to be low enough otherwise there is clipping!)

#### filters/filters-singlepulse-defaultparameters.apx

Short

:   Shows synchronisation using a soundcard pulse

Description

:   When you click on ’play with default parameters’, a pulse is
    presented in your left ear simultaneously with a stimulus (wd1.wav -
    een/one)

How

:   datablock/stimulus + filter: generator makes a soundcard pulse
    (negative polarity)

#### filters/filters-singlepulse-polarity.apx

Short

:   Shows synchronisation for L34 using a soundcard pulse with a
    negative polarity (filter - pulsegen)

Description

:   When you click on send, a pulse is presented in your left ear and a
    stimulus (datablock: \_Input44\_2.wav) is presented in your right
    ear

How

:   filters: generator makes a soundcard pulse with a negative polarity

#### filters/filters-vocoder.apx

Short

:   Demonstration of vocoder plugin.

Description

:   Play a sentence either vocoded or unvocoded

How

:   A vocoder-plugin filter has been added in the connections.

#### filters/filters-wavamplifier.apx

Short

:   Regression test for training procedure - Output: number according to
    last input

Description

:   When you click on the button ’play’: in the left ear/channel: “een”,
    is presented and in the right ear/channel: “twee”, 20dB louder
    compared to the left ear, is presented.

How

:   filter: amplifier =&gt; basegain (-10) + gain left
    channel(-10)/right channel(+10)

#### filters/filters-wavfadeincos.apx

Short

:   Regression test for fader (begin/end of stimulus is not abrupt)

Description

:   When you click on the button ’play fade/play NO fade’, in both ears
    a sinus is presented with at the beginning of the stimulus a
    fade-in/NO fade-in (click)

How

:   filter: fader in begin (cosine) =&gt; length: 400ms, direction: in
    (fade-in: begin of stimulus)

#### filters/filters-wavfadeinlin.apx

Short

:   Regression test for fader (begin/end of stimulus is not abrupt)

Description

:   When you click on the button play fade/play NO fade’, in both ears a
    sinus is presented with at the beginning of the stimulus a
    fade-in/NO fade-in (click)

How

:   filter: fader(linear) =&gt; length: 400ms, direction: in (fade-in:
    begin of stimulus)

#### filters/filters-wavfadeoutcos.apx

Short

:   Regression test for fader (begin/end of stimulus is not abrupt)

Description

:   When you click on the button ’play fade/play NO fade’, in both ears
    a sinus is presented with at the end of the stimulus a fade-out/NO
    fade-in (click)

How

:   filter: fader(cosine) =&gt; length: 400ms, direction: out (fade-out:
    end of stimulus)

#### filters/filters-wavfadeoutlin.apx

Short

:   Regression test for fader (begin/end of stimulus is not abrupt)

Description

:   When you click on the button ’play fade/play NO fade’, in both ears
    a sinus is presented with at the end of the stimulus a fade-out/NO
    fade-in (click)

How

:   filter: fader(linear) =&gt; length: 400ms, direction: out (fade-out:
    end of stimulus)

### general

#### general/general-autosave.apx

Short

:   Automatically save results to apr-file after experiment

Description

:   Closed-set identification task with automatic saving of results

How

:   General - autosave

#### general/general-exitafter.apx

Short

:   Exit experiment when finished

Description

:   Closed-set identification task, exit after finish

How

:   General - exitafter=true

#### general/general-prefix-absolutepath.apx

Short

:   Get the datablock prefix from the absolute path

Description

:   Closed-set identification task

How

:   datablocks uri\_prefix full path

#### general/general-prefix-frommainconfig.apx

Short

:   Get the datablock prefix from the main configfile

Description

:   Closed-set identification task

How

:   source=apexconfig

#### general/general-prefix-invalid.apx

Short

:   Get the datablock prefix from the main configfile

Description

:   Invalid prefix id specified: wavfile not found wd1.wav

How

:   source=apexconfig

#### general/general-scriptparameters.apx

Short

:   Demonstrate the use of general/scriptparameters

Description

:   Digits 1-7 are presented monaurally, you can click on a button to go
    to the next one

How

:   Scriptparameters - name=path, scriptparameters.js needed

#### general/general-showresults.apx

Short

:   Show results after experiment

Description

:   When the experiment is finished and the results are saved, you can
    choose to see the results

How

:   Results - showafterexperiment=true

#### general/general-xmlerror.apx

Short

:   Error in XML file

Description

:   

How

:   

### interactive

#### interactive/invalid-entry.apx

Short

:   Change small parameter right before the start of the experiment

Description

:   GUI is shown with the element to be changed

How

:   Interactive

#### interactive/setgain.apx

Short

:   GUI to change some settings right before the experiment

Description

:   GUI appears to set/change some settings

How

:   Interactive

#### interactive/subject.apx

Short

:   GUI to change some settings right before the experiment

Description

:   GUI appears to set/change some settings

How

:   The name of the subject, in &lt;results&gt;/&lt;subject&gt; is set
    from the interactive dialog that appears before the experiment
    is loaded.

### l34

#### l34/addinvalidfilter.apx

Short

:   Try to add wav filter to L34Device

Description

:   Since it is not possible to add a wav filter to an L34Device, an
    error occurs

How

:   see filters and device

#### l34/bilateral.apx

Short

:   Test of synchronized bilateral CI setup

Description

:   Test whether the stimuli are given from the first, the second or
    both L34 devices

How

:   Including two L34 devices, stimuli can be presented stimultaneously

#### l34/bilateral-singlepulse.apx

Short

:   Test of synchronized bilateral CI setup with pulse stimuli

Description

:   Test whether the stimuli are given from the first, the second or
    both L34 devices

How

:   Including two L34 devices, stimuli can be presented stimultaneously

#### l34/invaliddatablock.apx

Short

:   Try to parse invalid datablock, should return error

Description

:   datablock does not exist in folder stimuli, running the experiment
    gives an error.

How

:   uri\_prefix is used to find stimuli, invalid.aseq does not exist

#### l34/L34DummySync.apx

Short

:   Shows synchronisation for L34 using a soundcard pulse

Description

:   2 channels of stereo file are mixed into output ch 0, the pulse goes
    to ch 1

How

:   use dummyDeviceType and wavDeviceType and check connections

#### l34/L34Sync.apx

Short

:   Shows synchronisation for L34 using a soundcard pulse

Description

:   2 channels of stereo file are mixed into output ch 0, the pulse goes
    to ch 1

How

:   use dummyDeviceType and wavDeviceType and check connections

#### l34/loudness\_balancing.apx

Short

:   Loudness balancing between two stimulation CI electrodes

Description

:   Two signals are presented consecutively in the same CI, at two
    different stimulation electrodes. One electrode is the reference
    electrode and has a fixed current level, the comparison electrode
    has changing levels. The participant has to judge which signal
    is louder. An adaptive procedure is used to determine the
    balanced level.

How

:   Use of adaptiveProcedure, plugindatablocks, balancing.js needed

#### l34/mapping1.apx

Short

:   Use default map with current units between 1 and 255

Description

:   

How

:   See defaultmap

#### l34/mapping2.apx

Short

:   Use real map to check mapping

Description

:   

How

:   See defaultmap values

#### l34/r126wizard.apx

Short

:   R126 is the clinical fitting software

Description

:   

How

:   Use &lt;defaultmap&gt; &lt;from\_r126/&gt;

#### l34/rfgenxs-bilateral.apx

Short

:   Test of synchronized bilateral CI setup with an RFGenXS

Description

:   Test whether the stimuli are given on the first, the second or both
    channels

How

:   With an RFGenXS, stimuli can be presented stimultaneously

#### l34/setvolume.apx

Short

:   Change volume of CI stimuli (send simple XML file to L34 device)

Description

:   Change the volume of the CI stimuli to 10,50,70 or 100 current units

How

:   Add variable parameter id=l34volume, see device: volume 100 is
    default

#### l34/simpleaseq.apx

Short

:   

Description

:   

How

:   

#### l34/simpleaseq-dummy.apx

Short

:   

Description

:   

How

:   

#### l34/simpleaseq-examples.apx

Short

:   power-up test?

Description

:   

How

:   

#### l34/simpleaseq-sp12.apx

Short

:   

Description

:   

How

:   

#### l34/triggertest.apx

Short

:   

Description

:   

How

:   

#### l34/wordsaseq.apx

Short

:   

Description

:   

How

:   

### manual

#### manual/closedsetword.apx

Short

:   Closed-set identification of words in noise with figures

Description

:   A word is presented in noise and the subject responds by clicking on
    one of the 4 figures on the screen, repeated 3 times. Initial SNR is
    set via interactive GUI.

How

:   full experiment

#### manual/gapdetection.apx

Short

:   Gap detection

Description

:   

How

:   

#### manual/gapdetectionchild.apx

Short

:   gap detection for children

Description

:   

How

:   

#### manual/opensetidentification.apx

Short

:   Open set identification task

Description

:   A word is presented in noise and the subject responds by typing the
    word, repeated 3 times. Initial SNR is set via interactive GUI.

How

:   Full experiment

#### manual/sentenceinnoise.apx

Short

:   Sentences in noise task

Description

:   A sentence is presented in noise, the subject responds verbally, the
    test leader can indicatie whether the response was correct or
    incorrect, repeated 4 times. Initial SNR is set via interactive GUI.

How

:   Full experiment

#### manual/trainingprocedure.apx

Short

:   Training for constant procedure

Description

:   A stimulus is presented after the user had indicated the trial by
    pressing a button on the screen

How

:   apex:trainingProcedure

### parameters

#### parameters/parameters-connection-filter.apx

Short

:   Change the channel of a connection + stimulus has gain

Description

:   Click on the right (or left button) to hear the sound in your right
    ear (or left ear).

How

:   Training procedure, Fixed (stimulus) and variable
    parameters (channel) + possibility in code to change the gain of the
    stimuli with filter (amplifier)

#### parameters/parameters-connection-soundcard.apx

Short

:   Change the channel of a connection using a parameter

Description

:   Click on the right (or left button) to hear the sound in your right
    ear (or left ear)

How

:   training procedure, Fixed (stimulus) and variable
    parameters (channel)

#### parameters/parameters-device-allchannels.apx

Short

:   Change parameter (gain) of device channel

Description

:   gain of device varies in BOTH channels according to user input (1
    button = louder, 1 button = quieter)

How

:   adaptive procedure, fixed (stimulus) and variable (gain) parameters,
    stepsize of 2 dB

#### parameters/parameters-device-singlechannel.apx

Short

:   Change parameter (gain) of device channel

Description

:   Gain of device varies in LEFT channel according to user input (1
    button = louder, 1 button = quieter)

How

:   Adaptive procedure, fixed (stimulus) and variable (gain) parameters,
    stepsize of 2 dB

#### parameters/parameters-filter.apx

Short

:   Change parameter (gain) of device channel

Description

:   gain of device varies in BOTH channel according to user input (1
    button = louder, 1 button = quieter) + gain is visible on the screen

How

:   adaptive procedure, fixed (stimulus) and variable (gain) parameters,
    stepsize of 2 dB, Parameterlists are introduced that show Left or
    Right gain on the screen

#### parameters/parameters-filter-channel.apx

Short

:   Change parameter (gain) of device channel

Description

:   gain of device varies in RIGHT channel according to user input (1
    button = louder, 1 button = quieter) + gain is visible on screen

How

:   adaptive procedure, fixed (stimulus) and variable (gain) parameters,
    stepsize of 2 dB, Parameterlists are introduced that show Left or
    Right gain on the screen

#### parameters/parameters-filter-channel-probe.apx

Short

:   Change parameter (gain) of device channel

Description

:   gain of device varies in LEFT channel according to user input (1
    button = louder, 1 button = quieter) + gain is visible on screen

How

:   adaptive procedure, fixed (stimulus) and variable (gain) parameters,
    stepsize of 10 dB, Parameterlists are introduced that show Left or
    Right gain on the screen. Additionally there is a probe filter that
    saves the output to disk.

#### parameters/parameters-restore.apx

Short

:   Demonstrate restoring parameter values

Description

:   When a parameter is set from a stimulus, and subsequently a stimulus
    is played in which the parameter is not set, it should be restored
    to its default value

How

:   Parameter gain is set in stimulus1, and is not set in stimulus2.
    Therefore a gain of 0dB should be applied for stimulus2

#### parameters/parameters-spinbox.apx

Short

:   Adjust noise manually while presenting speech and noise

Description

:   gain of noise varies in right channel according to user input (1
    button = arrow up, 1 button = arrow down) while the stimulus stays
    the same.

How

:   constant procedure, Fixed (stimulus) and variable (noise gain),
    stepsize of 1 dB, noise is generated by filter

#### parameters/parameters-wavfadeinout.apx

Short

:   Set parameters of fader

Description

:   The fader filter allows to apply an up and down ramp to a channel

How

:   There are two faders in &lt;filters, faderin and faderout. Their
    parameters are set from the stimuli.

### procedure

#### procedure/adaptive-1up-1down.apx

Short

:   1up-1down Adaptive procedure: Frequency of ups-downs is the same -
    Experiment stops after 6 reversals

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in both ears. When you click again, the gain of
    the stimulus decreases/increases according to the
    button (correct/false)

How

:   adaptiveProcedure - stop\_after\_type: reversals - nUp/nDown -
    adapt\_parameter (gain: gain of stimulus) -

#### procedure/adaptive-1up-2down.apx

Short

:   1up-2down Adaptive procedure: Frequency of ups-downs is NOT the
    same - Experiment stops after 6 reversals

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in both ears. When you click again, the gain of
    the stimulus StaysTheSame/Decreases/Increases according to the
    button (correct/false) and the number of ups and downs

How

:   adaptiveProcedure - stop\_after\_type: reversals - nUp/nDown -
    adapt\_parameter (gain: gain of stimulus)

#### procedure/adaptive-2up-1down.apx

Short

:   2up-1down Adaptive procedure: Frequency of ups-downs is NOT the
    same - Experiment stops after 6 reversals

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in both ears. When you click again, the gain of
    the stimulus StaysTheSame/Decreases/Increases according to the
    button (correct/false) and the number of ups and downs

How

:   adaptiveProcedure - stop\_after\_type: reversals - nUp/nDown -
    adapt\_parameter (gain: gain of stimulus)

#### procedure/adaptive\_stopafter\_presentations.apx

Short

:   Stop after a specified number of presentations (3) - should in this
    case stop after 6 trials (presentation: every trial is presented
    once =&gt; 2 trials =&gt; 2x3 = 6)

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in both ears. When you click again, the gain of
    the stimulus StaysTheSame/Decreases/Increases according to the
    button (correct/false) and the number of ups and downs

How

:   adaptiveProcedure - stop\_after\_type: presentations -
    adapt\_parameter (gain: gain of stimulus)

#### procedure/adaptive\_stopafter\_presentations\_invalid.apx

Short

:   Error message because \#presentations is not equal to \#stop\_after

Description

:   Point of interest if you want to stop the experiment after some
    presentations! =&gt; \#presentations = \#stop\_after

How

:   procedure: presentations - stop\_after

#### procedure/adaptive\_stopafter\_reversals.apx

Short

:   Stop after a specified number of reversals (6) - changes between
    correct-false

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in both ears. When you click again, the gain of
    the stimulus Decreases/Increases according to the
    button (correct/false) and the stepsizes changes after 3 reversals

How

:   adaptiveProcedure - stop\_after\_type: reversals - adapt\_parameter
    (gain: gain of stimulus) - stepsize: change\_after: reversals

#### procedure/adaptive\_stopafter\_trials.apx

Short

:   Stop after a specified number of trials (10)

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in the left ear. When you click again, the gain
    of the stimulus Decreases/Increases according to the
    button (correct/false)

How

:   adaptiveProcedure - stop\_after\_type: trials - adapt\_parameter
    (gain: gain of stimulus)

#### procedure/adjustment-pluginprocedure.apx

Short

:   Adjustment of stimuli with a pluginprocedure

Description

:   There are 6 buttons:

How

:   adaptiveProcedure - stop\_after\_type: trials - adapt\_parameter
    (gain: gain of stimulus)

#### procedure/adp1.apx

Short

:   Regression test for ADP - Experiment stops after 4 reversals

Description

:   adaptive procedure, one of the following sequences are played: noise
    een noise, een noise noise, noise noise een =&gt; answer: one of the
    three sequences. Stepsize changes after 2 trials =&gt; stepsize
    determines the parameter ’snr’

How

:   adaptiveProcedure - adapt\_parameter (snr: snr (order, not in dB) -
    intervals - stepsize determines the change in snr (1-2-3) not the
    value in dB

#### procedure/adp2.apx

Short

:   Regression test for ADP - Experiment stops after 10 reversals

Description

:   adaptive procedure, one of the following sequences are played: noise
    een noise, een noise noise, noise noise een =&gt; answer: one of the
    three sequences. Stepsize changes after 2 trials =&gt; stepsize
    determines the parameter ’snr’

How

:   adaptiveProcedure - adapt\_parameter (snr: snr (order, not in dB) -
    intervals - stepsize determines the change in snr (1-2-3) not the
    value in dB

#### procedure/afc-choices.apx

Short

:   Regression test - 4 Intervals - 4 Choices, select interval 2-3

Description

:   Four intervals are possible= ’noise noise noise een’, ’noise noise
    een noise’, ’noise een noise noise’ and ’een noise noise noise’.
    However only possibility number “2” and “3” are selected

How

:   constantProcedure - intervals- count/possibilities: 4, select(ion):
    2 and 3

#### procedure/afc-uniquestandard.apx

Short

:   Regression test for Intervals, Standard(reference signal),
    Uniquestandard(true/false = If uniquestandard is true and multiple
    standards are defined per trial, Apex will try to present another
    standard in each interval of the trial)

Description

:   In each trial, the 3 standards should be used (uniquestandard
    = true)

How

:   constantProcedure - intervals- uniquestandard: true

#### procedure/aid1.apx

Short

:   Regression test - Different stimuli can occur during one trial
    (according to the difficulty of the experiment (’snr’) (- no
    connections or gain!)

Description

:   adaptive procedure, one of the following stimuli are played: ’een’,
    ’twee’, ’drie’, ’vier’ or ’vijf’ =&gt; If you click correct/false
    the experiment becomes more difficult/easy corresponding with the
    ’snr’ parameter - the larger the values of snr, the easier the
    experiment

How

:   adaptiveProcedure - adapt\_parameter (snr: snr (order, not in dB) -
    different stimuli (trial) - stepsize determines the change in
    snr (1-2-3-4-5) - changes afther 5 trials

#### procedure/aid-selectrandom.apx

Short

:   Regression test - Different stimuli can occur during one trial
    (according to the difficulty of the experiment (’snr’)

Description

:   adaptive procedure, one of the following stimuli are played: ’een’,
    ’twee’, ’drie’, ’vier’ or ’vijf’, each with a different gain =&gt;
    If you click correct/false the experiment becomes more
    difficult/easy corresponding with the ’snr’ parameter

How

:   adaptiveProcedure - adapt\_parameter (snr: snr (order, not in dB) -
    different stimuli (trial) - stepsize determines the change in snr
    (1-2-3-4-5), gain(filters)

#### procedure/choices-randomstimulus-randomstandard.apx

Short

:   

Description

:   

How

:   

#### procedure/continuous-waitbeforefirst.apx

Short

:   Shows how to start continuous filters etc before the first trial is
    presented

Description

:   The noise is playing continuously over trials (dataloop) - the noise
    starts 5s before the first trial (presenting: wd1.wav: ’een’) begins

How

:   procedure: time\_before\_first\_trial: in seconds - filters:
    dataloop - continuous: true

#### procedure/cst-multistimulus.apx

Short

:   multiple stimuli per trial - one should be chosen randomly - 2
    presentations (2trials =&gt; 2x2 = 4)

Description

:   The experiment has 2 trials (trial1: button 1-2-3 - trial2:
    button a-b-c) =&gt; In trial1/trial2=&gt; correct answer is
    always: button1,buttonb.

How

:   constantProcedure - 2 trials with !each! 3 different stimuli but
    with the !same! buttons

#### procedure/extrasimple.apx

Short

:   Click on the corresponding button - Stop after 2 presentations
    (2trials =&gt; 2x2 = 4)

Description

:   The experiment has 2 trials with different stimuli (trial1: house -
    trial2: mouse) =&gt; In trial1/trial2 =&gt; correct answer is:
    house/mouse (stimulus is heard in the left ear)

How

:   constantProcedure - 2 trials with different stimuli corresponding
    with the buttons

#### procedure/fixedparameternotfound.apx

Short

:   Errormessage - fixed parameter with id=“test” not defined

Description

:   example of an error message about an undefined fixed parameter of a
    stimulus

How

:   stimuli - fixed parameters

#### procedure/heartrainprocedure.apx

Short

:   Regression test for heartrainprocedure.js

Description

:   example of an error message about an undefined fixed parameter of a
    stimulus

How

:   stimuli - fixed parameters

#### procedure/heartrainprocedure\_short.apx

Short

:   

Description

:   

How

:   

#### procedure/idn1.apx

Short

:   Matching of stimuli and buttons - Different trials (+ answers) - 1
    presentations (6 trials)

Description

:   auditive stimulus 1 2 3 4 5 6 - input: buttons 1 2 3 4 5 6 =&gt;
    match the stimulus with the button

How

:   6 trials with each trial a different correct answer - 6 stimuli and
    6 buttons - order: sequential

#### procedure/idn1-mono.apx

Short

:   Matching of stimulus and button - 5 presentations (1 trial)

Description

:   auditive stimulus in right ear ’1’ - input: buttons 1 2 3 4 5 6
    =&gt; match the stimulus with the button (stimulus1 =&gt; button1)

How

:   1 trial - 6 stimuli and 6 buttons - 1 correct answer

#### procedure/idn1-skip.apx

Short

:   skip = 2: Number of trials that will be presented before the actual
    presentations start.

Description

:   skip=2 and presentations=2 =&gt; first 2 trials and then 2\*6trials
    = 12 trials. If the order is sequential, the skipped trials will be
    the first skip trials from the trial list, repeated if necessary.

How

:   2 presentations - 6 trials - skip = 2 (6 stimuli - 6 buttons)

#### procedure/idn1-waitforstart.apx

Short

:   Demonstrate use of wait for start

Description

:   The next trial is only presented after selecting Start from the
    Experiment menu or pressing F5.

How

:   &lt;waitforstart&gt; function in &lt;general&gt;

#### procedure/idn2.apx

Short

:   Matching of stimuli and buttons - Different trials (+ answers) - 1
    presentations (6 trials)

Description

:   auditive stimulus 1 2 3 4 5 6 - input: buttons 1 2 3 4 5 6 =&gt;
    match the stimulus with the button

How

:   6 trials with each trial a different correct answer - 6 stimuli and
    6 buttons - order: random

#### procedure/idn2-skip.apx

Short

:   skip = 3: Number of trials that will be presented before the actual
    presentations start.

Description

:   skip=3 and presentations=2 =&gt; first 3 trials and then 2\*6trials
    = 12 trials. If the order is random, the skipped trials will be
    picked from the trial list without replacement, repeating this
    procedure if necessary.

How

:   2 presentations - 6 trials - skip = 3 (6 stimuli - 6 buttons)

#### procedure/input-during-stimulus.apx

Short

:   Input during stimulus is allowed

Description

:   skip=3 and presentations=2 =&gt; first 3 trials and then 2\*6trials
    = 12 trials. If the order is random, the skipped trials will be
    picked from the trial list without replacement, repeating this
    procedure if necessary.

How

:   Input during stimulus: true - trials with buttons (input) -
    stimuli (output)

#### procedure/kaernbach.apx

Short

:   1up-1down Kaernbach procedure: Frequency of ups-downs is the same -
    Up stepsize is 1, down stepsize is 7 - Experiment stops after 6
    reversals

Description

:   When you click on the button ’correct or wrong’, you hear a stimulus
    ’wd1.wav (een/one)’ in both ears. When you click again, the gain of
    the stimulus decreases/increases according to the
    button (correct/false)

How

:   adaptiveProcedure - stop\_after\_type: reversals - nUp/nDown -
    adapt\_parameter (gain: gain of stimulus) -

#### procedure/multiparameters-fixed.apx

Short

:   Adaptation of multiple parameters (order(fixed) & gain(adapt))

Description

:   Two buttons: louder - quieter =&gt; louder: increasing of order and
    gain with stepsize=2, quieter: decreasing of order and gain with
    stepsize=2 (see output box)

How

:   Adapt\_parameter =&gt; order & gain - stepsize for gain(filter)
    & order(stimuli) = 2

#### procedure/multiparameters-invalid.apx

Short

:   Error message - parameter you want to be adapted is a fixed
    parameter (2nd adapt\_parameter: order: is a fixed parameter)

Description

:   error message: only first adaptive parameter can be a fixed
    parameter

How

:   Adapt\_parameter =&gt; gain(adapt) & order(fixed)

#### procedure/multiparameters-variable.apx

Short

:   Adaptation of multiple parameters: GainL & GainR

Description

:   2 buttons: louder - quieter - when you click on louder/quieter the
    gain in right and left channel is increased/decreased with 2
    dB (stepsize) - see output box

How

:   Adapt\_parameter =&gt; gainL(adapt) & gainR(adapt)

#### procedure/multiprocedure-aid.apx

Short

:   Multiprocedure - 2 adaptiveprocedures with each their own
    parameters, procedure and trials

Description

:   2 procedures =&gt; procedure1/procedure2: 1trial -
    button\_correct/button\_wrong - When you click correct, the
    experiment in the next trial of the same procedure becomes more
    difficult (you hear a higher number =&gt; larger value of snr =
    stimulus (twee, drie, vier, vijf: stepsize:1)

How

:   procedure =&gt; multiProcedure - larger\_is\_easier =&gt; snr(not in
    dB): larger value: easier (so if you click on the wrong\_button
    =&gt; the experiment becomes easier)

#### procedure/multiprocedure.apx

Short

:   Multiprocedure - 2 constantprocedures with each their own
    parameters, procedure and trials

Description

:   2 procedures =&gt; procedure1/procedure2: 3 trials -
    button1/button4 - button2/button5 - button3/button6 but different
    stimuli in each trial (Match the stimulus with the button)

How

:   procedure =&gt; multiProcedure

#### procedure/multiprocedure-choices.apx

Short

:   Multiprocedure - 2 constantprocedures with each their own
    parameters, procedure and trials - 2 different choices of intervals

Description

:   2 procedures =&gt; procedure1/procedure2: 1trial - 3intervals -
    Match the stimulus (place of stimulus within noise) with the correct
    button

How

:   procedure =&gt; multiProcedure - intervals/choices - 3 choices -
    standardstimulus:noise

#### procedure/multiprocedure-choices-corrector.apx

Short

:   Multiprocedure - 2 constantprocedures with each their own
    parameters, procedure and trials, \#presentations

Description

:   2 procedures =&gt; procedure1: 3presentations/1trial =&gt; 3x -
    3intervals: Match the stimulus (place of stimulus within noise) with
    the correct button

How

:   procedure =&gt; multiProcedure - intervals/choices -
    standardstimulus:noise

#### procedure/multiprocedure-constant-train.apx

Short

:   Multiprocedure - 2procedures (1constant - 1training) with each their
    own parameters, procedure and trials

Description

:   2 procedures =&gt; constantprocedure1: 4presentations/1trial =&gt;
    4x - 3intervals: Match the stimulus (place of stimulus within noise)
    with the correct button

How

:   procedure =&gt; multiProcedure - intervals/choices -
    standardstimulus:noise - constantProcedure - TrainingProcedure

#### procedure/multiprocedure-idn.apx

Short

:   Multiprocedure - 2constantprocedures with each their own parameters,
    procedure and trials

Description

:   2 procedures =&gt; constantprocedure1: order: onebyone -
    4presentations/1trial =&gt; 4x - 3intervals: Match the stimulus
    (place of stimulus within noise) with the correct button

How

:   procedure =&gt; multiProcedure - intervals/choices -
    standardstimulus:noise - constantProcedure

#### procedure/multiprocedure-mixed.apx

Short

:   Multiprocedure - 2constantprocedures with the SAME procedure,
    parameters and trials

Description

:   2 procedures =&gt; constantprocedure1: 1presentations/3trials =&gt;
    3x - 3buttons: 1-2-3 Match the stimulus (place of stimulus
    within noise) with the correct button

How

:   procedure =&gt; multiProcedure - constantProcedure

#### procedure/multiprocedure-onebyone.apx

Short

:   Multiprocedure-order! - 3constantprocedures with the SAME procedure,
    parameters and trials (different stimuli)

Description

:   2 procedures =&gt; constantprocedure1: 2presentations/1trial =&gt;
    2x - 2buttons: name of procedure & number: click on number (=1) if
    you have heard the stimulus (corresponding with the number)

How

:   procedure =&gt; multiProcedure: order: ONEBYONE - constantProcedure

#### procedure/multiprocedure-random.apx

Short

:   Multiprocedure-order! - 3constantprocedures with the SAME procedure,
    parameters and trials (different stimuli)

Description

:   2 procedures =&gt; constantprocedure1: 2presentations/1trial =&gt;
    2x - 2buttons: name of procedure & number: click on number (=1) if
    you have heard the stimulus (corresponding with the number)

How

:   procedure =&gt; multiProcedure: order: RANDOM - constantProcedure

#### procedure/multiprocedure-train-train.apx

Short

:   Multiprocedure-order - 2trainingProcedures with DIFFERENT procedure,
    parameters and trials (different stimuli)

Description

:   2 procedures =&gt; trainingProcedure1: 3trials =&gt; 3x???? -
    3buttons: click on a button and hear the corresponding stimulus

How

:   procedure =&gt; multiProcedure: order: ONEBYONE- trainingProcedure

#### procedure/multiscreen-idn1.apx

Short

:   Multiscreen - 1 procedure with for the first trials: screen 1 and
    the last trials: screen 2 =&gt; Same outcome as sequential
    Multiprocedure

Description

:   6 trials - 1 presentation: 3 buttons(screen1): 1-2-3 and 3
    buttons(screen2): 4-5-6 =&gt; Match the stimulus with the
    corresponding button

How

:   procedure =&gt; 1procedure but several screens: multiscreen-
    constantProcedure

#### procedure/multistandard-unique.apx

Short

:   Multiple standards - Unique standard

Description

:   2 trials: trial1 = match one of the button with the noise-stimulus
    (standards/reference-signals = numbers)

How

:   2 trials - constantProcedure

#### procedure/noanswer.apx

Short

:   Show warning if no answer is defined for a stimulus

Description

:   Error message =&gt; Cannot show feedback because: no screen was
    found for (button turns red)

How

:   procedure - trial1 - no answer (see comment)

#### procedure/nostandardfound.apx

Short

:   Show warning if no answer is defined for a stimulus

Description

:   Error message =&gt; Cannot show feedback because: no screen was
    found for (button turns red)

How

:   procedure - trial1 - no answer (see comment)

#### procedure/open-set-constant.apx

Short

:   Open set experiment - Constant stimuli

Description

:   6 trials (1 presentation) - You hear a stimulus (1-2-3-4-5-6) in
    both ears - Typ the answer/stimulus (een, twee, drie, ...) in the
    textbox below ’answer’.

How

:   trial - answer&gt;ANSWER THAT THE SUBJECT HAS TO TYPE&lt; -
    corrector xsi:type=“apex:isequal”

#### procedure/pause\_between\_stimuli.apx

Short

:   a pause of 500ms should be introduced between the stimulus and the
    standards

Description

:   1 trials (10 presentations =&gt; 10x) - You hear a
    stimulus (1-2-3-4-5-6) in both ears - Typ the answer/stimulus (een,
    twee, drie, ...) in the textbox below ’answer’.

How

:   pause between stimuli (in seconds)

#### procedure/pluginprocedure.apx

Short

:   Pluginprocedure =&gt; script: testprocedure

Description

:   When you click on a button with a certain number, the next time you
    have to click on the number you just have heard - parameter:
    stepsize & startvalue

How

:   pluginProcedure with a certain script

#### procedure/pluginprocedure-matrixtest.apx

Short

:   

Description

:   

How

:   

#### procedure/randomchannel.apx

Short

:   Randomgenerator test

Description

:   the stimulus “een” & “twee” are played simultaneously but randomly
    in the left or right channel according the value returnd by random1

How

:   trainingProcedure - randomgenerator - connections -
    stimuli:datablocks: simultaneous

#### procedure/repeatuntillcorrect-endaftertrials.apx

Short

:   The first selected trial (of the first presentation) shoud be
    repeatedly presented (stop after 5 presentations, several trials)

Description

:   5 presentations x 5 trials = 25x - When you click correct =&gt; the
    next trial begins / When you click wrong, the first trial of the
    first presentation is repeated

How

:   repeat\_first\_until\_correct: true

#### procedure/repeatuntillcorrect-multitrial.apx

Short

:   The first selected trial (of the first presentation) shoud be
    repeatedly presented (stop after 5 reversals, multiple trials)

Description

:   5 trials, stop after 5 reversals - When you click correct =&gt; the
    next trial begins / When you click wrong, the first trial of the
    first presentation is repeated

How

:   adaptiveprocedure - REPEAT-FIRST-UNTIL-CORRECT: TRUE -
    adapt\_parameter

#### procedure/repeatuntillcorrect-singletrial.apx

Short

:   The first selected trial shoud be repeatedly presented: DOESN’T
    WORK! =&gt; single trial + adapt\_parameter

Description

:   1 trial, stop after 5 reversals - When you click correct =&gt; the
    next trial begins / When you click wrong, the first trial begins

How

:   adaptiveprocedure - repeat\_first\_until\_correct: true -
    ADAPT\_PARAMETER

#### procedure/selectrandom.apx

Short

:   Test of random stimulus output when more than one stimulus defined
    in the experiment

Description

:   Random selection of stimuli in a trial if there are more than one
    (=&gt; sequential presentation of stimuli =&gt; more trials) - When
    you click on 1, ’een’, ’twee’, ’drie’ is presented randomly in both
    ears

How

:   constantprocedure - more than 1 stimulus in 1 trial =&gt; random
    selection of the stimulus

#### procedure/simple.apx

Short

:   Randomgenerator test

Description

:   the stimulus “house”/“mouse” are played in the left channel =&gt;
    click the right button (house/mouse)

How

:   constantProcedure - stimuli - datablocks

#### procedure/soundquality.apx

Short

:   Interval - select element ok, number 2

Description

:   

How

:   

#### procedure/time-between-trials.apx

Short

:   Demonstrates the use of feedback to enforce a pauze between trials

Description

:   If you click on a button, you hear a stimulus (1-2-3-4-5-6) in both
    ears =&gt; click on the button corresponding with the stimulus

How

:   feedback length (in ms)

#### procedure/train1.apx

Short

:   Trainingprocedure: demonstrate the presentation of a stimulus with a
    number according to last input

Description

:   when you click on 1/2/3/4/5/6 =&gt; you hear
    een/twee/drie/vier/vijf/zes

How

:   trainingProcedure with 6 trials and buttons

#### procedure/train2.apx

Short

:   Show the presentation of a stimulus with a random number out of the
    possible choices according to last input

Description

:   3 buttons: 1-2 3-4 5-6 =&gt; when you click on 5-6, you hear either
    5 or 6

How

:   trainingProcedure - multiple trials =&gt; button5 is the answer of
    the trial corresponding with stimulus5 & stimulus6

#### procedure/trial-nostimulus.apx

Short

:   Make a trial whithout a stimulus

Description

:   when you click on stimulus =&gt; apex shuts down

How

:   no stimuli - datablock

#### procedure/uniform-double.apx

Short

:   Random generator test - gain of stimulus is randomly chosen

Description

:   You hear ’een’ in your left ear - the gain of this stimulus changes
    randomly, each time you click the button ’1’

How

:   random generator - type: DOUBLE (fractional number, e.g. 0.1) -
    parameter =&gt; see filter

#### procedure/uniform-int.apx

Short

:   Random generator test - gain of stimulus is randomly chosen

Description

:   You hear ’een’ in your left ear - the gain of this stimulus changes
    randomly, each time you click the button ’1’

How

:   random generator - type: WHOLE(fractional number, e.g. -5) -
    parameter =&gt; see filter

### randomgenerator

#### randomgenerator/multi-interval.apx

Short

:   Randomgenerator: random gain of each datablock of a multi-interval
    stimulus

Description

:   The stimulus “één één één” is heard each time by pressing on the “1”
    button, and the gain of each “één” has a random gain

How

:   The id of the gain is set to be the parameter of the
    randomgenerator, three intervals are defined in procedure

#### randomgenerator/randomchannel.apx

Short

:   Randomgenerator: random channel for stimulus

Description

:   The stimulus “één” is played on randomly the left or right channel

How

:   The value returnd by random1 decides whether it is channel 0 or
    channel 1, see &lt;randomgenerators&gt;

#### randomgenerator/uniform-double.apx

Short

:   Randomgenerator: random gain of stimulus presented in 1 channel,
    possibility to use non-integer values (double type)

Description

:   The stimulus “één” is heard in the right channel each time by
    pressing on the “1” button, and the gain of “één” has a random gain

How

:   See &lt;randomgenerators&gt;, the id of the gain is set to be the
    parameter of the randomgenerator, minimum value contains a decimal
    value

#### randomgenerator/uniform-int.apx

Short

:   Randomgenerator: random gain of stimulus presented in 1 channel,
    only possible to use integer values

Description

:   The stimulus “één” is heard in the left channel each time by
    pressing on the “1” button, and the gain of “één” has a random gain

How

:   See &lt;randomgenerators&gt;, the id of the gain is set to be the
    parameter of the randomgenerator, minimum and maximum values are
    integers

#### randomgenerator/uniform-int-invalid.apx

Short

:   Randomgenerator should have integer values, similar to
    uniform-int.apx

Description

:   Warning (no error) appears in messages window because random
    generator limits are not integer

How

:   Randomgenerator max value is 2.5

#### randomgenerator/uniform-smallint.apx

Short

:   Randomgenerator: random gain of stimulus presented in 1 channel,
    only possible to use integer values, only small range is used

Description

:   The stimulus “één” is heard in the left channel each time by
    pressing on the “1” button, and the gain of “één” has a random gain
    (small range in this case)

How

:   See &lt;randomgenerators&gt;, the id of the gain is set to be the
    parameter of the randomgenerator, minimum and maximum values are
    integers

### results

#### results/results-adaptive-saveprocessedresults.apx

Short

:   Demonstrate the use of result parameters

Description

:   The parameters under ’results’ will be written to the results file.

How

:   Use of rtresults-test-procedureparameter.html

#### results/results-confusionmatrix.apx

Short

:   Demonstrate the use of a confusionmatrix

Description

:   Results are converted into a confusionmatrix

How

:   Default results file for a confusionmatrix

#### results/results-procedureparameter-after.apx

Short

:   Demonstrate the use of result parameters

Description

:   Results are shown after the experiment is done.

How

:   Results: show results after experiment

#### results/results-procedureparameter-during.apx

Short

:   Demonstrate the use of real-time results

Description

:   Real-time results are shown during the experiment in a separate
    window

How

:   Results: show results during experiment

#### results/results-psignifit.apx

Short

:   Demonstrate the use of ’psignifit’ results

Description

:   APEX contains an implementation of the psignifit library, which can
    fit psychometric functions to data

How

:   Psignifit is called from the resultsviewer and the SRT and slope are
    shown numerically

#### results/results-resultparameters.apx

Short

:   Demonstrate the use of resultparameters

Description

:   Resultparameters specified in
    &lt;results&gt;/&lt;resultparameters&gt; will be passed on to the
    resultviewer

How

:   These parameters will be made available in a hash params.

#### results/results-resultparams-localization-polarplot.apx

Short

:   

Description

:   

How

:   

#### results/results-saveprocessedresults.apx

Short

:   Demonstrate the use of the saveprocessedresults tag

Description

:   Results in CSV format will be appended to the results file, for easy
    importing in other software

How

:   &lt;saveprocessedresults&gt; is set to true

#### results/results-subject.apx

Short

:   Demonstrate how you can add the subjects name to the results file

Description

:   When the subject types his/her name in the interactive, this will
    automatically appear in the results file and be appended to the
    results filename

How

:   subject function in ’results’, ’interactive’ entry is added

### screen

#### screen/button-shortcuts.apx

Short

:   shows how to use shortcuts to answer

Description

:   The screen shows different buttons: you can either click on them, or
    use a predefined shortcut button on the keyboard

How

:   shortcuts are implemented for the buttons

#### screen/currentfeedback.apx

Short

:   highlighting of the played stimulus

Description

:   the currently playing stimulus/button is highlighted

How

:   &lt;showcurrent&gt;true

#### screen/feedback-answer.apx

Short

:   feedback with picture and highlighting of the CLICKED element

Description

:   When clicking a button, a feedback picture (thumb up or down) is
    shown in the right panel and the CLICKED button is highlighted.

How

:   &lt;feedback\_on&gt; clicked

#### screen/feedback-both.apx

Short

:   highlighted stimulus + feedback with picture and highlighting

Description

:   The currently playing stimulus/button is highlighted. When clicking
    a button, a feedback picture (thumb up or down) is shown in the
    right panel and the correct button is highlighted.

How

:   &lt;showcurrent&gt; true, &lt;feedback&gt; true

#### screen/feedback-ledfeedback.apx

Short

:   

Description

:   

How

:   

#### screen/feedback-ledfeedback-localization.apx

Short

:   

Description

:   

How

:   

#### screen/feedback-ledfeedback-showcurrent.apx

Short

:   

Description

:   

How

:   

#### screen/feedback-multistimboth.apx

Short

:   highlighted stimulus + feedback with picture and highlighting for
    multiple stimuli

Description

:   The currently playing stimulus/button is highlighted. When clicking
    a button, a feedback picture (thumb up or down) is shown in the
    right panel and the correct button is highlighted.

How

:   &lt;showcurrent&gt; true, &lt;feedback&gt; true

#### screen/feedback-multistimnormal.apx

Short

:   feedback with picture and highlighting for multiple stimuli

Description

:   When clicking a button, a feedback picture (thumb up or down) is
    shown in the right panel and the correct button is highlighted.

How

:   &lt;feedback&gt; true

#### screen/feedback-multistimshowcurrent.apx

Short

:   highlighted stimulus for multiple stimuli

Description

:   the currently playing stimulus/button is highlighted

How

:   &lt;showcurrent&gt; true

#### screen/feedback-nohighlight.apx

Short

:   feedback without highlighting

Description

:   When clicking a button, a feedback picture (thumb up or down) is
    shown in the right panel and NO elements are highlighted.

How

:   &lt;feedback\_on&gt;: none

#### screen/feedback-normal.apx

Short

:   feedback with picture and highlighting

Description

:   When clicking a button, a feedback picture (thumb up or down) is
    shown in the right panel and the correct button is highlighted.

How

:   &lt;feedback&gt; true

#### screen/feedback-onlywait.apx

Short

:   no visual feedback

Description

:   No feedback is given after clicking the correct/wrong button
    (no highlighting/picture). Subject has to wait between trials.

How

:   &lt;feedback length= ...&gt; false

#### screen/feedback-plugin.apx

Short

:   Feedback using a plugin

Description

:   Own feedback can be tested if dummyfeedbackplugin is changed

How

:   dummyfeedbackplugin

#### screen/feedback-showcurrent.apx

Short

:   highlighted stimulus

Description

:   the currently playing stimulus/button is highlighted

How

:   &lt;showcurrent&gt; true

#### screen/fgcolor.apx

Short

:   shows how to change the font color of a button/label

Description

:   A label and button with a different font color are shown on the
    screen

How

:   &lt;fgcolor&gt;

#### screen/flash-disabled.apx

Short

:   Shows full use of flash movies, but without the actual childmode.
    Shows how to disable a button.

Description

:   the middle egg should not be clickable (disabled)

How

:   &lt;disabled&gt;true

#### screen/flash-flashfeedback.apx

Short

:   Shows full use of flash movies, but without the actual childmode

Description

:   Corresponding flash movies are shown during presentation of the
    stimuli and after clicking the correct/wrong button.

How

:   introduce flash elements: &lt;flash&gt;, &lt;uri&gt;,
    &lt;feedback&gt;

#### screen/flash-normalfeedback.apx

Short

:   Pictures instead of buttons

Description

:   Three eggs instead of buttons are shown.

How

:   flashelements for screen

#### screen/fullscreen.apx

Short

:   shows how to conduct an experiment in full screen modus

Description

:   A typical experiment is shown, but the Windows titlebar/taskbar/...
    is hidden

How

:   &lt;fullscreen&gt;true&lt;fullscreen&gt;

#### screen/general-itiscreen-annoyingtext.apx

Short

:   shows how to use an intertrial screen

Description

:   A screen is shown between two trials.

How

:   &lt;screens&gt; &lt;general&gt; &lt;intertrialscreen&gt;iti

#### screen/general-itiscreen-picture.apx

Short

:   shows how to use an intertrial picture

Description

:   A picture is shown between two trials.

How

:   &lt;screens&gt; &lt;general&gt; &lt;intertrialscreen&gt;iti

#### screen/layout-arc.apx

Short

:   example of different arc layouts for buttons

Description

:   Buttons are placed in several semi-circles on the screen

How

:   &lt;arcLayout&gt; upper, lower, left, right, full

#### screen/layout-arc-invalidwidth.apx

Short

:   Shows problems with arc layout for buttons

Description

:   Error when loading the experiment: something went wrong with the
    placing of the buttons.

How

:   &lt;arcLayout&gt; The number of x-values can’t exceed the width of
    the arc. In this case, the width has to be changed to 9 (instead
    of 2)

#### screen/layout-arcs.apx

Short

:   example of different arc layouts for buttons

Description

:   Buttons are placed in several semi-circles on the screen

How

:   &lt;arcLayout&gt; upper, lower, left, right, full

#### screen/layout-arc-single.apx

Short

:   example of simple archwise organisation of buttons

Description

:   Buttons are placed archwise/in a semi-circle on the screen.

How

:   &lt;arcLayout&gt; upper, lower, left or right

#### screen/layout-grid.apx

Short

:   example of gridlayout

Description

:   buttons are placed in a grid

How

:   &lt;gridLayout&gt;

#### screen/layout-grid-nested.apx

Short

:   example of nested gridlayout

Description

:   buttons are placed in nested grids

How

:   &lt;gridLayout&gt; in &lt;gridLayout&gt;

#### screen/layout-grid-rowcol.apx

Short

:   shows the use of the row and col attributes instead of x and y

Description

:   first screen you see is made with col & row, second screen with x &
    y

How

:   &lt;gridLayout&gt; replacing “x” & “y” by “row” & “col”. col =
    column in the grid, is the same as x. row = row in the grid, is the
    same as y

#### screen/layout-grid-rowcol-invalid.apx

Short

:   shows wrong usage of the row and col attributes

Description

:   first screen you see is made with col & row, second screen with x &
    y

How

:   &lt;gridLayout&gt; col should not be used together with x! col is
    the same as x, refers to column in the grid

#### screen/layout-grid-stretchfactors.apx

Short

:   shows how to change the size of the buttons

Description

:   Buttons showed on the screen have different sizes/stretchfactors.

How

:   Stretch factor for the columns/rows: a list of integers separated
    by comma’s. The width of the columns/rows will be proportional to
    the numbers.

#### screen/layout-grid-stretchfactors-invalid.apx

Short

:   Shows problems with changing the size of the buttons

Description

:   Error when loading the experiment: something went wrong when
    changing the size of the buttons.

How

:   The number of the column stretch factors has to be equal to
    its width.

#### screen/layout-hlayout.apx

Short

:   shows horizontal layout

Description

:   buttons are placed next to each other, in a horizontal layout

How

:   &lt;hLayout&gt;

#### screen/layout-mixed.apx

Short

:   example of mixed layouts

Description

:   buttons are placed in semi-circles and grids on the screen

How

:   &lt;arcLayout&gt;, &lt;gridLayout&gt;

#### screen/nomenu.apx

Short

:   shows how to hide the menu bar

Description

:   A typical experiment is shown, but the menu bar on top of the screen
    is hidden.

How

:   &lt;showmenu&gt;false&lt;showmenu&gt;

#### screen/panel-feedbackpictures.apx

Short

:   shows how to implement a feedback picture, other than the
    traditional thumb

Description

:   A green square is shown as feedback when clicking the correct
    button, a red square is shown when you give a wrong answer

How

:   &lt;feedback\_picture\_positive&gt; &
    &lt;feedback\_picture\_negative&gt;

#### screen/panel-feedbackpictures-filenotfound.apx

Short

:   shows error message for feedback picture

Description

:   Experiment doesn’t load, because the feedback pictures are
    not found.

How

:   Wrong naming of feedback pictures / pictures don’t exist

#### screen/panel-nopanel.apx

Short

:   shows how to hide the panel

Description

:   A typical experiment is shown, but the panel on the right side
    is hidden. Start the experiment by pressing F5.

How

:   &lt;showpanel&gt;false&lt;showpanel&gt;

#### screen/panel-showrepeatbutton.apx

Short

:   shows the use of a repeatbutton

Description

:   A repeatbutton is shown on the screen, just above the progressbar.
    When clicking the button, the last stimulus is repeated.

How

:   &lt;repeatbutton&gt;true&lt;repeatbutton&gt;

#### screen/panel-showstatuspicture.apx

Short

:   shows the use of a statuspicture

Description

:   A statuspicture (dot) is shown, just above the progress bar. The dot
    changes color: red when presenting the stimulus, green when waiting
    for an answer

How

:   &lt;statuspicture&gt;true&lt;statuspicture&gt;

#### screen/panel-showstopandrepeatbutton.apx

Short

:   shows how to use a stopbutton & repeatbutton

Description

:   A red stopbutton and a repeatbutton are displayed on the screen,
    just above the progressbar. When clicking the stopbutton, all output
    is immediately stopped & apex is shut down. When clicking the
    repeatbutton, the last stimulus is repeated.

How

:   &lt;stopbutton&gt; & &lt;repeatbutton&gt;

#### screen/panel-showstopbutton.apx

Short

:   shows how to use the stopbutton

Description

:   A red stopbutton is displayed on the screen, just above
    the progressbar. When clicking this button, all output is
    immediately stopped & apex is shut down

How

:   &lt;stopbutton&gt;true&gt;stopbutton&gt;

#### screen/prefix-apexconfig.apx

Short

:   shows how to use an ’apexconfig’ as a prefix for a filename

Description

:   idem

How

:   &lt;uri\_prefix source=“apexconfig”&gt;pictures&lt;/uri\_prefix&gt;

#### screen/prefix-inline.apx

Short

:   shows how to use an ’inline’ as a prefix for a filename

Description

:   idem

How

:   &lt;uri\_prefix&gt;../../pictures&lt;/uri\_prefix&gt;

#### screen/screenelements-all.apx

Short

:   example of a multitude of screenelements

Description

:   

How

:   

#### screen/screenelements-allbutflash.apx

Short

:   assembly of different kinds of screenelements

Description

:   Different labels and buttons are shown on the screen. First row
    containg a button and answerlabel, second row containing a label and
    a parameterlist, last row a textedit and picture

How

:   &lt;button&gt;, &lt;answerlabel&gt;, &lt;label&gt;,
    &lt;parameterlist&gt;, &lt;textEdit&gt;, &lt;picture&gt;

#### screen/screenelements-answerlabel.apx

Short

:   shows the use of an answerlabel

Description

:   An answerlabel is shown on the screen, containing the correct answer
    for the current trial.

How

:   add &lt;answerlabel&gt; to &lt;gridLayout&gt;

#### screen/screenelements-button-disabled.apx

Short

:   shows how to disable a button

Description

:   Two buttons are shown on the screen: nothing happens when clicking
    the disabled one

How

:   &lt;disabled&gt; true

#### screen/screenelements-checkbox.apx

Short

:   shows how to use a checkbox

Description

:   Checkboxes are shown on the screen: if a checkbox is clicked, the
    subject is assumed to have responded to the trial

How

:   &lt;checkBox&gt;

#### screen/screenelements-html.apx

Short

:   how to use a html page

Description

:   a html page is shown on the screen

How

:   &lt;html&gt; &lt;page&gt;

#### screen/screenelements-label.apx

Short

:   shows how to add text to a label

Description

:   screen shows a large label filled with text

How

:   &lt;label&gt; &lt;text&gt;

#### screen/screenelements-label-bgcolor.apx

Short

:   shows how to change background and font color of a label/button

Description

:   screen shows different labels/buttons with a different background
    color or font color

How

:   &lt;bgcolor&gt; changes the background color, &lt;fgcolor&gt;
    changes the font color

#### screen/screenelements-label-html.apx

Short

:   shows how to use a bold font by using html code

Description

:   screen shows the text written in html code

How

:   &lt;b&gt; text &lt;/b&gt;

#### screen/screenelements-label-resize.apx

Short

:   shows how to resize the text on a label

Description

:   screen shows different labels, with and without resizing of the text

How

:   &lt;text&gt; split text into different lines with ’enter’ & ’&\#xd;

#### screen/screenelements-matrix.apx

Short

:   example of a matrix layout for buttons

Description

:   Screen shows buttons in a 2x2 matrix. After listening to the
    stimulus, you have to select one button in each column before
    clicking OK.

How

:   &lt;matrix&gt;

#### screen/screenelements-parameterlabel.apx

Short

:   shows how to implement a parameterlabel on the screen

Description

:   A parameterlabel is shown on the screen, i.e. a label containg the
    value of a parameter.

How

:   screen: &lt;parameterlabel&gt;, stimuli: define parameter

#### screen/screenelements-parameterlist.apx

Short

:   shows how to implement a parameterlist on the screen

Description

:   Parameters of the played stimuli are shown on the screen. This
    example shows two different parameterlists (two different trials).

How

:   &lt;parameterlist&gt;

#### screen/screenelements-picture-changepicture.apx

Short

:   example of a picture-button

Description

:   Click the picture after listening to the stimulus

How

:   &lt;picture&gt;

#### screen/screenelements-picture-notfound.apx

Short

:   shows error message for picture

Description

:   experiment doesn’t load, error message: picture not found

How

:   wrong &lt;uri&gt; (’abc’-picture doesn’t exist, can’t be found in
    the pictures-folder)

#### screen/screenelements-pictures.apx

Short

:   shows how to implement different pictures in the screen

Description

:   4 pictures are shown

How

:   &lt;picture&gt;

#### screen/screenelements-pictures-disabled.apx

Short

:   shows how to disable a picture-button

Description

:   4 pictures are shown on the screen, one is disabled (number 4)

How

:   &lt;picturelabel&gt; instead of &lt;picture&gt; + picture 4 is not
    included in the button group

#### screen/screenelements-pictures-feedback.apx

Short

:   

Description

:   

How

:   

#### screen/screenelements-pictures-invalid.apx

Short

:   shows error message for picture

Description

:   experiment doesn’t load, error message: picture invalid

How

:   wrong file extension for picture (xxx in stead of .png)

#### screen/screenelements-pictures-resize2.apx

Short

:   example of screen layout with pictures

Description

:   Screen shows 4 pictures, after hearing a stimulus you should click
    one

How

:   &lt;picture&gt;

#### screen/screenelements-pictures-resize.apx

Short

:   example of screen layout with pictures

Description

:   Screen shows 4 pictures, after hearing a stimulus you should click
    one

How

:   &lt;picture&gt;

#### screen/screenelements-samename.apx

Short

:   Check whether everything is OK when two screenelements in different
    screens have the same name

Description

:   /

How

:   /

#### screen/screenelements-slider.apx

Short

:   shows how to implement a vertical slider on the screen

Description

:   Slider is shown on the screen. Sliding the bar doens’t change
    anything to the presented stimuli.

How

:   &lt;slider&gt;

#### screen/screenelements-slider-horizontal.apx

Short

:   shows how to implement a horizontal slider on the screen

Description

:   Slider is shown on the screen. Sliding the bar doens’t change
    anything to the presented stimuli.

How

:   &lt;slider&gt;

#### screen/screenelements-spinbox.apx

Short

:   example of a spinbox

Description

:   a spinbox is an input field that only accepts numbers and has 2
    buttons to respectively increase or decrease its value

How

:   add &lt;spinBox&gt; to Layout, define: startvalue, minimum & maximum

#### screen/screenelements-spinbox-parameter.apx

Short

:   example of a spinbox with adjustable parameter

Description

:   a spinbox is an input field that only accepts numbers and has 2
    buttons to respectively increase or decrease its value

How

:   add &lt;spinBox&gt; to Layout, define: startvalue, minimum &
    maximum, parameter to be adjusted

#### screen/screenplugin-emptyresult.apx

Short

:   shows example of error message

Description

:   Demonstrate a screen generated by javascript, show error message
    because of invalid xml

How

:   wrong naming of source &lt;pluginscreens&gt;

#### screen/stylesheet-elements.apx

Short

:   example of a different screenstyle

Description

:   screen has an other background color, buttons changed color,
    highlighting changed, ...

How

:   &lt;style\_apex&gt;

#### screen/textinput-font.apx

Short

:   example of font input

Description

:   screen shows two ’input fields’ with different fonts

How

:   &lt;textEdit&gt; &lt;font&gt;

#### screen/textinput-inputmasks.apx

Short

:   example of various restricted inputs

Description

:   Screen shows four ’entry fields’, but input is restricted (for
    example, only numbers allowed)

How

:   &lt;textEdit&gt;, &lt;inputmask&gt; (Defined in the
    Qt documentation!)

#### screen/textinput-setfocus.apx

Short

:   example of restricted input

Description

:   Screen shows two ’entry fields’, but input is restricted (for
    example, only numbers allowed)

How

:   &lt;textEdit&gt;, &lt;inputmask&gt; numbers = only numeric input
    will be allowed

### screenplugin

#### screenplugin/screenplugin-emptyresult.apx

Short

:   

Description

:   

How

:   

### soundcard

#### soundcard/soundcard-asio-multiplecard.apx

Short

:   Demonstrate the usage of multiple soundcards

Description

:   two soundcards are defined in &lt;devices&gt;

How

:   an extra soundcard is defined in &lt;devices&gt;, each with their
    own parameters

#### soundcard/soundcard-cardname-inline.apx

Short

:   Specify the name of the sound card to be used

Description

:   Specify the sound card to be used.

How

:   The name of the sound card to be used can be specified
    in devices/device/card. A list of devices in your system can be
    obtained by running apex from the commandline with parameter
    –soundcards

#### soundcard/soundcard-clipping.apx

Short

:   Demonstrate what happens when the output clips

Description

:   The output clips of the first two trials of this experiment

How

:   In the results file, the following will be shown to indicate
    clipping occured: &lt;device id=“soundcard”&gt; &lt;clipped
    channel=“0”/&gt; &lt;/device&gt;

#### soundcard/soundcard-defaultsettings.apx

Short

:   Demonstrate default wavdevice settings

Description

:   Shows default settings in ’devices’

How

:   Devices: type of soundcard, number of channels (2), gain (0),
    samplerate (44100)

#### soundcard/soundcard-driver-asio.apx

Short

:   Demonstrate the usage of different drivers and their parameters

Description

:   In this example asio is used as a driver
    (basicwav-coreaduio/jack/portaudio show examples of
    different drivers)

How

:   Devices: &lt;driver&gt;asio&lt;/driver&gt;

#### soundcard/soundcard-driver-coreaudio.apx

Short

:   Demonstrate the usage of different drivers and their parameters

Description

:   In this example coreaudio is used as a driver
    (basicwav-asio/jack/portaudio show examples of different drivers)

How

:   Devices: &lt;driver&gt;coreaudio&lt;/driver&gt;

#### soundcard/soundcard-driver-jack.apx

Short

:   Demonstrate the usage of different drivers and their parameters

Description

:   In this example jack is used as a driver
    (basicwav-coreaduio/asio/portaudio show examples of
    different drivers)

How

:   Devices: &lt;driver&gt;jack&lt;/driver&gt;

#### soundcard/soundcard-driver-portaudio.apx

Short

:   Demonstrate the usage of different drivers and their parameters

Description

:   In this example portaudio is used as a driver
    (basicwav-coreaduio/jack/asio show examples of different drivers)

How

:   Devices: &lt;driver&gt;asio&lt;/driver&gt;

#### soundcard/soundcard-gain-invalidchannel.apx

Short

:   Demonstrate the misapplication of adjusting gain to the wrong
    channel

Description

:   Gain is applied to the wrong channel (channel = 10, when there are
    only 2 channels)

How

:   Devices: &lt;gain channel=“10”&gt;0&lt;/gain&gt; -&gt; it should be
    channel=“0” of channel=“1”, because there are only 2
    channels defined.

#### soundcard/soundcard-mono-2channel.apx

Short

:   Demonstrate what happens when you use wavdevice with one channel not
    connected

Description

:   When you only use one of two defined channels you will not get an
    error, only a message: “not every inputchannel of soundcard is
    connected”

How

:   Devices: 2 channels defined, only 1 used in &lt;connections&gt;

#### soundcard/soundcard-portaudio-6channels.apx

Short

:   Demonstrate the usage of multiple channels

Description

:   Add more channels to your experiment

How

:   set more channels (e.g. &lt;channels&gt;6&lt;/channels, for 6
    channels&gt; and make connections with them so that the right
    stimulus is send to the right channel

#### soundcard/soundcard-portaudio-bigbuffersize.apx

Short

:   

Description

:   

How

:   &lt;buffersize&gt;8192&lt;/buffersize&gt; is used

#### soundcard/soundcard-portaudio-cardnotfound.apx

Short

:   Demonstrate the misapplication of &lt;card&gt;

Description

:   &lt;card&gt; specifies the name of the sound card. If you use the
    wrong cardname, you will get an error when you try to open your
    experiment in apex.

How

:   wrong card name used in &lt;card&gt; -&gt; ’Soundmax’ should be
    default or Soundmax should be defined in apexconfig.xml

#### soundcard/soundcard-portaudio-interactivecard.apx

Short

:   Demonstrate the misapplication of &lt;card&gt;

Description

:   &lt;card&gt; specifies the name of the sound card to be used. If you
    use the wrong cardname, you will get an error when you try to open
    your experiment in apex.

How

:   wrong card name used in &lt;card&gt; -&gt; ’invalid’ should be
    default or an other name defined in apexconfig.xml

#### soundcard/soundcard-wav-padzero.apx

Short

:   Demonstrate the use of padzero

Description

:   Add the given number of empty (filled with zero’s) buffers to the
    output on the end of a stream. This avoids dropping of the last
    frames on some soundcards.

How

:   Devices: Add padzero function (e.g.
    &lt;padzero&gt;2&lt;/padzero&gt;)

### xmlplugin

#### xmlplugin/autostimulus.apx

Short

:   Demonstrate datablocks and stimuli generated by javascript

Description

:   The datablocks and stimuli are automatically generated in this
    experiment using javascript

How

:   using plugindatablocks and pluginstimuli, setting the
    pluginparameters and referring to the wanted javascript in
    &lt;general&gt; (here autostimulus.js)

#### xmlplugin/autotrials.apx

Short

:   Demonstrate trials, datablocks and stimuli generated by javascript

Description

:   The trials, datablocks and stimuli are automatically generated in
    this experiment using javascript

How

:   using plugintrials, plugindatablocks and pluginstimuli, setting the
    pluginparameters and referring to the wanted javascript in
    &lt;general&gt; (here autostimulus.js)

#### xmlplugin/datablockplugin.apx

Short

:   Demonstrate a datablock generated by javascript

Description

:   The datablock is automatically generated in this experiment using
    javascript

How

:   using plugindatablocks and referring to the wanted javascript in
    &lt;datablocks&gt; (here datablockgenerator.js)

#### xmlplugin/datablockplugin\_l34.apx

Short

:   Demonstrate generating XML code for L34 directly from
    plugindatablocks and pluginstimuli

Description

:   The datablocks are automatically generated in this experiment using
    javascript

How

:   using plugindatablocks, pluginstimuli, setting the pluginparameters
    and referring to the wanted javascript in &lt;datablocks&gt; and
    &lt;stimuli&gt; (here datablockgenerator\_l34.js)

#### xmlplugin/screenplugin.apx

Short

:   Demonstrate a screen generated by javascript

Description

:   The screen is automatically generated in this experiment using
    javascript

How

:   using pluginscreens, setting the pluginparameters and referring to
    the wanted javascript in &lt;screens&gt; (here screengenerator.js)

#### xmlplugin/screenplugin-inlibrary.apx

Short

:   Demonstrate the misapplication of a screen generated by javascript

Description

:   The screen is not generated in this experiment because the script
    source is invalid. You will get an error message when you try to
    open the experiment.

How

:   script source refers to &lt;general&gt; instead of the
    script itself. It should be: &lt;script
    source=“screengenerator-library”/&gt; in &lt;screens&gt; OR
    &lt;scriptlibrary&gt;screengenerator-library.js&lt;/scriptlibrary&gt;
    in &lt;general&gt;

#### xmlplugin/screenplugin-invalidscript.apx

Short

:   Demonstrate the misapplication of a screen generated by javascript

Description

:   The screen is not generated in this experiment because the defined
    javascript is empty.

How

:   invalidxml.js is an empty/invalid javascript so no screen can
    be generated.

#### xmlplugin/screenplugin-invalidxml.apx

Short

:   Demonstrate the misapplication of a screen generated by javascript

Description

:   The screen is not generated in this experiment because the defined
    javascript is empty. You will get an error when you try to open
    the experiment.

How

:   the javascript contains an error so no screen can be generated.

#### xmlplugin/screenplugin-multiscreen.apx

Short

:   Demonstrate a screen generated by javascript

Description

:   The screen is automatically generated in this experiment using
    javascript

How

:   using pluginscreens, setting the pluginparameters and referring to
    the wanted javascript in &lt;screens&gt;
    (here screengenerator-multiscreen.js)

#### xmlplugin/stimulusplugin.apx

Short

:   Demonstrate stimuli generated by javascript

Description

:   The stimuli are automatically generated in this experiment using
    javascript

How

:   using pluginstimuli, setting the pluginparameters and referring to
    the wanted javascript in &lt;stimuli&gt; (here stimulusgenerator.js)

Example 1: Closed-set identification of words in noise with figures
-------------------------------------------------------------------

### General description of the experiment

See . This is an example of a closed-set identification test for
children. A word (wave file) is presented and the child responds by
clicking on one of four figures on the screen
(figure \[fig:closedset\]). Subsequently, a new set of figures is shown,
and again, a word corresponding to one of these figures is routed to the
sound card. This is repeated three times. The three words are embedded
in noise at a certain signal-to-noise ratio. In this example the level
of the noise is fixed and the level of the word varies. At the beginning
of the experiment Apex3 queries for the SNR (signal to noise ratio, in
dB). After having entered this value four figures will appear. Press
Start to start the experiment and to hear the first stimulus (speech in
noise). After the experiment has finished the results are written to a
results file and the percentage correct is determined.

### Conceptual

The experiment as described in the previous paragraph should first be
translated to concepts understood by Apex3. The main concepts in this
example are *datablock*, *stimulus*, *screen* and *procedure*.
For each of the 3 words to be presented to the subject, a wave file is
available on disk. For each wave file, a datablock is defined, and for
each resulting datablock a stimulus is defined. Everything that is
defined is assigned an ID, to be able to refer to it later on.
Therefore, now we have 3 stimuli with IDs , and .

We should also define the things to be shown on the screen during the
experiment. This is done by defining a screen for each word. Each screen
contains 4 pictures, of which one corresponds to the word. Each screen
again gets an ID, in this case we name the screen by the pictures it
contains. Therefore we now have 3 screens with ID , and .

To indicate the order in which the words should be presented, and
together with which screen, a procedure is defined. In the procedure a
number of trials is defined. Recall that a trial is the combination of a
screen, a stimulus and a correct answer. Therefore a trial is a way to
link each of our stimuli with a screen.

Now only the output logic remains to be defined. The idea is to
continuously present a noise signal and to set the level of the words
such that a certain SNR is obtained. To achieve this, we define 2
filters, the first one is a generator (i.e., a filter without input
channels) that will generate the noise signal. The other one is an
amplifier, which will amplify or attenuate the words to obtain the
desired SNR.

In the following sections each of the elements of the file necessary to
implement the latter concepts will be described in detail. The first one
is .

### Detailed description of various elements

```xml
      <procedure xsi:type="apex:constantProcedure">
        <parameters>
          <presentations>1</presentations>
          <order>sequential</order>
        </parameters>
        <trials>
          <trial id="trial_star">
            <answer>picturestar</answer>
            <screen id="screenstar_horse_vase_moon"/>
            <stimulus id="stimulus_star"/>
          </trial>
          <trial id="trial_fly">
            <answer>picturefly</answer>
            <screen id="screenknee_fly_mouse_star"/>
            <stimulus id="stimulus_fly"/>
          </trial>
          <trial id="trial_mouse">
            <answer>picturemouse</answer>
            <screen id="screenmouse_fly_star_moon"/>
            <stimulus id="stimulus_mouse"/>
          </trial>
        </trials>
      </procedure>
```

The attribute indicates that a constant stimuli procedure is used. This
means that the procedure will select the next trial from the trial list
and that it completes after every trial has been presented a certain
number of times.

-   defines the behavior of the procedure

    -   every trial is presented once

    -   the three trials are presented sequentially, in the order that
        is specified in the experiment file

-   contains different elements to specify a trial. After selecting a
    trial the will show the specified screen and send the stimulus to
    the device. Each trial is given an ID (arbitrary name), eg , such
    that it can be referred to later on or viewed in the results file.

-   the correct answer, to be used by Apex3 to determine whether the
    subject’s response is correct. Here, the subject gets the
    opportunity to click on one of four pictures. The result from the
    screen (the subject’s response) will be the ID of the element of the
    screen that was clicked. Therefore, in this example, we specify the
    ID of the picture corresponding to the stimulus that is presented.

A trial must be defined for all the words of the experiment.

```xml
        <corrector xsi:type="apex:isequal"/>
```

The corrector checks whether the response is correct or not. The
attribute compares whether the two input values are exactly the same. In
this example compares the answer specified under trial and the ID
corresponding to the picture that has been clicked.

```xml
    <screens>
        <uri_prefix>closedset</uri_prefix>
        <reinforcement>
          <progressbar>true</progressbar>
          <feedback length="600">false</feedback>
        </reinforcement>
        <screen id="screenstar_horse_vase_moon">
          <gridLayout height="2" width="2">
            <picture id="picturestar" row="1" col="1">
              <path>star.jpg</path>
            </picture>
            <picture id="picturehorse" row="2" col="1">
              <path>horse.jpg</path>
            </picture>
            <picture id="picturevase" row="1" col="2">
              <path>vase.jpg</path>
            </picture>
            <picture id="picturemoon" row="2" col="2">
              <path>moon.jpg</path>
            </picture>
                </gridLayout>
          <buttongroup id="buttongroup1">
            <button id="picturestar"/>
            <button id="picturehorse"/>
            <button id="picturevase"/>
            <button id="picturemoon"/>
          </buttongroup>
          <default_answer_element>buttongroup1</default_answer_element>
        </screen>
        <screen id="screenknee_fly_mouse_star">
          <gridLayout height="2" width="2">
            <picture id="pictureknee" row="1" col="1">
              <path>knee.jpg</path>
            </picture>
            <picture id="picturefly" row="2" col="1">
              <path>fly.jpg</path>
            </picture>
            <picture id="picturemouse" row="1" col="2">
              <path>mouse.jpg</path>
            </picture>
            <picture id="picturestar" row="2" col="2">
              <path>star.jpg</path>
            </picture>
          </gridLayout>
          <buttongroup id="buttongroup2">
            <button id="pictureknee"/>
            <button id="picturefly"/>
            <button id="picturemouse"/>
            <button id="picturestar"/>
          </buttongroup>
          <default_answer_element>buttongroup2</default_answer_element>
        </screen>
        <screen id="screenmouse_fly_star_moon">
          <gridLayout height="2" width="2">
            <picture id="picturemouse" row="1" col="1">
              <path>mouse.jpg</path>
            </picture>
            <picture id="picturefly" row="2" col="1">
              <path>fly.jpg</path>
            </picture>
            <picture id="picturestar" row="1" col="2">
              <path>star.jpg</path>
            </picture>
            <picture id="picturemoon" row="2" col="2">
              <path>moon.jpg</path>
            </picture>
          </gridLayout>
          <buttongroup id="buttongroup3">
            <button id="picturemouse"/>
            <button id="picturefly"/>
            <button id="picturestar"/>
            <button id="picturemoon"/>
          </buttongroup>
          <default_answer_element>buttongroup3</default_answer_element>
        </screen>
      </screens>
```

contains several elements that can be referred to elsewhere in the
experiment file (e.g., in above).

-   a relative path is specified here (relative with respect to the
    experiment file). Since Apex3 knows the location of the experiment
    file, only the folder containing the wave files and pictures must
    be specified. It is also possible to give the absolute path,
    starting at the root. There are three ways to specify a prefix: by
    directly specifying an absolute path, by directly specifying a path
    relative to the experiment file or by referring to a prefix stored
    in . Please refer to [Using prefixes](../creatingexperimentfiles#prefixes) for more information.

-   includes

    -   As the value is a progressbar will be displayed in the right
        hand bottom corner of the screen that indicates the percentage
        of trials that have been completed.

    -   duration of time after response (in msec) that Apex3 waits
        before presenting the next trial. During this interval, feedback
        can be displayed. In this case, no feedback (thumb up,
        thumb down) is given as the value is .

<!-- -->

-   For each word to be presented, a screen is defined. Each screen has
    an ID by which it can be referred to elsewhere in the experiment
    file (e.g. in to associate it with a stimulus).

    -   specifies how the four figures will be arranged on the screen. A
        GridLayout creates a regular grid on the screen with the
        specified number of rows and columns. In this example there are
        2 rows and 2 columns. Each figure is defined by means of
        a element. Such a definition can be seen as associating a
        graphics file with an ID and specifying at what position of the
        layout it should be shown. In this case, jpeg files are used,
        but other formats are also possible (e.g., png, bmp, gif).

        -   since the prefix as specified under is preprended to each it
            suffices to give the name of the .jpeg file in . The path is
            relative to the experiment file.

    <!-- -->

    -   defines a group of screen elements, namely those (four figures)
        that are displayed on the screen. The ID is defined before.

    -   As many elements can be defined in a screen, Apex3 has no way to
        know which element contains the subject’s response. If, for
        example, a text box is shown and 2 buttons, it is unclear which
        is to be used to determine whether the answer is correct or not.
        Therefore, in the element is designated that contains the
        subject’s response. In the case of screen elements that are
        clicked in order to respond, the example is further complicated
        by the fact that we cannot specify just one of the elements
        (buttons, pictures), but the response rather comes from a group
        of elements. This is when a can be used to group together
        different screen elements.

<!-- -->

```xml
     <datablocks>
        <uri_prefix>closedset/</uri_prefix>
        <datablock id="datablock_star">
          <device>wavdevice</device>
          <uri>star.wav</uri>
        </datablock>
        <datablock id="datablock_fly">
          <device>wavdevice</device>
          <uri>fly.wav</uri>
        </datablock>
        <datablock id="datablock_mouse">
          <device>wavdevice</device>
          <uri>mouse.wav</uri>
        </datablock>
        <datablock id="noisedata">
          <device>wavdevice</device>
          <uri>noise.wav</uri>
        </datablock>
        <datablock id="silence">
          <device>wavdevice</device>
          <uri>silence:500</uri>
        </datablock>
      </datablocks>
```

A datablock must be made for all wave files used in the experiment,
including the noise (that will be used by the noise generator). In this
example 5 datablocks are defined, 3 for the word files, 1 for the noise
and 1 for silence. contains 5 elements and a prefix.

-   a relative path is specified here. It is also possible to give the
    absolute path, starting at the root (see [Using prefixes](../creatingexperimentfiles#prefixes)).

-   For each wave file a datablock is made, with an ID. In datablock the
    special syntax is demonstrated for creating a datablock containing
    only silence (i.e., all samples are zeros). This is done to put
    silence before and after the word, to prevent the speech and noise
    from starting at the same time. The length of the silence datablock
    is specified in ms after the prefix . It is added before the signal
    (in element ), not before the noise.

-   Each datablock is associated to a (by means of the ID of
    the device).

In the next sections, the output logic will be defined.
Figure \[fig:ex1-output\] gives an overview of the building blocks that
are used in this example. On the left hand side the datablocks are
shown. In the middle the noise generator and the amplifier and on the
right hand side the sound card. As the words are to be amplified or
attenuated according to the desired SNR, the corresponding datablocks
are routed through the amplifier.

![The connection window<span
data-label="fig:ex1-output"></span>](connectionswindow.png)

In the next element the output device is specified.

```xml
     <devices>
        <device id="wavdevice" xsi:type="apex:wavDeviceType">
          <driver>portaudio</driver>
          <card>default</card>
          <channels>1</channels>
          <samplerate>44100</samplerate>
        </device>
      </devices>
```

All devices defined in the experiment file are grouped in . In this
example there is only 1 element. Its ID is set to . As an ID is unique
for an entire experiment file, it can be used later on to refer to this
device.

-   The attribute tells Apex3 that a sound card is used. The experiment
    only starts if all devices can be opened.

-   specifies the software driver to be used for sound output. If
    unsure, set it to .

-   specifies the name of the sound card to be used. The system default
    card can be used by specifying as a card name. Other card names can
    be defined in .

-   specifies the number of channels of the sound card to be used. The
    number of channels is restricted to the selected sound card, with a
    maximum of 2 for portaudio.

-   the sampling rate of the sound card; Not all sampling rates are
    supported by all devices, and some drivers automatically convert the
    sampling rate. Check your sound card documentation. The sample rate
    of the sound card should correspond to the sampling rates of all
    datablocks used with it. If not, an error message will be shown.

Filters must be defined whenever the signal (or noise) is manipulated.
In this example the level of the noise remains constant and the signal
is amplified or attenuated using an amplifier filter (loop of ).

```xml
    <filters>
        <filter xsi:type="apex:dataloop" id="noisegen"> (*@\label{xml:filter1}@*)
          <device>wavdevice</device>
          <channels>1</channels>
          <continuous>false</continuous>
          <datablock>noisedata</datablock>
          <basegain>-5</basegain>
          <gain id="noisegain">0</gain>
          <randomjump>true</randomjump>
        </filter>
        <filter xsi:type="apex:amplifier" id="amplifier">  (*@\label{xml:filter2}@*)
          <device>wavdevice</device>
          <channels>1</channels>
          <basegain>-5</basegain>
          <gain id="gain">0</gain>
        </filter>
      </filters>
```

contains individual elements, which specify a filter, or as a special
case a generator (i.e., a filter without input channels).

-   on line \[xml:filter1\] the attribute tells Apex3 that a dataloop
    generator has to be created. This is a generator that takes a
    datablock and loops it infinitely. The datablock to be looped is
    specified by its ID . The dataloop generator itself is assigned the
    ID .

-   on line \[xml:filter2\] the attribute tells Apex3 that an amplifier
    has to be created. The gain of this amplifier will be varied to
    change the amplitude of the words and thus the SNR. It is assigned
    ID . The gain of the amplifier is made a variable parameter by
    assigning it ID

    -   The device with which the filter is associated

    -   The number of channels of the filter. The available number of
        channels is dependent on the type of filter. An amplifier can
        have any number of channels.

    -   If is set to , the filter or generator will keep on running in
        between two trials (i.e., when the subject is responding). In
        this example it stops when the stimulus stops playing ().

    -   The datablock with ID noisedata, specified under will be looped.

    -   the total gain of the amplifier is basegain+gain. Basegain
        cannot be a parameter, gain can be a parameter. The total gain
        of the complete output system should not be larger than 0 to
        avoid clipping of the signal. This is why basegain = -5.

    -   extent to which the signal is amplified.

    -   If is set to , when the dataloop is started, it will jump to a
        random sample in the datablock. Thereafter it is looped.

```xml
    <stimuli>
        <stimulus id="stimulus_star">
          <datablocks>
            <sequential>
              <datablock id="silence"/>
              <datablock id="datablock_star"/>
              <datablock id="silence"/>
            </sequential>
          </datablocks>
        </stimulus>
        <stimulus id="stimulus_fly">
          <datablocks>
            <sequential>
              <datablock id="silence"/>
              <datablock id="datablock_fly"/>
              <datablock id="silence"/>
            </sequential>
          </datablocks>
        </stimulus>
        <stimulus id="stimulus_mouse">
          <datablocks>
            <sequential>
              <datablock id="silence"/>
              <datablock id="datablock_mouse"/>
              <datablock id="silence"/>
            </sequential>
          </datablocks>
        </stimulus>
      </stimuli>
```

contains different , each with an ID.

-   can be combined in order (as opposed to .

This is repeated for all the stimuli in the experiment.

```xml
    <connections>
        <connection>
          <from>
            <id>_ALL_</id>
            <channel>0</channel>
          </from>
          <to>
            <id>amplifier</id>
            <channel>0</channel>
          </to>
        </connection>
        <connection>                (*@\label{xml:cha}@*)
          <from>
            <id>amplifier</id>
            <channel>0</channel>
          </from>
          <to>
            <id>wavdevice</id>
            <channel>0</channel>
          </to>
        </connection>               (*@\label{xml:chb}@*)
        <connection>                (*@\label{xml:chc}@*)
          <from>
            <id>noisegen</id>
            <channel>0</channel>
          </from>
          <to>
            <id>wavdevice</id>
            <channel>0</channel>
          </to>
        </connection>               (*@\label{xml:chd}@*)
      </connections>
```

defines how the different datablocks and filters are routed to the
output device. The ID stands for all the datablocks. In this example
they are routed to the first channel of the filter with ID
<span>amplifier</span> (defined under ). In the amplifier the signal is
amplified or attenuated and sent to the wavdevice on lines \[xml:cha\]
to \[xml:chb\]. At the same time the noise, generated by a generator
with ID noisegen, is sent to the same channel of the wavdevice. The
channels are numbered from 0 onwards. The level of the noise remains
constant and does not pass through an amplifier (lines \[xml:chc\]
to \[xml:chd\]).

A visual representation of connections (see Figure \[fig:ex1-output\])
can be obtained by choosing “Show stimulus connections” under
“Experiment”in the main Apex3 menu (top left menu bar).

```xml
     <results>
      <page>apex:resultsviewer.html</page>
      <resultparameters/>
      <showduringexperiment>false</showduringexperiment>
      <showafterexperiment>true</showafterexperiment>
      <saveprocessedresults>true</saveprocessedresults>
     </results>
```

Even if is not specified in the experiment file Apex3 will deliver a
results file in XML.

-   URL of the html page to show in the results window. The page should
    have the appropriate java script methods embedded. More example
    pages can be found in the Apex3 resultsviewer folder.

-   Parameters to be passed to the results page. Each parameter will be
    set in hash params.

-   If true, an extra window will be created which will show the results
    of the current experiment while the experiment is being executed.
    Javascript embedded in the page will be executed upon each
    new trial.

-   If true, when the experiment is finished, a dialog box will appear
    querying whether results should be shown. If the answer is
    affirmative, a new window will be opened and the results will be
    shown after javascript processing.

-   If the experimenter will be asked whether the processed results must
    be appended to the results file. This will only work if the results
    are successfully saved to disk and your javascript code supports
    this transformation.

```xml
    <interactive>
       <entry type="int" description="SNR in dB"
        expression="/apex:apex/filters/filter[@id='amplifier']/gain"
        default="0"/>
     </interactive>
```

If a small part of an experiment file has to be changed right before the
start of an experiment (e.g. a start value, a gain value, the subject’s
name), Apex3 can show the experimenter a small containing the elements
to be changed. This is accomplished by defining the element in the
experiment file.

In this example we will modify the gain of the filter with ID to a value
that is entered by the experimenter at the start of the experiment.

It is only possible to change the value of an existing element of the
experiment file, elements cannot be added. For each element to be
changed, an should be defined under . has four attributes that should be
defined:

-   specifies the type of input element that will be shown. If it is , a
    spinbox[^1] will be shown. If it is a plain text box will be shown.
    In this case a spinbox will be shown as a gain is always numeric.

-   defines the text to be shown in the next to the input element, such
    that the experimenter knows exactly what to fill in.

-   defines the element of the experiment file to be changed. It is
    specified by a so-called XPath expression [^2]. For a description of
    XPath, we refer to the according standard or a good XML book.

-   specifies the default value to be shown in the input element.

Example 2: Identification of speech in noise using an adaptive method
---------------------------------------------------------------------

### General description of the experiment

See . This is an example of an adaptive speech in noise test. It
determines the , the 50 percent correct point, using the 1-down, 1-up
method described by @PM79. In this adaptive procedure the first sentence
is repeated with increasing level until it is identified correctly.
Subsequently, the SRT is determined by increasing or decreasing the
level in steps of 2 dB, according to the last response. Other decision
procedures (eg 2-down 1-up) can also be implemented using Apex3. In this
example the 5 sentences are scored on the basis of their keywords. The
keywords are indicated in bold on the screen. The experimenter/clinician
is seated in front of the screen and decides whether the subject has
repeated the keywords (of the sentence) correctly, after which the
correct or incorrect button is clicked(figure \[fig:sentencenoise\]). No
feedback is provided. The starting level is given in signal-to noise
ratio (the level of the noise remains the same, that of the signal
varies). In this example speech and noise are routed to the same
channel, i.e. one speaker or one earpiece of the headphone. The results
are written to a results file.

![Example of adaptive sentence in noise experiment<span
data-label="fig:sentencenoise"></span>](example2sentenceinnoise.png)

### Conceptual

The experiment as described in the previous paragraph should first be
translated to concepts understood by Apex3. The main concepts in this
example are *datablock*, *stimulus*, *screen*, *procedure*, a
*variable* parameter (to change the gain) and *fixed Parameter* to
show a sentence on the screen. For each sentence a datablock is defined,
and for each resulting datablock a stimulus is defined. As always,
everything that is defined, is assigned an ID, to be able to refer to it
later on. In this example there are 5 stimuli with IDs , , , and . The
procedure defines a number of trials. Recall that a trial is the
combination of a screen (always the same in this example), a stimulus
and a response.

As we are dealing with an adaptive procedure a fixed or variable
parameter is adapted. In this example a variable parameter is adapted to
change the gain of the sentence, and a fixed parameter is used to show a
sentence on the screen. The screen also shows the signal-to-noise ratio
(SNR) under test and the response alternatives “correct” and
“incorrect”.

Now only the output logic remains to be defined. The idea is to
continuously present a noise signal and to vary the level of the
sentences adaptively. To achieve this, we define 2 filters, the first
one is a generator (i.e., a filter without input channels) that will
generate the noise signal. The other one is an amplifier, which will
amplify or attenuate the sentences to obtain the desired SNR. Both are
connected to one channel of the wavdevice.

In the following sections each of the elements of the XML file necessary
to implement the latter concepts will be described in detail.

### Detailed description of various elements

```xml
    <procedure xsi:type="apex:adaptiveProcedure">
      <parameters>
        <presentations>1</presentations>
        <order>sequential</order>
        <corrector xsi:type="apex:isequal"/>
        <nUp>1</nUp>
        <nDown>1</nDown>
        <adapt_parameter>gain</adapt_parameter>
        <start_value>0</start_value>
        <stop_after_type>presentations</stop_after_type>
        <stop_after>1</stop_after>
        <larger_is_easier>true</larger_is_easier>
        <repeat_first_until_correct>true</repeat_first_until_correct>
        <stepsizes><stepsize begin="0" size="2"/></stepsizes>
      </parameters>

      <trials>
        <trial id="trial_sentence1">
             <answer>button_correct</answer>
             <screen id="screen"/>
             <stimulus id="stimulus_sentence1"/>
        </trial>

        <trial id="trial_sentence2">
             <answer>button_correct</answer>
             <screen id="screen"/>
             <stimulus id="stimulus_sentence2"/>
        </trial>

        <trial id="trial_sentence3">
            <answer>button_correct</answer>
            <screen id="screen"/>
            <stimulus id="stimulus_sentence3"/>
        </trial>

        <trial id="trial_sentence4">
            <answer>button_correct</answer>
            <screen id="screen"/>
            <stimulus id="stimulus_sentence4"/>
        </trial>

        <trial id="trial_sentence5">
            <answer>button_correct</answer>
            <screen id="screen"/>
            <stimulus id="stimulus_sentence5"/>
        </trial>
      </trials>
    </procedure>
```

The attribute of refers to a procedure in which a parameter is changed
according to the response of the subject. In this example the gain of
the amplifier is adapted. The procedure selects the next trial from the
trial list and it completes after every trial has been presented a
certain number of times.

-   defines the behavior of the procedure

    -   every trial is presented once

    -   the trials are presented sequentially, in the order that is
        specified in the experiment file (if would be specified, they
        would be presented in random order).

    -   the corrector checks whether the response is correct or not. The
        attribute compares whether the two input values are exactly
        the same. In this example compares the answer specified under
        trial () and the ID corresponding to the picture that has been
        clicked (either or ).

    -   the level is increased after n (here 1) incorrect response(s);
        cf

    -   the level is decreased after n (here 1) correct response(s); cf

    -   to be adapted

    -   the experiment starts with gain=0 (input of user). The value
        here will be replaced by the entry value. Please refer to
        section \[sec:Interactive\] for more information.

    -   the experiment stops after a specified number of presentations
        of each trial is completed (it is also possible to stop after .

    -   the experiment stops after 1 presentation of each trial

    -   If , then larger values of the parameter are easier than
        smaller values. It is used to determine and .

    -   the first trial is repeated with increasing gain until it is
        identified correctly.

    -   from the beginning of the experiment (begin=0) the stepsize is
        2 dB.

-   contains different elements to specify a trial. Once a trial is
    selected, will show the specified screen and send the stimulus to
    the device. Each trial is given an ID (arbitrary name), eg , such
    that it can be referred to later on or viewed in the results file. A
    trial must be defined for all the sentences of the experiment.

-   the correct answer, to be used by Apex3 to determine whether the
    subject’s response is correct. In this example the experimenter will
    click on “correct” or “incorrect”. The result from the screen will
    be the ID of the element of the screen that was clicked ( or ).

<!-- -->

```xml
    <screens>
            <reinforcement>
                <progressbar>true</progressbar>
                <feedback length="600">false</feedback>
            </reinforcement>

            <screen id="screen">
           <gridLayout height="2" width="1" id="main_layout" rowstretch="1,2">
                   <gridLayout width="4" height="4" columnstretch="1,4,2,2"
                   rowstretch="1,1,2,2" id="parameter_layout" row="1" col="1">

        <label id="snrlabel" row="1" col="1">
        <text>snr</text>
        </label>

        <parameterlabel id="snr" row="2" col="1">
          <parameter>gain</parameter>
        </parameterlabel>

        <label id="sentence" row="1" col="2">
        <text>sentence</text>
        </label>

        <parameterlabel id="sentence" row="2" col="2">
        <fontsize>12</fontsize>
        <parameter>sentence</parameter>
        </parameterlabel>

        </gridLayout>

        <gridLayout height="1" width="2" id="answer_layout" x="1" y="2">
        <button x="1" y="1" id="button_correct">
        <text>Correct</text>
         </button>
        <button x="2" y="1" id="button_wrong">
        <text>Incorrect</text>
        </button>

        </gridLayout>
        </gridLayout>
        <buttongroup id="buttongroup">
        <button id="button_correct"/>
        <button id="button_wrong"/>
        </buttongroup>
        <default_answer_element>buttongroup</default_answer_element>
        </screen>
        </screens>
```

contains element that is referred to in .

-   includes elements on progress and feedback

    -   If the value is a progress bar will be displayed in the right
        hand bottom corner of the screen that indicates the percentage
        of trials that have been completed.

    -   duration of the time after response (in msec) that Apex3 waits
        before presenting the next trial. During this interval feedback
        can be displayed. In this example, no feedback (thumb up,
        thumb down) is given as the value is .

-   the screen has an ID by which it can be referred to in each trial.
    In this example the screen displays four blocks in the top left
    corner to indicate the SNR and the sentence. In addition, the labels
    “Correct” and “Incorrect” are displayed on the buttons at the bottom
    of the screen (cfr screenshot, figure...).

    -   places elements in an irregular grid. The screen is divided into
        sections according to the values. In this example there are two
        nested layouts. The main layout divides the screen in two main
        rows (height = 2, width = 1). The parameter layout divides the
        first of these rows in four rows and four columns, of which only
        the two first ones are filled. The answer layout divides the
        second main row in two columns, each with one button. The
        stretch factor for the columns is a list of integers, separated
        by comma’s. There should be as many integers as columns. The
        width of the columns is proportional to the numbers. With , and
        implies that the second row will be twice as wide as the
        first one. If , and and the second column will be four times as
        wide as the first and two times as wide as the 3rd and 4th. With
        the 3rd and 4th rows will be twice as wide as the 1st and 2nd.

    -   the labels on the left are fixed and display the “SNR” and
        “Sentence”. The button “Correct” and “Incorrect” are indicated
        at the bottom of the screen

    -   the blocks on the right display the values of the
        fixed parameters. Gain is the (variable) SNR level, and sentence
        is a fixed parameter, defined in

    -   can be specified in points

    -   defines a group of screen elements (those that are displayed on
        the screen). As many elements can be defined in a screen, Apex3
        has no way to know which element contains the
        subject’s response. Therefore, in the element is designated that
        contains the subject’s response. In the case of screen elements
        that are clicked in order to respond, the example is further
        complicated by the fact that we cannot specify just one of the
        elements (one button, one picture), but that the response rather
        comes from a group of elements. This is when a can be used to
        group together different screen elements.

```xml
    <datablocks>
      <uri_prefix>sentences/</uri_prefix>
      <datablock id="datablock_sentence1">
        <device>wavdevice</device>
        <uri>sent1.wav</uri>
      </datablock>
      <datablock id="datablock_sentence2">
        <device>wavdevice</device>
        <uri>sent2.wav</uri>
      </datablock>
      <datablock id="datablock_sentence3">
        <device>wavdevice</device>
        <uri>sent3.wav</uri>
      </datablock>
      <datablock id="datablock_sentence4">
        <device>wavdevice</device>
        <uri>sent4.wav</uri>
      </datablock>
      <datablock id="datablock_sentence5">
        <device>wavdevice</device>
        <uri>sent5.wav</uri>
      </datablock>
      <datablock id="noisedata">
        <device>wavdevice</device>
        <uri>noise.wav</uri>
      </datablock>
      <datablock id="silence">
        <device>wavdevice</device>
        <uri>silence:500</uri>
      </datablock>
    </datablocks>
```

contains a list of elements and a prefix.

-   a relative path is specified here (relative with respect to the
    experiment file). Since Apex3 knows the location of the experiment
    file, only the folder containing the wave files and pictures must
    be specified. It is also possible to give the absolute path,
    starting at the root. There are 3 ways to specify a prefix: by
    directly specifying an absolute path, by directly specifying a path
    relative to the experiment file or by referring to a prefix stored
    in . Please refer to [Using prefixes](../creatingexperimentfiles#prefixes) for more information.

-   for each wave file a datablock is made, with an ID.

    In datablock the special syntax is demonstrated for creating a
    datablock containing only silence (i.e., zeros). This is done to put
    silence before and after the sentence, to prevent the speech and
    noise from starting at the same time. The length of the silence
    datablock is specified in ms after the prefix . It is added before
    the signal (in element ), not before the noise.

    -   the datablock is associated with the sound card with ID , as
        defined in the section.

    -   the URI is appended to the prefix defined above

```xml
    <devices>
        <device id="wavdevice" xsi:type="apex:wavDeviceType">
        <driver>portaudio</driver>
        <card>default</card>
        <channels>1</channels>
        <samplerate>44100</samplerate>
        </device>
    </devices>
```

All devices defined in the experiment file are grouped in .The ID is set
to . As an ID is unique for an entire experiment file, it can be used
later on to refer to this device (eg in datablocks).

-   the attribute tells Apex3 that a sound card is used. The experiment
    only starts if all devices can be opened.

-   specifies the software driver to be used for sound output. If
    unsure, set it to .

-   specifies the name of the sound card to be used. The system default
    card can be used by specifying as a card name. Other card names can
    be defined in .

-   the sampling frequency of the sound card. Not all sampling rates are
    supported by all devices, and some drivers automatically convert the
    sampling rate. Check your sound card documentation. The sample rate
    of the sound card should correspond to the sampling rates of all
    used with it.

```xml
    <filters>
    <filter xsi:type="apex:dataloop" id="noisegen">
        <device>wavdevice</device>
        <channels>1</channels>
        <continuous>false</continuous>
        <datablock>noisedata</datablock>
        <basegain>-5</basegain>
        <gain>0</gain>
        <randomjump>true</randomjump>
        </filter>
    <filter xsi:type="apex:amplifier" id="amplifier" >
        <device>wavdevice</device>
        <channels>1</channels>
        <basegain>-5</basegain>
        <gain id="gain">0</gain>
    </filter>
    </filters>
```

contains individual elements, which specify a filter, or as a special
case a generator (i.e., a filter without input channels).

-   The attribute tells Apex3 that a dataloop generator has to
    be created. This is a generator that takes a datablock and loops
    it infinitely. The datablock to be looped is specified by its ID .

-   on line \[xml:filter2\] the attribute tells Apex3 that an amplifier
    has to be created. The gain of this amplifier will be varied to
    change the amplitude of the words and thus the SNR. It is assigned
    ID . The gain of the amplifier is made a variable parameter by
    assigning it ID

    -   the device with which the filter is associated

    -   The number of channels of the filter. The available number of
        channels depends on the type of filter. An amplifier can have
        any number of channels.

    -   If set to the noise is presented during the speech, but not
        during the subject’s response.

    -   the datablock with ID noisedata, specified under will be looped.

    -   : the total gain of the amplifier is basegain+gain. The total
        gain of the complete output system should not be larger than 0
        to avoid clipping of the signal. This is why basegain = -5.

    -   the gain value that is changed. E.g. if the targetamplitude is
        65 dB SPL (see Calibration) the signal and noise have equal
        amplitude if gain = 0. Gain is changed by other modules.

    -   If is set to , when the dataloop is started, it will jump to a
        random sample in the datablock. Thereafter it is looped.

<!-- -->
```xml
    <stimuli>
      <fixed_parameters>
        <parameter id="sentence"/>
      </fixed_parameters>

      <stimulus id="stimulus_sentence1">
        <datablocks>
        <sequential>
            <datablock id="silence"/>
            <datablock id="datablock_sentence1"/>
            <datablock id="silence"/>
        </sequential>
        </datablocks>

      <fixedParameters>
        <parameter id="sentence">This is <b> sentence </b> <b>one</b></parameter>
      </fixedParameters>
      </stimulus>

      <stimulus id="stimulus_sentence2">
        <datablocks>
        <sequential>
            <datablock id="silence"/>
            <datablock id="datablock_sentence2"/>
            <datablock id="silence"/>
        </sequential>
        </datablocks>

      <fixedParameters>
        <parameter id="sentence">This is <b> sentence </b> <b>two</b></parameter>
        </fixedParameters> </stimulus>

      <stimulus id="stimulus_sentence3">
        <datablocks>
        <sequential>
            <datablock id="silence"/>
            <datablock id="datablock_sentence3"/>
            <datablock id="silence"/>
        </sequential>
        </datablocks>

    <fixedParameters>
    <parameter id="sentence">This is <b> sentence </b> <b>three</b></parameter>
    </fixedParameters>
    </stimulus>

    <stimulus id="stimulus_sentence4">
        <datablocks>
        <sequential>
            <datablock id="silence"/>
            <datablock id="datablock_sentence4"/>
            <datablock id="silence"/>
        </sequential>
        </datablocks>

    <fixedParameters>
    <parameter id="sentence">This is <b> sentence </b> <b>four</b></parameter>
    </fixedParameters>
    </stimulus>

    <stimulus id="stimulus_sentence5">
        <datablocks>
        <sequential>
            <datablock id="silence"/>
            <datablock id="datablock_sentence5"/>
            <datablock id="silence"/>
        </sequential>
        </datablocks>

       <fixedParameters> <parameter id="sentence">This is <b> sentence
       </b> <b>five</b> </parameter> </fixedParameters> </stimulus>
    </stimuli>
```

contains different , each with an ID.

-   is used here to be able to show the sentence under test on
    the Screen. Fixed parameters are discussed
    in section \[sec:Parameters\].

    -   the fixed parameter is defined here and should also be defined
        in each

-   Each stimulus gets an ID (referred to in )

    -   here one refers to the described before; it includes the
        sentence file.

        -   The sequence of is indicated here.

    -   sets the fixed parameter for each .

    -   indicates which words (the keywords) should appear in bold
        on screen.

This is repeated for all the sentences.

```xml
    <connections>
        <connection>
            <from>
            <id>_ALL_</id>
            <channel>0</channel>
            </from>
            <to>
            <id>amplifier</id>
            <channel>0</channel>
            </to>
        </connection>
        <connection>                (*@\label{xml:ch1}@*)
            <from>
            <id>amplifier</id>
            <channel>0</channel>
            </from>
            <to>
            <id>wavdevice</id>
            <channel>0</channel>
            </to>
        </connection>               (*@\label{xml:ch2}@*)
        <connection>                (*@\label{xml:ch3}@*)
            <from>
            <id>noisegen</id>
            <channel>0</channel>
            </from>
            <to>
            <id>wavdevice</id>
            <channel>0</channel>
            </to>
            </connection>           (*@\label{xml:ch4}@*)
    </connections>
```

-   defines how the different datablocks and filters are routed to the
    output device. The ID stands for all the datablocks. In this example
    they are routed to the first channel of the filter with ID
    <span>amplifier</span> (defined under ). In the amplifier the signal
    is amplified or attenuated and sent to the wavdevice on
    lines \[xml:ch1\] to \[xml:ch2\]. At the same time the noise,
    generated by a generator with ID noisegen, is sent to the same
    channel of the wavdevice. The channels are numbered from 0 onwards.
    The level of the noise remains constant and does not pass through an
    amplifier (lines \[xml:ch3\] to  \[xml:ch4\]). Although noisedata is
    a and connected to *amplifier* it is not specified in and does not
    pass through *amplifier*.

A visual representation of connections can be obtained by choosing “Show
stimulus connections” under “Experiment”in the main Apex3 menu (top left
menu bar).

```xml
     <results>
      <page>apex:resultsviewer.html</page>
      <resultparameters>
       <parameter name="reversals for mean">4</parameter>
      </resultparameters>
      <showduringexperiment>false</showduringexperiment>
      <showafterexperiment>true</showafterexperiment>
      <saveprocessedresults>true</saveprocessedresults>
     </results>
```

Even if is not specified in the experiment file Apex3 will save a
results file in XML.

-   URL of the html page to show in the results window. The page should
    have the appropriate java script methods embedded. More example
    pages can be found in the Apex3 resultsviewer folder.

-   Parameters to be passed to the results page. Each parameter will be
    set in hash params. In this example, the result will be calculated
    as the mean of the variable parameter (gap) at the last 4 reversals.

-   If true, an extra window will be created which will show the results
    of the current experiment while the experiment is being executed.
    Javascript embedded in the page will be executed upon each
    new trial.

-   If true, when the experiment is finished, a dialog box will appear
    querying whether results should be shown. If the answer is
    affirmative, a new window will be opened and the results will be
    shown after javascript processing.

-   If the experimenter will be asked whether the processed results must
    be appended to the results file. This will only work if the results
    are successfully saved to disk and your javascript code supports
    this transformation.

<!-- -->
```xml
    <interactive> <entry type="int" description="SNR start value"
      expression="/apex:apex/procedure/parameters/start_value" default="4" />
    </interactive>
```

If a small part of an experiment file has to be changed right before the
start of an experiment (e.g. a start value, a gain value, the subject’s
name), Apex3 can show the experimenter a small containing the elements
to be changed. This is accomplished by defining the element in the
experiment file.

In this example we will modify the gain of the filter with ID to a value
that is entered by the experimenter at the start of the experiment.

-   a value is entered, here corresponding to SNR in dB. It is also
    possible to enter a string, e.g. name of the subject (see
    Reference Manual).

-   the interactive window will show 4 as default value; more than one
    entry may be defined.

Example 3: Gap detection: determining the Just Noticeable Difference
--------------------------------------------------------------------

### General description of the experiment

See . This is an example of a gap detection task: two stimuli are
presented to the listener in random order, a stationary noise and an
interrupted noise. During the presentation the intervals are
highlighted. The subject’s task is to indicate the interval with the
interruption(figure \[fig:gapdetection\]). Feedback is provided (thumb
up, thumb down) and the minimal detectable gap is determined by means of
an adaptive procedure (here 2-down, 1-up). Stimuli (wave files) are
generated off line. This example only includes 5 wave files. Usually
many more are generated. The results are written to a text file.

![Example of adaptive gap detection experiment<span
data-label="fig:gapdetection"></span>](example3gapdetection.png)

### Conceptual

The experiment as described in the previous paragraph should first be
translated to concepts understood by Apex3. The main concepts used here
are *procedure*, *screens*, *datablocks* and *stimuli*.

For each wave file (noise with or without a gap) a is defined, and for
each datablock a is defined. In this example there are 5 wave files,
with gap sizes ranging between 1 and 5 ms. Their IDs are , , , and . In
an adaptive procedure either a fixed or variable parameter is defined.
In this example a fixed parameter is used, i.e. the gap size in the
stimulus. The wave files with different gap sizes are stored on disk,
and are assigned a certain gap value. This gap value is used to
determine the gap threshold. Also, a is defined that shows the two
response intervals indicating “1” and “2”.

To indicate the order in which the stimuli should be presented a is
defined. An adaptive procedure is defined containing 1 with five
variable stimuli (with gap) and the standard stimulus without the gap.
Recall that a is the combination of a (always the same in this example),
a stimulus and a correct answer. The standard and stimuli occur randomly
in one of both intervals.

Now only the output logic remains to be defined. The idea is to present
the standard and a variable stimulus after each other, and the stimulus
to be presented depends on the response of the subject. Filters are not
used since the signals are not changed.

In the following sections each of the elements of the XML file necessary
to implement the latter concepts will be described in detail.

### Detailed description of various elements

```xml
    <procedure xsi:type="apex:adaptiveProcedure">
       <parameters>
         <presentations>1</presentations>
         <order>sequential</order>
         <intervals count="2">
    	<interval number="1" element="button1"/>
    	<interval number="2" element="button2"/>
    	</intervals>
         <pause_between_stimuli>1000</pause_between_stimuli>
         <nUp>1</nUp>
         <nDown>2</nDown>
         <adapt_parameter>gap</adapt_parameter>
         <start_value>5</start_value>
         <stop_after_type>reversals</stop_after_type>
         <stop_after>5</stop_after>
         <min_value>0</min_value>
         <max_value>5</max_value>
         <larger_is_easier>true</larger_is_easier>
         <stepsizes>
           <change_after>reversals</change_after>
           <stepsize  begin="0" size="2"/>
           <stepsize begin="2" size="1"/>
         </stepsizes>
      </parameters>

      <trials>
        <trial id="trial1" >
         <screen id="screen1" />
         <stimulus id="stimulus1" />
         <stimulus id="stimulus2"/>
         <stimulus id="stimulus3"/>
         <stimulus id="stimulus4"/>
         <stimulus id="stimulus5"/>
         <standard id="standard"/>
        </trial>
      </trials>
    </procedure>
```

The attribute refers to a procedure in which a parameter is changed
according to the response of the subject. In this example 1 trial is
defined with different stimuli from which the adaptive procedure
chooses.

-   defines the behavior of the procedure (eg., nr of presentations,
    order of presentation, response strategy).

    -   every trial is presented once.

    -   applies to the order of the trials. Since there is only 1 , it
        does not matter here wether the order is specified as or .

    -   the corrector checks whether the response is correct or not. In
        this example the number of choices in was 2. This means that
        will present the target stimulus in either interval 1 or 2.
        informs about the correct interval. also receives the clicked
        button from the screen and looks up the corresponding number in
        the element above and compares it with the number it received
        from .

    -   number of alternatives to choose from (here: 2 intervals) and
        the IDs of the buttons (defined under they correspond to.

    -   a pause of 1000 ms will be introduced between successive wave
        files;

    -   number of items after incorrect trials to increase the gap

    -   number of items after correct trials to decrease the gap

    -   the gap size cannot be smaller than 0.

    -   the gap size cannot be larger than 5

    -   the parameter to change, here the “gap”, see also

    -   the gap at which to start, here 5, see under

    -   reversals (it can also stop after a number of presentations
        or trials)

    -   number of reversals after which the procedure is stopped

    -   the number of reversals that are used for the average threshold

    -   here (the smaller the gap, the more difficult the task)

    -   the stepsize

    <!-- -->

    -   the is changed after a specified number of reversals. In this
        example a stimulus is skipped until the second reversal (start
        at 5, then 3, etc). Thereafter no stimulus is skipped.

-   only 1 trial is defined

    -   the ID of the trial

    -   the ID of the screen

    -   the ID of the stimulus

    -   the ID of the standard

<!-- -->
```xml
    <screens>
      <reinforcement>
        <progressbar>true</progressbar>
        <feedback length="300">true</feedback>
        <showcurrent>true</showcurrent>
      </reinforcement>

      <screen id="screen1" >
        <gridLayout height="1" width="2" id="main_layout">
          <button x="1" y="1" id="button1" >
           <text>1</text>
          </button>
          <button x="2" y="1" id="button2" >
           <text>2</text>
          </button>
        </gridLayout>
      <buttongroup id="buttongroup1">
         <button id="button1"/>
         <button id="button2"/>
      </buttongroup>
        <default_answer_element>buttongroup1</default_answer_element>
     </screen>
    </screens>
```

-   contains several screen elements that can be referred to elsewhere
    in the experiment file (e.g. in above).

-   includes elements on progress and feedback

    -   if a progress bar will be displayed in the right hand bottom
        corner of the screen. The progress bar can indicate the
        percentage of trials that have been done or it shows when a
        reversal occurs in an adaptive procedure. In the latter case the
        progress bar will increase at every reversal while the number of
        trials varies.

    -   duration of the time after response (in msec) that Apex3 waits
        before presenting the next trial. During this interval feedback
        can be displayed.

    -   if the interval is highlighted while a signal is presented.

-   each screen has an ID by which it can be referred to elsewhere in
    the experiment. In this example the screen displays two intervals.

    -   places elements in an irregular grid. The screen is divided into
        sections according to the values. In this example there is an
        equal number of rows (x) and columns (y).

        Gridlayout defines a group of screen elements (those that are
        displayed on the screen).

        -   for each interval a button is specified; this
            button (interval) displays a number on the screen

        -   the left interval denotes “1”, the right one denotes “2”.

    -   defines a group of screen elements (those that are displayed on
        the screen). As many elements can be defined in a screen, Apex3
        has no way to know which element contains the
        subject’s response. Therefore, in the element is designated that
        contains the subject’s response. In the case of screen elements
        that are clicked in order to respond, the example is further
        complicated by the fact that we cannot specify just one of the
        elements (one button, one picture), but that the response rather
        comes from a group of elements. This is when a can be used to
        group together different screen elements.

<!-- -->
```xml
    <datablocks>
      <uri_prefix>Gapfiles</uri_prefix>
        <datablock id="g5ms" >
          <device>wavdevice</device>
          <uri>g5.wav</uri>
        </datablock>

        <datablock id="g4ms" >
          <device>wavdevice</device>
          <uri>g4.wav</uri>
        </datablock>

        <datablock id="g3ms"  >
          <device>wavdevice</device>
          <uri>g3.wav</uri>
        </datablock>

        <datablock id="g2ms" >
          <device>wavdevice</device>
          <uri>g2.wav</uri>
        </datablock>

        <datablock id="g1ms"  >
          <device>wavdevice</device>
          <uri>g1.wav</uri>
        </datablock>

        <datablock id="datablockref">
          <device>wavdevice</device>
          <uri>ref500.wav</uri>
        </datablock>
      </datablocks>
```

contains a list of elements and a prefix.

-   : a relative path is specified here. Since Apex3 knows the location
    of the experiment file, only the folder containing the wave files
    and pictures must be specified.

-   for each wave file a unique datablock is defined by an ID. The
    standard signal (uninterrupted noise) must also be specified here.

-   each datablock includes the audio device to which the signal is
    routed

-   since the path is defined in it is not necessary to specify the
    entire path again

<!-- -->
```xml
    <devices>
     <device id="wavdevice" xsi:type="apex:wavDeviceType">
     <driver>portaudio</driver>
     <card>default</card>
     <channels>2</channels>
     <gain>0</gain>
     <samplerate>44100</samplerate>
     </device>
    </devices>
```

all devices defined in the experiment file are grouped in the . Only 1
is used in this example. The ID is set to soundcard. As an ID is unique
for an entire experiment file, it can be used later on to refer to this
device.

-   the attribute tells Apex3 that a sound card is used. The experiment
    only starts if all devices can be opened.

-   specifies the software driver to be used for sound output. If
    unsure, set it to .

-   specifies the name of the sound card to be used. The system default
    card can be used by specifying as a card name. Other card names can
    be defined in ApexConfig.

-   2 channels are specified here, because the signal is stereo.

-   the sampling frequency of the wave files. Not all sampling rates are
    supported by all devices, and some drivers automatically convert the
    sampling rate. Check your sound card documentation. The sample rate
    of the sound card should correspond to the sampling rates of all
    datablocks used with it.

<!-- -->
```xml
    <stimuli>
       <fixed_parameters>
         <parameter id="gap"/>
       </fixed_parameters>

       <stimulus id="stimulus1" >
         <description>noisewithgap1</description>
         <datablocks>
           <datablock id="g5ms" />
         </datablocks>
         <fixedParameters>
           <parameter id="gap">5</parameter>
         </fixedParameters>
       </stimulus>

       <stimulus id="stimulus2" >
         <description>noisewithgap2</description>
         <datablocks>
           <datablock id="g4ms" />
         </datablocks>
           <fixedParameters>
            <parameter id="gap">4</parameter>
         </fixedParameters>
       </stimulus>

       <stimulus id="stimulus3" >
         <description>noisewithgap3</description>
         <datablocks>
           <datablock id="g3ms" />
         </datablocks>
          <fixedParameters>
           <parameter id="gap">3</parameter>
          </fixedParameters>
       </stimulus>

       <stimulus id="stimulus4" >
         <description>noisewithgap4</description>
         <datablocks>
           <datablock id="g2ms" />
         </datablocks>
           <fixedParameters>
            <parameter id="gap">2</parameter>
           </fixedParameters>
       </stimulus>

       <stimulus id="stimulus5" >
         <description>noisewithgap5</description>
         <datablocks>
           <datablock id="g1ms" />
         </datablocks>
           <fixedParameters>
             <parameter id="gap">1</parameter>
           </fixedParameters>
       </stimulus>

       <stimulus id="standard">
         <datablocks>
          <datablock id="datablockref"/>
         </datablocks>
           <fixedParameters>
            <parameter id="gap">0</parameter>
         </fixedParameters>
       </stimulus>
    </stimuli>
```

defines the auditory events, e.g. noise with gap and noise without gap.

-   -   the gap is a fixed parameter that is identified by an ID

-   this element includes a description of the (variable) stimulus

    -   -   the ID refers to the wave file corresponding to the
        datablock

    -   -   includes information on the size of the variable gap

<!-- -->
```xml
    <connections>
       <connection>
         <from>
           <id>_ALL_</id>
           <channel>0</channel>
         </from>
         <to>
           <id>wavdevice</id>
           <channel>1</channel>
         </to>
       </connection>

     </connections>
```

-   defines how the different datablocks and filters are routed to the
    output device. The ID stands for all the datablocks. In this example
    they are routed to 1 channel of the wavdevice.

A visual representation of connections can be obtained by choosing “Show
stimulus connections” under “Experiment”in the main Apex3 menu (top left
menu bar).

```xml
    <results>
      <page>apex:resultsviewer.html</page>
      <resultparameters>
       <parameter name="reversals for mean">3</parameter>
      </resultparameters>
      <showduringexperiment>false</showduringexperiment>
      <showafterexperiment>true</showafterexperiment>
      <saveprocessedresults>true</saveprocessedresults>
     </results>
```

Even if is not specified in the experiment file Apex3 will save a
results file in XML. The results file will display the correct answers,
the reversals, the entire sequence of responses, and the average
threshold based on the number of reversals and the magnitude of the
corresponding gap parameter specified in the experiment file.

-   URL of the html page to show in the results window. The page should
    have the appropriate java script methods embedded. More example
    pages can be found in the Apex3 resultsviewer folder.

-   Parameters to be passed to the results page. Each parameter will be
    set in hash params.

-   If , an extra window will be created which will show the results of
    the current experiment while the experiment is being executed.
    Javascript embedded in the page will be executed upon each
    new trial.

-   If , when the experiment is finished, a dialog box will appear
    querying whether results should be shown. If the answer is
    affirmative, a new window will be opened and the results will be
    shown after javascript processing.

-   If the experimenter will be asked whether the processed results must
    be appended to the results file. This will only work if the results
    are successfully saved to disk and your javascript code supports
    this transformation.

<!-- -->
```xml
     <general>
      <exitafter>true</exitafter>
     </general>
```

defines some general parameters.

-   if Apex3 will close after the experiment has ended and results have
    been shown.

Example 4: Gap detection in child mode
--------------------------------------

### General description of the experiment

See . This is an example of a gap detection task adapted to the interest
of children: two stimuli are presented to the listener in random order,
a stationary noise and an interrupted noise. It is the same experiment
as example 3, but pictures (movies) of cars replace buttons on the
screen, and a smiley panel is shown (figure \[fig:gapchild\]). During
the presentation of stimuli the cars are animated. The child’s task is
to indicate the stimulus with the interruption. Feedback is provided by
smileys and the minimal detectable gap is determined by means of an
adaptive procedure (here 2-down, 1-up). Only those elements that are
specific to the child mode are described in this example.

![Example of gap detection in child mode<span
data-label="fig:gapchild"></span>](example4gapdetectionchild.png)

### Conceptual

The main concepts illustrated in this example are *childmode*, and
*reinforcement*. Child mode involves a different panel, and
intro/outro movies.

### Detailed description of various elements

```xml
    <screens>
        <uri_prefix>car/</uri_prefix>
        <general>
          <showpanel>true</showpanel>
        </general>
        <reinforcement>
          <progressbar>true</progressbar>
          <feedback length="300">true</feedback>
          <showcurrent>true</showcurrent>
        </reinforcement>
        <childmode>
          <panel>reinforcement.swf</panel>
        </childmode>
        <screen id="screen1">
          <gridLayout height="1" width="2" id="main_layout">
            <flash row="1" col="1" id="button1">
              <path>stillcar.swf</path>
              <feedback>
                <highlight>rijden.swf</highlight>
                <positive>goodcar.swf</positive>
                <negative>badcar.swf</negative>
              </feedback>
            </flash>

            <flash row="1" col="2" id="button2">
              <path>stillcar.swf</path>
              <feedback>
                <highlight>rijden.swf</highlight>
                <positive>goodcar.swf</positive>
                <negative>badcar.swf</negative>
              </feedback>
            </flash>
          </gridLayout>

          <buttongroup id="buttongroup1">
            <button id="button1"/>
            <button id="button2"/>
          </buttongroup>
          <default_answer_element>buttongroup1</default_answer_element>
        </screen>
      </screens>
```

-   a relative path is specified here (relative with respect to the
    experiment file). This only applies to the movies in . There are 3
    ways to specify a prefix: by directly specifying an absolute path,
    by directly specifying a path relative to the experiment file or by
    referring to a prefix stored in . Please refer to
    [Using prefixes](../creatingexperimentfiles#prefixes) for more information.

-   -   if a panel will be shown with smileys (see figure *presented
        above*)

-   -   If a progress bar will be displayed on the right hand side
        with smileys. The progress bar shows when a reversal occurs in
        an adaptive procedure (while the number of trials varies).

    -   duration of the time after response (in msec) that Apex3 waits
        before presenting the next trial. During this interval feedback
        can be displayed

    -   the interval is highlighted while a signal is presented. In this
        childfriendly example, the car corresponding to the present
        interval is moving.

-   replaces the “standard” panel, sets some defaults, enables
    intro/outro movies and changes the background colour

    -   the name of the movie file, per frame (smileys and panel are a
        collection of frames)

Each screen has an ID by which it can be referred to elsewhere in the
experiment. In this example the screen displays two movies, each
containing a car.

-   places elements in an irregular grid. The screen is divided into
    sections according to the values. In this example there are two
    columns and one row.

    -   replaces of the standard version

        -   the name of the flash movie file on disk, for the case
            without animation (still car).

        -   -   the name of the movie file indicating the present
                interval is active (here the moving car)

            -   the name of the movie file following a correct response
                (here a monkey waving)

            -   the name of the movie file following an incorrect
                response (the trunk of an elephant)

-   defines a group of screen elements (those that are displayed on
    the screen). As many elements can be defined in a screen, Apex3 has
    no way to know which element contains the subject’s response.
    Therefore, in the element is designated that contains the
    subject’s response. In the case of screen elements that are clicked
    in order to respond, the example is further complicated by the fact
    that we cannot specify just one of the elements (buttons, pictures),
    but that the response rather comes from a group of elements. This is
    when a can be used to group together different screen elements.

[^1]: A spinbox is an input field that only accepts numbers and has 2
    buttons to respectively increase or decrease its value

[^2]: see <http://www.w3.org/TR/xpath>
