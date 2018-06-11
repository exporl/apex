/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

/** @file
 * Plugin documentation.
 */

/** @mainpage Apex MWF Filter Plugins
 *
 * The following plugins are available:
 * - \subpage amplifier
 * - \subpage emphasis
 * - \subpage beamformer
 * - \subpage hrtf
 * - \subpage threadedhrtf
 * - \subpage wiener
 * - \subpage vad
 * - \subpage adaptivewiener
 * - \subpage filesink
 *
 * A fully documented example plugin can be found at
 * - \subpage plugin
 *
 * An example setup could look like this:
 * \image html example.png
 */

/** @page amplifier Amplifier Filter
 *
 * @plugin
 *   amplifier in libamplifierfilter.so / amplifierfilter.dll
 * @plugindesc
 *   Simple amplifier that amplifies all channels equally by a given factor.
 * @plugininput
 *   The input channels contain the data to be amplified.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, the input data is amplified by <code>basegain + gain</code>.
 * @pluginparams
 *   @paramname basegain
 *   @paramchannel global
 *   @paramtype double
 *   @paramrange (-∞,&nbsp;+∞)
 *   @paramdefault 0.0
 *   @paramdesc Base gain in dB of the amplifier. The total gain is
 *     <code>basegain + gain</code>.
 * @paramnext
 *   @paramname gain
 *   @paramchannel global
 *   @paramtype double
 *   @paramrange (-∞,&nbsp;+∞)
 *   @paramdefault 0.0
 *   @paramdesc Gain of the amplifier. The total gain is <code>basegain +
 *     gain</code>.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @pluginend
 */

/** @page beamformer Beamformer Filter
 *
 * @plugin
 *   beamformer in libbeamformerfilter.so / beamformerfilter.dll
 * @plugindesc
 *   Beamformer FIR filter. The filter coefficents are read from a file with the
 *   following format:
 *   @code
 *   size  type    description
 *      8  double  number of channels (c)
 *      8  double  number of filter coefficients per channel (n)
 *    n*8  double  filter coefficients of the first channel
 *    n*8  double  filter coefficients of the second channel
 *      .
 *      .
 *      .
 *   @endcode
 *   The number of channels from the file must match the number of channels of
 *   the plugin and the APEX streaming blocksize needs to be an integer multiple
 *   of the filter length.
 * @plugininput
 *   The input channels contain the data to be filtered.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, the first channel contains the average of all filtered channels,
 *   and all other channels contain <code>(channel_0 - channel_i)</code>.
 * @pluginparams
 *   @paramname uri
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc File path for the filter coefficients. Other parameters can be
 *   included in the file name with @c ${name}.
 * @paramnext
 *   @paramname limit
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0
 *   @paramdesc Maximum length of the FIR filter that does the filtering, all
 *   additional filter coefficients are ignored. If this is @c 0, the number of
 *   filter coefficients is not limited.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @paramnext
 *   @paramname ...
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc Additonal parameters can be specified which can be used to
 *   modify the filter file path.
 * @pluginend
 */

/** @page hrtf HRTF Filter
 *
 * @plugin
 *   hrtf in libhrtffilter.so / hrtffilter.dll
 * @plugindesc
 *   HRTF FIR filter. The filter coefficents are read from a file with the
 *   following format:
 *   @code
 *   size  type    description
 *      8  double  number of channels (c)
 *      8  double  number of filter coefficients per channel (n)
 *    n*8  double  filter coefficients of the first channel
 *    n*8  double  filter coefficients of the second channel
 *      .
 *      .
 *      .
 *   @endcode
 *   The number of channels from the file must match the number of channels of
 *   the plugin and the APEX streaming blocksize needs to be an integer multiple
 *   of the filter length.
 * @plugininput
 *   The first input channel contains the data to be filtered. All other
 *   channels are ignored.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, each output channel contains the data from the first channel
 *   filtered with the corresponding FIR coefficients.
 * @pluginparams
 *   @paramname uri
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc File path for the filter coefficients. Other parameters can be
 *   included in the file name with @c ${name}.
 * @paramnext
 *   @paramname limit
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0
 *   @paramdesc Maximum length of the FIR filter that does the filtering, all
 *   additional filter coefficients are ignored. If this is @c 0, the number of
 *   filter coefficients is not limited.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @paramnext
 *   @paramname ...
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc Additonal parameters can be specified which can be used to
 *   modify the filter file path.
 * @pluginend
 */

