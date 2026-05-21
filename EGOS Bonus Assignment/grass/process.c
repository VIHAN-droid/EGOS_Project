/*
 * (C) 2025, Cornell University
 * All rights reserved.
 *
 * Description: helper functions for process management
 */

#include "process.h"

#define MLFQ_NLEVELS          5
#define MLFQ_RESET_PERIOD     10000000         /* 10 seconds */
#define MLFQ_LEVEL_RUNTIME(x) (x + 1) * 100000 /* e.g., 100ms for level 0 */
extern struct process proc_set[MAX_NPROCESS + 1];

static void proc_set_status(int pid, enum proc_status status) {
    for (uint i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid == pid) proc_set[i].status = status;
}

void proc_set_ready(int pid) { proc_set_status(pid, PROC_READY); }
void proc_set_running(int pid) { proc_set_status(pid, PROC_RUNNING); }
void proc_set_runnable(int pid) { proc_set_status(pid, PROC_RUNNABLE); }
void proc_set_pending(int pid) { proc_set_status(pid, PROC_PENDING_SYSCALL); }

int proc_alloc() {
    static uint curr_pid = 0;
    for (uint i = 1; i <= MAX_NPROCESS; i++)
        if (proc_set[i].status == PROC_UNUSED) {
            proc_set[i].pid    = ++curr_pid;
            proc_set[i].status = PROC_LOADING;
            /* Student's code goes here (Preemptive Scheduler | System Call). */

            /* Initialization of lifecycle statistics, MLFQ or process sleep. */

            //Basic-aah stats allocation
            proc_set[i].start_time = mtime_get();

            proc_set[i].turnaround_time = -1; //to denote it hasnt terminated yet
            proc_set[i].response_time = -1; // to denote that it hasnt been scheduled yet
            proc_set[i].cpu_time = 0;
            proc_set[i].n_timer_interrupts = 0;

            //stuff for MLFQ
            proc_set[i].queue_level = 0;
            proc_set[i].remaining_runtime = MLFQ_LEVEL_RUNTIME(proc_set[i].queue_level);

            /* Student's code ends here. */
            return curr_pid;
        }

    FATAL("proc_alloc: reach the limit of %d processes", MAX_NPROCESS);
}

void proc_free(int pid) {
    /* Student's code goes here (Preemptive Scheduler). */

    /* Print the lifecycle statistics of the terminated process or processes. */
    if (pid != GPID_ALL) {
        earth->mmu_free(pid);
        proc_set_status(pid, PROC_UNUSED);

        for (uint i = 0; i<MAX_NPROCESS; i++){
            if (proc_set[i].pid == pid){

                ulonglong end_time = mtime_get();

                proc_set[i].turnaround_time = end_time - proc_set[i].start_time;
                printf("Turnaround Time: %u \nResponse Time: %u \nCPU Time: %u \nNo. of Timer Interrupts: %u \n", (uint) proc_set[i].turnaround_time, (uint) proc_set[i].response_time, (uint) proc_set[i].cpu_time, proc_set[i].n_timer_interrupts);

                break;
            }
        }
    } else {
        /* Free all user processes. */
        for (uint i = 0; i < MAX_NPROCESS; i++)
            if (proc_set[i].pid >= GPID_USER_START &&
                proc_set[i].status != PROC_UNUSED) {
                earth->mmu_free(proc_set[i].pid);
                proc_set[i].status = PROC_UNUSED;

                ulonglong end_time = mtime_get();

                proc_set[i].turnaround_time = end_time - proc_set[i].start_time;
                printf("Turnaround Time: %u \nResponse Time: %u \nCPU Time: %u \nNo. of Timer Interrupts: %u \n", (uint) proc_set[i].turnaround_time, (uint) proc_set[i].response_time, (uint) proc_set[i].cpu_time, proc_set[i].n_timer_interrupts);
            }
    }

    /* Student's code ends here. */
}

void mlfq_update_level(struct process* p, ulonglong runtime) {
    /* Student's code goes here (Preemptive Scheduler). */

    /* Update the MLFQ-related fields in struct process* p after this
     * process has run on the CPU for another runtime microseconds. */

    if (runtime >= p->remaining_runtime){
        //process has burned through its remaining time on current level and needs to be demoted
        if ((p -> queue_level) < 4){
            (p->queue_level)++;
        }
        p -> remaining_runtime = MLFQ_LEVEL_RUNTIME(p->queue_level);
    } else {
        p -> remaining_runtime -= runtime;
    }
    /* Student's code ends here. */
}

void mlfq_reset_level() {
    /* Student's code goes here (Preemptive Scheduler). */
    if (!earth->tty_input_empty()) {
        /* Reset the level of GPID_SHELL if there is pending keyboard input. */
        proc_set[GPID_SHELL].queue_level = 0;
        proc_set[GPID_SHELL].remaining_runtime = MLFQ_LEVEL_RUNTIME(proc_set[GPID_SHELL].queue_level);
    }

    static ulonglong MLFQ_last_reset_time = 0;
    ulonglong current_time = mtime_get();

    /* Reset the level of all processes every MLFQ_RESET_PERIOD microseconds. */
    if (current_time - MLFQ_last_reset_time > MLFQ_RESET_PERIOD) {
        for (int i = 1; i <= MAX_NPROCESS; i++) {
            if (proc_set[i].status != PROC_UNUSED) {
                proc_set[i].queue_level = 0;
                proc_set[i].remaining_runtime = MLFQ_LEVEL_RUNTIME(0);
            }
        }
        MLFQ_last_reset_time = current_time; // Update the last reset time
    }
    /* Student's code ends here. */
}

void proc_sleep(int pid, uint usec) {
    /* Student's code goes here (System Call & Protection). */

    /* Update the sleep-related fields in the struct process for process pid. */

    /* Student's code ends here. */
}

void proc_coresinfo() {
    /* Student's code goes here (Multicore & Locks). */

    /* Print out the pid of the process running on each CPU core. */

    /* Student's code ends here. */
}
