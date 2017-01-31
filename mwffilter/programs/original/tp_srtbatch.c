/************************************
 *  Program:    Program for testing noise cancellation in speech
 *
 *  File:       tp.c
 *
 *  Component:  TP - main function component
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       21.06.2006
 *
 *
 *
 ***********************************/

/************************************
 * INCLUDE FILES
 ***********************************/
#include "tp.h"
#include "tp_srtbatch.h"
#include "tpsrt_auxiliary.h"
#include "tpresample.h"
/* The file below contains strings for all the sentences used in the test */
#include "text/man.inc"

#include "tpfile.h"
#include "tpjack.h"
#include "tp_jackcb.h"

/************************************
 * GLOBAL DEFENITIONS
 ***********************************/

/** The default name of the config file */
#define     SRT_BATCH_TESTING_CONFIG_FILENAME          "data/original/srtbatch.config"

#if !FIXME_REMOVE

    #define     MIC_DIR_LEFT                (0)
    #define     MIC_OMNI_LEFT               (1)
    #define     MIC_DIR_RIGHT               (2)
    #define     MIC_OMNI_RIGHT              (3)
#endif
/************************************
 * GLOBAL VARIABLES
 ***********************************/
/** Structure containing the config data for the test, buffers,filters */
static struct TPSrtBatch_config_t       TPSrtBatch_testData;

/** Flag for running the processing driven by JACK callback function for main test */
static volatile Bool_t                  TPSrtBatch_performProcessing = TP_FALSE;

/** Flag for running the processing driven by JACK callback function for splCalibration */
static volatile Bool_t                  TPSrtBatch_calibrateSPL = TP_FALSE;

/** Flag for running the processing driven by JACK callback function for sentence presentation */
static volatile Bool_t                  TPSrtBatch_presentSentence = TP_FALSE;

/** Array used for storing parsed config data */
char                                    TPSrtBatch_configData[NUMBER_OF_CONFIG_ELEMS][MAX_CONFIG_DATA_LENGTH];

/** Array used for storing name of the output file*/
static char                             TPSrtBatch_outputFile[MAX_CONFIG_DATA_LENGTH];

/** To differ between monaural/binaural processing - used as a bit mask!!! */
static Bool_t                           TP_saveOutput = TP_TRUE;

/** Determines whether to resample */
Bool_t                                  TP_resample = TP_FALSE;

/** Files longer then length will be truncated */
static int                              TP_maxProcessingLenSamples = 0;

/** Global counter to keep track of the currently processed sentence */
static int                              TP_testSentenceNumber = 0;

/** Array to store the SNR staircase for the tested list */
static int                              TP_SNR_TEST_LIST[NUMBER_SENTENECS_PER_LIST+1];

/************************************
 * LOCAL FUNCTIONS
 ***********************************/
/* Two functions should: 1. setup the test enviroment 2. clean test enviroment   */
static int tpSrtBatch_initTest(struct TPSrtBatch_config_t *testData);
static int tpSrtBatch_closeTest(struct TPSrtBatch_config_t *testData);

/* 2 local functions below are the main processing blocks in any scheme */
int tpSrtBatch_generateInput(struct TPSrtBatch_config_t *testData);
int tpSrtBatch_performProcessing(struct TPSrtBatch_config_t *testData);

/* Functions which make up the test procedure */
int tpSrtBatch_calibrateSPLSetup(struct TPSrtBatch_config_t *testData);
int tpSrtBatch_calibrateSPLClean(struct TPSrtBatch_config_t *testData);
int tpSrtBatch_setupSentencePresentation(struct TPSrtBatch_config_t *testData);

