#include "MyUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>

void Help()
{
    printf("Flags:\n--version\n--help\n-send\n-receive\n-file\n-socket\n");
    exit(0);
}

void Version()
{

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Version: 1.0\n");
        }
        #pragma omp section
        {
            printf("Completed: 04.30\n");
        }
        #pragma omp section
        {
            printf("Developer: Divinyi Balázs\n");
        }
    }
}

void CMDArgs(int *PROG_MODE, int *COMM_MODE, int argc, char *argv[])
{
    if (!strcmp(argv[0], "chart"))
    {
        fprintf(stderr, "[ERROR] File is not \"chart\"\n");
        exit(1);
    }

    if (argc == 1)
    {
        return;
    }

    if (argc > 3 || !strcmp(argv[1], "--help"))
    {
        Help();
    }

    if (!strcmp(argv[1], "--version"))
    {
        Version();
        exit(0);
    }

    if (argc == 2)
    {
        if (!strcmp(argv[1], "-receive"))
        {
            *PROG_MODE = 1;
            return;
        }
        if (!strcmp(argv[1], "-socket"))
        {
            *COMM_MODE = 1;
            return;
        }
    }

    if (argc == 3)
    {
        if ((!strcmp(argv[1], "-receive") || !strcmp(argv[2], "-receive")) &&
            (!strcmp(argv[1], "-socket") || !strcmp(argv[2], "-socket")) &&
            (strcmp(argv[1], argv[2]) != 0))
        {
            *PROG_MODE = 1;
            *COMM_MODE = 1;
            return;
        }
        else if ((!strcmp(argv[1], "-file") || !strcmp(argv[1], "-send")) &&
                 (!strcmp(argv[2], "-file") || !strcmp(argv[2], "-send")) &&
                 (strcmp(argv[1], argv[2]) != 0))
        {
            return;
        }
        else if ((!strcmp(argv[1], "-file") || !strcmp(argv[1], "-receive")) &&
                 (!strcmp(argv[2], "-file") || !strcmp(argv[2], "-receive")) &&
                 (strcmp(argv[1], argv[2]) != 0))
        {
            *PROG_MODE = 1;
            return;
        }
        else if ((!strcmp(argv[1], "-socket") || !strcmp(argv[1], "-send")) &&
                 (!strcmp(argv[2], "-socket") || !strcmp(argv[2], "-send")) &&
                 (strcmp(argv[1], argv[2]) != 0))
        {
            *COMM_MODE = 1;
            return;
        }
    }

    Help();
}

int Measurement(int **Values)
{
    time_t t;
    int helper_var;
    struct tm *local_t;

    helper_var = time(&t);

    local_t = localtime(&t);

    int minute = (*local_t).tm_min;
    int second = (*local_t).tm_sec;

    int quarter = (minute % 15) * 60 + second;

    int count = 100;
    if (quarter > count)
    {
        count = quarter;
    }

    srand(time(NULL));
    double rnd_val;
    *Values = (int *)malloc(sizeof(int) * count),
    *(*Values + 0) = 0;

    for (int i = 1; i < count; i++)
    {
        rnd_val = (double)rand() / ((unsigned)RAND_MAX + 1);

        if (rnd_val <= 0.428571)
        {
            *(*Values + i) = *(*Values + (i - 1)) + 1;
        }
        else if (rnd_val <= 11 / 31 + 0.428571)
        {
            *(*Values + i) = *(*Values + (i - 1)) - 1;
        }
        else
        {
            *(*Values + i) = *(*Values + (i - 1));
        }
    }

    return count;
}