/** @page threadedhrtf Threaded HRTF Filter
 *
 * @plugin
 *   threadedhrtf in libthreadedhrtffilter.so / threadedhrtffilter.dll
 * @plugindesc
 *   Threaded version of the HRTF FIR filter that will be faster with longer
 *   filters and on systems with multiple processor cores. The filter
 *   coefficents are read from a file with the following format:
 *   @code
 *   size  type    description
 *      8  double  number of channels (c)
 *      8  double  number of filter coefficients per channel (n)
 *    n*8  double  filter coefficients of the first channel
 *    n*8  double  filter coefficients of the second channel
 *      .
 *      .
 *      .
 *   @endcode
 *   The number of channels from the file must match the number of channels of
 *   the plugin and the APEX streaming blocksize needs to be an integer multiple
 *   of the filter length.
 * @plugininput
 *   The first input channel contains the data to be filtered. All other
 *   channels are ignored.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, each output channel contains the data from the first channel
 *   filtered with the corresponding FIR coefficients.
 * @pluginparams
 *   @paramname uri
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc File path for the filter coefficients. Other parameters can be
 *   included in the file name with @c ${name}.
 * @paramnext
 *   @paramname limit
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0
 *   @paramdesc Maximum length of the FIR filter that does the filtering, all
 *   additional filter coefficients are ignored. If this is @c 0, the number of
 *   filter coefficients is not limited.
 * @paramnext
 *   @paramname threads
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0
 *   @paramdesc Maximum number of threads that should be used for filtering. If
 *   this is @c 0, the number of threads is determined automatically based on
 *   the number of processor cores of the system.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @paramnext
 *   @paramname ...
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc Additonal parameters can be specified which can be used to
 *   modify the filter file path.
 * @pluginend
 */

/** @page emphasis Preemphasis and Deemphasis Filter
 *
 * @plugin
 *   emphasis in libemphasisfilter.so / emphasisfilter.dll
 * @plugindesc
 *   Preemphasis and deemphasis AR filter. Use the appropriate filter file to
 *   perform preemphasis or deemphasis. All channels use the same filter
 *   coefficients which are read from a file with the following format:
 *   @code
 *   size  type    description
 *      8  double  number of channels (c), always 2
 *      8  double  number of filter coefficients per channel (n)
 *    n*8  double  MA filter coefficients, must be 1.0 followed zeros
 *    n*8  double  actual filter coefficients, first coefficient must be 1.0
 *   @endcode
 * @plugininput
 *   The input channels contain the data to be filtered.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, all channels are contain the filtered input.
 * @pluginparams
 *   @paramname uri
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc File path for the filter coefficients. Other parameters can be
 *   included in the file name with @c ${name}.
 * @paramnext
 *   @paramname limit
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0
 *   @paramdesc Maximum length of the AR filter that does the filtering, all
 *   additional filter coefficients are ignored. If this is @c 0, the number of
 *   filter coefficients is not limited.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @paramnext
 *   @paramname ...
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc Additonal parameters can be specified which can be used to
 *   modify the filter file path.
 * @pluginend
 */

/** @page wiener Wiener Filter
 *
 * @plugin
 *   wiener in libwienerfilter.so / wienerfilter.dll
 * @plugindesc
 *   Offline Wiener FIR filter. The filter coefficents are read from a file with
 *   the following format:
 *   @code
 *   size  type    description
 *      8  double  number of channels (c)
 *      8  double  number of filter coefficients per channel (n)
 *    n*8  double  filter coefficients of the first channel
 *    n*8  double  filter coefficients of the second channel
 *      .
 *      .
 *      .
 *   @endcode
 *   The number of channels from the file must match the number of channels of
 *   the plugin and the APEX streaming blocksize needs to be an integer multiple
 *   of the filter length.
 * @plugininput
 *   The input channels contain the data to be filtered.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, the first output channel contains the sum of all filtered input
 *   channels. All other channels are set @c 0.0.
 * @pluginparams
 *   @paramname uri
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc File path for the filter coefficients. Other parameters can be
 *   included in the file name with @c ${name}.
 * @paramnext
 *   @paramname limit
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0
 *   @paramdesc Maximum length of the FIR filter that does the filtering, all
 *   additional filter coefficients are ignored. If this is @c 0, the number of
 *   filter coefficients is not limited.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @paramnext
 *   @paramname ...
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc Additonal parameters can be specified which can be used to
 *   modify the filter file path.
 * @pluginend
 */

