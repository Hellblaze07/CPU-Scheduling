#include <stdio.h>
#include "../include/scheduler.h"

struct Result* sjf_scheduler(struct Process processes[], int num_processes) {
    int current_time = 0, completed = 0, busy_time = 0, context_switches = 0;
    float total_wt = 0, total_tat = 0, total_rt = 0;
    static struct Result result;
    init_trace(&result.trace);
    result.total_time = 0;
    int running_idx = -1;

    while (completed < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == current_time) {
                record_event(&result.trace, current_time, EVENT_ARRIVAL, processes[i].pid, 0, 0, 0, 0, "Arrived");
            }
        }

        int idx = running_idx;
        if (idx == -1) {
            int shortest = 9999;
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].burst_time < shortest) {
                    shortest = processes[i].burst_time;
                    idx = i;
                }
            }
        }

        if (idx != running_idx) {
            if (running_idx != -1 && processes[running_idx].remaining_time > 0) {
                record_event(&result.trace, current_time, EVENT_PREEMPTION, processes[running_idx].pid, 0, 0, 0, 0, "Preempted");
            }
            if (idx != -1) {
                context_switches++;
                record_event(&result.trace, current_time, EVENT_DISPATCH, processes[idx].pid, 0, 0, 0, 0, "SJF Dispatch");
                if (!processes[idx].started) {
                    processes[idx].response_time = current_time - processes[idx].arrival_time;
                    processes[idx].started = 1;
                }
            } else {
                if (result.trace.event_count == 0 || result.trace.events[result.trace.event_count - 1].type != EVENT_IDLE) {
                    record_event(&result.trace, current_time, EVENT_IDLE, 0, 0, 0, 0, 0, "Idle");
                }
            }
            running_idx = idx;
        }

        if (running_idx != -1) {
            processes[running_idx].remaining_time--;
            busy_time++;
            if (result.total_time < MAX_TIME) result.timeline[result.total_time++] = processes[running_idx].pid;
            current_time++;

            if (processes[running_idx].remaining_time == 0) {
                completed++;
                processes[running_idx].completion_time = current_time;
                processes[running_idx].turnaround_time = current_time - processes[running_idx].arrival_time;
                processes[running_idx].waiting_time = processes[running_idx].turnaround_time - processes[running_idx].burst_time;
                record_event(&result.trace, current_time, EVENT_COMPLETION, processes[running_idx].pid, 0, 0, 0, 0, "Completed");
                
                total_wt += processes[running_idx].waiting_time;
                total_tat += processes[running_idx].turnaround_time;
                total_rt += processes[running_idx].response_time;
                running_idx = -1;
            }
        } else {
            if (result.total_time < MAX_TIME) result.timeline[result.total_time++] = 0;
            current_time++;
        }
    }
    
    result.avg_wt = total_wt / num_processes;
    result.avg_tat = total_tat / num_processes;
    result.avg_rt = total_rt / num_processes;
    result.cpu_util = (current_time > 0) ? ((float)busy_time / current_time) * 100 : 0.0;
    result.context_switches = context_switches;
    return &result;
}
