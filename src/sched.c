#include "common.h"
#include "sched.h"
#include "irq.h"
#include "debug.h"
#include "fork.h"
#include "utils.h"
#include "mm.h"

static struct task_struct init_task = INIT_TASK;

struct task_struct *current = &(init_task);
struct task_struct *task[NUM_TASKS] = {&(init_task), };

i32 num_tasks = 1;

void preempt_disable(void){
    current->preempt_count++;
}

void preempt_enable(void){
    current->preempt_count--;
}

void _schedule(void) {
    preempt_disable();

    i32 next, c;
    struct task_struct *p;

    while (1) {
        c = -1;
        next = 0;

        // Find the task in TASK_RUNNING state w/ the maximum counter value
        for (int i = 0; i < NUM_TASKS; i++) {
            p = task[i];
            if (p && p->state == TASK_RUNNING && p->counter > c) {
                c = p->counter;
                next = i;
            }
        }
        if (c) {
            break;
        }

        // If no tasks are running, halve all of the counters and increment by priority
        for (int i = 0; i < NUM_TASKS; i++) {
            p = task[i];
            if (p){
                p->counter = (p->counter >> 1) + p->priority;  // NOTE: x >> 1 is a halving operation
            }
        }
    }
    switch_to(task[next], next);

    preempt_enable();
}

void schedule(void) {
    current->counter = 0;
    _schedule();
}

void switch_to(struct task_struct * next, u32 index) {

    // TODO: Try moving prev decl here

    if (current == next) {
        return;
    }

    struct task_struct *prev = current;
    current = next;

    cpu_switch_to(prev, next);
}

void schedule_tail(void){
    preempt_enable();
}

void timer_tick() {
    --current->counter;

    if (current->counter > 0 || current->preempt_count > 0) {
        return;
    }

    current->counter = 0;

    irq_enable();
    _schedule();
    irq_disable();
}

void exit_process() {
    preempt_disable();

    for (int i = 0; i < NUM_TASKS; i++) {
        if (task[i] == current) {
            task[i]->state = TASK_ZOMBIE;
            break;
        }
    }
    if (current->stack) {
        free_page(current->stack);
    }

    preempt_enable();
    schedule();
}
