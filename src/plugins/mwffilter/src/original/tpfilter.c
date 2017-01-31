/************************************
 *  Program:    Program for testing noise cancellation in speech
 *
 *  File:       tpfilter.c
 *
 *  Component:  TP - file with the filters used
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
#include "tpfilter.h"

 /************************************
 * GLOBAL DEFENITIONS & TYPES
 ***********************************/

/************************************
 * GLOBAL VARIABLES
 ***********************************/

/** The HRTF filter taps are stored globally */
static int              filter_number_channels;
static int              filter_length;
static Bool_t           MemoryAllocTapValues = TP_FALSE;
static Bool_t           MemoryAllocOutBuffer = TP_FALSE;

/************************************
 * LOCAL FUNCTIONS
 ***********************************/
int read_params_from_file(FILE *fid, int *NrChannels, int *DataLength) ;
int read_data_from_file(FILE *fid, SAMPLETYPE **tapsData, int nrChannels, int dataLength);
void fir_filter(SAMPLETYPE *in, SAMPLETYPE *out, int length, SCFIR_Filter *Filter);
int iir_filter(SAMPLETYPE * in, SAMPLETYPE * out, SCIIR_Filter *filter, int samplesToProcess);

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/

/************************************
 * TPFilter_noise()
 ***********************************/

int TPFilter_MIMOFir(SAMPLETYPE **bufferIn, SAMPLETYPE **bufferOut, MCFIR_Filter * mcFilter, int bufferLength)
{
    int             retVal = TP_SUCCESS;
    int             i = 0;
    SCFIR_Filter    *scFilter;

    if ((bufferIn == NULL) || (bufferOut == NULL))
    {
        retVal = TP_ERROR_PARAMS;
        printf("\nTPFilter_noise: ERROR! Input params");
    }

    if (retVal == TP_SUCCESS)
    {
        scFilter = (SCFIR_Filter *) calloc(1, sizeof(SCFIR_Filter));
        scFilter->length = mcFilter->length;

        /* the basic filtering operation is performed for 1 input 1 output filters: here we perpare data for this and filter */
        for (i = 0; i < mcFilter->numChannels; i++)
        {
            scFilter->filterTaps = mcFilter->filterTaps[i];
            scFilter->dlyLine = mcFilter->dlyLine[i];

            /* Straightforward filtering through the single channel filter */
            fir_filter(bufferIn[i], bufferOut[i], bufferLength, scFilter);
        }
    }

    free(scFilter);

    return(retVal);
}

/************************************
 * TPFilter_SIMOFir()
 ***********************************/

int TPFilter_SIMOFir(SAMPLETYPE * bufferIn, SAMPLETYPE **bufferOut, MCFIR_Filter * filter, int bufferLength)
{
    int             retVal = TP_SUCCESS;
    int             i = 0;
    SAMPLETYPE      **bufferSignalSplit = NULL;
    SCFIR_Filter    singleChannelfilter;

    if ((bufferIn == NULL) || (bufferOut == NULL))
    {
        retVal = TP_ERROR_PARAMS;
        printf("\nTPFilter_SIMOFir: ERROR! Input params");
    }

    /* split the single input channel into filter->numChannels, to input into the multichannel filter structure
     *  FIRST - allocate the data
     */
    if (retVal == TP_SUCCESS)
    {
        bufferSignalSplit = (SAMPLETYPE **) calloc(filter->numChannels, sizeof(SAMPLETYPE*));
        if (bufferSignalSplit == NULL)
        {
            printf("\nTPFilter_SIMOFir(): ERROR! Filter multichannel array allocation failed");
            retVal = TP_FAILURE;
        }
        else
        {
            for (i = 0; (i < filter->numChannels) && (retVal == TP_SUCCESS); i++)
            {
                bufferSignalSplit[i] = (SAMPLETYPE *) calloc(bufferLength, sizeof(SAMPLETYPE));
                if (bufferSignalSplit[i] == NULL)
                {
                    printf("\nTPFilter_SIMOFir(): ERROR! Filter multichannel array allocation failed");
                    retVal = TP_FAILURE;
                }
            }
        }

    }

    /* Now th true signal split - copy single channel to all channels */
    if (retVal == TP_SUCCESS)
    {
        for (i = 0; (i < filter->numChannels) && (retVal == TP_SUCCESS); i++)
        {
            memcpy(bufferSignalSplit[i], bufferIn, (bufferLength *sizeof(SAMPLETYPE)));
        }
    }

    if (retVal == TP_SUCCESS)
    {
        retVal = TPFilter_MIMOFir(bufferSignalSplit, bufferOut, filter, bufferLength);
    }

    /* IMPORTANT: free the memory */
    if (bufferSignalSplit != NULL)
    {
        for (i = 0; (i < filter->numChannels); i++)
        {
            if (bufferSignalSplit[i] != NULL)
            {
                free(bufferSignalSplit[i]);
            }
        }

        free(bufferSignalSplit);
    }

    return(retVal);
}
/************************************
 * TPFilter_prepareFirFilter()
 ***********************************/