int tpSrtBatch_sentencePresentationClean(struct TPSrtBatch_config_t *testData);
int TPfile_save(void **fileDesc, SAMPLETYPE **buffer, int samplesPerChannel);
void MC_FIR_Freq(SAMPLETYPE **MC_in, SAMPLETYPE **MC_out, int N, MCFIR_Filter *MC_Filter, FFTW_state *fft);

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/
int TPSTRBatch_test(void)
{
    int         i = 0;
    int         retVal = TP_SUCCESS;
    Bool_t      splCablirated = TP_FALSE;
    Bool_t      snrCalibrated = TP_FALSE;
    char        *shortString = NULL;
    FILE        *fid;

    printf("\n***************************************");
    printf("\n* WELCOME TO BEAM2 TESTING:  ");
    printf("\n****************************************");

    /* Perform all initial tasks such as: set sampling frequency, cofign file parsing, filter/buffer alloc etc.*/
    retVal = tpSrtBatch_initTest(&TPSrtBatch_testData);

    printf("\n\n\n\nBeginning SRT procedure.\n\n\n\n");
    if (retVal == TP_SUCCESS)
    {
/******************************
 * SPL CALIBRATION
 ******************************/
        if (TP_TRUE == tpSrtBatch_askUser("Calibrate SPL?"))
        {
            /* First we calibrate the SPL level only using the HRTF filtrated noise */
            while((splCablirated == TP_FALSE) && (retVal == TP_SUCCESS))
            {
                retVal = tpSrtBatch_calibrateSPLSetup(&TPSrtBatch_testData);
                TPSrtBatch_calibrateSPL = TP_TRUE;
                TPSrtBatch_performProcessing = TP_TRUE;
                do
                {
                }while(TPSrtBatch_performProcessing == TP_TRUE);
                TPSrtBatch_calibrateSPL = TP_FALSE;
                retVal =  tpSrtBatch_calibrateSPLClean(&TPSrtBatch_testData);
                splCablirated = tpSrtBatch_askUser("Have you finished adjusting the SPL?");
            }
        }
/******************************
 * SNR CALIBRATION
 ******************************/

        /* go then directly to the SNR calibration procedure */
        TPSrtBatch_testData.SNRLeveldB = TPSrtBatch_testData.minStartSNR;
        TPSrtBatch_testData.currentSnrScalingFactor = (TPSrtBatch_testData.snr0dBScalingFactor*pow(10,(TPSrtBatch_testData.SNRLeveldB/20)));
        do
        {
            /* We now need to read the first sentence, and calibrate the SNR level */
            retVal = tpSrtBatch_setupSentencePresentation(&TPSrtBatch_testData);
            printf("\nThe SNR is: %8.2f [dB]",TPSrtBatch_testData.SNRLeveldB);
            printf("\nThe scaling factor is: %8.2f ",TPSrtBatch_testData.currentSnrScalingFactor);
            printf("\nThe sentence number %d is:",TP_testSentenceNumber+1);
            printf("\t\"%s\"\n", manCorrectSentence[TPSrtBatch_testData.speechListNum - 1][TP_testSentenceNumber]);

            TPSrtBatch_presentSentence = TP_TRUE;
            TPSrtBatch_performProcessing = TP_TRUE;
            do
            {
            }while(TPSrtBatch_performProcessing == TP_TRUE);
            TPSrtBatch_presentSentence = TP_FALSE;
            retVal = tpSrtBatch_sentencePresentationClean(&TPSrtBatch_testData);
            snrCalibrated = tpSrtBatch_askUser("Was the question correctly recognized?");
            if (snrCalibrated == TP_TRUE)
            {
                TP_SNR_TEST_LIST[TP_testSentenceNumber] = TPSrtBatch_testData.SNRLeveldB;
                /* The next sentence will be played 2dB lower */
                TPSrtBatch_testData.SNRLeveldB -= TWO_dB;
                TPSrtBatch_testData.currentSnrScalingFactor = (TPSrtBatch_testData.snr0dBScalingFactor*pow( 10, (TPSrtBatch_testData.SNRLeveldB/20)));
                TP_testSentenceNumber++;
            }
            else
            {
                /* Sentence still not recognized: raise the sound level by 2 dB */
                TPSrtBatch_testData.SNRLeveldB += TWO_dB;
                if (TPSrtBatch_testData.SNRLeveldB >= 10)
                {
                    printf("\nGoing into saturation with dB10.");
                    TPSrtBatch_testData.SNRLeveldB = 10;
                }
                TPSrtBatch_testData.currentSnrScalingFactor = (TPSrtBatch_testData.snr0dBScalingFactor*pow( 10, (TPSrtBatch_testData.SNRLeveldB/20)));
            }
        }while(snrCalibrated == TP_FALSE);

/******************************
 * SRT LEVEL
 ******************************/

        /* Now we perform the test which consists of a list of sentences */
        if ((retVal == TP_SUCCESS) && (TP_testSentenceNumber == 1))
        {
            /* zero the SRT for the new test */
            TPSrtBatch_testData.SRTLevel = 0;

            for(; (TP_testSentenceNumber < NUMBER_SENTENECS_PER_LIST) && (retVal == TP_SUCCESS); TP_testSentenceNumber++)
            {
                retVal = tpSrtBatch_setupSentencePresentation(&TPSrtBatch_testData);
                printf("\nThe SNR is: %8.2f [dB]",TPSrtBatch_testData.SNRLeveldB);
                printf("\nThe scaling factor is: %8.2f ",TPSrtBatch_testData.currentSnrScalingFactor);
                printf("\nThe sentence number %d is:",TP_testSentenceNumber+1);
                printf("\t\"%s\"\n", manCorrectSentence[TPSrtBatch_testData.speechListNum - 1][TP_testSentenceNumber]);

                if (TP_testSentenceNumber >= NUMBER_SENTENECS_PER_LIST - (TPSrtBatch_testData.numSentAvgForSRT - 1)) /* - 1 as the last value is the one after the last sentece */
                {
                    TPSrtBatch_testData.SRTLevel += TPSrtBatch_testData.SNRLeveldB;
                    printf("\nThe SRTLevel is: %8.2f [dB]",TPSrtBatch_testData.SRTLevel);
                }

                TPSrtBatch_presentSentence = TP_TRUE;
                TPSrtBatch_performProcessing = TP_TRUE;
                do
                {
                }while(TPSrtBatch_performProcessing == TP_TRUE);
                TPSrtBatch_presentSentence = TP_FALSE;
                retVal = tpSrtBatch_sentencePresentationClean(&TPSrtBatch_testData);

                /* remember the SNR values to reproduce the stair-case in SRT test */
                TP_SNR_TEST_LIST[TP_testSentenceNumber] = TPSrtBatch_testData.SNRLeveldB;

                snrCalibrated = tpSrtBatch_askUser("Was the question correctly recognized?");
                if (snrCalibrated == TP_TRUE)
                {
                    /* The next sentence will be played 2dB lower */
                    TPSrtBatch_testData.SNRLeveldB -= TWO_dB;
                    TPSrtBatch_testData.currentSnrScalingFactor = (TPSrtBatch_testData.snr0dBScalingFactor*pow( 10, (TPSrtBatch_testData.SNRLeveldB/20)));
                }
                else
                {
                    /* The next sentence will be played 2dB louder */
                    TPSrtBatch_testData.SNRLeveldB += TWO_dB;
                    if (TPSrtBatch_testData.SNRLeveldB >= 10)
                    {
                        TPSrtBatch_testData.SNRLeveldB = 10;
                        printf("\nGoing into saturation with dB10.");
                    }
                TPSrtBatch_testData.currentSnrScalingFactor = (TPSrtBatch_testData.snr0dBScalingFactor*pow( 10, (TPSrtBatch_testData.SNRLeveldB/20)));
                }

                if (TP_testSentenceNumber+1 == NUMBER_SENTENECS_PER_LIST)
                {
                    TPSrtBatch_testData.SRTLevel += TPSrtBatch_testData.SNRLeveldB;
                    /* remember the SNR values to reproduce the stair-case in SRT test */
                    TP_SNR_TEST_LIST[TP_testSentenceNumber+1] = TPSrtBatch_testData.SNRLeveldB;
                    printf("\nThe SRTLevel is: %8.2f [dB]",TPSrtBatch_testData.SRTLevel);
                }
            }

            printf("\nThe SRT averaged on last %d sentences was: %8.2f [dB]",TPSrtBatch_testData.numSentAvgForSRT, TPSrtBatch_testData.SRTLevel/TPSrtBatch_testData.numSentAvgForSRT);
            printf("\n\n\nThe staircase scenario was as follows:\n");
            for (i=0; i < NUMBER_SENTENECS_PER_LIST + 1; i++)
            {
                printf("\nsentence %d: %ddB",i+1, TP_SNR_TEST_LIST[i]);
            }
            shortString = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));
            if (shortString == NULL)
            {
                printf("\nTPSrtBatch_realTime(): ERROR allocating memory!");
            }
            else
            {
                sprintf(shortString, "SRT_staircase_list_num_%d.dat",TPSrtBatch_testData.speechListNum);
                fid = fopen(shortString, "w");
                fwrite(&TP_SNR_TEST_LIST[0], sizeof(int), (NUMBER_SENTENECS_PER_LIST + 1),fid);
                fclose(fid);
                printf("\n\nSaved SRT staircase to file %s \n",shortString);
                free(shortString);
            }
        }
    }
    else
    {
        printf("\nTPSrtBatch_realTime(): ERROR initializing test... Aborting");
    }
    /* Clean test enviroment regardles of previous results*/
    retVal = tpSrtBatch_closeTest(&TPSrtBatch_testData);
    if (retVal != TP_SUCCESS)
    {
        printf("\nTPSrtBatch_realTime(): ERROR cleaning test enviroment)");
    }

    return(retVal);
}

