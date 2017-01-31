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
***********************************/

/************************************
 * INCLUDE FILES
 ***********************************/
#include "tp.h"
#include "tp_noproc.h"

#include "tpfile.h"
#include "tpjack.h"
#include "tp_jackcb.h"

/************************************
 * GLOBAL DEFENITIONS
 ***********************************/
/** The default name of the config file */
#define     SRT_NO_PROC_CONFIG_FILENAME          "data/original/tpnoproc.config"

/** The default name of the config file */
#define     NUM_WAV_FILE_CHANNELS                (2)

/************************************
 * GLOBAL TYPE DEFENITIONS
 ***********************************/
typedef enum {
        NO_PROC_MAN_OR_WOMAN = 0,
        NO_PROC_LIST_NUM,
        NO_PROC_PLAYBACK_FILE,
        NO_PROC_MONO_STEREO,
        NO_PROC_NOISE_SPL,
        NO_PROC_BLOCK_SIZE,
        MIN_START_SNR,
        NUM_SENT_AVG_SRT,
        OUTPUT_GAIN,
        NO_PROC_NUM_CONFIG_ELEMS

} NoProcConfig_list_elems_t;

struct TPNoProc_config_t
    {
        int             outputSamplingRateHz;
        int             totalFramesToProcess;
        int             currentFrame;
        int             frameSize;
        int             currentBlockFrame;
        int             monoOrStereo;
        char            manOrWoman[MAX_CONFIG_DATA_LENGTH];
        char            splFile[MAX_CONFIG_DATA_LENGTH];
        char            speechFile[MAX_CONFIG_DATA_LENGTH];
        int             numSentAvgForSRT;
        int             minStartSNR;
        SAMPLETYPE      *fileBuffer;
        int             speechListNum;
        char            speechFileStem[MAX_CONFIG_DATA_LENGTH];
        void            *fileDesc;
        int             SNRLeveldB;
        void            *outputFileDescriptor;
        SAMPLETYPE      *outputBuffer[NUMBER_CHANNELS_OUTPUT];
        SAMPLETYPE      SRTLevel;
        SAMPLETYPE      outputGainLin;
   };
/************************************
 * GLOBAL VARIABLES
 ***********************************/
#if !FIXME_sort_out_issue_with_includig
 /** The correct sentences are already included */
 extern const char *manCorrectSentence[39][NUMBER_SENTENECS_PER_LIST];
 extern const char *womanCorrectSentence[39][NUMBER_SENTENECS_PER_LIST];
 #endif
/** Structure containing the config data for the test, buffers,filters */
static struct TPNoProc_config_t         TPNoProc_testData;

/** Flag for running the processing driven by JACK callback function for main test */
static volatile Bool_t                  TPNoProc_performProcessing = TP_FALSE;

/** Flag for running the processing driven by JACK callback function for splCalibration */
static volatile Bool_t                  TPNoProc_calibrateSPL = TP_FALSE;

/** Flag for running the processing driven by JACK callback function for sentence presentation */
static volatile Bool_t                  TPNoProc_presentSentence = TP_FALSE;

/** Global counter to keep track of the currently processed sentence */
static int                              TP_testSentenceNumber = 0;

/** Array to store the SNR staircase for the tested list */
static int                              TP_SNR_TEST_LIST[NUMBER_SENTENECS_PER_LIST+1];

/************************************
 * MODULE DEFENITIONS
 ***********************************/
#define     NO_CHANNEL (0)
#define     LEFT    (1)
#define     RIGHT   (2)
#define     STEREO  (LEFT + RIGHT)

/************************************
 * LOCAL FUNCTIONS
 ***********************************/

/* Auxialiary functions used by the previous local fnctions*/
int tpNoProc_parseConfigFile(char* filename,struct TPNoProc_config_t *testData);

Bool_t tpNoProc_askUser(char * question);

/* Two functions should: 1. setup the test enviroment 2. clean test enviroment   */
static int tpNoProc_initTest(struct TPNoProc_config_t *testData);
static int tpNoProc_closeTest(struct TPNoProc_config_t *testData);

/* 2 local functions below are the main processing blocks in any scheme */
int tpNoProc_readFromFile(struct TPNoProc_config_t *testData);

/* Functions which make up the test procedure */
int tpNoProc_setupSoundFilePresentation(struct TPNoProc_config_t *testData);
void setNewSentence(struct TPNoProc_config_t *testData);