/** @page vad Voice Activity Detector
 *
 * @plugin
 *   vad in libvadfilter.so / vadfilter.dll
 * @plugindesc
 *   Voice activity detector developed by Simon Doclo and Erik De Clippel. The
 *   APEX streaming blocksize needs to be an integer multiple of the VAD length.
 * @plugininput
 *   The input channels contain the data to be analyzed.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain @c 1.0.
 *   Otherwise, all channels are analyzed independently for voice activity. If
 *   voice activity is detected, the output is @c 1.0, otherwise it is @c 0.0.
 *   If it is not possible to decide, the output is @c 0.5.
 * @pluginparams
 *   @paramname length
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [1,&nbsp;+∞)
 *   @paramdefault blockSize
 *   @paramdesc Number of frames that is analyzed at once. The APEX streaming
 *     blocksize needs to be an integer multiple of this value.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the output is set to @c 1.0.
 * @pluginend
 */

/** @page adaptivewiener Adaptive Wiener Filter
 *
 * @plugin
 *   adaptivewiener in libadaptivewienerfilter.so / adaptivewienerfilter.dll
 * @plugindesc
 *   Adaptive noise supression with a Wiener filter. Needs an external VAD to
 *   work.
 * @plugininput
 *   The number of processing channels is determined with n = (channels - 1) /
 *   2. There must be at least 3 input channels. The first n channels contain
 *   the signal that will be filtered, the next n channels contain the signal
 *   that will be used to update the filter. The last channel must contain the
 *   VAD data (@c 1.0 is speech, @c 0.0 is noise, all other values are regarded
 *   as unknown).
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, the first output channel contains the filter output. All other
 *   channels are set to @c 0.0.
 * @pluginparams
 *   @paramname muinv
 *   @paramchannel global
 *   @paramtype double
 *   @paramrange [0.0,&nbsp;1.0]
 *   @paramdefault 0.0
 *   @paramdesc 1/μ parameter of the filter
 * @paramnext
 *   @paramname length
 *   @paramchannel global
 *   @paramtype int
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault blockSize
 *   @paramdesc Length of the adaptive Wiener filter. The APEX streaming
 *     blocksize needs to be an integer multiple of this value.
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @pluginend
 */

/** @page filesink Audio File Sink
 *
 * @plugin
 *   filesink in libsinkfilter.so / sinkfilter.dll
 * @plugindesc
 *   Saves all input channels to a wave file. The format of the wave file is
 *   determined by the @a format parameter.
 * @plugininput
 *   The input channels contain the data to be saved.
 * @pluginoutput
 *   All output channels contain the input data.
 * @pluginparams
 *   @paramname uri
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc File path for the wave file. Other parameters can be included in
 *   the file name with @c ${name}. Use @c ${count} to include a unique number
 *   that is incremented on every trial and @c ${timestamp} to include the
 *   number of seconds since 1970.
 * @paramnext
 *   @paramname format
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault float
 *   @paramdesc The file format of the wave file, valid values are @c pcm8, @c
 *     pcm16, @c pcm24, @c pcm32, @c float and @c double.
 * @paramnext
 *   @paramname ...
 *   @paramchannel global
 *   @paramtype string
 *   @paramrange
 *   @paramdefault
 *   @paramdesc Additonal parameters can be specified which can be used to
 *   modify the wave file path.
 * @pluginend
 */