/************************************
 * TPSRTBatch_processDataBlock()
 ***********************************/

int TPSRTBatch_processDataBlock(SAMPLETYPE *bufferLeft, SAMPLETYPE *bufferRight, int numFramesReguested)
{
    int         retVal = TP_SUCCESS;
    int         i = 0;

    if (TPSrtBatch_performProcessing == TP_TRUE)
    {
        if (TPSrtBatch_testData.currentBlockFrame == 0)
        {
            tpSrtBatch_generateInput(&TPSrtBatch_testData);

            tpSrtBatch_performProcessing(&TPSrtBatch_testData);
        }
        /* output volume control */
        for(i = 0; i < numFramesReguested; i++)
        {
            TPSrtBatch_testData.outputStageBuffer[STEREO_LEFT][TPSrtBatch_testData.currentBlockFrame + i] = TPSrtBatch_testData.outputGainLin*TPSrtBatch_testData.outputStageBuffer[STEREO_LEFT][TPSrtBatch_testData.currentBlockFrame + i];

            TPSrtBatch_testData.outputStageBuffer[STEREO_RIGHT][TPSrtBatch_testData.currentBlockFrame + i] = TPSrtBatch_testData.outputGainLin*TPSrtBatch_testData.outputStageBuffer[STEREO_RIGHT][TPSrtBatch_testData.currentBlockFrame + i];
        }


        memcpy(bufferLeft,
                (TPSrtBatch_testData.outputStageBuffer[STEREO_LEFT] + TPSrtBatch_testData.currentBlockFrame),
                numFramesReguested*sizeof(SAMPLETYPE));

        memcpy(bufferRight,
                (TPSrtBatch_testData.outputStageBuffer[STEREO_RIGHT] + TPSrtBatch_testData.currentBlockFrame),
                numFramesReguested*sizeof(SAMPLETYPE));

        TPSrtBatch_testData.currentBlockFrame += numFramesReguested;
        if(TPSrtBatch_testData.currentBlockFrame >= TPSrtBatch_testData.frameSize)
        {
            if((TP_saveOutput== TP_TRUE) && (TPSrtBatch_calibrateSPL != TP_TRUE))
            {
                retVal = TPfile_save(&(TPSrtBatch_testData.outputFileDescriptor),
                                        TPSrtBatch_testData.outputStageBuffer,
                                        TPSrtBatch_testData.frameSize);
            }
            TPSrtBatch_testData.currentFrame += TPSrtBatch_testData.frameSize;
            TPSrtBatch_testData.currentBlockFrame = 0;
        }

        if (TPSrtBatch_testData.currentFrame >= TPSrtBatch_testData.totalFramesToProcess)
        {
            TPSrtBatch_performProcessing = TP_FALSE;
        }

    }
    else
    {
        memset(bufferLeft,0, TPSrtBatch_testData.frameSize*sizeof(SAMPLETYPE));
        memset(bufferRight,0, TPSrtBatch_testData.frameSize*sizeof(SAMPLETYPE));
    }

    return(retVal);

}

