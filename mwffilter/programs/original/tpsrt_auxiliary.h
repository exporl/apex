/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TPSRT_AUXILIARY_H_

#define _EXPORL_MWFFILTER_PROGRAMS_ORIGINAL_TPSRT_AUXILIARY_H_
/*
 *  Program:    Program for testing speech noise cancellation
 *
 *  File:       tpsrt_auxilairy.h
 *
 *  Component:  TP - header file with general defenitions
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       15.05.2007
 */


 /*
 * Other include files
 */
#include    "tp_srtbatch.h"
#include    "tpfilter.h"


/************************************
 * GLOBAL TYPE DEFENITIONS
 ***********************************/
typedef enum {
        MAN_OR_WOMAN = 0,
        LIST_NUM,
        INPUT_SAMPLING_RATE,
        OUTPUT_SAMPLING_RATE,
        MWF_FILTER,
        SPEECH_HRTF,
        MISMATCH,
        NO_MWF,
        SNR_FILE,
        EMP_FILTER,
        DEMP_FILTER,
        SPEECH_DIR,
        SILENCE_BEFORE,
        SILENCE_AFTER,
        FIXED_BF,
        FILTER_SNR_OFFSET,
        NUM_NOISE_SOURCES,
        MONO_STEREO,
        BLOCK_SIZE,
        TOTAL_NUM_MICS,
        MIN_START_SNR,
        NUM_SENT_AVG_SRT,
        OUTPUT_GAIN,
        NUMBER_OF_CONFIG_ELEMS
} TPConfig_list_elems_t;

struct TPSrtBatch_config_t
    {
        int             inputSamplingRateHz;
        int             outputSamplingRateHz;
        int             totalFramesToProcess;
        int             sentenceFramesToProcess;
        int             currentFrame;
        int             frameSize;
        int             currentBlockFrame;
        int             monoOrStereo;
        int             numNoiseSources;
        char            **noiseFiles;
        char            **noiseHrtfFiles;
        void            **noiseFileDescs;
        SAMPLETYPE      **noiseBuffers;
        int             speechListNum;
        char            speechFile[NUMBER_SENTENECS_PER_LIST][MAX_CONFIG_DATA_LENGTH];
        void            *speechFileDesc;
        SAMPLETYPE      *speechBuffer;
        int             numChannels;
        int             silenceBefore;
        int             silenceAfter;
        int             numSentAvgForSRT;
        int             minStartSNR;
        SAMPLETYPE      snr0dBScalingFactor;
        SAMPLETYPE      currentSnrScalingFactor;
        SAMPLETYPE      SNRLeveldB;
        Bool_t          noMWF;
        int             gainMismatch;
        int             filterSNROffset;
        void            *outputFileDescriptor;
        SAMPLETYPE      **inputStageBuffer;
        SAMPLETYPE      **intermediateStageBuffer1;
        SAMPLETYPE      **intermediateStageBuffer2;
        SAMPLETYPE      *outputStageBuffer[NUMBER_CHANNELS_OUTPUT];
        SAMPLETYPE      *dataToJackBuffer[NUMBER_CHANNELS_OUTPUT];
        MCFIR_Filter    **filtersSpatialNoise;
        MCFIR_Filter    *filterSpatialSpeech;
        MCFIR_Filter    *fixedBeamformer;
        char            filterNoiseLeftFile[MAX_CONFIG_DATA_LENGTH];
        MCFIR_Filter    *filterNoiseLeft;
        char            filterNoiseRightFile[MAX_CONFIG_DATA_LENGTH];
        MCFIR_Filter    *filterNoiseRight;
        MCFIR_Filter    *filterEmphasis;
        SCIIR_Filter    *filterDemphasis;
        SCFIR_Filter     *resampleFilter;
        FFTW_state      *fft_resample;
        FFTW_state      *fft_HRTF;
        FFTW_state      *fft_MWF;
        FFTW_state      *fft_Emp;
        FFTW_state      *fft_Fb;
        SAMPLETYPE      SRTLevel;
        SAMPLETYPE      outputGainLin;
   };

/************************************
 * MODULE DEFENITIONS
 ***********************************/
#define     NO_CHANNEL (0)
#define     LEFT    (1)
#define     RIGHT   (2)
#define     STEREO  (LEFT + RIGHT)


/************************************
 * MODULE VARIABLES
 ***********************************/

 /** Array used for storing parsed config data */
extern char                 TPSrtBatch_configData[NUMBER_OF_CONFIG_ELEMS][MAX_CONFIG_DATA_LENGTH];

/** Indicate resampling */
extern Bool_t               TP_resample;

/*
 * Exported function declarations
 */

/**
 *  @brief Called by JACK AUDIO callback function from test module
 *
 *  @param bufferLeft buffer for left channel
 *  @param bufferRight buffer for right channel
 *
 *  @retval TP_SUCCESS Data processed
 *  @retval TP_FAILURE Failure during processing
 *
  */

/* Auxialiary functions used by the previous local fnctions*/
int tpSrtBatch_parseConfigFile(char* filename,struct TPSrtBatch_config_t *testData);

int tpSrtBatch_clearConfigData(struct TPSrtBatch_config_t *testData);
int tpSrtBatch_clearFilters(struct TPSrtBatch_config_t *testData);

void tpSrtBatch_buildSentenceList(struct TPSrtBatch_config_t *testData);

int tpSrtBatch_prepareFilters(struct TPSrtBatch_config_t *testData);
Bool_t tpSrtBatch_askUser(char * question);

int tpSrtBatch_initSNR(SAMPLETYPE *snrValue);

int tpSrtBatch_setNewMWF(int filterSNR, struct TPSrtBatch_config_t *testData);
#endif //<TP_SRT_AUX_H>
