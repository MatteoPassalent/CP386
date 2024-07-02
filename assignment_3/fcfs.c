#include <stdio.h>
#include <stdlib.h>

struct threadinfo
{
    int p_id;
    int arr_time;
    int burst_time;
    int waiting_time;
    int turn_around_time;
};

int main()
{
    struct threadinfo threads[100];
    int num_threads = 0;

    FILE *file = fopen("sample_in_schedule.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return 1;
    }

    while (fscanf(file, "%d, %d, %d,", &threads[num_threads].p_id, &threads[num_threads].arr_time, &threads[num_threads].burst_time) == 3)
    {
        num_threads++;
    }

    fclose(file);

    printf("Thread ID\tArrival Time\tBurst Time\tCompletion Time\tTurn-Around Time\tWaiting Time\n");
    int total_time = 0;

    int num = 0;
    int cumm_waiting_time = 0;
    int cumm_turn_around_time = 0;
    while (num < num_threads)
    {
        if (threads[num].arr_time > total_time)
            total_time = threads[num].arr_time + threads[num].burst_time;
        else
            total_time += threads[num].burst_time;
        threads[num].turn_around_time = total_time - threads[num].arr_time;
        threads[num].waiting_time = threads[num].turn_around_time - threads[num].burst_time;
        cumm_waiting_time += threads[num].waiting_time;
        cumm_turn_around_time += threads[num].turn_around_time;

        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%d\n", threads[num].p_id, threads[num].arr_time, threads[num].burst_time, total_time, threads[num].turn_around_time, threads[num].waiting_time);
        num++;
    }
    float avg_waiting_time = (float)cumm_waiting_time / num;
    float avg_turn_around_time = (float)cumm_turn_around_time / num;

    printf("The average waiting time: %.2f\n", avg_waiting_time);
    printf("The average turn-around time: %.2f\n", avg_turn_around_time);

    return 0;
}
