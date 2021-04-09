#include "mm.h"
#include "sched.h"
#include "entry.h"
#include "debug.h"

int copy_process(u64 fn, u64 arg) {
    preempt_disable();

    // printf("\n");
    // DEBUG("\t\t0x%X", &current);
    // DEBUG("\t\t0x%X", current);
    // printf("\n");
    // DEBUG("0x%x", current->cpu_context.x19);
    // DEBUG("0x%x", current->cpu_context.x20);
    // DEBUG("0x%x", current->cpu_context.x21);
    // DEBUG("0x%x", current->cpu_context.x22);
    // DEBUG("0x%x", current->cpu_context.x23);
    // DEBUG("0x%x", current->cpu_context.x24);
    // DEBUG("0x%x", current->cpu_context.x25);
    // DEBUG("0x%x", current->cpu_context.x26);
    // DEBUG("0x%x", current->cpu_context.x27);
    // DEBUG("0x%x", current->cpu_context.x28);
    // DEBUG("0x%x", current->cpu_context.fp);
    // DEBUG("0x%x", current->cpu_context.sp);
    // DEBUG("0x%x", current->cpu_context.pc);
    // printf("\n");
    // DEBUG("\t\t0x%X", current->state);
    // DEBUG("\t0x%X", current->counter);
    // DEBUG("\t0x%X", current->priority);
    // DEBUG("\t0x%X", current->preempt_count);

    struct task_struct *p;
    int pid;

    p = (struct task_struct *)get_free_page();

    if (!p) {
        return 1;
    }

    // DEBUG("0x%X", current->priority);

    p->priority = current->priority;

    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1;

    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
    p->cpu_context.pc = (u64)ret_from_fork;
    p->cpu_context.sp = (u64)p + THREAD_SIZE;

    pid = num_tasks++;
    task[pid] = p;

    preempt_enable();

    return 0;
}
