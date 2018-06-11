Bertha in APEX
==============

The goal of this page is to document how Bertha interfaces with APEX, what
functions it performs, and to discuss future improvements. The previous system
will be called Streamapp-connections, and the new system Bertha. APEX can switch
between the two systems with the cli argument `--disable-bertha` or using
apexconfig.

Bertha is responsible for most of the audio processing, Bertha's output gets
handed to APEX which forwards it to the soundcard.

Previous system:

Streamapp-connections &rarr; ringbuffer &rarr; soundcard (Asio, Portaudio, etc...)

New system:

Bertha &rarr; ringbuffer &rarr; soundcard (Asio, Portaudio, etc...)

APEX data to Bertha data
--------------------------------------------

When APEX parses an experiment, it will build the `ExperimentData`. This object
contains `ConnectionsData`, which describes how individual blocks (audio
sources, filters, ...) should be arranged and connected.

The first step is converting the Streamapp-connections data to Bertha data. This
happens in `ExperimentRunDelegate`. During the conversion summation blocks are
inserted when multiple block outputs get connected to a single input of a block,
as Bertha doesn't do implicit summation. And parameterized connections
(connections that could change at runtime), are converted into mixer blocks.

Bertha also needs a soundcard, which is provided through
`ApexSoundCardPlugin`. The purpose of which is explained later.

Bertha builds a tree structure based on this data, where the leaf nodes are
audio sources and the root node is the `ApexSoundCardPlugin`.

#### Using Streamapp-connections blocks in Bertha

Streamapp-connections filters are usable in Bertha with help from the
`ApexAdapterPlugin`. The latter is a Bertha block which instantiates a
`PluginFilter`. By using an adapter the filters are identical between the two
systems, as any filter implemented as `PluginFilter` can be used by both
systems.

Streamapp-connections datablocks have been implemented as
`ApexCompatibleDataBlockPlugin`. This is a Bertha block which offers the same
functionality as the Streamapp-connections datablocks.

During the Experiment
---------------------

### BerthaBuffer

The interface between APEX and Bertha is located in `BerthaBuffer`. It provides
an interface to start, stop, and set parameters at runtime. Note that it doesn't
contain an actual sound buffer, so the naming is a bit unfortunate.

The major difference between Streamapp-connections and Bertha during the
experiment is that Bertha doesn't stop between trials. It is always
continuous[^1].

During the experiments some leaf nodes in Bertha's tree structure are
"activated", indicating that these nodes should process audio. Any nodes that
are redundant, are discarded. The set of active leaf nodes can change each
trial, and all nodes are reinstantiated at the start of each trial.

Some functions that require explanation:

* `BerthaBuffer::addPermanentLeafNode`: these permanent leaf nodes will always
  be active as long as Bertha is running. In practice these are *continuous*
  filters.

* `BerthaBuffer::setActiveDataBlocks`: these datablocks will be active the next
  time `start` is called.

* `BerthaBuffer::runPermanentLeafNodesOnly`: activate only the permanent (or
  *continuous*) leaf nodes. In practice this is called after a trial is
  done. Between trials only these leaf nodes are active. If there are no
  permanent leaf nodes, silence is sent to the soundcard.

* `BerthaBuffer::queueParameter`: this parameter will be applied the next time
  `start` is called.

* `BerthaBuffer::setParameter`: this parameter will be set as soon as possible.

* `BerthaBuffer::start`: a list will be composed of the datablocks set with
  `setActiveDataBlocks`, all filters, and the permanent leaf nodes. Then it will
  atomically set the new leaf nodes and any queued parameters. Guaranteeing that
  the next time `processStream` is called, the new configuration is active.

The reason all filters are added during `start` is because filters might also be
leaf nodes, and when they are they should always be present during trials (but
not necessarily between trials). So a filter might be a leaf node but is not
always a permanent leaf node.

When a filter isn't a leaf node and not connected to any leaf nodes, it's simply
discarded. So no harm is done by adding all filters.

### Bertha to APEX

`WavDeviceBufferThread` is responsible for copying the data from Bertha to the
ringbuffer. When this thread is running it calls `BerthaBuffer::processStream`
which returns a `Stream` object containing the output from Bertha's root
node. This `Stream`'s content is then copied to the ringbuffer.

As written above the root node of the Bertha tree is the
`ApexSoundCardPlugin`. All this plugin does is copy data from an input buffer to
an output buffer. The output buffer here being `BerthaBuffer`'s `Stream` object.

Testing
-------

The "linux-experimenttests" testbench is by far the best way to guarantee
identical sound output between the two systems. Each time a mistake or
difference between the two systems is discovered, an experiment should be added
to cover the fix.

### To add an experiment to the testbench:

1. Create the experiment file in the test reference project. Found at
   [apex3-test-reference](https://github.com/exporl/apex3-test-reference) on
   github.

2. Run it once with Bertha disabled. Copy the resulting sound files and apr file
   to the reference project.

3. Run it again with Bertha disabled, make sure to select the correct audio
   compare function. See `tools/linux-experimenttest-audio-compare-functions.py`
   and `tools/experiments-to-test.txt`. Make sure that the test succeeds.

4. Run it with Bertha enabled, make sure that the test still succeeds.

5. Run it two more times (with and without Bertha) without the fix to verify
   that the test indeed covers the fix.

6. Commit the files to the test reference project, and add the test to
   `tools/experiments-to-test.txt` to make sure they run each build on Jenkins.

### Pitfalls

Testing a continuous experiment is feasible with either audio output from
continuous filters or audio output from stimuli. But not when audio output is
coming from both in the same experiment.

The reason for this is, is that the length between trials can't be controlled.
And as continuous filters keep running, a trial might start at an arbitrary
point in their audio stream. This results in audio that's very hard to compare.

Future improvements
-------------------

The goal of Bertha is to greatly reduce dependency on Streamapp. A large step in
that direction has been taken. But there are still some issues which need to be
resolved.

All of these will also require extensive changes in `WavDevice` and
`WavDeviceIO`, where the "linux-experimenttests" with and without Bertha should
always be the guide.

### Ringbuffer

The ringbuffer is an instance of `BufferedProcessing`, located in
Streamapp. Bertha also has a ringbuffer implementation, but considering the
existing one is heavily tied in with the callbacks to and from the ringbuffer,
it's not usable as is. The callbacks also manage signaling the starting and
stopping of audio processing, checking for buffer underrun, and checking for
clipping.

These checks should happen when audio is copied from Bertha's root node to the
ringbuffer, and from the ringbuffer to the soundcard.

### Soundcards

Streamapp also contains the soundcards providing the actual sound output. These
soundcards could also be implemented in Bertha. Provided that the
above-mentioned checks would work with the Bertha soundcards, in particular the
underrun callback.

### Remaining data structures

Some of Streamapp's data structures are still used where there might be room for
alternatives. For instance the `Stream` object in `BerthaBuffer`.

#### Notes

[^1]: It is not always continuous with a virtual soundcard used for testing, to
    guarantee identical audio output between Streamapp-connections and Bertha.
    As long as APEX ships both systems, the "linux-experimenttests" should run
    with both systems.