int tpNoProc_cleanSoundFilePresentation(struct TPNoProc_config_t *testData);
/************************************
 * FUNCTION DEFINITIONS
 ***********************************/
int TPNoProc_test(void)
{
    int         i = 0;
    int         retVal = TP_SUCCESS;
    Bool_t      splCablirated = TP_FALSE;
    Bool_t      snrCalibrated = TP_FALSE;
    char        *shortString = NULL;
    FILE        *fid;

    printf("\n***************************************");
    printf("\n* WELCOME TO SRT TESTING (NO-PROCESSING):  ");
    printf("\n****************************************");

    /* Perform all initial tasks such as: set sampling frequency, cofign file parsing, filter/buffer alloc etc.*/
    retVal = tpNoProc_initTest(&TPNoProc_testData);

    printf("\n\n\n\nBeginning SRT procedure.\n\n\n\n");
    if (retVal == TP_SUCCESS)
    {
/******************************
 * SPL CALIBRATION
 ******************************/
        if (TP_TRUE == tpNoProc_askUser("Calibrate SPL?"))
        {
            /* First we calibrate the SPL sing dedicated file */
            while((splCablirated == TP_FALSE) && (retVal == TP_SUCCESS))
            {
                /* First indicate what we are doing and then call function to setup presented sound file */
                TPNoProc_calibrateSPL = TP_TRUE;
                retVal = tpNoProc_setupSoundFilePresentation(&TPNoProc_testData);
                TPNoProc_performProcessing = TP_TRUE;
                do
                {
                }while(TPNoProc_performProcessing == TP_TRUE);

                /* First clean up and then turn the flag off! */
                retVal =  tpNoProc_cleanSoundFilePresentation(&TPNoProc_testData);
                TPNoProc_calibrateSPL = TP_FALSE;

                splCablirated = tpNoProc_askUser("Have you finished adjusting the SPL?");
            }
        }
/******************************
 * SNR CALIBRATION
 ******************************/
        /* go then directly to the SNR calibration procedure */
        TPNoProc_testData.SNRLeveldB = TPNoProc_testData.minStartSNR;
        do
        {
            /* We now need to read the first sentence, and calibrate the SNR level */
            /* First indicate what we are doing and then call function to setup presented sound file */
            TPNoProc_presentSentence = TP_TRUE;
            setNewSentence(&TPNoProc_testData);
            retVal = tpNoProc_setupSoundFilePresentation(&TPNoProc_testData);

            printf("\nThe SNR is: %d [dB]",TPNoProc_testData.SNRLeveldB);
            printf("\nThe sentence number %d is:",TP_testSentenceNumber+1);
            printf("\t\"%s\"\n", manCorrectSentence[TPNoProc_testData.speechListNum - 1][TP_testSentenceNumber]);

            TPNoProc_performProcessing = TP_TRUE;
            do
            {
            }while(TPNoProc_performProcessing == TP_TRUE);

            /* First clean up and then turn the flag off! */
            retVal = tpNoProc_cleanSoundFilePresentation(&TPNoProc_testData);
            TPNoProc_presentSentence = TP_FALSE;

            snrCalibrated = tpNoProc_askUser("Was the question correctly recognized?");
            if (snrCalibrated == TP_TRUE)
            {
                TP_SNR_TEST_LIST[TP_testSentenceNumber] = TPNoProc_testData.SNRLeveldB;
                /* The next sentence will be played 2dB lower */
                TPNoProc_testData.SNRLeveldB -= TWO_dB;
                TP_testSentenceNumber++;
            }
            else
            {
                /* Sentence still not recognized: raise the sound level by 2 dB */
                TPNoProc_testData.SNRLeveldB += TWO_dB;
                if (TPNoProc_testData.SNRLeveldB >= 10)
                {
                    printf("\nGoing into saturation with dB10.");
                    TPNoProc_testData.SNRLeveldB = 10;
                }
            }
        }while(snrCalibrated == TP_FALSE);

/******************************
 * SRT LEVEL
 ******************************/

        /* Now we perform the test which consists of a list of sentences */
        if ((retVal == TP_SUCCESS) && (TP_testSentenceNumber == 1))
        {
            /* zero the SRT for the new test */
            TPNoProc_testData.SRTLevel = 0;

            for(; (TP_testSentenceNumber < NUMBER_SENTENECS_PER_LIST) && (retVal == TP_SUCCESS); TP_testSentenceNumber++)
            {
                TPNoProc_presentSentence = TP_TRUE;
                setNewSentence(&TPNoProc_testData);
                retVal = tpNoProc_setupSoundFilePresentation(&TPNoProc_testData);
                printf("\nThe SNR is: %d [dB]",TPNoProc_testData.SNRLeveldB);
                printf("\nThe sentence number %d is:",TP_testSentenceNumber+1);
                printf("\t\"%s\"\n", manCorrectSentence[TPNoProc_testData.speechListNum - 1][TP_testSentenceNumber]);

                if (TP_testSentenceNumber >= NUMBER_SENTENECS_PER_LIST - (TPNoProc_testData.numSentAvgForSRT - 1)) /* - 1 as the last value is the one after the last sentece */
                {
                    TPNoProc_testData.SRTLevel += (SAMPLETYPE)TPNoProc_testData.SNRLeveldB;
                    printf("\nThe SRTLevel is: %8.2f [dB]",TPNoProc_testData.SRTLevel);
                }

                TPNoProc_performProcessing = TP_TRUE;
                do
                {
                }while(TPNoProc_performProcessing == TP_TRUE);

                /* First clean up and then turn the flag off! */
                retVal = tpNoProc_cleanSoundFilePresentation(&TPNoProc_testData);
                TPNoProc_presentSentence = TP_FALSE;

                /* remember the SNR values to reproduce the stair-case in SRT test */
                TP_SNR_TEST_LIST[TP_testSentenceNumber] = TPNoProc_testData.SNRLeveldB;

                snrCalibrated = tpNoProc_askUser("Was the question correctly recognized?");
                if (snrCalibrated == TP_TRUE)
                {
                    /* The next sentence will be played 2dB lower */
                    TPNoProc_testData.SNRLeveldB -= TWO_dB;
                }
                else
                {
                    /* The next sentence will be played 2dB louder */
                    TPNoProc_testData.SNRLeveldB += TWO_dB;
                    if (TPNoProc_testData.SNRLeveldB >= 10)
                    {
                        TPNoProc_testData.SNRLeveldB = 10;
                        printf("\nGoing into saturation with dB10.");
                    }
                }

                if (TP_testSentenceNumber+1 == NUMBER_SENTENECS_PER_LIST)
                {
                    TPNoProc_testData.SRTLevel += (SAMPLETYPE)TPNoProc_testData.SNRLeveldB;
                    /* remember the SNR values to reproduce the stair-case in SRT test */
                    TP_SNR_TEST_LIST[TP_testSentenceNumber+1] = TPNoProc_testData.SNRLeveldB;
                    printf("\nThe SRTLevel is: %8.2f [dB]",TPNoProc_testData.SRTLevel);
                }
            }

            printf("\nThe SRT averaged on last %d sentences was: %8.2f [dB]",TPNoProc_testData.numSentAvgForSRT, TPNoProc_testData.SRTLevel/TPNoProc_testData.numSentAvgForSRT);
            printf("\n\n\nThe staircase scenario was as follows:\n");
            for (i=0; i < NUMBER_SENTENECS_PER_LIST + 1; i++)
            {
                printf("\nsentence %d: %ddB",i+1, TP_SNR_TEST_LIST[i]);
            }
            shortString = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));
            if (shortString == NULL)
            {
                printf("\nTPNoProc_realTime(): ERROR allocating memory!");
            }
            else
            {
                sprintf(shortString, "SRT_staircase_list_num_%d.dat",TPNoProc_testData.speechListNum);
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
        printf("\nTPNoProc_realTime(): ERROR initializing test... Aborting");
    }
    /* Clean test enviroment regardles of previous results*/
    retVal = tpNoProc_closeTest(&TPNoProc_testData);
    if (retVal != TP_SUCCESS)
    {
        printf("\nTPNoProc_realTime(): ERROR cleaning test enviroment)");
    }

    return(retVal);
}

