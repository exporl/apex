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
#include "tp_test.h"

#include "tpfile.h"
#include "tpjack.h"
#include "tp_jackcb.h"

/************************************
 * GLOBAL DEFENITIONS
 ***********************************/
#define     NO_CHANNEL (0)
#define     LEFT    (1)
#define     RIGHT   (2)
#define     STEREO  (LEFT + RIGHT)
#define     TESTING_CONFIG_FILENAME "data/original/realtime.config"

#if !FIXME_REMOVE

    /** Defines stereo right channel number in channel array*/
    #define     STEREO_RIGHT                (1)

    /** Defines stereo left channel number in channel array*/
    #define     STEREO_LEFT                 (0)
/** The number of channels is hardcoced */
#define     NUMBER_CHANNELS             (2)

#endif

/************************************
 * GLOBAL TYPE DEFENITIONS
 ***********************************/
typedef enum {
        MONO_STEREO = 0,
        BLOCK_SIZE,
        SOUND_FILE,
        NUMBER_OF_CONFIG_ELEMS
} TPConfig_list_elems_t;

struct TPTest_config_t
    {
        int             samplingRateHz;
        int             framesToProcess;
        int             currentFrame;
        int             frameSize;
        int             numInputFiles;
        void            *inputFileDescriptor1;
        void            *outputFileDescriptor;
        SAMPLETYPE      *inputStageBuffer;
        SAMPLETYPE      *outputStageBuffer[NUMBER_CHANNELS];

   };

/************************************
 * GLOBAL VARIABLES
 ***********************************/
/** Structure containing the config data for the test, buffers,filters */
static struct TPTest_config_t       TPTest_testData;

/** Flag for running the processing driven by JACK callback function */
static volatile Bool_t              TPTest_testStarted = TP_FALSE;

/** Array used for storing parsed config data */
static char                         TPTest_configData[NUMBER_OF_CONFIG_ELEMS][MAX_CONFIG_DATA_LENGTH];

/** Array used for storing name of the input file*/
static char                         TPTest_soundFile[MAX_CONFIG_DATA_LENGTH];

/** Array used for storing name of the output file*/
static char                         *TPTest_outputFile = "ouputFile.wav";

/** Array used for storing names of ouptu files */
static char                         TP_ouput_file[MAX_CONFIG_DATA_LENGTH];

/** Process block size */
static int                          TPTest_blockSize = 0;

/** To differ between monaural/binaural processing - used as a bit mask!!! */
static int                          TP_stereo = NO_CHANNEL;

/** To differ between monaural/binaural processing - used as a bit mask!!! */
static Bool_t                       TP_saveOutput = TP_TRUE;

/** Strings used for user information purposes */
const char                          *TP_config_list_string[]={"MONO_STEREO",
                                                              "BLOCK_SIZE",
                                                              "SOUND_FILE"
                                                             };

/************************************
 * LOCAL FUNCTIONS
 ***********************************/
/* Two functions should: 1. setup the test enviroment 2. clean test enviroment   */
static int test_initTest(struct TPTest_config_t *testData, int samplingRateHz);
static int test_closeTest(struct TPTest_config_t *testData);

/* 2 local functions below are the main processing blocks in any scheme */
int test_generateInput(struct TPTest_config_t *testData);
int test_performProcessing(struct TPTest_config_t *testData);

/* Auxialiary functions used by the previous local fnctions*/
static int test_parseConfigFile(char* filename);

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/
int TPTest_realTime(void)
{
    int         i = 0;
    int         retVal = TP_SUCCESS;
    int         samplingRateHz = 0;

    printf("\n******************************************************************************");
    printf("\nWELCOME TO RT TEST: Please specify a sampling frequency for test input files: ");
    scanf("%d",&samplingRateHz);
    printf("******************************************************************************");

    /* Perform all initial tasks such as: set sampling frequency, cofign file parsing, filter/buffer alloc etc.*/
    retVal = test_initTest(&TPTest_testData, samplingRateHz);

    printf("\n\n\n\nPlaying noise file to test real-time operations.\n\n\n\n");

    if (retVal == TP_SUCCESS)
    {
        /* trigger testing with global variable */
        TPTest_testStarted = TP_TRUE;
        do
        {
        }while(TPTest_testStarted == TP_TRUE);

        /* Clean test enviroment regardles of previous results, shutdown JACK */
        retVal = TPAudio_stop();
        if (retVal != TP_SUCCESS)
        {
            printf("\nTPTest_realTime(): ERROR shutting down JACK");
        }
        retVal = test_closeTest(&TPTest_testData);
        if (retVal != TP_SUCCESS)
        {
            printf("\nTPTest_realTime(): ERROR cleaning test enviroment)");
        }
    }
    else
    {
        printf("\nTPTest_realTime(): ERROR initializing test... Aborting");
    }

    return(retVal);
}


