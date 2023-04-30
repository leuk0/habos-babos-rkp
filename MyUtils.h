#ifndef MYUTILS_H
#define MYUTILS_H

void Help();

void Version();

void CMDArgs(int* PROG_MODE, int* COMM_MODE, int argc, char *argv[]);

int Measurement(int **Values);

void BMPcreator(int *Values, int NumValues);

int FindPID();

void SendViaFile(int *Values, int NumValues);

void ReceiveViaFile(int sig);

int SendViaSocket(int *Values, int NumValues);

int ReceiveViaSocket();

void SignalHandler(int sig);

#endif