MCFIR_Filter * TPFilter_prepareFirFilter(const char * path, Bool_t loadData)
{

    FILE            *fd = NULL;
    int             retVal = TP_SUCCESS;
    Bool_t            fileOpen = TP_FALSE;
    MCFIR_Filter       *filter;
    const char      *mode = "rb";        /* only read mode for the filters */
    int             i = 0;

    filter = (MCFIR_Filter *)calloc(1, sizeof(MCFIR_Filter));


    if (filter == NULL)
    {
        printf("\nTPFilter_perpareFilter(): ERROR! Filter strucutre allocation failed!");
        retVal = TP_FAILURE;
    }

    /* Open the file with the filter data */
    if (retVal == TP_SUCCESS)
    {
        fd = fopen(path, mode);
        if (fd == NULL)
        {
            printf("\nTPFilter_perpareFilter: ERROR opening filter file");
            retVal = TP_FAILURE;
        }
        else
        {
            fileOpen = TP_TRUE;
        }
    }

    /* read the filter data: nuber of channels and filter length */
    if (retVal == TP_SUCCESS)
    {
        retVal = read_params_from_file(fd, &(filter->numChannels), &(filter->length));
        if (retVal != TP_SUCCESS)
        {
            printf("\nTPFilter_perpareFilter(): ERROR! Filter params not read");
        }
    }

    /* Allocate the array of pointers to filter tap data. One delay line per channel. calloc() zeros the memory allocated*/
    if (retVal == TP_SUCCESS)
    {
        filter->filterTaps = (SAMPLETYPE **) calloc(filter->numChannels, sizeof(SAMPLETYPE*));
        if (filter->filterTaps == NULL)
        {
            printf("\nTPFilter_perpareFilter(): ERROR! Filter taps array allocation failed");
            retVal = TP_FAILURE;
        }
        else
        {
            for (i = 0; (i < filter->numChannels) && (retVal == TP_SUCCESS); i++)
            {
                filter->filterTaps[i] = (SAMPLETYPE *) calloc(filter->length, sizeof(SAMPLETYPE));
                if (filter->filterTaps[i] == NULL)
                {
                    printf("\nTPFilter_perpareFilter(): ERROR! Filter taps array allocation failed");
                    retVal = TP_FAILURE;
                }
            }
        }
    }

    /* read the filter coefficients */
    if (retVal == TP_SUCCESS)
    {
        retVal = read_data_from_file(fd, filter->filterTaps, filter->numChannels, filter->length);
        if (retVal != TP_SUCCESS)
        {
            /* sth. went wrong - notify*/
            printf("\nTPFilter_perpareFilter(): ERROR! Filter coeff. values not read");
        }
    }

    /* Always remember to close the file */
    if (fileOpen == TP_TRUE)
    {
        retVal = fclose(fd);
        if (retVal == 0)
        {
            retVal = TP_SUCCESS;
        }
        else
        {
            /* we still continue with program execution */
            printf("\nTPFilter_perpareFilter(): WARNING! Filter data file not closed properly");
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* Allocate mem for the dealy lines and zero them. calloc() does this in one call. */
        filter->dlyLine = (SAMPLETYPE **) calloc(filter->numChannels, sizeof(SAMPLETYPE*));
        if (filter->dlyLine == NULL)
        {
            printf("\nTPFilter_perpareFilter(): ERROR! Delay line array allocation failed");
            retVal = TP_FAILURE;
        }
        else
        {
            for (i = 0; (i < filter->numChannels) && (retVal == TP_SUCCESS); i++)
            {
                filter->dlyLine[i] = (SAMPLETYPE *) calloc((filter->length - 1), sizeof(SAMPLETYPE));
                if (filter->dlyLine[i] == NULL)
                {
                    printf("\nTPFilter_perpareFilter(): ERROR! Filter taps array allocation failed");
                    retVal = TP_FAILURE;
                }
            }
        }
    }

    return(filter);

}



/************************************
        * TPFilter_prepareIirFilter()
 ***********************************/
int TPFilter_prepareIirFilter(SCIIR_Filter *filter, char * path)
{

    FILE            *fd = NULL;
    int             retVal = TP_SUCCESS;
    Bool_t            fileOpen = TP_FALSE;
    const char      *mode = "rb";        /* only read mode for the filters */
    int             i = 0;
    int             numDataFields = 0;
    SAMPLETYPE **   tempMem = NULL;

    if (filter == NULL)
    {
        printf("\nTPFilter_prepareIirFilter(): ERROR! Filter strucutre allocation failed!");
        retVal = TP_FAILURE;
    }

    /* Open the file with the filter data */
    if (retVal == TP_SUCCESS)
    {
        fd = fopen(path, mode);
        if (fd == NULL)
        {
            printf("\nTPFilter_prepareIirFilter: ERROR opening filter file: %s",path);
            retVal = TP_FAILURE;
        }
        else
        {
            fileOpen = TP_TRUE;
        }
    }
    /* read the filter data */
    if (retVal == TP_SUCCESS)
    {
        retVal = read_params_from_file(fd, &numDataFields, &(filter->denLength));
        filter->numLength = filter->denLength;

        if (retVal != TP_SUCCESS)
        {
            printf("\nTPFilter_prepareIirFilter(): ERROR! Filter params not read");
        }
    }

    if (numDataFields != 2)
    {
        printf("\nTPFilter_prepareIirFilter(): ERROR - expecting numerator and denominator got: %d fields!", numDataFields);
        retVal = TP_FAILURE;
    }
    else
    {
        /* as the interface to read_data_from_file() requires we will provide it with a double pointer */
        tempMem = (SAMPLETYPE **) calloc(numDataFields, sizeof(SAMPLETYPE*));
        if (tempMem == NULL)
        {
            printf("\nTPFilter_prepareIirFilter(): ERROR! Array allocation failed");
            retVal = TP_FAILURE;
        }
        else
        {
            for (i = 0; (i < numDataFields) && (retVal == TP_SUCCESS); i++)
            {
                tempMem[i] = (SAMPLETYPE *) calloc(filter->denLength, sizeof(SAMPLETYPE));
                if (tempMem[i] == NULL)
                {
                    printf("\nTPFilter_perpareFilter(): ERROR! Filter taps array allocation failed");
                    retVal = TP_FAILURE;
                }
            }
        }

        /* read the filter coefficients */
        if (retVal == TP_SUCCESS)
        {
            retVal = read_data_from_file(fd, tempMem, numDataFields, filter->denLength);
            if (retVal != TP_SUCCESS)
            {
                /* sth. went wrong - notify*/
                printf("\nTPFilter_perpareFilter(): ERROR! Filter coeff. values not read");
            }
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* Allocate the array of pointers to filter taps data and one delay line per num/den. calloc() zeros the memory allocated*/

        filter->numerator = (SAMPLETYPE *) calloc(filter->numLength, sizeof(SAMPLETYPE));
        filter->denominator = (SAMPLETYPE *) calloc(filter->denLength, sizeof(SAMPLETYPE));

        if ((filter->numerator == NULL) || (filter->denominator == NULL))
        {
            printf("\nTPFilter_prepareIirFilter(): ERROR! Filter memory allocation failed");
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* we copy the cooefs into the filter strucutre */
        memcpy(filter->numerator, tempMem[0], filter->numLength*sizeof(SAMPLETYPE));
        memcpy(filter->denominator, tempMem[1], filter->denLength*sizeof(SAMPLETYPE));

        for (i = 0; (i < numDataFields) && (retVal == TP_SUCCESS); i++)
        {
            free(tempMem[i]);
        }

        free(tempMem);
    }

    /* Always remember to close the file */
    if (fileOpen == TP_TRUE)
    {
        retVal = fclose(fd);
        if (retVal == 0)
        {
            retVal = TP_SUCCESS;
        }
        else
        {
            /* we still continue with program execution */
            printf("\nTPFilter_prepareIirFilter(): WARNING! Filter data file not closed properly");
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* Allocate mem for the dealy lines and zero them. calloc() does this in one call. */
        filter->dlyLineNum = (SAMPLETYPE *) calloc(filter->numLength, sizeof(SAMPLETYPE));
        filter->dlyLineDen = (SAMPLETYPE *) calloc(filter->denLength, sizeof(SAMPLETYPE));

        if ((filter->dlyLineDen == NULL) || (filter->dlyLineNum == NULL))
        {
            printf("\nTPFilter_prepareIirFilter(): ERROR! Filter delay lines allocation failed");
            retVal = TP_FAILURE;
        }
    }

    return(retVal);

}

/************************************
 * TPFilter_closeFilter()
 ***********************************/
int TPFilter_closeFilter(MCFIR_Filter * filter)
{
    int     i = 0;
    int     retVal = TP_SUCCESS;


    // FIR Filter memory freeing
    for (i = 0; i < filter->numChannels; i++)
    {
        free(filter->filterTaps[i]);
        free(filter->dlyLine[i]);
    }

    free(filter->filterTaps);
    free(filter->dlyLine);

    free(filter);

    return(retVal);

 }


 /************************************
 * TPFilter_closeIirFilter() - Free IIR fitler memory
 ***********************************/
 int TPFilter_closeIirFilter(SCIIR_Filter * filter)
 {
     int     retVal = TP_SUCCESS;

    // IIR Filter memory freeing
    free(filter->numerator);
    free(filter->denominator);
    free(filter->dlyLineNum);
    //free(filter->dlyLineDen);

    free(filter);

    return(retVal);

 }


/*****************
 *TPFilter_SISOIir()
 ******************/
int TPFilter_SISOIir(SAMPLETYPE *bufferIn, SAMPLETYPE *bufferOut, SCIIR_Filter * scFilter, int bufferLength)
{
    int             retVal = TP_SUCCESS;
    int             i = 0;

    if ((bufferIn == NULL) || (bufferOut == NULL) || (scFilter == NULL))
    {
        retVal = TP_ERROR_PARAMS;
        printf("\nTPFilter_SISOFir: ERROR! Input params");
    }

    if (retVal == TP_SUCCESS)
    {
        iir_filter(bufferIn, bufferOut, scFilter, bufferLength);
    }

    return(retVal);
}

/************************************
 * LOCAL FUNCTION DEFENITIONS
 ***********************************/


/**
 *  @brief Function to read HRT filter parameters from a file (number of channels and the coeffs).
 *
 *  @param fid  Handle to file, where the data is stored.
 *  @param nrChannels On return contains number of channels or 0 in case of error
 *  @param dataLength On return contains the number of filter coeffs or 0 in case of error
 *
 *  @retval TP_SUCCESS Data successfully read
 *  @retval TP_FAILURE Error when reading data
 *  @retval TP_ERROR_PARAMS Error on input params
 *
 */
int read_params_from_file(FILE *fid, int *nrChannels, int *dataLength)
{
// Read NrChannels/DataLength from binary file (double precision)

    int             retVal = TP_SUCCESS;
    size_t          numRead = 0;
    Bool_t            tmpAllocated = TP_FALSE;
    SAMPLETYPE      *tmp = NULL;

    if((fid == NULL) ||(nrChannels == NULL) || (dataLength == NULL))
    {
        printf("\nread_params_from_file(): ERROR! Null pointer in input params.");
        retVal = TP_ERROR_PARAMS;
    }

    if (retVal == TP_SUCCESS)
    {
        tmp = (SAMPLETYPE *) malloc(2*sizeof(SAMPLETYPE));
        if (tmp == NULL)
        {
            printf("\nread_params_from_file(): ERROR! Unable to allocate memroy.");
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        numRead = fread(tmp,sizeof(SAMPLETYPE),2,fid);
        if (numRead == 2)
        {
            *nrChannels = (int) tmp[0];
            *dataLength = (int) tmp[1];
        }
        else
        {
            /* in case of failure, zero the data */
            retVal = TP_FAILURE;
            *nrChannels = 0;
            *dataLength = 0;
        }

        free(tmp);
    }

    return(retVal);
}

int read_data_from_file(FILE *fid, SAMPLETYPE **filterTaps, int nrChannels, int dataLength)
{
// Read Multichannel data from file

    int         i = 0;
    int         retVal = TP_SUCCESS;
    size_t      dataRead = 0;

    for (i = 0; i < nrChannels; i++)
    {
        dataRead = fread(filterTaps[i], sizeof(SAMPLETYPE), dataLength, fid);
        if(dataRead != (size_t)dataLength)
        {
            retVal = TP_FAILURE;
            printf("\nread_data_from_file(): ERROR! Wrong number of coeffs read for channel %d.", i);
        }
    }

    return(retVal);

}

/**
 *  @brief Single channel FIR filtering function (1 input, 1 output)
            *
            *  @param fid  Handle to file, where the data is stored.
            *  @param nrChannels On return contains number of channels or 0 in case of error
            *  @param dataLength On return contains the number of filter coeffs or 0 in case of error
            *
            *  @retval TP_SUCCESS Data successfully read
            *  @retval TP_FAILURE Error when reading data
            *  @retval TP_ERROR_PARAMS Error on input params
            *
 */
void fir_filter(SAMPLETYPE *in, SAMPLETYPE *out, int signalLength, SCFIR_Filter *singleChannelfilter)
{

    SAMPLETYPE      *data, *tmp_data;
    int             i = 0;
    int             j = 0;

    // Copy delayline + input to data
    data = (SAMPLETYPE *) calloc(signalLength + singleChannelfilter->length-1, sizeof(SAMPLETYPE));

    tmp_data = data;
    memcpy(tmp_data, singleChannelfilter->dlyLine, (singleChannelfilter->length-1)*sizeof(SAMPLETYPE));
    tmp_data += singleChannelfilter->length-1;
    memcpy(tmp_data, in, signalLength*sizeof(SAMPLETYPE));

    // perform filtering operation
    for (i = 0; i < signalLength; i++)
    {
        tmp_data = data + singleChannelfilter->length-1 + i;

        for ( j = 0; j < singleChannelfilter->length; j++)
        {
            out[i] += (singleChannelfilter->filterTaps[j])*(*tmp_data--);
        }
    }

    tmp_data = data + singleChannelfilter->length-1 + i;

    // copy last samples to delayline for the given channel
    memcpy(singleChannelfilter->dlyLine,
           tmp_data - singleChannelfilter->length+1,
           (singleChannelfilter->length-1)*sizeof(SAMPLETYPE));

    // free data
    free(data);
}
/**
 *  @brief Single channel IIR filtering function (1 input, 1 output)
            *
            *  @param fid  Handle to file, where the data is stored.
            *  @param nrChannels On return contains number of channels or 0 in case of error
            *  @param dataLength On return contains the number of filter coeffs or 0 in case of error
            *
            *  @retval TP_SUCCESS Data successfully read
            *  @retval TP_FAILURE Error when reading data
            *  @retval TP_ERROR_PARAMS Error on input params
            *
 */

int iir_filter(SAMPLETYPE * in, SAMPLETYPE * out, SCIIR_Filter *filter, int samplesToProcess)
{
     /*
    ACHTUNG!!!!!   the current implementation is STRICTLY AUTOREGRESSIVE!!!!!!! ONLY - not ARMA!
    */

    int             i = 0;
    int             j = 0;
    int             retVal = TP_SUCCESS;
    SAMPLETYPE      xIn, xOut;
    SAMPLETYPE      *moveBuffer;

    moveBuffer = (SAMPLETYPE *) calloc(filter->denLength, sizeof(SAMPLETYPE));
    // perform filtering operation

    for (i = 0; (i < samplesToProcess) && (moveBuffer != NULL); i++)
    {
        xIn=in[i];
#if FIXME         /* NEVER TESTED! */
        /* move buffer */
        memcpy(moveBuffer, filter->dlyLineNum, sizeof(SAMPLETYPE)*filter->numLength-1);
        memcpy((filter->dlyLineNum+1), moveBuffer, sizeof(SAMPLETYPE)*filter->numLength-1);
        filter->dlyLineNum[0]=xIn;

        xOut=0.0;

        for (j=0;j< filter->numLength+1;j++)
        {
            xOut=xOut+filter->numerator[j]*filter->dlyLineNum[j];
        }
#else
        /* told ya.... strictly AR */
        xOut=0.0;
        xOut=xOut+filter->numerator[0]*xIn;
#endif

        for (j=0;j<filter->denLength-1;j++)
        {
            xOut=xOut-filter->denominator[j+1]*filter->dlyLineDen[j];
        }


        out[i]=xOut;

        memcpy(moveBuffer, filter->dlyLineDen, sizeof(SAMPLETYPE)*filter->denLength-1);
        memcpy((filter->dlyLineDen+1), moveBuffer, sizeof(SAMPLETYPE)*filter->denLength-1);
        filter->dlyLineDen[0]=xOut;

    }

    free(moveBuffer);

    return(retVal);
}