/************************************
 * TPSRTBatch_processDataBlock()
 ***********************************/

int TPNoProc_processDataBlock(SAMPLETYPE *bufferLeft, SAMPLETYPE *bufferRight, int numFramesReguested)
{
    int         retVal = TP_SUCCESS;
    int         i = 0;

    if (TPNoProc_performProcessing == TP_TRUE)
    {
        if (TPNoProc_testData.currentBlockFrame == 0)
        {
            tpNoProc_readFromFile(&TPNoProc_testData);
        }

        for(i = 0; i < numFramesReguested; i++)
        {
            TPNoProc_testData.outputBuffer[STEREO_LEFT][TPNoProc_testData.currentBlockFrame + i] = TPNoProc_testData.outputGainLin*TPNoProc_testData.outputBuffer[STEREO_LEFT][TPNoProc_testData.currentBlockFrame + i];

            TPNoProc_testData.outputBuffer[STEREO_RIGHT][TPNoProc_testData.currentBlockFrame + i] = TPNoProc_testData.outputGainLin*TPNoProc_testData.outputBuffer[STEREO_RIGHT][TPNoProc_testData.currentBlockFrame + i];
        }

        memcpy(bufferLeft,
                (TPNoProc_testData.outputBuffer[STEREO_LEFT] + TPNoProc_testData.currentBlockFrame),
                numFramesReguested*sizeof(SAMPLETYPE));

        memcpy(bufferRight,
                (TPNoProc_testData.outputBuffer[STEREO_RIGHT] + TPNoProc_testData.currentBlockFrame),
                numFramesReguested*sizeof(SAMPLETYPE));

        TPNoProc_testData.currentBlockFrame += numFramesReguested;
        if(TPNoProc_testData.currentBlockFrame >= TPNoProc_testData.frameSize)
        {
            TPNoProc_testData.currentFrame += TPNoProc_testData.frameSize;
            TPNoProc_testData.currentBlockFrame = 0;
        }

        if (TPNoProc_testData.currentFrame >= TPNoProc_testData.totalFramesToProcess)
        {
            TPNoProc_performProcessing = TP_FALSE;
        }

    }
    else
    {
        memset(bufferLeft,0, TPNoProc_testData.frameSize*sizeof(SAMPLETYPE));
        memset(bufferRight,0, TPNoProc_testData.frameSize*sizeof(SAMPLETYPE));
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

int tpNoProc_initTest(struct TPNoProc_config_t * testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    int     jackSamplingRate = 0;

    /* Parse the input config file - hardcoded file name */
    retVal = tpNoProc_parseConfigFile(SRT_NO_PROC_CONFIG_FILENAME, testData);
    if (retVal != TP_SUCCESS)
    {
        printf("\ntpNoProc_initTest(): ERROR in tpNoProc_parseConfigFile()");
        retVal = TP_FAILURE;
    }
    else
    {
        testData->currentFrame = 0;
        testData->currentBlockFrame = 0;

        for(i = 0; (i < NUMBER_CHANNELS_OUTPUT) && (retVal == TP_SUCCESS); i++)
        {
            testData->outputBuffer[i] = (SAMPLETYPE *)calloc(testData->frameSize, sizeof(SAMPLETYPE));
            if (testData->outputBuffer[i] == NULL)
            {
                printf("\ntpNoProc_initTest(): ERROR allocating outputBuffer[%d]",i);
                retVal = TP_FAILURE;
            }
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

    testData->fileBuffer = (SAMPLETYPE *)calloc(testData->frameSize*NUM_WAV_FILE_CHANNELS, sizeof(SAMPLETYPE));
    if (testData->fileBuffer == NULL)
    {
        printf("\ntpNoProc_initTest(): ERROR allocating fileBuffer[%d]",i);
        retVal = TP_FAILURE;
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

int tpNoProc_closeTest(struct TPNoProc_config_t * testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    /* Begin with shutting down jack */
    retVal = TPAudio_stop();
    if (retVal != TP_SUCCESS)
    {
        printf("\nTPNoProc_realTime(): ERROR shutting down JACK");
    }

    if (testData->outputBuffer != NULL)
    {
        for(i = 0; i < NUMBER_CHANNELS_OUTPUT; i++)
        {
            if (testData->outputBuffer[i] != NULL)
            {
                free(testData->outputBuffer[i]);
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

int tpNoProc_readFromFile(struct TPNoProc_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;
    int     framesLoaded = 0;


    retVal = TPFile_readFrame(&(testData->fileDesc),
                                testData->fileBuffer,
                                testData->frameSize,
                                &framesLoaded);

    if (framesLoaded <= testData->frameSize)
    {
        memset((testData->fileBuffer+NUM_WAV_FILE_CHANNELS*framesLoaded),
                0,
                NUM_WAV_FILE_CHANNELS*sizeof(SAMPLETYPE)*(testData->frameSize - framesLoaded));
    }

    if (testData->monoOrStereo & LEFT)
    {
        for(i = 0; i < testData->frameSize; i++)
        {
            testData->outputBuffer[STEREO_LEFT][i] = testData->fileBuffer[(NUM_WAV_FILE_CHANNELS*i)];
        }
    }
    else
    {
        memset(testData->outputBuffer[STEREO_LEFT],0,testData->frameSize*sizeof(SAMPLETYPE));
    }
    if(testData->monoOrStereo & RIGHT)
    {
        for(i = 0; i < testData->frameSize; i++)
        {
            testData->outputBuffer[STEREO_RIGHT][i] = testData->fileBuffer[(NUM_WAV_FILE_CHANNELS*i)+1];
        }
    }
    else
    {
        memset(testData->outputBuffer[STEREO_RIGHT],0,testData->frameSize*sizeof(SAMPLETYPE));
    }

    return(retVal);
}

/**
 *  @brief Clean up data after sentence presentation
 *
 *  @param testData Strucutre with the test enviroment
 *
 *  @retval TP_SUCCESS on success
 *  @retval TP_FAILURE when an error occures
 */
int tpNoProc_setupSoundFilePresentation(struct TPNoProc_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     fileFs = 0;
    char    *file;

    if (TPNoProc_calibrateSPL == TP_TRUE)
    {
        file = testData->splFile;
    }
    else if (TPNoProc_presentSentence == TP_TRUE)
    {
        file = testData->speechFile;
        printf("\nLoading file: %s",file);
    }
    else
    {
        file = NULL;
    }

    if (file != NULL)
    {
        retVal = TPFile_open(file,
                             &testData->totalFramesToProcess,
                             &(testData->fileDesc),
                             &fileFs,
                             ACCESS_READ,
                             0/*saveFs - ignored for ACCESS_READ*/);
        if (retVal != TP_SUCCESS)
        {
            printf("\ntpNoProc_setupSoundFilePresentation: ERROR opening file: %s",file);
        }
    }

    /* zero the processing frame counter */
    testData->currentFrame = 0;

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
int tpNoProc_cleanSoundFilePresentation(struct TPNoProc_config_t *testData)
{
    int     retVal = TP_SUCCESS;

    if(testData->fileDesc != NULL)
    {
        retVal = TPFile_close(&(testData->fileDesc));
        testData->fileDesc = NULL;
    }

    return(retVal);
}


/**
 *  @brief Perform parsing of the config file for the program
 *
 *  @param filename poitner to string containing the file name
 *
 *  @retval TP_SUCCESS when parameters are succesfuly read from config file
 *  @retval TP_FAILURE when an error occures when parsing config file
 */

int tpNoProc_parseConfigFile(char* filename, struct TPNoProc_config_t *testData)
{

    int             i = 0;
    int             size = 0;
    FILE            *fd = NULL;
    char            *startPtr;
    char            *endPtr;
    char            *buffer1;
    char            *buffer2;
    int             retVal = TP_SUCCESS;

    buffer1 = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));
    buffer2 = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));

    if(filename == NULL)
    {
        printf("\nPlease specify a config file, Exiting...\n");
        retVal = TP_FAILURE;
    }
    else /* we have a path - let us try it */
    {
        fd = fopen( filename, "r");

        if (fd == NULL)
        {
            printf("\nERROR opening config file. Exiting...\n");
            retVal = TP_FAILURE;
        }
        else
        {
            printf("\n Retrieving config data... \n");

            for(i = 0; i < NO_PROC_NUM_CONFIG_ELEMS;)
            {
                /* Get a whole line of the file */
                if (fgets(&buffer1[0],MAX_CONFIG_DATA_LENGTH, fd) == NULL)
                    break;

                if ((buffer1[0] != '#') && (buffer1[0] != '\n') && (buffer1[0] != ' '))
                {
                    startPtr = strstr(buffer1, "=" );
                    startPtr++;
                    endPtr = strstr(buffer1, "\n" );
                    size = endPtr - startPtr;
                    memset(buffer2, 0, MAX_CONFIG_DATA_LENGTH*sizeof(char));
                    strncpy(buffer2, startPtr, size);

                    if (i == NO_PROC_BLOCK_SIZE)
                    {
                        testData->frameSize = atoi(buffer2);
                        printf("\nBLOCK_SIZE:\t\t%d",testData->frameSize);
                    }
                    if (i == NO_PROC_NOISE_SPL)
                    {
                        strncpy(&TPNoProc_testData.splFile[0], startPtr, size);
                        printf("\nNOISE_SPL:\t\t%s",&TPNoProc_testData.splFile[0]);
                    }
                    if (i == NO_PROC_MAN_OR_WOMAN)
                    {
                        strncpy(&TPNoProc_testData.manOrWoman[0], startPtr, size);
                        printf("\nMAN_OR_WOMAN:\t\t%s",&TPNoProc_testData.manOrWoman[0]);
                    }
                    if (i == NO_PROC_PLAYBACK_FILE)
                    {
                        strncpy(&TPNoProc_testData.speechFileStem[0], startPtr, size);
                        printf("\nPLAYBACK_FILE:\t\t%s",&TPNoProc_testData.speechFileStem[0]);
                    }
                    if (i == NO_PROC_LIST_NUM)
                    {
                        TPNoProc_testData.speechListNum = atoi(buffer2);
                        printf("\nLIST_NUM:\t\t%d",TPNoProc_testData.speechListNum);
                    }
                    if (i == NO_PROC_MONO_STEREO)
                    {
                        if (0 == strcmp("LEFT", buffer2))
                        {
                            testData->monoOrStereo = LEFT;
                        }
                        else if (0 == strcmp("RIGHT", buffer2))
                        {
                            testData->monoOrStereo = RIGHT;
                        }
                        else if (0 == strcmp("STEREO", buffer2))
                        {
                            testData->monoOrStereo = STEREO;
                        }
                        else    /* actually this is an error! */
                        {
                            testData->monoOrStereo = NO_CHANNEL;
                        }
                        printf("\nMONO_STEREO:\t\t%s",buffer2);
                        printf("\nmonoOrStereo:\t\t%d",testData->monoOrStereo);
                    }

                    if (i == MIN_START_SNR)
                    {
                        testData->minStartSNR = atoi(buffer2);
                        printf("\nMIN_START_SNR:\t\t%d",TPNoProc_testData.minStartSNR);
                    }
                    if (i == NUM_SENT_AVG_SRT)
                    {
                        testData->numSentAvgForSRT = atoi(buffer2);
                        printf("\nNUM_SENT_AVG_SRT:\t%d",TPNoProc_testData.numSentAvgForSRT);
                    }
                    if (i == OUTPUT_GAIN)
                    {
                        testData->outputGainLin = atof(buffer2);
                        printf("\nOUTPUT_GAIN:\t\t%f",TPNoProc_testData.outputGainLin);
                    }
                    i++;
                }/*else: # indicates a comment, do not parse this, ignore empty lines and ones with withespace*/
            }

            fclose(fd);
        }
    }

    free(buffer1);
    free(buffer2);

    return(retVal);

}

/**
 *  @brief Ask a question to the user and wait for the answer
 *
 *  @retval TP_SUCCESS When all filters initialized
 *  @retval TP_FAILURE When an error has occured - all filters are needed thus no distinguishing
 */
Bool_t tpNoProc_askUser(char * question)
{
    Bool_t    retVal = TP_FALSE;
    char    read_char;

    if (question != NULL)
    {
        /* Present question */
        printf("\n%s<y/n>\n", question);
        do
        {
            read_char = getchar();
            if ((read_char == 'y') || (read_char == 'Y'))
            {
                retVal = TP_TRUE;
            }
            else
            {
                retVal = TP_FALSE;
            }
        }
        while ((read_char != 'n') && (read_char != 'N') && (read_char != 'Y') && (read_char != 'y'));
    }
    return(retVal);
 }

void setNewSentence(struct TPNoProc_config_t *testData)

{
    char    numberString[20];

    strcpy(&(testData->speechFile[0]), testData->speechFileStem);

    sprintf(numberString, "_%d",testData->speechListNum);
    strcat(&(testData->speechFile[0]), numberString);

    sprintf(numberString, "_%d",(TP_testSentenceNumber+1)); /* as counters in real life start from 1... what is 0 anyway???? */
    strcat(&(testData->speechFile[0]), numberString);

    sprintf(numberString, "_SNR_%d",testData->SNRLeveldB);
    strcat(&(testData->speechFile[0]), numberString);

    strcat(&(testData->speechFile[0]), ".wav");
}