/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/

/**
 *  @brief Setup the test enviroment
 *
*  @param testData strucutre with the test neviroment
 *
 *  @retval TP_SUCCESS when all parameters are set for test execution
 *  @retval TP_FAILURE when an error occures and test cannot proceed
 */

int tpSrtBatch_initTest(struct TPSrtBatch_config_t * testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    int     jackSamplingRate = 0;

    /* Parse the input config file - hardcoded file name */
    retVal = tpSrtBatch_parseConfigFile(SRT_BATCH_TESTING_CONFIG_FILENAME, testData);
    if (retVal != TP_SUCCESS)
    {
        printf("\ntpSrtBatch_initTest(): ERROR in tpSrtBatch_parseConfigFile()");
        retVal = TP_FAILURE;
    }
    else
    {
        /* check if resampling is needed */
        if (testData->outputSamplingRateHz != testData->inputSamplingRateHz)
        {
            TP_resample = TP_TRUE;
            if ((testData->inputSamplingRateHz == 16000) && (testData->outputSamplingRateHz == 32000 ))
            {
                retVal = TPResample_init16000to32000(testData);
                if (retVal != TP_SUCCESS)
                {
                    printf("\ntpSrtBatch_initTest(): ERROR in TPResample_init16000to32000()!");
                }
            }
        }

        TP_maxProcessingLenSamples = testData->inputSamplingRateHz*MAX_SOUND_FILE_LENGTH_SECONDS;
        testData->currentFrame = 0;
        testData->currentBlockFrame = 0;
        tpSrtBatch_buildSentenceList(testData);
    }

    if(retVal == TP_SUCCESS)
    {
        retVal = tpSrtBatch_prepareFilters(testData);
        if (retVal != TP_SUCCESS)
        {
            printf("\ntpSrtBatch_initTest(): ERROR in tpSrtBatch_prepareFilters()!");
        }
    }

    if (retVal == TP_SUCCESS)
    {
        testData->inputStageBuffer = calloc(testData->numChannels, sizeof(SAMPLETYPE*));
        if (testData->inputStageBuffer == NULL)
        {
            printf("\ntpSrtBatch_initTest(): ERROR allocating inputStageBuffer");
            retVal = TP_FAILURE;
        }
        else
        {
            for(i = 0 ; i < testData->numChannels ;i++)
            {
                testData->inputStageBuffer[i] = calloc(testData->frameSize, sizeof(SAMPLETYPE));
                if (testData->inputStageBuffer[i] == NULL)
                {
                    printf("\ntpSrtBatch_initTest(): ERROR allocating inputStageBuffer[%d]",i);
                    retVal = TP_FAILURE;
                }
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        for(i = 0; (i < NUMBER_CHANNELS_OUTPUT) && (retVal == TP_SUCCESS); i++)
        {
            testData->outputStageBuffer[i] = (SAMPLETYPE *)calloc(testData->frameSize, sizeof(SAMPLETYPE));
            if (testData->outputStageBuffer[i] == NULL)
            {
                printf("\ntpSrtBatch_initTest(): ERROR allocating outputStageBuffer[%d]",i);
                retVal = TP_FAILURE;
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        for(i = 0; (i < NUMBER_CHANNELS_OUTPUT) && (retVal == TP_SUCCESS); i++)
        {
            testData->dataToJackBuffer[i] = (SAMPLETYPE *)calloc(testData->frameSize, sizeof(SAMPLETYPE));
            if (testData->dataToJackBuffer[i] == NULL)
            {
                printf("\ntpSrtBatch_initTest(): ERROR allocating dataToJackBuffer[%d]",i);
                retVal = TP_FAILURE;
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        testData->noiseFileDescs = calloc(testData->numNoiseSources, sizeof(void*));
        if (testData->noiseFileDescs == NULL)
        {
            printf("\ntpSrtBatch_initTest(): ERROR allocating noiseFileDescs");
            retVal = TP_FAILURE;
        }
        else
        {
            for(i = 0 ; i < testData->numNoiseSources ;i++)
            {
                testData->noiseFileDescs[i] = calloc(1, sizeof(void));
                if (testData->noiseFileDescs[i] == NULL)
                {
                    printf("\ntpSrtBatch_initTest(): ERROR allocating noiseFileDescs[%d]",i);
                    retVal = TP_FAILURE;
                }
            }

        }
    }

    if (retVal == TP_SUCCESS)
    {
        testData->noiseBuffers = calloc(testData->numNoiseSources, sizeof(SAMPLETYPE*));
        if (testData->noiseBuffers == NULL)
        {
            printf("\ntpSrtBatch_initTest(): ERROR allocating noiseBuffers");
            retVal = TP_FAILURE;
        }
        for(i = 0 ; (i <testData->numNoiseSources) && (retVal == TP_SUCCESS);i++)
        {
            testData->noiseBuffers[i] = calloc(testData->frameSize, sizeof(SAMPLETYPE));
            if (testData->noiseBuffers[i] == NULL)
            {
                printf("\ntpSrtBatch_initTest(): ERROR allocating noiseBuffers[%d]",i);
                retVal = TP_FAILURE;
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        testData->speechBuffer = calloc(testData->frameSize, sizeof(SAMPLETYPE));
        if (testData->speechBuffer == NULL)
        {
            printf("\ntpSrtBatch_initTest(): ERROR allocating speechBuffer");
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        retVal = TPAudio_start(&jackSamplingRate, testData->frameSize);
        if(retVal == TP_SUCCESS)
        {
            printf("\nAudio device succesfully started");
            TP_AudioRunning = TP_TRUE;
        }
        else
        {
            printf("\nAudio device init failed!");
            /* Just to be on the safe side... */
            TPAudio_stop();
        }
    }

    if( retVal == TP_SUCCESS)
    {
        /* read the initial SNR 0dB value - now mixing is not 1*/
        retVal = tpSrtBatch_initSNR(&(testData->snr0dBScalingFactor));
        if (retVal != TP_SUCCESS)
        {
            printf("\ntpSrtBatch_initTest(): ERROR!!! reading snr scaling factor file");
        }
    }

    if ((testData->noMWF == TP_FALSE) && (retVal == TP_SUCCESS))
    {
        testData->intermediateStageBuffer1 = calloc(testData->numChannels, sizeof(SAMPLETYPE*));
        if (testData->intermediateStageBuffer1 == NULL)
        {
            printf("\ntpSrtBatch_initTest(): ERROR allocating intermediateStageBuffer1");
            retVal = TP_FAILURE;
        }
        for(i = 0 ; (i <testData->numChannels) && (retVal == TP_SUCCESS);i++)
        {
            testData->intermediateStageBuffer1[i] = calloc(testData->frameSize, sizeof(SAMPLETYPE));
            if (testData->intermediateStageBuffer1[i] == NULL)
            {
                printf("\ntpSrtBatch_initTest(): ERROR allocating intermediateStageBuffer1[%d]",i);
                retVal = TP_FAILURE;
            }
        }

        testData->intermediateStageBuffer2 = calloc(testData->numChannels, sizeof(SAMPLETYPE*));
        if (testData->intermediateStageBuffer2 == NULL)
        {
            printf("\ntpSrtBatch_initTest(): ERROR allocating intermediateStageBuffer1");
            retVal = TP_FAILURE;
        }
        for(i = 0 ; (i <testData->numChannels) && (retVal == TP_SUCCESS);i++)
        {
            testData->intermediateStageBuffer2[i] = calloc(testData->frameSize, sizeof(SAMPLETYPE));
            if (testData->intermediateStageBuffer2[i] == NULL)
            {
                printf("\ntpSrtBatch_initTest(): ERROR allocating intermediateStageBuffer1[%d]",i);
                retVal = TP_FAILURE;
            }
        }
    }


    return(retVal);
}

/**
 *  @brief Clean the test enviroment
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS when all parameters are set for test execution
 *  @retval TP_FAILURE when an error occures and test cannot proceed
 */

int tpSrtBatch_closeTest(struct TPSrtBatch_config_t * testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    /* Begin with shutting down jack */
    retVal = TPAudio_stop();
    if (retVal != TP_SUCCESS)
    {
        printf("\nTPSrtBatch_realTime(): ERROR shutting down JACK");
    }

    retVal = tpSrtBatch_clearConfigData(testData);
    if (retVal != TP_SUCCESS)
    {
        printf("\nTPSrtBatch_realTime(): ERROR in tpSrtBatch_clearConfigData()");
    }

    retVal = tpSrtBatch_clearFilters(testData);
    if (retVal != TP_SUCCESS)
    {
        printf("\nTPSrtBatch_realTime(): ERROR in tpSrtBatch_clearFilters()");
    }

    if (testData->inputStageBuffer != NULL)
    {
        for (i = 0; i < testData->numChannels; i++)
        {
            if (testData->inputStageBuffer[i] != NULL)
            {
                free(testData->inputStageBuffer[i]);
            }
        }
        free(testData->inputStageBuffer);
    }

    if (testData->outputStageBuffer != NULL)
    {
        for(i = 0; i < NUMBER_CHANNELS_OUTPUT; i++)
        {
            if (testData->outputStageBuffer[i] != NULL)
            {
                free(testData->outputStageBuffer[i]);
            }
        }
    }

    if (testData->dataToJackBuffer != NULL)
    {
        for(i = 0; i < NUMBER_CHANNELS_OUTPUT; i++)
        {
            if (testData->dataToJackBuffer[i] != NULL)
            {
                free(testData->dataToJackBuffer[i]);
            }
        }
    }

    if (testData->intermediateStageBuffer1 != NULL)
    {
        for(i = 0; i < testData->numChannels; i++)
        {
            if (testData->intermediateStageBuffer1[i] != NULL)
            {
                free(testData->intermediateStageBuffer1[i]);
            }
        }
    }

    if (testData->intermediateStageBuffer2 != NULL)
    {
        for(i = 0; i < testData->numChannels; i++)
        {
            if (testData->intermediateStageBuffer2[i] != NULL)
            {
                free(testData->intermediateStageBuffer2[i]);
            }
        }
    }

    /* zero the sentence counter */
    TP_testSentenceNumber = 0;
    return(retVal);
}


/**
 *  @brief Generate input signals for processing
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS when all singlas generated
 *  @retval TP_FAILURE when an error occures
 */

int tpSrtBatch_generateInput(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    int     framesLoaded = 0;

    /* Clean input stage buffer */
    for(i = 0 ; i <testData->numChannels ;i++)
    {
        memset(testData->inputStageBuffer[i],0,sizeof(SAMPLETYPE)*testData->frameSize);
    }

    for(i = 0 ; i <testData->numNoiseSources ;i++)
    {
        retVal |= TPFile_readFrame(&(testData->noiseFileDescs[i]),
                                testData->noiseBuffers[i],
                                testData->frameSize,
                                &framesLoaded);

        if (framesLoaded < testData->frameSize)
        {
            memset((testData->noiseBuffers[i]+framesLoaded),0,sizeof(SAMPLETYPE)*(testData->frameSize - framesLoaded));
        }
    }

    if (TPSrtBatch_calibrateSPL == TP_TRUE)
    {
        memset(testData->speechBuffer, 0, (testData->frameSize)*sizeof(SAMPLETYPE));
    }

    if (TPSrtBatch_presentSentence == TP_TRUE)
    {
        if ((testData->currentFrame >= testData->silenceBefore)
            && (testData->currentFrame <= testData->silenceBefore + testData->sentenceFramesToProcess))
        {
            retVal |= TPFile_readFrame(&(testData->speechFileDesc),
                                       testData->speechBuffer,
                                       testData->frameSize,
                                       &framesLoaded);

            if (framesLoaded < testData->frameSize)
            {
                memset((testData->speechBuffer+framesLoaded),0,sizeof(SAMPLETYPE)*(testData->frameSize-framesLoaded));
            }
        }
        else
        {
            memset(testData->speechBuffer, 0, (testData->frameSize)*sizeof(SAMPLETYPE));
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* Filter the noise with the speech HRTF */
        MC_FIR_Freq_SIMO_Mix(testData->speechBuffer,
                             testData->numNoiseSources,
                             testData->noiseBuffers,
                             testData->inputStageBuffer,
                             testData->frameSize,
                             testData->filterSpatialSpeech,
                             testData->filtersSpatialNoise,
                             testData->currentSnrScalingFactor,
                             testData->fft_HRTF);
    }
    return(retVal);
}

/**
 *  @brief Perform processing on input signals
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS when all singlas processed
 *  @retval TP_FAILURE when an error occures
 */

int tpSrtBatch_performProcessing(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    int     j = 0;

    if( TPSrtBatch_presentSentence == TP_TRUE)
    {
        /* clean all the buffers before we start processing */
        if (testData->noMWF == TP_TRUE)
        {
            if (testData->monoOrStereo & LEFT)
            {
                memcpy(testData->outputStageBuffer[STEREO_LEFT], testData->inputStageBuffer[0],testData->frameSize*sizeof(SAMPLETYPE));
            }
            if(testData->monoOrStereo & RIGHT)
            {
                memcpy(testData->outputStageBuffer[STEREO_RIGHT], testData->inputStageBuffer[testData->numChannels - 1],testData->frameSize*sizeof(SAMPLETYPE));
            }
        }
        else /*  i.e. perform Wiener filtering */
        {    /* first we apply the microphone mismatch */
            if(testData->gainMismatch != 0)
            {
                /* singals are now in the testData->inputStageBuffer */
                for(i = 0; i < testData->frameSize; i++)
                {
                    if (testData->monoOrStereo & LEFT)
                    {
                        testData->inputStageBuffer[1][i] = pow(10,(SAMPLETYPE)testData->gainMismatch/20)*testData->inputStageBuffer[1][i];
                    }
                    if(testData->monoOrStereo & RIGHT)
                    {
                        testData->inputStageBuffer[testData->numChannels - 1][i] = pow(10,(SAMPLETYPE)testData->gainMismatch/20)*testData->inputStageBuffer[testData->numChannels - 1][i];
                    }
                }
            }

            /* first stage of actual processing is the fixedBeamformer */
            if (testData->fixedBeamformer != NULL)
            {
                MC_FIR_Freq(testData->inputStageBuffer,
                            testData->intermediateStageBuffer1,
                            testData->frameSize,
                            testData->fixedBeamformer,
                            testData->fft_Fb);

                // count the mean for the first channel, put it in the CLEAN buffer TP_output_buffer[][] to obtain speech reference
                for ( i = 0; i <testData->numChannels; i++)
                {
                    memset(testData->intermediateStageBuffer2[i], 0, testData->frameSize*sizeof(SAMPLETYPE));
                }
                for (i = 0; i < testData->numChannels; i++)
                {
                    for(j = 0; j < testData->frameSize; j++)
                    {
                        testData->intermediateStageBuffer2[0][j] += testData->intermediateStageBuffer1[i][j]/testData->numChannels;
                    }
                }
                // now obtain the noise references
                for (i = 1; i < testData->numChannels; i++)
                {
                    for(j = 0; j < testData->frameSize; j++)
                    {
                        testData->intermediateStageBuffer2[i][j] = testData->intermediateStageBuffer1[0][j] - testData->intermediateStageBuffer1[i][j];
                    }
                }
            }/* DONE FB! */
            else
            {
                for (i = 0; i < testData->numChannels; i++)
                {
                    memcpy(testData->intermediateStageBuffer2[i], testData->inputStageBuffer[i],testData->frameSize*sizeof(SAMPLETYPE));
                }
            }
#if 0
            /* perform ephasis */
            if (testData->filterEmphasis != NULL)
            {
                /* First perform ephasis */
                MC_FIR_Freq(testData->intermediateStageBuffer2,
                            testData->intermediateStageBuffer1,
                            testData->frameSize,
                            testData->filterEmphasis,
                            testData->fft_Emp);
            }
            else
            {
                for ( i = 0; i <testData->numChannels; i++)
                {
                    memcpy(testData->intermediateStageBuffer1[i],
                           testData->intermediateStageBuffer2[i],
                           testData->frameSize*sizeof(SAMPLETYPE));
                }
            }
#endif
            // Filter noisy microphone signals
            if (testData->monoOrStereo & LEFT)
            {

                MC_FIR_Freq_FilterSum(testData->intermediateStageBuffer2,
                                      testData->outputStageBuffer[STEREO_LEFT],
                                      testData->frameSize,
                                      testData->filterNoiseLeft,
                                      testData->fft_MWF);
#if 0
                if (testData->filterDemphasis != NULL)
                {
                    //we need to prepare the numerator delay line of the preemphasis filter
                    if (i == 0)
                    {
                        memcpy(testData->filterDemphasis->dlyLineNum,
                               testData->intermediateStageBuffer1,
                               testData->filterDemphasis->numLength*sizeof(SAMPLETYPE));

                        memset(testData->filterDemphasis->dlyLineDen, 0, testData->filterDemphasis->denLength*sizeof(SAMPLETYPE));
                    }

                        retVal = TPFilter_SISOIir(TP_output_buffer_mwf[0],
                                                    &TP_ProcessedDataBuffer[STEREO_LEFT][i],
                                                    filterDemphasis,
                                                    TP_BlockSize);
                }
                else
                {
                    memcpy(&TP_ProcessedDataBuffer[STEREO_LEFT][i],TP_output_buffer_mwf[0],TP_BlockSize*sizeof(SAMPLETYPE));
                }
#endif
            }

            if (testData->monoOrStereo & RIGHT)
            {
                MC_FIR_Freq_FilterSum(testData->intermediateStageBuffer2,
                                      testData->outputStageBuffer[STEREO_RIGHT],
                                      testData->frameSize,
                                      testData->filterNoiseRight,
                                      testData->fft_MWF);
            }
        }
    }
    else /* calibrate SPL procedure, just pass the input signals to output*/
    {
        if (testData->monoOrStereo & LEFT)
        {
            memcpy(testData->outputStageBuffer[STEREO_LEFT], testData->inputStageBuffer[0], testData->frameSize*sizeof(SAMPLETYPE));
        }
        else
        {
            memset(testData->outputStageBuffer[STEREO_LEFT],0,testData->frameSize*sizeof(SAMPLETYPE));
        }
        if(testData->monoOrStereo & RIGHT)
        {
            memcpy(testData->outputStageBuffer[STEREO_RIGHT],
                   testData->inputStageBuffer[testData->numChannels - 1],
                   testData->frameSize*sizeof(SAMPLETYPE));
        }
        else
        {
            memset(testData->outputStageBuffer[STEREO_RIGHT],0,testData->frameSize*sizeof(SAMPLETYPE));
        }
    }

    if (TP_resample == TP_TRUE)
    {

    }
    return(retVal);
}

/**
 *  @brief Calibrate the SNR on first sentence before proceeding
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS when first sentence recognized
 *  @retval TP_FAILURE when an error occures in processing
 */
int tpSrtBatch_calibrateSPLSetup(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    int     fileFs;

    for(i = 0 ; (i <testData->numNoiseSources) && (retVal == TP_SUCCESS) && (testData->noiseFileDescs[i] != NULL);i++)
    {
        retVal = TPFile_open(testData->noiseFiles[i],
                             &testData->totalFramesToProcess,
                             &(testData->noiseFileDescs[i]),
                             &fileFs,
                             ACCESS_READ,
                             0/*saveFs - ignored for ACCESS_READ*/);
    }

    if (testData->totalFramesToProcess >= TP_maxProcessingLenSamples)
    {
        testData->totalFramesToProcess = TP_maxProcessingLenSamples;
    }
    /* zero the processing frame counter */
    testData->currentFrame = 0;
    testData->currentSnrScalingFactor = 0; /* no speech in SPL calibration */

    return(retVal);

}
/**
 *  @brief Calibrate the SNR on first sentence before proceeding
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS when first sentence recognized
 *  @retval TP_FAILURE when an error occures in processing
 */

int tpSrtBatch_calibrateSPLClean(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    /* Close input files, noise and speech */
    for(i = 0 ; (i <testData->numNoiseSources);i++)
    {
        if(testData->noiseFileDescs[i] != NULL)
        {
            retVal = TPFile_close(&(testData->noiseFileDescs[i] ));
            testData->noiseFileDescs[i] = NULL;
        }
    }

    return(retVal);

}

/**
 *  @brief Cleaan up data after sentence presentation
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS on success
 *  @retval TP_FAILURE when an error occures
 */
int tpSrtBatch_setupSentencePresentation(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     fileFs = 0;
    int     i = 0;
    int     j = 0;
    int     fftLen = 0;
    char    *shortString;
    int     dummyFrames = 0;


    /* first the noise files*/
    for(i = 0 ; (i <testData->numNoiseSources) && (retVal == TP_SUCCESS);i++)
    {
        retVal = TPFile_open(testData->noiseFiles[i],
                             &testData->totalFramesToProcess,
                             &(testData->noiseFileDescs[i]),
                             &fileFs,
                             ACCESS_READ,
                             0/*saveFs - ignored for ACCESS_READ*/);

        if (retVal != TP_SUCCESS)
        {
            printf("\ntpSrtBatch_setupSentencePresentation(): ERROR opening file: %s",testData->noiseFiles[i]);
        }
        else
        {
            if (fileFs != testData->inputSamplingRateHz)
            {
                printf("\ntpSrtBatch_setupSentencePresentation(): ERROR!! noise file sampling rate different then in config file");
                retVal = TP_FAILURE;
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        retVal = TPFile_open(testData->speechFile[TP_testSentenceNumber],
                                &testData->sentenceFramesToProcess,
                                &(testData->speechFileDesc),
                                &fileFs,
                                ACCESS_READ,
                                0/*saveFs - ignored for ACCESS_READ*/);

        if (retVal != TP_SUCCESS)
        {
            printf("\ntpSrtBatch_setupSentencePresentation(): ERROR opening file: %s",testData->speechFile[TP_testSentenceNumber]);
        }
        else
        {
            testData->totalFramesToProcess = testData->sentenceFramesToProcess + testData->silenceBefore + testData->silenceAfter;
            if (fileFs != testData->inputSamplingRateHz)
            {
                printf("\ntpSrtBatch_setupSentencePresentation(): ERROR!! speech file sampling rate different then in config file");
                retVal = TP_FAILURE;
            }
        }
    }

    if ((retVal == TP_SUCCESS) && (TP_saveOutput== TP_TRUE))
    {
        strcpy(TPSrtBatch_outputFile, "output");
        shortString = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));
        if (shortString == NULL)
        {
            printf("\ntpSrtBatch_setupSentencePresentation(): ERROR allocating memory!");
            retVal = TP_FAILURE;
        }
        else
        {
            sprintf(shortString, "_SNR%2.0f_sentence%d.wav",testData->SNRLeveldB, TP_testSentenceNumber+1);
            strcat(TPSrtBatch_outputFile, shortString);
            if (TP_resample == TP_TRUE)
            {
            }
            free(shortString);
        }

        /* Open output files */
        retVal = TPFile_open(TPSrtBatch_outputFile,
                             &dummyFrames, /* will be set to zero for ACCESS_WRITE */
                             &(testData->outputFileDescriptor),
                             &fileFs,
                             ACCESS_WRITE,
                             testData->outputSamplingRateHz);
    }

    /* zero the processing frame counter */
    testData->currentFrame = 0;

    if(testData->noMWF == TP_FALSE)
    {
        /* The MWF depend on the SNR level and need to be reloaded, each time the level changes */
        tpSrtBatch_setNewMWF(testData->SNRLeveldB + testData->filterSNROffset, testData);

        if (testData->monoOrStereo & LEFT)
        {
            retVal = MC_FIR_Freq_Init(testData->filterNoiseLeft, testData->filterNoiseLeftFile);
            fftLen = testData->filterNoiseLeft->length;
        }

        if (testData->monoOrStereo & RIGHT)
        {
            retVal = MC_FIR_Freq_Init(testData->filterNoiseRight, testData->filterNoiseRightFile);
            fftLen = testData->filterNoiseRight->length;
        }

        if (retVal == TP_SUCCESS)
        {
            retVal = FFTW_Init(testData->fft_MWF, 2*fftLen); /* assume both filters same length! */
        }
    }

    // zero the delay lines of the filters for new sentence
    for(i = 0 ; (i <testData->numNoiseSources) && (retVal == TP_SUCCESS);i++)
    {
        for(j = 0; j < testData->filtersSpatialNoise[i]->numChannels; j++)
        {
            memset(testData->filtersSpatialNoise[i]->dlyLine[j], 0, testData->filtersSpatialNoise[i]->length*sizeof(SAMPLETYPE));
        }
    }
    for(i = 0; i < testData->filterSpatialSpeech->numChannels; i++)
    {
        memset(testData->filterSpatialSpeech->dlyLine[i], 0, testData->filterSpatialSpeech->length*sizeof(SAMPLETYPE));
    }
    if (testData->fixedBeamformer != NULL)
    {
        for(i = 0; i < testData->fixedBeamformer->numChannels; i++)
        {
            memset(testData->fixedBeamformer->dlyLine[i], 0, testData->fixedBeamformer->length*sizeof(SAMPLETYPE));
        }
    }

    return(retVal);
};
/**
 *  @brief Calibrate the SNR on first sentence before proceeding
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS when first sentence recognized
 *  @retval TP_FAILURE when an error occures in processing
 */
int tpSrtBatch_sentencePresentationClean(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    /* first the noise files*/
    for(i = 0 ; (i <testData->numNoiseSources);i++)
    {
        if(testData->noiseFileDescs[i] != NULL)
        {
            retVal = TPFile_close(&(testData->noiseFileDescs[i] ));
            testData->noiseFileDescs[i] = NULL;
         }
    }
    if(testData->speechFileDesc != NULL)
    {
        retVal = TPFile_close(&(testData->speechFileDesc));
        testData->speechFileDesc = NULL;
    }

    if((TP_saveOutput== TP_TRUE) && (testData->outputFileDescriptor != NULL))
    {
        retVal = TPFile_close(&(testData->outputFileDescriptor));
        testData->speechFileDesc = NULL;
    }
    return(retVal);
};
