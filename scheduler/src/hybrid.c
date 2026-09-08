#include <stdio.h>
#include "../include/scheduler.h"

// Helper function to count ready interactive processes
static int count_ready_interactive(struct Process processes[], int num_processes, int current_time) {
    int count = 0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].current_class == INTERACTIVE) {
            count++;
        }
    }
    return count;
}

// Helper to determine adaptive quantum
static int get_adaptive_quantum(int interactive_count) {
    if (interactive_count >= 6) return 2;
    if (interactive_count >= 3) return 3;
    return 4; // 1-2 interactive processes
}

struct Result* hybrid_scheduler(struct Process processes[], int num_processes, int aging_threshold) {
    int current_time = 0, completed = 0, last_interactive = -1;
    int busy_time = 0, context_switches = 0;
    float total_wt = 0, total_tat = 0, total_rt = 0;
    
    static struct Result result;
    init_trace(&result.trace);
    result.total_time = 0;
    
    int running_idx = -1;
    int current_quantum = 0;
    int quantum_used = 0;
    
    // Initialize processes
    for (int i = 0; i < num_processes; i++) {
        processes[i].current_class = processes[i].original_type;
        processes[i].wait_time_in_class = 0;
        processes[i].promotions = 0;
        processes[i].started = 0;
    }

    while (completed < num_processes) {
        // Record arrivals
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == current_time) {
                record_event(&result.trace, current_time, EVENT_ARRIVAL, processes[i].pid, 0, 0, 0, 0, "Arrived");
            }
        }
        
        // Handle Aging
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && i != running_idx) {
                processes[i].wait_time_in_class++;
                if (processes[i].wait_time_in_class >= aging_threshold) {
                    int from = processes[i].current_class;
                    if (processes[i].current_class == BATCH) {
                        processes[i].current_class = INTERACTIVE;
                        processes[i].promotions++;
                        processes[i].wait_time_in_class = 0;
                        record_event(&result.trace, current_time, EVENT_AGING, processes[i].pid, from, INTERACTIVE, aging_threshold, 0, "Promoted to Interactive");
                    } else if (processes[i].current_class == INTERACTIVE) {
                        processes[i].current_class = REAL_TIME;
                        processes[i].promotions++;
                        processes[i].wait_time_in_class = 0;
                        record_event(&result.trace, current_time, EVENT_AGING, processes[i].pid, from, REAL_TIME, aging_threshold, 0, "Promoted to Real-Time");
                    }
                }
            }
        }

        // Selection phase
        int next_idx = -1;
        int highest_priority = 9999;
        
        // 1. Real-Time (Priority)
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].current_class == REAL_TIME) {
                if (processes[i].priority < highest_priority) {
                    highest_priority = processes[i].priority;
                    next_idx = i;
                }
            }
        }

        // 2. Interactive (Round Robin)
        if (next_idx == -1) {
            // Check if current running process is interactive and its quantum hasn't expired yet
            if (running_idx != -1 && processes[running_idx].remaining_time > 0 && processes[running_idx].current_class == INTERACTIVE && quantum_used < current_quantum) {
                next_idx = running_idx;
            } else {
                // If quantum expired, record it
                if (running_idx != -1 && processes[running_idx].remaining_time > 0 && processes[running_idx].current_class == INTERACTIVE && quantum_used >= current_quantum) {
                    record_event(&result.trace, current_time, EVENT_QUANTUM_EXPIRE, processes[running_idx].pid, 0, 0, 0, current_quantum, "Quantum expired");
                }
                
                int interactive_count = count_ready_interactive(processes, num_processes, current_time);
                if (interactive_count > 0) {
                    current_quantum = get_adaptive_quantum(interactive_count);
                    quantum_used = 0;
                    
                    for (int i = 1; i <= num_processes; i++) {
                        int j = (last_interactive + i) % num_processes;
                        if (processes[j].arrival_time <= current_time && processes[j].remaining_time > 0 && processes[j].current_class == INTERACTIVE) {
                            next_idx = j;
                            last_interactive = j;
                            break;
                        }
                    }
                }
            }
        }

        // 3. Batch (SRTF)
        if (next_idx == -1) {
            int shortest = 9999;
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].current_class == BATCH) {
                    if (processes[i].remaining_time < shortest) {
                        shortest = processes[i].remaining_time;
                        next_idx = i;
                    }
                }
            }
        }

        // Handle context switch
        if (next_idx != running_idx) {
            if (running_idx != -1 && processes[running_idx].remaining_time > 0) {
                if (processes[running_idx].current_class != INTERACTIVE || next_idx != -1) { // We already logged quantum expire
                   if(processes[running_idx].current_class != INTERACTIVE || quantum_used < current_quantum) {
                       record_event(&result.trace, current_time, EVENT_PREEMPTION, processes[running_idx].pid, 0, 0, 0, 0, "Preempted");
                   }
                }
            }
            if (next_idx != -1) {
                context_switches++;
                int quantum = (processes[next_idx].current_class == INTERACTIVE) ? current_quantum : 0;
                char reason[100];
                if (processes[next_idx].current_class == REAL_TIME) sprintf(reason, "RT Priority %d", processes[next_idx].priority);
                else if (processes[next_idx].current_class == INTERACTIVE) sprintf(reason, "RR turn");
                else sprintf(reason, "SRTF %d left", processes[next_idx].remaining_time);
                
                record_event(&result.trace, current_time, EVENT_DISPATCH, processes[next_idx].pid, 0, 0, 0, quantum, reason);
                
                if (!processes[next_idx].started) {
                    processes[next_idx].response_time = current_time - processes[next_idx].arrival_time;
                    processes[next_idx].started = 1;
                }
                
                if (processes[next_idx].current_class != INTERACTIVE) {
                    quantum_used = 0; // reset for non-RR just in case
                }
            } else {
                if (completed < num_processes) {
                     // Check if previous was idle to avoid spam
                     if (result.trace.event_count == 0 || result.trace.events[result.trace.event_count - 1].type != EVENT_IDLE) {
                         record_event(&result.trace, current_time, EVENT_IDLE, 0, 0, 0, 0, 0, "Idle");
                     }
                }
            }
            running_idx = next_idx;
        }

        // Execute 1 unit of current_time
        if (running_idx != -1) {
            processes[running_idx].remaining_time--;
            processes[running_idx].wait_time_in_class = 0; // Reset wait current_time while running
            quantum_used++;
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
    result.cpu_util = ((float)busy_time / current_time) * 100;
    result.context_switches = context_switches;
    
    return &result;
}