void BMPcreator(int *Values, int NumValues)
{
    int width = NumValues, height = NumValues;
    int size = width * height;
    unsigned int header_start[] = {0x42, 0x4d};

    int file = open("chart.bmp", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IROTH);
    write(file, &header_start[0], sizeof(unsigned char));
    write(file, &header_start[1], sizeof(unsigned char));

    unsigned int size_the_second = (62 + (size / 8));
    write(file, &size_the_second, sizeof(unsigned char));

    unsigned int header_mid[] = {0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x28};
    int size_of_header_mid = 13 - 1;
    for (int i = 0; i < size_of_header_mid; i++)
    {
        write(file, &header_mid[i], sizeof(unsigned char));
    }

    unsigned int header_end[] = {0x00, 0x00, 0x00, width % 256,
                                         width / 256, 0x00, 0x00, height % 256, height / 256, 0x00, 0x00, 1, 0x00, 1,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 143, 168, 114, 255, 122, 19, 8, 255};
    int size_of_header_end = 47;
    for (int i = 0; i < size_of_header_end; i++)
    {
        write(file, &header_end[i], sizeof(unsigned char));
    }

    int line = NumValues, colum = NumValues;
    if (NumValues % 32 != 0)
    {
        line = width + (32 - (width % 32));
        colum = height + (32 - (height % 32));
    }

    int array_size = (line * colum) / 8;
    unsigned char *px = calloc(array_size, sizeof(unsigned char));

    int location;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            location = (y * line + x) / 8;
            if (y == Values[x] + height / 2)
            {
                px[location] += (1 << (7 - (x % 8)));
            }
        }
    }
    write(file, px, array_size);
    free(px);
    close(file);
}

