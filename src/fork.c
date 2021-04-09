#include "mm.h"
#include "sched.h"
#include "entry.h"

int copy_process(u64 fn, u64 arg) {
    preempt_disable();

    struct task_struct *p;
    int pid;

    p = (struct task_struct *)get_free_page();

    if (!p) {
        return 1;
    }

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
