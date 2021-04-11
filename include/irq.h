#pragma once

void enable_interrupt_controller(void);

void irq_init_vectors(void);
void irq_enable(void);
void irq_disable(void);