int FindPID()
{
    int own_PID = (int)getpid();
    DIR *d0, *d1;
    struct dirent *entry, *entry1;
    int found_PID = -1;

    d0 = opendir("/proc/");
    while ((entry = readdir(d0)) != NULL)
    {
        if ((*entry).d_name[0] != '.' && isdigit((*entry).d_name[0]))
        {
            char current_folder[1000] = "/proc/";
            strcat(current_folder, (*entry).d_name);
            d1 = opendir(current_folder);
            while ((entry1 = readdir(d1)) != NULL)
            {
                char status_path[1000] = {0};
                if (current_folder != "")
                {
                    strcpy(status_path, current_folder);
                    strcat(status_path, "/status");
                    FILE *status_file = fopen(status_path, "r");
                    if (status_file == NULL)
                    {
                        continue;
                    }
                    char line_of_name[1000] = {0};
                    fscanf(status_file, "Name:\t%s\n", line_of_name);
                    char more_lines[1000];

                    if (!strcmp(line_of_name, "chart"))
                    {
                        while (fgets(more_lines, 1000, status_file) != NULL)
                        {
                            char *thingies = strstr(more_lines, "Pid:\t");
                            if (thingies != NULL)
                            {
                                char *at_last = strtok(thingies, "\t");
                                at_last = strtok(NULL, " ");
                                if (atoi(at_last) != own_PID)
                                {
                                    found_PID = atoi(at_last);
                                    fclose(status_file);
                                    break;
                                }
                                else
                                {
                                    fclose(status_file);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            closedir(d1);
            if (found_PID != -1)
            {
                break;
            }
        }
    }
    closedir(d0);

    return found_PID;
}

void SendViaFile(int *Values, int NumValues)
{
    char *file_name = "/Measurement.txt";
    char *home_dir = getenv("HOME");
    char *file_path = malloc(strlen(home_dir) + strlen(file_name) + 1);
    strncpy(file_path, home_dir, strlen(home_dir) + 1);
    strncat(file_path, file_name, strlen(file_name) + 1);

    FILE *file_out = fopen(file_path, "w");

    for (int i = 0; i < NumValues; i++)
    {
        fprintf(file_out, "%d\n", Values[i]);
    }

    free(file_path);
    fclose(file_out);

    pid_t pid = FindPID();
    if (pid == -1)
    {
        fprintf(stderr, "[ERROR] There are no receiving program!\n");
        exit(2);
    }
    else
    {
        kill(pid, SIGUSR1);
    }
}

void ReceiveViaFile(int sig)
{
    char *file_name = "/Measurement.txt";
    char *home_dir = getenv("HOME");
    char *file_path = malloc(strlen(home_dir) + strlen(file_name) + 1);
    strncpy(file_path, home_dir, strlen(home_dir) + 1);
    strncat(file_path, file_name, strlen(file_name) + 1);

    FILE *file_out = fopen(file_path, "r");
    if (file_out == NULL)
    {
        fprintf(stderr, "[ERROR] Cannor open file!\n");
        exit(10);
    }
    int size = 500;
    int *readed_nums = (int *)calloc(size, sizeof(int));
    char current_line[1000] = {0};
    int index = 0;
    while (fgets(current_line, 1000, file_out) != NULL)
    {
        if (index == 495)
        {
            size *= 2;
            readed_nums = (int *)realloc(readed_nums, size * sizeof(int));
        }
        *(readed_nums + index) = atoi(current_line);
        index++;
    }
    fclose(file_out);

    BMPcreator(readed_nums, index);
    free(readed_nums);
    free(file_path);
}

int SendViaSocket(int *Values, int NumValues)
{
    int s;
    int bytes;
    int flag;
    char on;
    unsigned int server_size;
    struct sockaddr_in server;
    int PORT_NO = 3333;

    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT_NO);
    server_size = sizeof server;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "Socket creation error.\n");
        exit(2);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    printf("To send a message: ");

    bytes = sendto(s, &NumValues, sizeof(int), flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        fprintf(stderr, "Sending error.\n");
        exit(3);
    }
    printf("%i byte was sent.\n", bytes - 1);

    signal(SIGALRM, SignalHandler);
    alarm(1);

    int got_size = 0;
    bytes = recvfrom(s, &got_size, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (NumValues != got_size)
    {
        fprintf(stderr, "[ERROR] The sent and received sizes are not equal.\n");
        exit(4);
    }
    printf("The server (%s:%d) receipt:\n  %d\n",
           inet_ntoa(server.sin_addr), ntohs(server.sin_port), got_size);

    int current_size = NumValues * sizeof(int);
    bytes = sendto(s, Values, current_size, flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        fprintf(stderr, "Sending error.\n");
        exit(3);
    }
    printf("%i byte was sent.\n", bytes - 1);

    got_size = 0;
    bytes = recvfrom(s, &got_size, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (current_size != got_size)
    {
        fprintf(stderr, "[ERROR] The sent and received sizes are not equal.\n");
        exit(4);
    }
    printf("The server (%s:%d) receipt:\n  %d\n",
           inet_ntoa(server.sin_addr), ntohs(server.sin_port), got_size);

    close(s);

    return EXIT_SUCCESS;
}

int ReceiveViaSocket()
{
    int s;
    int bytes;
    int err;
    int flag;
    char on;
    unsigned int server_size;
    unsigned int client_size;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int PORT_NO = 3333;

    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NO);
    server_size = sizeof server;
    client_size = sizeof client;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "[ERROR] Cannot create socket.\n");
        exit(2);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    err = bind(s, (struct sockaddr *)&server, server_size);
    if (err < 0)
    {
        fprintf(stderr, "Connection error.\n");
        exit(3);
    }

    while (1)
    {
        printf("\n Waitiong for response...\n");
        int got_size = 0;
        bytes = recvfrom(s, &got_size, sizeof(int), flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            fprintf(stderr, "Reception error.\n");
            exit(4);
        }
        printf(" %d byte received from the (%s:%d) client.\n Client's message:\n  %d",
               bytes - 1, inet_ntoa(client.sin_addr), ntohs(client.sin_port), got_size);

        int current_size = got_size * sizeof(int);
        int *values = calloc(got_size, current_size);

        bytes = sendto(s, &got_size, sizeof(int), flag, (struct sockaddr *)&client, client_size);
        if (bytes <= 0)
        {
            fprintf(stderr, "Sending Error.\n");
            exit(5);
        }
        printf(" Receipt sent to the client.\n");

        printf("\n Waitiong for response...\n");

        bytes = recvfrom(s, values, current_size, flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            fprintf(stderr, "Reception error.\n");
            exit(4);
        }
        printf(" %d byte received from the (%s:%d) client.\n Client's message: \n",
               bytes - 1, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        bytes = sendto(s, &current_size, sizeof(int), flag, (struct sockaddr *)&client, client_size);
        if (bytes <= 0)
        {
            fprintf(stderr, "Sending error.\n");
            exit(5);
        }
        printf(" Receipt sent to the client.\n");

        BMPcreator(values, got_size);

        free(values);
    }

    return EXIT_SUCCESS;
}

void SignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nGot SIGINT signal!\n");
        printf("Goodbye!\n");
        exit(0);
    }
    if (sig == SIGUSR1)
    {
        printf("\nGot SIGUSR1 signal!\n");
        printf("File transfer is unavailable...!\n");
        exit(1);
    }
    if (sig == SIGALRM)
    {
        printf("\nsGot SIGALRM signal!\n");
        printf("SConnection timed out!\n");
        exit(2);
    }
}