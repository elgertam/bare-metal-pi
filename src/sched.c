#include "sched.h"
#include "irq.h"
#include "printf.h"

static struct task_struct init_task = INIT_TASK;

struct task_struct *current = &(init_task);
struct task_struct *task[NUM_TASKS] = {&(init_task), };

int num_tasks = 1;

void preempt_disable(void){
    current->preempt_count++;
}

void preempt_enable(void){
    current->preempt_count--;
}

void _schedule(void) {
    preempt_disable();

    int next, c;
    struct task_struct *p;

    while (1) {
        c = -1;
        next = 0;

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

        for (int i = 0; i < NUM_TASKS; i ++) {
            p = task[i];
            if (p){
                p->counter = (p->counter >> 1) + p->priority;  // NOTE: x >> 1 is a halving operation
            }
        }
    }
    switch_to(task[next]);

    preempt_enable();
}

void schedule(void) {
    current->counter = 0;
    _schedule();
}

void switch_to(struct task_struct * next) {
    struct task_struct *prev;

    if (current == next) {
        return;
    }

    prev = current;
    current = next;

    cpu_switch_to(prev, next);
}

void schedule_tail(void){
    preempt_enable();
}

void timer_tick() {
    --current->counter;

    printf("\n\tIn timer_tick\tcurrent->counter: %d\tcurrent->preempt_count: %d\n", current->counter, current->preempt_count);

    if (current->counter <= 0 && current->preempt_count <= 0) {
        printf("timer_tick\tif\n");
        current->counter = 0;

        irq_enable();
        _schedule();
        irq_disable();

    }
}