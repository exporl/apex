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
/************************************
 * GLOBAL DEFENITIONS
 ***********************************/

/************************************
 * GLOBAL TYPE DEFENITIONS
 ***********************************/

/************************************
 * GLOBAL VARIABLES
 ***********************************/
/** Strings used for user information purposes */
static const char     *TP_config_list_string[]={"MAN_OR_WOMAN",
                                                "LIST_NUM",
                                                "INPUT_SAMPLING_RATE",
                                                "OUTPUT_SAMPLING_RATE",
                                                "MWF_FILTER",
                                                "SPEECH_HRTF",
                                                "MISMATCH",
                                                "NO_MWF",
                                                "SNR_FILE",
                                                "EMP_FILTER",
                                                "DEMP_FILTER",
                                                "SPEECH_DIR",
                                                "SILENCE_BEFORE",
                                                "SILENCE_AFTER",
                                                "FIXED_BF",
                                                "FILTER_SNR_OFFSET",
                                                "NUM_NOISE_SOURCES",
                                                "MONO_STEREO",
                                                "BLOCK_SIZE",
                                                "TOTAL_NUM_MICS",
                                                "MIN_START_SNR",
                                                "NUM_SENT_AVG_SRT",
                                                "OUTPUT_GAIN"
                                                };

/** Indicate resampling */
static char         *resampleFilterFile = "/users/sista/sszczepa/tp/upsample2_filter.bin";


/************************************
 * LOCAL FUNCTIONS
 ***********************************/


/************************************
 * FUNCTION DEFINITIONS
 ***********************************/


/**
 *  @brief Perform parsing of the config file for the program
 *
 *  @param filename poitner to string containing the file name
 *
 *  @retval TP_SUCCESS when parameters are succesfuly read from config file
 *  @retval TP_FAILURE when an error occures when parsing config file
 */

