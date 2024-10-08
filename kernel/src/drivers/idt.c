#include "../../include/drivers/idt.h"
#include <string.h>

#define IDT_SIZE 256

extern void keyboard_interrupt_handler();

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idt_ptr;

struct terminal* global_term;

void idt_set_gate(int n, uint64_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // kernel code segment
    idt[n].ist = 0;
    idt[n].type_attr = 0x8E; // interrupt gate
    idt[n].offset_mid = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].zero = 0;
}

void load_idt(struct idt_ptr* idt_ptr) {
    asm volatile ("lidt (%0)" : : "r"(idt_ptr)); // load idt
    asm volatile ("sti"); // enable interrupts
}

void idt_install(struct terminal* term) {
    idt_ptr.limit = sizeof(struct idt_entry) * IDT_SIZE - 1;
    idt_ptr.base = (uint64_t)&idt;

    memset(&idt, 0, sizeof(struct idt_entry) * IDT_SIZE);

    global_term = term;

    idt_set_gate(33, (uint64_t)keyboard_interrupt_handler); // IRQ1

    load_idt(&idt_ptr);
}