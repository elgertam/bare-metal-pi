#include "mm.h"
#include "sched.h"
#include "debug.h"
#include "fork.h"
#include "entry.h"
#include "utils.h"


i32 copy_process(u64 clone_flags, u64 fn, u64 arg, u64 stack) {
    preempt_disable();

    struct task_struct *p;
    i32 pid;

    p = (struct task_struct *)get_free_page();
    if (!p) {
        return -1;
    }

    struct pt_regs *childregs = task_pt_regs(p);
    memzero((u64)childregs, sizeof(struct pt_regs));
    memzero((u64)&p->cpu_context, sizeof(struct cpu_context));

    if (clone_flags & PF_KTHREAD) {
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;
    } else {
        struct pt_regs * cur_regs = task_pt_regs(current);
        *childregs = *cur_regs;
        childregs->regs[0] = 0;
        childregs->sp = stack + PAGE_SIZE;
        p->stack = stack;
    }

    p->flags = clone_flags;
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1;

    p->cpu_context.pc = (u64)ret_from_fork;
    p->cpu_context.sp = (u64)childregs;

    pid = num_tasks++;
    task[pid] = p;

    preempt_enable();

    return pid;
}

i32 move_to_user_mode(u64 pc) {
    struct pt_regs *regs = task_pt_regs(current);
    memzero((u64)regs, sizeof(*regs));
    regs->pc = pc;
    regs->pstate = PSR_MODE_EL0t;

    u64 stack = get_free_page();
    if (!stack){
        return -1;
    }

    regs->sp = stack + PAGE_SIZE;
    current->stack = stack;

    return 0;
}

struct pt_regs * task_pt_regs(struct task_struct *tsk) {
    u64 p = (u64)tsk + THREAD_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs *)p;
}
