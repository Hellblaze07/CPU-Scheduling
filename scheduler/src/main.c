#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/scheduler.h"
#include "../include/json.h"

void copy_processes(struct Process src[], struct Process dest[], int num_processes) {
    for (int i = 0; i < num_processes; i++)
        dest[i] = src[i];
}

void print_gantt(int timeline[], int length) {
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < length; i++) {
        if (timeline[i] != 0)
            printf(" P%d |", timeline[i]);
        else
            printf(" Idle |");
    }
    printf("\n0");
    for (int i = 1; i <= length; i++) {
        printf("    %d", i);
    }
    printf("\n");
}

void print_comparison(struct Result *h, struct Result *f, struct Result *s, struct Result *processes, struct Result *result) {
    printf("\n\n============================================================\n");
    printf("                PERFORMANCE COMPARISON\n");
    printf("============================================================\n");
    printf("%-15s %-12s %-12s %-12s\n", "Algorithm", "Avg WT", "Avg TAT", "CPU Util (%)");
    printf("------------------------------------------------------------\n");
    printf("%-15s %-12.2f %-12.2f %-12.2f\n", "Hybrid", h->avg_wt, h->avg_tat, h->cpu_util);
    printf("%-15s %-12.2f %-12.2f %-12.2f\n", "FCFS", f->avg_wt, f->avg_tat, f->cpu_util);
    printf("%-15s %-12.2f %-12.2f %-12.2f\n", "SJF", s->avg_wt, s->avg_tat, s->cpu_util);
    printf("%-15s %-12.2f %-12.2f %-12.2f\n", "Priority", processes->avg_wt, processes->avg_tat, processes->cpu_util);
    printf("%-15s %-12.2f %-12.2f %-12.2f\n", "Round Robin", result->avg_wt, result->avg_tat, result->cpu_util);
    printf("============================================================\n");
}

int main(int argc, char *argv[]) {
    int json_mode = 0;
    char algorithm[20] = "HYBRID";
    int time_quantum = 2; // Default for RR
    int aging_threshold = AGING_THRESHOLD; // Default for HYBRID
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--json") == 0) {
            json_mode = 1;
        } else if (strcmp(argv[i], "--algo") == 0 && i + 1 < argc) {
            strncpy(algorithm, argv[i+1], 19);
            algorithm[19] = '\0';
            i++;
        } else if (strcmp(argv[i], "--quantum") == 0 && i + 1 < argc) {
            time_quantum = atoi(argv[i+1]);
            if (time_quantum <= 0) time_quantum = 2;
            i++;
        } else if (strcmp(argv[i], "--aging") == 0 && i + 1 < argc) {
            aging_threshold = atoi(argv[i+1]);
            if (aging_threshold <= 0) aging_threshold = AGING_THRESHOLD;
            i++;
        }
    }

    struct Process original[MAX_PROCESSES];
    struct Process temp[MAX_PROCESSES];
    struct Result *r_h, *r_f, *r_s, *r_p, *r_rr;
    int num_processes;

    if (!json_mode) {
        printf("Enter number of processes: ");
        if(scanf("%d", &num_processes) != 1 || num_processes <= 0 || num_processes > MAX_PROCESSES) {
            printf("Invalid number of processes.\n");
            return 1;
        }
        
        for (int i = 0; i < num_processes; i++) {
            original[i].pid = i + 1;
            printf("\nProcess %d\n", original[i].pid);
            printf("Arrival Time: ");
            if (scanf("%d", &original[i].arrival_time) != 1 || original[i].arrival_time < 0) return 1;
            printf("Burst Time: ");
            if (scanf("%d", &original[i].burst_time) != 1 || original[i].burst_time <= 0) return 1;
            printf("Priority (Lower = Higher priority): ");
            if (scanf("%d", &original[i].priority) != 1 || original[i].priority < 0) return 1;
            printf("Type (1=RT, 2=Interactive, 3=Batch): ");
            if (scanf("%d", &original[i].original_type) != 1 || original[i].original_type < 1 || original[i].original_type > 3) return 1;
            
            original[i].remaining_time = original[i].burst_time;
            original[i].completion_time = 0;
            original[i].waiting_time = 0;
            original[i].turnaround_time = 0;
            original[i].response_time = -1;
            original[i].started = 0;
            original[i].current_class = original[i].original_type;
        }
    } else {
        if (scanf("%d", &num_processes) != 1 || num_processes <= 0 || num_processes > MAX_PROCESSES) return 1;
        for (int i = 0; i < num_processes; i++) {
            original[i].pid = i + 1;
            if (scanf("%d %d %d %d", &original[i].arrival_time, &original[i].burst_time, &original[i].priority, &original[i].original_type) != 4) return 1;
            if (original[i].arrival_time < 0 || original[i].burst_time <= 0 || original[i].priority < 0) return 1;
            if (original[i].original_type < 1 || original[i].original_type > 3) return 1;
            original[i].remaining_time = original[i].burst_time;
            original[i].completion_time = 0;
            original[i].waiting_time = 0;
            original[i].turnaround_time = 0;
            original[i].response_time = -1;
            original[i].started = 0;
            original[i].current_class = original[i].original_type;
        }
    }

    copy_processes(original, temp, num_processes);
    if (json_mode) {
        struct Result *r_selected = NULL;
        if (strcmp(algorithm, "FCFS") == 0) {
            r_selected = fcfs_scheduler(temp, num_processes);
            print_json_output(temp, num_processes, r_selected, 0, 0, "FCFS");
        } else if (strcmp(algorithm, "SJF") == 0) {
            r_selected = sjf_scheduler(temp, num_processes);
            print_json_output(temp, num_processes, r_selected, 0, 0, "SJF");
        } else if (strcmp(algorithm, "PRIORITY") == 0) {
            r_selected = priority_scheduler(temp, num_processes);
            print_json_output(temp, num_processes, r_selected, 0, 0, "PRIORITY");
        } else if (strcmp(algorithm, "ROUND_ROBIN") == 0) {
            r_selected = round_robin_scheduler(temp, num_processes, time_quantum);
            print_json_output(temp, num_processes, r_selected, time_quantum, 0, "ROUND_ROBIN");
        } else {
            // Default to Hybrid
            r_selected = hybrid_scheduler(temp, num_processes, aging_threshold);
            print_json_output(temp, num_processes, r_selected, 0, aging_threshold, "HYBRID");
        }
        return 0;
    }

    r_h = hybrid_scheduler(temp, num_processes, aging_threshold);

    copy_processes(original, temp, num_processes);
    r_f = fcfs_scheduler(temp, num_processes);

    copy_processes(original, temp, num_processes);
    r_s = sjf_scheduler(temp, num_processes);

    copy_processes(original, temp, num_processes);
    r_p = priority_scheduler(temp, num_processes);

    copy_processes(original, temp, num_processes);
    r_rr = round_robin_scheduler(temp, num_processes, 2);

    print_gantt(r_h->timeline, r_h->total_time);
    print_comparison(r_h, r_f, r_s, r_p, r_rr);

    return 0;
}
