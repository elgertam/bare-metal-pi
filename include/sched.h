#pragma once

#define THREAD_CPU_CONTEXT  0

#ifndef __ASSEMBLER__

#include "common.h"

#define THREAD_SIZE         4096

#define NUM_TASKS            64

#define FIRST_TASK          task[0]
#define LAST_TASK           task[NUM_TASKS-1]

enum TaskState {
    TASK_RUNNING = 0,
    TASK_ZOMBIE = 1
};

#define PF_KTHREAD          0x00000002

extern struct task_struct *current;
extern struct task_struct *task[NUM_TASKS];
extern i32 num_tasks;

struct cpu_context {
    u64 x19;
    u64 x20;
    u64 x21;
    u64 x22;
    u64 x23;
    u64 x24;
    u64 x25;
    u64 x26;
    u64 x27;
    u64 x28;
    u64 fp;
    u64 sp;
    u64 pc;
};

struct task_struct {
    struct cpu_context cpu_context;
    enum TaskState state;
    i64 counter;
    i64 priority;
    i64 preempt_count;
    u64 stack;
    u64 flags;
};

extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct* next, u32 index);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern void exit_process(void);

/*      cpu_context=x19 20 21 22 23 24 25 26 27 28 fp sp pc  state         c  p  prec stk  flags      */
#define INIT_TASK { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, TASK_RUNNING, 0, 1, 0,   0,   PF_KTHREAD }

#endif