int TPTest_processDataBlock(SAMPLETYPE *bufferLeft, SAMPLETYPE *bufferRight)
{
    int         retVal = TP_SUCCESS;

    if (TPTest_testStarted == TP_TRUE)
    {
        test_generateInput(&TPTest_testData);

        test_performProcessing(&TPTest_testData);

        memcpy(bufferLeft,
               TPTest_testData.outputStageBuffer[STEREO_LEFT],
               TPTest_testData.frameSize*sizeof(SAMPLETYPE));

        memcpy(bufferRight,
               TPTest_testData.outputStageBuffer[STEREO_RIGHT],
               TPTest_testData.frameSize*sizeof(SAMPLETYPE));

//        printf("\n TPTest_testData.currentFrame: %d",TPTest_testData.currentFrame);

        if (TPTest_testData.currentFrame >= TPTest_testData.framesToProcess)
        {
            TPTest_testStarted = TP_FALSE;
        }

    }
    else
    {
        memset(bufferLeft,0, TPTest_testData.frameSize*sizeof(SAMPLETYPE));
        memset(bufferRight,0, TPTest_testData.frameSize*sizeof(SAMPLETYPE));
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
*  @param samplingRateHz value for starting JACK engine in [Hz]
 *
 *  @retval TP_SUCCESS when all parameters are set for test execution
 *  @retval TP_FAILURE when an error occures and test cannot proceed
 */

int test_initTest(struct TPTest_config_t * testData, int samplingRateHz)
{
    int     retVal = TP_SUCCESS;
    int     framesLoaded = 0;
    int     fileFs = 0;
    int     i = 0;
    int     jackSamplingRate = 0;
    int     dummyFrames = 0;

    /* Parse the input config file - hardcoded file name */
    retVal = test_parseConfigFile(TESTING_CONFIG_FILENAME);
    if (retVal != TP_SUCCESS)
    {
        printf("\ntest_initTest(): ERROR in test_parseConfigFile()");
        retVal = TP_FAILURE;
    }
    if (retVal == TP_SUCCESS)
    {
        retVal = TPAudio_start(&jackSamplingRate, TPTest_blockSize);
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

    /* put the sampling rate in the structure, processing block size from config file and currentFrame counter to 0*/
    testData->samplingRateHz = samplingRateHz;
    testData->frameSize = TPTest_blockSize;
    testData->currentFrame = 0;

    if (retVal == TP_SUCCESS)
    {
        testData->inputStageBuffer = (SAMPLETYPE *)calloc(TPTest_blockSize*NUMBER_CHANNELS, sizeof(SAMPLETYPE));
        if (testData->inputStageBuffer == NULL)
        {
            printf("\ntest_initTest(): ERROR allocating inputStageBuffer");
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        for(i = 0; (i < NUMBER_CHANNELS) && (retVal == TP_SUCCESS); i++)
        {
            testData->outputStageBuffer[i] = (SAMPLETYPE *)calloc(TPTest_blockSize, sizeof(SAMPLETYPE));
            if (testData->outputStageBuffer[i] == NULL)
            {
                printf("\ntest_initTest(): ERROR allocating outputStageBuffer[%d]",i);
                retVal = TP_FAILURE;
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* Open input files */
        retVal = TPFile_open(TPTest_soundFile,
                             &testData->framesToProcess,
                             &(testData->inputFileDescriptor1),
                             &fileFs,
                             ACCESS_READ,
                             0/*saveFs - ignored for ACCESS_READ*/);
    }

    if ((retVal == TP_SUCCESS) && (TP_saveOutput== TP_TRUE))
    {
        /* Open output files */
        retVal = TPFile_open(TPTest_outputFile,
                             &dummyFrames, /* will be set to zero for ACCESS_WRITE */
                             &(testData->outputFileDescriptor),
                             &fileFs,
                             ACCESS_WRITE,
                             16000/*saveFs*/);

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

int test_closeTest(struct TPTest_config_t * testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    /* Close input files */
    retVal = TPFile_close(&(testData->inputFileDescriptor1));

    if(TP_saveOutput== TP_TRUE)
    {
        retVal = TPFile_close(&(testData->outputFileDescriptor));
    }

    if (testData->inputStageBuffer != NULL)
    {
        free(testData->inputStageBuffer);
    }
    for(i = 0; i < NUMBER_CHANNELS; i++)
    {
        if (testData->outputStageBuffer[i] != NULL)
        {
            free(testData->outputStageBuffer[i]);
        }
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

int test_parseConfigFile(char* filename)
{

    int             i = 0;
    int             size = 0;
    int             retVal = TP_SUCCESS;
    FILE            *fd = NULL;
    char            buffer[MAX_CONFIG_DATA_LENGTH];
    char            *parsePointer1;
    char            *parsePointer2;

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

            for(i = 0; i < NUMBER_OF_CONFIG_ELEMS;)
            {
                /* Get a whole line of the file */
                fgets(&buffer[0],MAX_CONFIG_DATA_LENGTH, fd);

                if ((buffer[0] != '#') && (buffer[0] != '\n') && (buffer[0] != ' '))
                {
                    parsePointer1 = strstr(buffer, "=" );
                    parsePointer1++;
                    parsePointer2 = strstr(buffer, "\n" );
                    size = parsePointer2 - parsePointer1;
                    strncpy(&TPTest_configData[i][0], parsePointer1, size);
                    if (i == BLOCK_SIZE)
                    {
                        TPTest_blockSize = atoi(&TPTest_configData[i][0]);
                    }

                    if (i == SOUND_FILE)
                    {
                        strcpy(TPTest_soundFile, TPTest_configData[SOUND_FILE]);
                    }

                    if (i == MONO_STEREO)
                    {
                        if (0 == strcmp("LEFT", TPTest_configData[MONO_STEREO]))
                        {
                            TP_stereo = LEFT;
                        }
                        else if (0 == strcmp("RIGHT", TPTest_configData[MONO_STEREO]))
                        {
                            TP_stereo = RIGHT;
                        }
                        else if (0 == strcmp("STEREO", TPTest_configData[MONO_STEREO]))
                        {
                            TP_stereo = STEREO;
                        }
                        else    /* actually this is an error! */
                        {
                            TP_stereo = NO_CHANNEL;
                        }
                    }

                    printf("\n%s: \r\t\t\t%s ", TP_config_list_string[i], TPTest_configData[i]);
                    i++;
                }/*else: # indicates a comment, do not parse this, ignore empty lines and ones with withespace*/
            }

            fclose(fd);
        }
    }

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

int test_generateInput(struct TPTest_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     framesLoaded = 0;

    retVal = TPFile_readFrame(&(testData->inputFileDescriptor1),
                              testData->inputStageBuffer,
                              testData->frameSize,
                              &framesLoaded);
    if (framesLoaded <= testData->frameSize)
    {
        memset((testData->inputStageBuffer+NUMBER_CHANNELS*framesLoaded),
                0,
                NUMBER_CHANNELS*sizeof(SAMPLETYPE)*(testData->frameSize - framesLoaded));
    }
    TPTest_testData.currentFrame += framesLoaded;

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
int test_performProcessing(struct TPTest_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    if (TP_stereo & LEFT)
    {
        for(i = 0; i < testData->frameSize; i++)
        {
            testData->outputStageBuffer[STEREO_LEFT][i] = testData->inputStageBuffer[(NUMBER_CHANNELS*i)];
        }
    }
    else
    {
        memset(testData->outputStageBuffer[STEREO_LEFT],0,testData->frameSize*sizeof(SAMPLETYPE));
    }
    if(TP_stereo & RIGHT)
    {
        for(i = 0; i < testData->frameSize; i++)
        {
            testData->outputStageBuffer[STEREO_RIGHT][i] = testData->inputStageBuffer[(NUMBER_CHANNELS*i)+1];
        }
    }
    else
    {
        memset(testData->outputStageBuffer[STEREO_RIGHT],0,testData->frameSize*sizeof(SAMPLETYPE));
    }

    if(TP_saveOutput== TP_TRUE)
    {
        retVal = TPfile_save(&(testData->outputFileDescriptor), testData->outputStageBuffer, testData->frameSize);
    }
    return(retVal);
}
