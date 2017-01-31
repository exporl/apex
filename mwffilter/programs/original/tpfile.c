/************************************
 *  Program:    Program for testing noise cancellation in speech
 *
 *  File:       tpfile.c
 *
 *  Component:  TP - file access functions
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
#include "tpfile.h"


 /************************************
 * GLOBAL DEFENITIONS
 ***********************************/
#define     MAX_FILE_PATH_LENGTH        (256)
#define     OUTPUT_FILE_DIR             "data/original/"

#if !FIXME
    #define     NUM_CHANNELS                (2)
#endif

/************************************
 * GLOBAL VARIABLES
 ***********************************/

/**
 * The file information structure is as follows:
 *{   sf_count_t  frames ;
 *    int         samplerate ;
 *    int         channels ;
 *    int         format ;
 *    int         sections ;
 *    int         seekable ;};
 */
//SF_INFO      fileInfo;

/************************************
 * LOCAL FUNCTIONS
 ***********************************/

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/

/************************************
 * TPFile_open()
 ***********************************/

int TPFile_open(const char * filePath, int *framesLoaded, void **fileDesc, int *fileFs, int accessMode, int saveFileFs)
{
    int         retVal = TP_SUCCESS;
    SF_INFO     fileInfo;
    SF_INFO     saveFileInfo;

    if ((filePath == NULL))// || (framesLoaded == NULL))// || (fileDesc == NULL))
    {
        printf("\nTPFile_open(): ERROR input params!");
        retVal = TP_ERROR_PARAMS;
    }
    else
    {
        /* Open the file, get the file information */
        if(accessMode == SFM_READ)
        {
            fileInfo.format = 0;       /* required by libsndfile. see doc for sf_open() */
            *fileDesc = (void*)sf_open(filePath, accessMode, &fileInfo) ; /* cast mask libsndfile specific types in TPFile API*/
            *framesLoaded = (int)fileInfo.frames;
            *fileFs = fileInfo.samplerate;
        }
        else if(accessMode == SFM_WRITE)
        {
            saveFileInfo.channels = 2;
            saveFileInfo.samplerate = saveFileFs;
            saveFileInfo.format = 0x10002 ;
            *fileDesc = (void*)sf_open(filePath, accessMode, &saveFileInfo) ;
            *framesLoaded = 0;
            *fileFs = saveFileFs;
        }

        if (*fileDesc == NULL)
        {
            retVal = TP_FAILURE;
            printf("\nTPFile_open(): ERROR! sf_open(), fileDesc is NULL! File: %s",filePath);
        }
    }

    return(retVal);
}

/************************************
 * TPFile_close()
 ***********************************/


int TPFile_close(void **fileDesc)
{
    int         retVal = TP_SUCCESS;

    if ((fileDesc == NULL))
    {
        printf("\nTPFile_close(): ERROR input params!");
        retVal = TP_ERROR_PARAMS;
    }
    else
    {
        retVal = sf_close((SNDFILE*)(*fileDesc));
        if( retVal == 0)
        {
            retVal = TP_SUCCESS;
        }
        else
        {
            printf("\nTPFile_close(): ERROR closing file!");
            retVal = TP_FAILURE;
        }
    }

    return(retVal);
}

/************************************
* TPFile_readFrame()
***********************************/
int TPFile_readFrame(void **fileDesc, SAMPLETYPE * buffer, int framesRequested, int *framesLoaded)
{
    int         retVal = TP_SUCCESS;

    if ((fileDesc == NULL) || (buffer == NULL) || (framesLoaded == NULL))
    {
        printf("\nTPFile_readFrame(): ERROR input params!");
        retVal = TP_ERROR_PARAMS;
    }
    else
    {
        /* The sndfile library will convert the read data to a desired type on-the-fly */
        if (0 == strcmp(SAMPLETYPE_STRING, "float"))
        {
            *framesLoaded = sf_readf_float(*fileDesc, (float*)buffer, framesRequested);    /* cast eliminates compiler warnings */
        }
        else if(0 == strcmp(SAMPLETYPE_STRING, "double"))
        {
            *framesLoaded = sf_readf_double(*fileDesc, (double*)buffer, framesRequested); /* cast eliminates compiler warnings */
        }
    }

    return(retVal);
}


/************************************
 * TPfile_save() - TBD!!!! - now just relies on previous data!
 ***********************************/


int TPfile_save(void **fileDesc, SAMPLETYPE **buffer, int samplesPerChannel)
{

    int         retVal = TP_SUCCESS;
    int         i = 0;
    int         j = 0;
    sf_count_t  framesSaved = 0;
    SAMPLETYPE  *bufferOut = NULL;

    bufferOut = (SAMPLETYPE*)calloc(samplesPerChannel*2, sizeof(SAMPLETYPE));
    if (bufferOut != NULL)
    {
        /* To properly save the wav file we need to inter-weav the samples for the left [0] and right [1] channel */
        for (i = 0, j = 0; (j < samplesPerChannel); i+=2, j++)
        {
            bufferOut[i] = buffer[0][j];
            bufferOut[i+1] = buffer[1][j];

            if (( bufferOut[i] >= 1) || ( bufferOut[i+1] >= 1))
            {
                //printf("\nCLIPPING!!!");
            }
        }

        if (retVal == TP_SUCCESS)
        {
            if (0 == strcmp(SAMPLETYPE_STRING, "float"))
            {
                framesSaved = sf_writef_float(*fileDesc, (float*)bufferOut, samplesPerChannel);    /* cast eliminates compiler warnings */
                if (framesSaved != samplesPerChannel)
                {
                    //retVal = TP_FAILURE;
                    printf("\nTPfile_save: sf_writef_float returned: %zu", framesSaved);
                }
            }
            else if(0 == strcmp(SAMPLETYPE_STRING, "double"))
            {
                framesSaved = sf_writef_double(*fileDesc, (double*)bufferOut, samplesPerChannel); /* cast eliminates compiler warnings */
                if (framesSaved != samplesPerChannel)
                {
                    //retVal = TP_FAILURE;
                    printf("\nTPfile_save: sf_writef_double returned: %zu", framesSaved);
                }
            }
        }
        free(bufferOut);
    }
    else
    {
        retVal = TP_FAILURE;
        printf("\nTPfile_save: ERROR allocating buffer");
    }


    return(retVal);
}
