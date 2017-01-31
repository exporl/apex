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
#include "tpjack.h"

 /************************************
 * GLOBAL DEFENITIONS
 ***********************************/
/** Predefined name for the JACK client */
#define     JACK_CLIENT_NAME        "SistaProgram"

/************************************
 * GLOBAL VARIABLES
 ***********************************/

/************************************
 * LOCAL VARIABLES
 ***********************************/
/** The output port fo JACK */
static jack_port_t         *output_right = NULL;

/** The output port fo JACK */
static jack_port_t         *output_left = NULL;

/** Handle  for the JACK server */
static jack_client_t        *TPJack_jackHandle;

/** Buffer used to pass data to the output port */
static SAMPLETYPE*     TPJack_outputBufferLeft; /* 1 channel */

/** Buffer used to pass data to the output port */
static SAMPLETYPE*     TPJack_outputBufferRight; /* 1 channel */

/************************************
 * LOCAL FUNCTIONS
 ***********************************/
/** Callback function for the JACK server */
int callback_process (jack_nframes_t nframes, void *arg);

/** Shutdown function for the jack client */
void jack_shutdown (void *arg);

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/

int TPAudio_start(int * samplingRate, int blockSize)
{
    int             retVal = TP_SUCCESS;
    int             i = 0;
    const char      **ports = NULL;
    jack_status_t   status;

    /* make sure buffers are allocated first */
    if (retVal == TP_SUCCESS)
    {
        TPJack_outputBufferLeft = (SAMPLETYPE *)calloc(blockSize, sizeof(SAMPLETYPE));
        if (TPJack_outputBufferLeft == NULL)
        {
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        TPJack_outputBufferRight = (SAMPLETYPE *)calloc(blockSize, sizeof(SAMPLETYPE));
        if (TPJack_outputBufferRight == NULL)
        {
            retVal = TP_FAILURE;
        }
    }

    /* open a client connection to the JACK server */
    TPJack_jackHandle = jack_client_new(JACK_CLIENT_NAME);

    if (TPJack_jackHandle == NULL)
    {
        printf ("\nTPAudio_start: ERROR jack_client_new() \n");
        retVal = TP_FAILURE;
    }

    if (retVal == TP_SUCCESS)
    {
        /* Give JACK a function to call to communicate with the program */
        jack_set_process_callback (TPJack_jackHandle, callback_process, 0);

        /* If JACK shuts down, call this function */
        jack_on_shutdown (TPJack_jackHandle, jack_shutdown, 0);
    }

    if (retVal == TP_SUCCESS)
    {
        /* display the current sample rate... FANCY! */
        *samplingRate = jack_get_sample_rate (TPJack_jackHandle);
        printf ("\nJACK engine sample rate: %d" ,*samplingRate);
    }

    if (retVal == TP_SUCCESS)
    {
        /* create one new port */
        output_right = jack_port_register (TPJack_jackHandle, "outputRight", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

        output_left = jack_port_register (TPJack_jackHandle, "outputLeft", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

        if ((output_right == NULL) || (output_left == NULL))
        {
            printf("\n\tNo more JACK ports available\n");
            retVal = TP_FAILURE;
        }
    }

    /* Tell the JACK server that we are ready to roll.  Our callback_process() callback will start running now. */
    if (retVal == TP_SUCCESS)
    {
        if (jack_activate(TPJack_jackHandle))
        {
            printf ("\nCannot activate JACK client");
            retVal = TP_FAILURE;
        }
    }

    if (retVal == TP_SUCCESS)
    {
        /* connect the ports*/
        if ((ports = jack_get_ports (TPJack_jackHandle, NULL, NULL, JackPortIsPhysical|JackPortIsInput)) == NULL)
        {
            printf ("\nTPAudio_start():Cannot find any physical playback ports");
            retVal = TP_FAILURE;
        }
        else
        {
            i=0;
            if (jack_connect (TPJack_jackHandle, jack_port_name(output_left),ports[i]))
            {
                printf ("\nTPAudio_start():Cannot connect output ports\n");
                retVal = TP_FAILURE;
            }
            i++;
            if (jack_connect (TPJack_jackHandle, jack_port_name(output_right),ports[i]))
            {
                printf ("\nTPAudio_start():Cannot connect output ports\n");
                retVal = TP_FAILURE;
            }

        }

        if (ports != NULL)
        {
            free (ports);
        }

    }

    return(retVal);
}

/******************************
 *
 * TPAudio_stop()
 *
 ******************************/
int TPAudio_stop(void)
{
    int     retVal = TP_SUCCESS;

    retVal = jack_deactivate(TPJack_jackHandle);

    retVal = jack_client_close(TPJack_jackHandle);

    if (TPJack_outputBufferLeft != NULL)
    {
        free(TPJack_outputBufferLeft);
    }

    if (TPJack_outputBufferRight != NULL)
    {
        free(TPJack_outputBufferRight);
    }

    if(retVal == 0)
    {
        retVal = TP_SUCCESS;
    }
    else
    {
        retVal = TP_FAILURE;
    }

    return(retVal);
}
/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
*/
int callback_process (jack_nframes_t nframes, void *arg)
{
    jack_default_audio_sample_t     *out1;
    jack_default_audio_sample_t     *out2;
    jack_nframes_t                  i;

    if (TP_indicateTestRun == RUN_RT_TEST)
    {
        TPTest_processDataBlock(&TPJack_outputBufferLeft[0],
                                &TPJack_outputBufferRight[0]);
    }
    else if(TP_indicateTestRun == RUN_BATCH_SRT_TEST)
    {
        TPSRTBatch_processDataBlock(&TPJack_outputBufferLeft[0],
                                    &TPJack_outputBufferRight[0],
                                    (int) nframes);
    }
    else if(TP_indicateTestRun == RUN_NO_PROC_SRT_TEST)
    {
        TPNoProc_processDataBlock(&TPJack_outputBufferLeft[0],
                                    &TPJack_outputBufferRight[0],
                                    (int) nframes);
    }
    else
    {
        //JACK should not be running!!
        memset(&TPJack_outputBufferLeft[0],0,nframes*sizeof(SAMPLETYPE));
        memset(&TPJack_outputBufferRight[0],0,nframes*sizeof(SAMPLETYPE));
    }

    out1 = jack_port_get_buffer(output_left, nframes);
    out2 = jack_port_get_buffer(output_right, nframes);

    for (i = 0; (i < nframes); i++)
    {
        out1[i] = (jack_default_audio_sample_t)TPJack_outputBufferLeft[i];
        out2[i] = (jack_default_audio_sample_t)TPJack_outputBufferRight[i];
    }

    return 0;
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown (void *arg)
{
    printf("\nJACK shutdow NOW!\n");
    exit (1);
}
