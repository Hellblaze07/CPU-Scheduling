#include <stdio.h>
#include "../include/scheduler.h"

struct Result* round_robin_scheduler(struct Process processes[], int num_processes, int quantum) {
    int current_time = 0, completed = 0, busy_time = 0, context_switches = 0;
    float total_wt = 0, total_tat = 0, total_rt = 0;
    static struct Result result;
    init_trace(&result.trace);
    result.total_time = 0;
    int last_idx = -1;

    while (completed < num_processes) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == current_time) {
                record_event(&result.trace, current_time, EVENT_ARRIVAL, processes[i].pid, 0, 0, 0, 0, "Arrived");
            }
        }

        int idx = -1;
        for (int i = 1; i <= num_processes; i++) {
            int j = (last_idx + i) % num_processes;
            if (processes[j].arrival_time <= current_time && processes[j].remaining_time > 0) {
                idx = j;
                last_idx = j;
                break;
            }
        }

        if (idx == -1) {
            if (result.total_time < MAX_TIME) result.timeline[result.total_time++] = 0;
            if (result.trace.event_count == 0 || result.trace.events[result.trace.event_count - 1].type != EVENT_IDLE) {
                record_event(&result.trace, current_time, EVENT_IDLE, 0, 0, 0, 0, 0, "Idle");
            }
            current_time++;
            continue;
        }

        if (!processes[idx].started) {
            processes[idx].response_time = current_time - processes[idx].arrival_time;
            processes[idx].started = 1;
        }
        
        context_switches++;
        record_event(&result.trace, current_time, EVENT_DISPATCH, processes[idx].pid, 0, 0, 0, quantum, "RR turn");
        
        int slice = (processes[idx].remaining_time < quantum) ? processes[idx].remaining_time : quantum;
        for (int t = 0; t < slice; t++) {
            if (result.total_time < MAX_TIME) result.timeline[result.total_time++] = processes[idx].pid;
            current_time++;
            busy_time++;
            
            for(int i = 0; i < num_processes; i++) {
                if(processes[i].arrival_time == current_time && processes[i].remaining_time > 0 && processes[i].pid != processes[idx].pid)
                   record_event(&result.trace, current_time, EVENT_ARRIVAL, processes[i].pid, 0, 0, 0, 0, "Arrived");
            }
        }
        
        processes[idx].remaining_time -= slice;

        if (processes[idx].remaining_time == 0) {
            completed++;
            processes[idx].completion_time = current_time;
            processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            record_event(&result.trace, current_time, EVENT_COMPLETION, processes[idx].pid, 0, 0, 0, 0, "Completed");
            
            total_wt += processes[idx].waiting_time;
            total_tat += processes[idx].turnaround_time;
            total_rt += processes[idx].response_time;
        } else {
            record_event(&result.trace, current_time, EVENT_QUANTUM_EXPIRE, processes[idx].pid, 0, 0, 0, quantum, "Quantum Expired");
        }
    }
    
    result.avg_wt = total_wt / num_processes;
    result.avg_tat = total_tat / num_processes;
    result.avg_rt = total_rt / num_processes;
    result.cpu_util = (current_time > 0) ? ((float)busy_time / current_time) * 100 : 0.0;
    result.context_switches = context_switches;
    return &result;
}