/** @page plugin Fade Filter
 *
 * To create a filter plugin library, we only need the header file with the APEX
 * plugin filter interface (pluginfilterinterface.h) and a Qt4 installation. As
 * an example, we implement a filter that can fade in and fade out. Optionally
 * it should be possible to disable the filter.
 *
 * @section spec Specification
 *
 * @plugin
 *   fade in libfadefilter.so / fadefilter.dll
 * @plugindesc
 *   Simple fader that provides linear fadein and fadeout
 * @plugininput
 *   The input channels contain the data to be modified.
 * @pluginoutput
 *   If the plugin is disabled, all output channels contain the input data.
 *   Otherwise, the input data is faded with the given settings.
 * @pluginparams
 *   @paramname fadein
 *   @paramchannel global
 *   @paramtype double
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0.5
 *   @paramdesc Fadein duration in seconds
 * @paramnext
 *   @paramname fadeout
 *   @paramchannel global
 *   @paramtype double
 *   @paramrange [0,&nbsp;+∞)
 *   @paramdefault 0.0
 *   @paramdesc Fadeout duration in seconds
 * @paramnext
 *   @paramname disabled
 *   @paramchannel global
 *   @paramtype bool
 *   @paramrange
 *   @paramdefault false
 *   @paramdesc Disables the filter operation, the input data in passed through.
 * @pluginend
 *
 * @section make Makefile
 *
 * @dontinclude fadefilter.pro
 *
 * The easiest way to compile a plugin library is with qmake from Trolltech
 * which comes with Qt4. Just create a file @c fadefilter.pro with the following
 * contents and call qmake on it:
 * @skip QT
 * @until SOURCES
 *
 * If the APEX plugin filter interface header is not in the current directory,
 * the @c .pro file must also contain the following two lines to tell the
 * compiler the path to it:
 * @until DEPEND
 *
 * @section decl Declarations
 *
 * @dontinclude fadefilter.cpp
 *
 * First we include the APEX plugin filter interface header and other
 * miscellaneous headers:
 * @skip #include
 * @until <Q
 *
 * Each plugin library consists of two parts: the plugin filters itself that
 * have to implement PluginFilterInterface and a class to create the filter
 * instances that implements PluginFilterCreator. Lets declare the plugin
 * creator first:
 * @until }
 *
 * To export the plugin creator and make the plugin library work, some Qt4
 * magic is needed:
 * @until Q_EXPORT
 *
 * The filter itself is declared with
 * @until process
 *
 * It contains a couple of general fields
 * @until sampleRate
 *
 * and some fields that are specific to the filter functionality:
 * @until }
 *
 * A bit more Qt4 magic makes sure that the Qt4 type system knows about the
 * interfaces implemented by both classes.
 * @until #include
 *
 * @section creatorimpl PluginFilterCreator Implementation
 *
 * The implementation of the PluginFilterCreator is straightforward. The library
 * exports one filter @c fade, so we make this known by implementing
 * PluginFilterCreator#availablePlugins() appropriately:
 * @skip FadeFilterCreator::
 * @until }
 *
 * In PluginFilterCreator#createFilter(), we just create this filter if the
 * right name is passed, otherwise we return @c NULL. We also make sure to catch
 * any exceptions the constructor may throw, because this method is not supposed
 * to throw anything.
 * @until return NULL
 * @until }
 *
 * @section filterimpl PluginFilterInterface Implementation
 *
 * The constructor for the PluginFilterInterface class saves the passed
 * parameters and resets the filter parameters to the default values:
 * @skip FadeFilter::
 * @until }
 * @until }
 *
 * The default parameters are set to 0.5 seconds fadein and no fadeout.
 * @until }
 *
 * In PluginFilterInterface#isValidParameter(), we check for the three known
 * parameters, otherwise we set an error message and return @c false.
 * @until }
 *
 * PluginFilterInterface#setParameter() does the actual parameter parsing. The
 * @c fadein and @c fadeout parameters are checked for the right type and
 * converted to frames. The boolean @c disabled parameter is compared to
 * different strings that could mean @c true and is also stored. If the name and
 * channel matches no known parameter, we again set an error message and return
 * @c false.
 * @until Unknown parameter
 * @until }
 *
 * To prepare for the processing, we reset the position counter to @c 0 and
 * limit the fadein and fadeout durations to the total number of frames.
 * @until }
 *
 * The actual work is done in PluginFilterInterface#process(). If disabled, the
 * filter does nothing. Otherwise, we apply fadein and fadeout.
 * @until }
 * @until }
 *
 * The fadeout is a bit more complicated because the last buffer may extend
 * beyond the total number of samples that was passed to
 * PluginFilterInterface#prepare().
 * @until position +=
 * @until }
 */
