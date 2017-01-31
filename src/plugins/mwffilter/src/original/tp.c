/************************************
 *  Program:    Program for testing noise cancellation in speech
 *
 *  File:       tp.c
 *
 *  Component:  TP - main function component
 *
 *  Author:     Sylwester Szczepaniak (ESAT-SISTA)
 *
 *  Data:       15.05.2007
 *
 *
 *
 ***********************************/

/************************************
 * INCLUDE FILES
 ***********************************/
#include "tp.h"
#include "tp_srtbatch.h"
#include "tp_noproc.h"
#include "tp_test.h"
#include "tpjack.h"

/************************************
 * GLOBAL DEFENITIONS
 ***********************************/

/************************************
 * GLOBAL TYPE DEFENITIONS
 ***********************************/

/************************************
 * GLOBAL VARIABLES
 ***********************************/
Bool_t      TP_AudioRunning = TP_FALSE;
int         TP_indicateTestRun = RUN_NO_TEST;
/************************************
 * LOCAL FUNCTIONS
 ***********************************/
int unprocessedSignalPresentation(void);
int srtBatchNoiseCancelling(void);
int testRealTime(void);

/************************************
 * FUNCTION DEFINITIONS
 ***********************************/

/**
 *  @brief Start the program.
 *
 */
 int main()
 {
    int     retVal = TP_SUCCESS;
    int     program_option = 0;

    /* main loop - choosing the test procedure */
    do
    {
        printf("\n\tWELCOME TO TP! SELECT AN OPTION:\n\n\n");
        printf("\n\tPlaying wav files in SRT procedure\t<1>");
        printf("\n\tBatch noise cancellation testing \t<2>");
        printf("\n\tTest real-time \t\t\t\t<3>");
        printf("\n\tQuite the program \t\t\t<4>");
        /* ....add other options here */

        printf("\n\n\n\n\r");

        scanf("%d",&program_option);

        switch (program_option)
        {
            case 1:
                printf("\n%d",program_option);
                TP_indicateTestRun = RUN_NO_PROC_SRT_TEST;
                unprocessedSignalPresentation();
                break;
            case 2:
                printf("\n%d",program_option);
                TP_indicateTestRun = RUN_BATCH_SRT_TEST;
                srtBatchNoiseCancelling();
                break;
            case 3:
                printf("\n%d",program_option);
                TP_indicateTestRun = RUN_RT_TEST;
                testRealTime();
                break;
            case 4:
                printf("\nQuitting...\n\n");
                break;
            default:
                printf("Invalid key\n");
                break;
        }
    }while (program_option != 4);


    return(retVal);

 }

/************************************
 * LOCAL FUNCTION DEFINITIONS
 *
 * every program option should have a function which calls another function
 * defining the steps of the test procedure. The 'other' function must be placed
 * a seperate file and is the point where control is handed over to another module
 * which starts all the services it needs and performs all its actions.
 *
 ***********************************/
 int unprocessedSignalPresentation()
{
    int     retVal = TP_SUCCESS;

    retVal = TPNoProc_test();

    return(retVal);
}

int srtBatchNoiseCancelling()
{
    int     retVal = TP_SUCCESS;

    retVal = TPSTRBatch_test();

    return(retVal);
}

int testRealTime()
{
    int     retVal = TP_SUCCESS;

    retVal = TPTest_realTime();

    return(retVal);
}
