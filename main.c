/**
 * @file main.c
 * @author Davide Usberti (sdavide.usberti@itis.pr.it)
 * @brief Calculate Tw and Tr of FCFS, SJF, RR scheduling
 * @version 0.1
 * @date 2024-08-17
 *
 * @copyright Copyright (c) 2024 Davide Usberti
 *
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned short timeType;

timeType get_Ti()
{
    timeType ti_input;
    printf("Inserisci Ti: ");
    scanf("%hu", &ti_input);
    return ti_input;
}

timeType get_Di()
{
    timeType di_input;
    printf("Inserisci Di: ");
    scanf("%hu", &di_input);
    return di_input;
}

unsigned short processes = 0;

void save_process(timeType **schedule,
                  unsigned short task, timeType ti, timeType di)
{
    schedule[task][0] = ti;
    schedule[task][1] = di;
}

void print_processes(timeType **schedule,
                     unsigned short processes_)
{
    char processes_string[126];
    int tmp_burst = 0, old_burst = 0;

    for (int i = 0; i < sizeof(processes_string); i++)
    {
        processes_string[i] = ' ';
    }

    processes_string[sizeof(processes_string) - 1] = '\0';

    if (schedule[0][0] * 2 < sizeof(processes_string) - 1)
    {
        processes_string[schedule[0][0] * 2] = '|';
    }

    for (int i = 0; i < processes_; i++)
    {
        old_burst += tmp_burst;
        tmp_burst += schedule[i][1] * 2;

        if (tmp_burst < sizeof(processes_string) - 1)
        {
            processes_string[tmp_burst] = '|';
            int pos = tmp_burst - (int)(schedule[i][1] / 2) - 2;

            if (pos >= 0 && pos < sizeof(processes_string) - 1)
            {
                processes_string[pos] = 'P';
                processes_string[pos + 1] = '1' + i;
            }
        }
    }
    printf("\n%s%s", processes_string, "\n^ . . . . ^ . . . . ^ . . . . ^ . . . . ^ \n0         5         10        15        20");
}

// FCFS
float evaluate_fcfs_tw(timeType **schedule,
                       unsigned short processes_)
{
    float formule = 0.0;
    float tmp_burst = 0.0;

    formule += schedule[0][0];
    tmp_burst += schedule[0][1];

    for (int i = 1; i < processes_; i++)
    {
        formule += (tmp_burst - schedule[i][0]);
        tmp_burst += schedule[i][1];
    }

    formule /= processes_;
    return formule;
}

float evaluate_fcfs_tr(timeType **schedule,
                       unsigned short processes_)
{
    float totalResponseTime = 0.0;
    timeType currentTime = 0;

    for (int i = 0; i < processes_; i++)
    {
        if (currentTime < schedule[i][0])
        {
            currentTime = schedule[i][0];
        }

        timeType responseTime = (currentTime - schedule[i][0]) + schedule[i][1];

        totalResponseTime += responseTime;

        currentTime += schedule[i][1];
    }

    return totalResponseTime / processes_;
}

// SJF
float evaluate_sjf_tw(timeType **schedule,
                      unsigned short processes_)
{
    timeType *waitingTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType *completionTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType currentTime = 0;
    timeType totalWaitingTime = 0;
    int completed = 0;
    int is_completed[processes_];
    for (int i = 0; i < processes_; i++)
    {
        is_completed[i] = 0;
    }

    while (completed != processes_)
    {
        int idx = -1;
        timeType min_burst_time = 65535;

        for (int i = 0; i < processes_; i++)
        {
            if (schedule[i][0] <= currentTime && !is_completed[i] && schedule[i][1] < min_burst_time)
            {
                min_burst_time = schedule[i][1];
                idx = i;
            }
        }

        if (idx != -1)
        {
            currentTime += schedule[idx][1];
            completionTime[idx] = currentTime;
            waitingTime[idx] = completionTime[idx] - schedule[idx][0] - schedule[idx][1];
            totalWaitingTime += waitingTime[idx];
            is_completed[idx] = 1;
            completed++;
        }
        else
        {
            currentTime++;
        }
    }

    float averageWaitingTime = (float)totalWaitingTime / processes_;

    free(waitingTime);
    free(completionTime);

    return averageWaitingTime;
}

float evaluate_sjf_tr(timeType **schedule,
                      unsigned short processes_)
{
    timeType *completionTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType *turnaroundTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType currentTime = 0;
    timeType totalTurnaroundTime = 0;
    int completed = 0;
    int is_completed[processes_];
    for (int i = 0; i < processes_; i++)
    {
        is_completed[i] = 0;
    }

    while (completed != processes_)
    {
        int idx = -1;
        timeType min_burst_time = 65535;

        for (int i = 0; i < processes_; i++)
        {
            if (schedule[i][0] <= currentTime && !is_completed[i] && schedule[i][1] < min_burst_time)
            {
                min_burst_time = schedule[i][1];
                idx = i;
            }
        }

        if (idx != -1)
        {
            currentTime += schedule[idx][1];
            completionTime[idx] = currentTime;
            turnaroundTime[idx] = completionTime[idx] - schedule[idx][0];
            totalTurnaroundTime += turnaroundTime[idx];
            is_completed[idx] = 1;
            completed++;
        }
        else
        {
            currentTime++;
        }
    }

    float averageTurnaroundTime = (float)totalTurnaroundTime / processes_;

    free(completionTime);
    free(turnaroundTime);

    return averageTurnaroundTime;
}

float evaluate_rr_tw(timeType **schedule, unsigned short processes_, timeType time_quantum)
{
    timeType *waitingTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType *remainingTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType currentTime = 0;
    int completed = 0;

    for (int i = 0; i < processes_; i++)
    {
        remainingTime[i] = schedule[i][1];
        waitingTime[i] = 0;
    }

    while (completed != processes_)
    {
        for (int i = 0; i < processes_; i++)
        {
            if (remainingTime[i] > 0)
            {
                if (remainingTime[i] > time_quantum)
                {
                    currentTime += time_quantum;
                    remainingTime[i] -= time_quantum;
                }
                else
                {
                    currentTime += remainingTime[i];
                    waitingTime[i] = currentTime - schedule[i][1] - schedule[i][0];
                    remainingTime[i] = 0;
                    completed++;
                }
            }
        }
    }

    float totalWaitingTime = 0.0;
    for (int i = 0; i < processes_; i++)
    {
        totalWaitingTime += waitingTime[i];
    }

    free(waitingTime);
    free(remainingTime);

    return totalWaitingTime / processes_;
}

float evaluate_rr_tr(timeType **schedule, unsigned short processes_, timeType time_quantum)
{
    timeType *turnaroundTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType *remainingTime = (timeType *)malloc(processes_ * sizeof(timeType));
    timeType currentTime = 0;
    int completed = 0;

    for (int i = 0; i < processes_; i++)
    {
        remainingTime[i] = schedule[i][1];
        turnaroundTime[i] = 0;
    }

    while (completed != processes_)
    {
        for (int i = 0; i < processes_; i++)
        {
            if (remainingTime[i] > 0)
            {
                if (remainingTime[i] > time_quantum)
                {
                    currentTime += time_quantum;
                    remainingTime[i] -= time_quantum;
                }
                else
                {
                    currentTime += remainingTime[i];
                    turnaroundTime[i] = currentTime - schedule[i][0];
                    remainingTime[i] = 0;
                    completed++;
                }
            }
        }
    }

    float totalTurnaroundTime = 0.0;
    for (int i = 0; i < processes_; i++)
    {
        totalTurnaroundTime += turnaroundTime[i];
    }

    free(turnaroundTime);
    free(remainingTime);

    return totalTurnaroundTime / processes_;
}

int main(void)
{
    timeType ti_current, di_current;
    timeType time_quantum;

    printf("\n%s", "Benvenuto nella mia applicazione!");

    printf("\n%s", "Inserire numero di processi: ");
    scanf("%hu", &processes);

    printf("Inserisci il tempo per RR: ");
    scanf("%hu", &time_quantum);

    if (processes <= 0)
    {
        printf("\nERROR: Invalid number of procs");
        return 0;
    }

    timeType **schedule = malloc(processes * sizeof(timeType *));
    for (int i = 0; i < processes; i++)
    {
        schedule[i] = malloc(2 * sizeof(timeType));
    }

    for (unsigned short task = 0; task < processes; task++)
    {
        printf("\nProcess %hu\n____________\n", task + 1);
        ti_current = get_Ti();
        di_current = get_Di();

        save_process(schedule, task, ti_current, di_current);
    }

    // Print FCFS
    printf("\n___FCFS________________________________________________");
    print_processes(schedule, processes);
    printf("\nTw = %f", evaluate_fcfs_tw(schedule, processes));
    printf("        Tr = %f", evaluate_fcfs_tr(schedule, processes));

    // Print SJF

    // Sort
    for (int i = 0; i < processes - 1; i++)
    {
        for (int j = i + 1; j < processes; j++)
        {
            if (schedule[i][0] > schedule[j][0] ||
                (schedule[i][0] == schedule[j][0] && schedule[i][1] > schedule[j][1]))
            {
                timeType *temp = schedule[i];
                schedule[i] = schedule[j];
                schedule[j] = temp;
            }
        }
    }

    printf("\n\n___SJF________________________________________________");
    // print_processes(schedule, processes);
    printf("\nTw = %f        Tr = %f",
           evaluate_sjf_tw(schedule, processes),
           evaluate_sjf_tr(schedule, processes));

    printf("\n\n___RR________________________________________________");
    printf("\nTw = %f        Tr = %f",
           evaluate_rr_tw(schedule, processes, time_quantum),
           evaluate_rr_tr(schedule, processes, time_quantum));
           
    for (int i = 0; i < processes; i++)
    {
        free(schedule[i]);
    }
    free(schedule);

    return 0;
}
