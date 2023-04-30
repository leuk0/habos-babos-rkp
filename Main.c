#include "MyUtils.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <omp.h>

int PROG_MODE = 0;
int COMM_MODE = 0;

int main(int argc, char *argv[])
{
    CMDArgs(&PROG_MODE, &COMM_MODE, argc, argv);

    signal(SIGINT, SignalHandler);
    signal(SIGUSR1, SignalHandler);

    int *nums, measurements;
    if (PROG_MODE == 0 && COMM_MODE == 0)
    {
        measurements = Measurement(&nums);
        SendViaFile(nums, measurements);

        free(nums);
    }
    else if (PROG_MODE == 0 && COMM_MODE == 1)
    {
        measurements = Measurement(&nums);
        SendViaSocket(nums, measurements);

        free(nums);
    }
    else if (PROG_MODE == 1 && COMM_MODE == 1)
    {
        ReceiveViaSocket();
    }
    else if (PROG_MODE == 1 && COMM_MODE == 0)
    {
        while (1)
        {
            signal(SIGUSR1, ReceiveViaFile);
            pause();
        }
    }

    return EXIT_SUCCESS;
}