int tpSrtBatch_parseConfigFile(char* filename, struct TPSrtBatch_config_t *testData)
{

    int             i = 0;
    int             j = 0;
    int             size = 0;
    int             retVal = TP_SUCCESS;
    FILE            *fd = NULL;
    char            *buffer1;
    char            *buffer2;
    char            *parsePointer1;
    char            *parsePointer2;

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

            for(i = 0; i < NUMBER_OF_CONFIG_ELEMS;)
            {
                /* Get a whole line of the file */
                fgets(&buffer1[0],MAX_CONFIG_DATA_LENGTH, fd);

                if ((buffer1[0] != '#') && (buffer1[0] != '\n') && (buffer1[0] != ' '))
                {
                    parsePointer1 = strstr(buffer1, "=" );
                    parsePointer1++;
                    parsePointer2 = strstr(buffer1, "\n" );
                    size = parsePointer2 - parsePointer1;
                    /* Clean the configData, as the might be a re-reun */
                    memset(&TPSrtBatch_configData[i][0],0,MAX_CONFIG_DATA_LENGTH*sizeof(char));
                    strncpy(&TPSrtBatch_configData[i][0], parsePointer1, size);
                    if (i == NUM_NOISE_SOURCES)
                    {
                        /* 1. Check how many noise sources.
                         * 2. Allocate an array for the hrtf file names
                         * 3. File names should be following lines in config file - parse them!
                         * 4. Allocate an array for the file names
                         * 5. File names should be following lines in config file - parse them!
                         */
                        testData->numNoiseSources = atoi(&TPSrtBatch_configData[i][0]);

                        testData->noiseHrtfFiles = calloc(testData->numNoiseSources, sizeof(char*));
                        for(j = 0 ; j <testData->numNoiseSources ;j++)
                        {
                            testData->noiseHrtfFiles[j] = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));
                        }

                        for(j = 0 ; j <testData->numNoiseSources ;j++)
                        {
                            fgets(&buffer2[0],MAX_CONFIG_DATA_LENGTH, fd);
                            parsePointer1 = strstr(buffer2, "$" );
                            parsePointer1++;
                            parsePointer2 = strstr(buffer2, "\n" );
                            size = parsePointer2 - parsePointer1;
                            strncpy(&testData->noiseHrtfFiles[j][0], parsePointer1, size);
                        }

                        testData->noiseFiles = calloc(testData->numNoiseSources, sizeof(char*));
                        for(j = 0 ; j <testData->numNoiseSources ;j++)
                        {
                            testData->noiseFiles[j] = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));
                        }

                        for(j = 0 ; j <testData->numNoiseSources ;j++)
                        {
                            fgets(&buffer2[0],MAX_CONFIG_DATA_LENGTH, fd);
                            parsePointer1 = strstr(buffer2, "$" );
                            parsePointer1++;
                            parsePointer2 = strstr(buffer2, "\n" );
                            size = parsePointer2 - parsePointer1;
                            strncpy(&testData->noiseFiles[j][0], parsePointer1, size);
                        }

                    }
                    if (i == BLOCK_SIZE)
                    {
                        testData->frameSize = atoi(&TPSrtBatch_configData[i][0]);
                    }
                    if (i == INPUT_SAMPLING_RATE)
                    {
                        testData->inputSamplingRateHz = atoi(&TPSrtBatch_configData[i][0]);
                    }
                    if (i == OUTPUT_SAMPLING_RATE)
                    {
                        testData->outputSamplingRateHz = atoi(&TPSrtBatch_configData[i][0]);
                    }

                    if (i == MONO_STEREO)
                    {
                        if (0 == strcmp("LEFT", TPSrtBatch_configData[MONO_STEREO]))
                        {
                            testData->monoOrStereo = LEFT;
                        }
                        else if (0 == strcmp("RIGHT", TPSrtBatch_configData[MONO_STEREO]))
                        {
                            testData->monoOrStereo = RIGHT;
                        }
                        else if (0 == strcmp("STEREO", TPSrtBatch_configData[MONO_STEREO]))
                        {
                            testData->monoOrStereo = STEREO;
                        }
                        else    /* actually this is an error! */
                        {
                            testData->monoOrStereo = NO_CHANNEL;
                        }
                    }

                    if (i == SILENCE_BEFORE)
                    {
                        testData->silenceBefore = atoi(&TPSrtBatch_configData[i][0]);
                    }

                    if (i == SILENCE_AFTER)
                    {
                        testData->silenceAfter = atoi(&TPSrtBatch_configData[i][0]);
                    }

                    if (i == NO_MWF)
                    {
                        if (0 == strcmp("TRUE", TPSrtBatch_configData[NO_MWF]))
                        {
                            testData->noMWF = TP_TRUE;
                        }
                        else
                        {
                            testData->noMWF = TP_FALSE;
                        }
                    }

                    if ( i == MISMATCH)
                    {
                        testData->gainMismatch = atoi(&TPSrtBatch_configData[i][0]);
                    }

                    if (i == FILTER_SNR_OFFSET)
                    {
                        testData->filterSNROffset = atoi(&TPSrtBatch_configData[i][0]);
                    }
                    printf("\n%s: \r\t\t\t%s ", TP_config_list_string[i], TPSrtBatch_configData[i]);
                    if (i == NUM_NOISE_SOURCES)
                    {
                        for(j = 0 ; j <testData->numNoiseSources ;j++)
                        {
                            printf("\n\tNoise HRTF %d: \t%s ", j, testData->noiseHrtfFiles[j]);
                        }
                        for(j = 0 ; j <testData->numNoiseSources ;j++)
                        {
                            printf("\n\tNoise files %d: \t%s ", j, testData->noiseFiles[j]);
                        }

                    }
                    if (i == TOTAL_NUM_MICS)
                    {
                        testData->numChannels = atoi(&TPSrtBatch_configData[i][0]);
                    }
                    if (i == MIN_START_SNR)
                    {
                        testData->minStartSNR = atoi(&TPSrtBatch_configData[i][0]);
                    }
                    if (i == NUM_SENT_AVG_SRT)
                    {
                        testData->numSentAvgForSRT = atoi(&TPSrtBatch_configData[i][0]);
                    }
                    if (i == OUTPUT_GAIN)
                    {
                        testData->outputGainLin = atof(&TPSrtBatch_configData[i][0]);
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
 *  @brief Build the list of the speech files used for processing
 *
 *  @param None.
 *
 *  @retval None
 */

void tpSrtBatch_buildSentenceList(struct TPSrtBatch_config_t *testData)

{
    int     i = 0;
    int     j = 0;
    int     retVal = TP_SUCCESS;
    char    numberString[20];

    /* first part of the path is the direcotry */
    for(i = 0; i < NUMBER_SENTENECS_PER_LIST; i++)
    {
        strcpy(testData->speechFile[i], TPSrtBatch_configData[SPEECH_DIR]);
        /* add the missing slash, if needed */
        strcat(testData->speechFile[i], "/");
    }
    /* no we determine the first part of the file name */
    if (0 == strcmp(TPSrtBatch_configData[MAN_OR_WOMAN], "man"))
    {
        for(i = 0; i < NUMBER_SENTENECS_PER_LIST; i++)
        {
            if ((0 == strcmp("NONE", TPSrtBatch_configData[EMP_FILTER])) && (0 != strcmp("NONE", TPSrtBatch_configData[DEMP_FILTER])))
            {
                strcat(testData->speechFile[i], "preempman");
            }
            else
            {
                strcat(testData->speechFile[i], "man");
            }
        }

    }
    else
    {
        for(i = 0; i < NUMBER_SENTENECS_PER_LIST; i++)
        {
            strcat(testData->speechFile[i], "vrouw");
        }
    }

    testData->speechListNum = atoi(TPSrtBatch_configData[LIST_NUM]);

    /* the arrey with file names is numbered 0-12 while the senteces 1-13. This is the reason for +1, -1 in the code below */
    for(i = ((testData->speechListNum - 1)*NUMBER_SENTENECS_PER_LIST) + 1, j = 0; i < (testData->speechListNum * NUMBER_SENTENECS_PER_LIST) + 1; i++, j++)
    {
        if ((0 <= i) && (i <= 9))
        {
            sprintf(numberString, "00%d",i);
        }
        else if ((10 <= i) && (i <= 99))
        {
            sprintf(numberString, "0%d",i);
        }
        else
        {
            sprintf(numberString, "%d",i);
        }
        strcat(testData->speechFile[j], numberString);
        strcat(testData->speechFile[j], ".wav");
    }

}


/**
 *  @brief Prepare all filters used in the processing
 *
 *  @retval TP_SUCCESS When all filters initialized
 *  @retval TP_FAILURE When an error has occured - all filters are needed thus no distinguishing
 */
int tpSrtBatch_prepareFilters(struct TPSrtBatch_config_t *testData)
{
    int             i = 0, j = 0;
    int             retVal = TP_SUCCESS;
    Bool_t          loadData = TP_TRUE;/* in case of hrtf true, but for MWF not - they depend on snr! */

    testData->filtersSpatialNoise = calloc(testData->numNoiseSources, sizeof(MCFIR_Filter*));

    if (testData->filtersSpatialNoise == NULL)
    {
        printf("\ntpSrtBatch_prepareFilters(): ERROR allocating noise hrtf filter array!");
        retVal = TP_FAILURE;
    }
    else
    {
        for (i = 0; (i < testData->numNoiseSources) && (retVal == TP_SUCCESS );i++)
        {
            testData->filtersSpatialNoise[i] = (MCFIR_Filter *)calloc(1, sizeof(MCFIR_Filter));
            if (testData->filtersSpatialNoise[i] == NULL)
            {
                printf("\ntpSrtBatch_prepareFilters(): ERROR allocating noise hrtf filter nr %d",i);
                retVal = TP_FAILURE;
            }
            else
            {
                MC_FIR_Freq_Init(testData->filtersSpatialNoise[i], testData->noiseHrtfFiles[i]);
            }
        }
    }
    testData->filterSpatialSpeech = (MCFIR_Filter *) calloc(1, sizeof(MCFIR_Filter));
    MC_FIR_Freq_Init(testData->filterSpatialSpeech, TPSrtBatch_configData[SPEECH_HRTF]);

    if (retVal == TP_SUCCESS)
    {
        testData->fft_HRTF = (FFTW_state *) calloc(1, sizeof(FFTW_state));
        FFTW_Init(testData->fft_HRTF, 2*(testData->filterSpatialSpeech->length));
    }

    if (0 != strcmp("NONE", TPSrtBatch_configData[FIXED_BF]))
    {
        testData->fixedBeamformer = (MCFIR_Filter *) calloc(1, sizeof(MCFIR_Filter));
        MC_FIR_Freq_Init(testData->fixedBeamformer, TPSrtBatch_configData[FIXED_BF]);
        testData->fft_Fb = (FFTW_state *) calloc(1, sizeof(FFTW_state));
        FFTW_Init(testData->fft_Fb, 2*testData->fixedBeamformer->length);
        printf("\nFB filter loaded!!!");
    }
    else
    {
        testData->fixedBeamformer = NULL;
        printf("\nFB filter is NULL!!!");
    }

    if (0 != strcmp("NONE", TPSrtBatch_configData[EMP_FILTER]))
    {
        testData->filterEmphasis = (MCFIR_Filter *) calloc(1, sizeof(MCFIR_Filter));
        MC_FIR_Freq_Init(testData->filterEmphasis, TPSrtBatch_configData[EMP_FILTER]);
        testData->fft_Emp = (FFTW_state *) calloc(1, sizeof(FFTW_state));
        FFTW_Init(testData->fft_Emp, 2*testData->filterEmphasis->length);
        printf("\nFilter emphasis loaded!!!");
    }
    else
    {
        testData->filterEmphasis = NULL;
        printf("\nFilter emphasis is NULL!!!");
    }

    if (0 != strcmp("NONE", TPSrtBatch_configData[DEMP_FILTER]))
    {
        testData->filterDemphasis = (SCIIR_Filter *) calloc(1, sizeof(SCIIR_Filter));
        TPFilter_prepareIirFilter(testData->filterDemphasis, TPSrtBatch_configData[DEMP_FILTER]);
    }
    else
    {
        testData->filterDemphasis = NULL;
    }

    /* The MWF will only be allocated. As they depend on the SNR level, we will initialize them just before the filtering */
    if (testData->monoOrStereo & LEFT)
    {
        testData->filterNoiseLeft = (MCFIR_Filter *) calloc(1, sizeof(MCFIR_Filter));
    }

    if (testData->monoOrStereo & RIGHT)
    {
        testData->filterNoiseRight = (MCFIR_Filter *) calloc(1, sizeof(MCFIR_Filter));
    }

    if(testData->noMWF == TP_FALSE)
    {
        testData->fft_MWF = (FFTW_state *) calloc(1, sizeof(FFTW_state));
        /* init will be performed when appropriate filter is loaded */
    }

    return(retVal);
}


/**
 *  @brief Ask a question to the user and wait for the answer
 *
 *  @retval TP_SUCCESS When all filters initialized
 *  @retval TP_FAILURE When an error has occured - all filters are needed thus no distinguishing
 */
Bool_t tpSrtBatch_askUser(char * question)
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

/**
 *  @brief Read the initial SNR mixing value for 0db
 *
 *  @param snrValue Palceholder for the SNR value
 *
 *  @retval TP_SUCCESS when noise sources generated
 *  @retval TP_FAILURE when an error occures in processing
 */
int tpSrtBatch_initSNR(SAMPLETYPE *snrValue)
{
    int             retVal = TP_SUCCESS;
    int             i = 0;
    int             numElems, elemLength;
    SAMPLETYPE      **tmp_data = NULL;
    FILE            *fid = NULL;
    char            *path;

    /* we load the file indicated in the config data */
    path = calloc(MAX_CONFIG_DATA_LENGTH, sizeof(char));

    /* first the path */
    strcpy(path, TPSrtBatch_configData[SNR_FILE]);

    /* now open the file */
    fid = fopen(path, "r");

    if (fid != NULL)
    {
        retVal = read_params_from_file(fid, &numElems, &elemLength);
        if(retVal == TP_SUCCESS)
        {
            tmp_data = (SAMPLETYPE **) calloc(1, sizeof(SAMPLETYPE*));
            for (i = 0; i < numElems; i++)
            {
                tmp_data[i] = (SAMPLETYPE *) calloc(1, sizeof(SAMPLETYPE));
            }

            read_data_from_file(fid, tmp_data, numElems, elemLength);

            *snrValue = tmp_data[0][0];
            printf("\nTP_gainSNR0dB = %f",*snrValue);
        }
        else
        {
            printf("\nData not read from file");
        }
        fclose(fid);
    }
    else
    {
        printf("\ninitSNR(): ERROR opening file %s",path);
        retVal = TP_FAILURE;
    }

    if ((tmp_data) != NULL)
    {
        for (i = 0; i < numElems; i++)
        {
            free(tmp_data[i]);
        }

        free(tmp_data);
    }

    free(path);

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

int tpSrtBatch_setNewMWF(int filterSNR, struct TPSrtBatch_config_t *testData)
{

    int     retVal = TP_SUCCESS;
    char    *numberString;

    numberString = calloc(40, sizeof(char));

    strcpy(testData->filterNoiseLeftFile, TPSrtBatch_configData[MWF_FILTER]);
    strcpy(testData->filterNoiseRightFile, TPSrtBatch_configData[MWF_FILTER]);
    /* add the suffix to destinguish the side */
    strcat(testData->filterNoiseLeftFile, "_left_SNR");
    strcat(testData->filterNoiseRightFile, "_right_SNR");

    sprintf(numberString, "%d.bin",filterSNR);
    strcat(testData->filterNoiseLeftFile, numberString);
    strcat(testData->filterNoiseRightFile, numberString);
    printf("\nLODAING FILTER FOR SNR: %d [dB]",filterSNR );

    free(numberString);

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

int tpSrtBatch_clearConfigData(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    if (testData->noiseHrtfFiles != NULL)
    {
        for(i = 0 ; i <testData->numNoiseSources ;i++)
        {
            if (testData->noiseHrtfFiles[i] != NULL)
            {
                free(testData->noiseHrtfFiles[i]);
            }
        }
        free(testData->noiseHrtfFiles);
    }

    if (testData->noiseFiles != NULL)
    {
        for(i = 0 ; i <testData->numNoiseSources ;i++)
        {
            if (testData->noiseFiles[i] != NULL)
            {
                free(testData->noiseFiles[i]);
            }
        }
        free(testData->noiseFiles);
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

int tpSrtBatch_clearFilters(struct TPSrtBatch_config_t *testData)
{
    int     retVal = TP_SUCCESS;
    int     i = 0;

    if (testData->filtersSpatialNoise != NULL)
    {
        for (i = 0; (i < testData->numNoiseSources);i++)
        {
            if (testData->filtersSpatialNoise[i] != NULL)
            {
                MC_FIR_Freq_Free(testData->filtersSpatialNoise[i]);
                free(testData->filtersSpatialNoise[i]);
            }
        }
    }
    if (testData->filterSpatialSpeech != NULL)
    {
        MC_FIR_Freq_Free(testData->filterSpatialSpeech);
        free(testData->filterSpatialSpeech);
    }
    if (testData->fft_HRTF != NULL)
    {
        FFTW_Free(testData->fft_HRTF);
        free(testData->fft_HRTF);
    }
    if (testData->fixedBeamformer != NULL)
    {
        MC_FIR_Freq_Free(testData->fixedBeamformer);
        free(testData->fixedBeamformer);
    }
    if (testData->fft_Fb != NULL)
    {
        FFTW_Free(testData->fft_Fb);
        free(testData->fft_Fb);
    }
    if (testData->filterEmphasis != NULL)
    {
        MC_FIR_Freq_Free(testData->filterEmphasis);
        free(testData->filterEmphasis);
    }
    if (testData->filterNoiseLeft != NULL)
    {
        MC_FIR_Freq_Free(testData->filterNoiseLeft);
        free(testData->filterNoiseLeft);
    }
    if (testData->fft_Emp != NULL)
    {
        FFTW_Free(testData->fft_Emp);
        free(testData->fft_Emp);
    }
    if (testData->filterNoiseRight != NULL)
    {
        MC_FIR_Freq_Free(testData->filterNoiseRight);
        free(testData->filterNoiseRight);
    }
    if (testData->fft_MWF != NULL)
    {
        FFTW_Free(testData->fft_MWF);
        free(testData->fft_MWF);
    }

    if (testData->filterDemphasis != NULL)
    {
        SC_FIR_Freq_Free(testData->filterDemphasis);
        free(testData->filterDemphasis);
    }

    return(retVal);
}
