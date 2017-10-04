Example strategies
==================

N-AFC procedures
----------------

Roving <a name="roving"/>
------

randomgenerator

Roving another stimulus dimension
---------------------------------

Identification by typing a sentence
-----------------------------------

This is an example of an open set identification task: the subject
is required to type (or repeat) the word or sentence that has been
presented. The results are written to a text file.

In order to achieve this it is necessary to adapt , , and .

```xml
      <trial id="trial_maan">
            <answer>moon</answer>
            <screen id="screen"/>
            <stimulus id="stimulus_star"/>
          </trial>

        <screens>
        <reinforcement>
          <progressbar>true</progressbar>
          <feedback length="600">false</feedback>
        </reinforcement>
        <screen id="screen" >
          <gridLayout height="2" width="1" id="main_layout">
            <label x="1" y="1" id="helplabel">
              <text> Type the word you hear</text>
            </label>
            <textEdit x="1" y="2" id="text" >
            </textEdit>
          </gridLayout>
          <default_answer_element>word</default_answer_element>
        </screen>
      </screens>
```

![opensetidentification](opensetidentification.png)

Practice
--------

See . Here the subject clicks on an interval and a stimulus is
presented. This can be done as often as he/she wishes.

To achieve this should be used.

Higlighting
-----------

example threshold procedure, 2 interval, 1 tone

alle soorten van highlighting (theoretisch deel)

### Interleaving procedures

multiprocedure

cmd line parameter

-   procedure

-   corrector

-   ...
