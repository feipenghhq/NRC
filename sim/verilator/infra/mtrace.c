// ------------------------------------------------------------------------------------------------
// Copyright (c) 2023. Heqing Huang (feipenghhq@gmail.com)
//
// Project: NRC
// Author: Heqing Huang
// Date Created: 01/03/2024
//
// ------------------------------------------------------------------------------------------------

#include <string.h>
#include "mtrace.h"

#ifdef CONFIG_MTRACE

#define MSG_LEN CONFIG_MRINGBUF_SIZE

extern FILE *mtrace_fp;
extern word_t dpi_mem_access_pc;
static ringbuf *rb = NULL;

void mtrace_init() {
    rb = ringbuf_create(CONFIG_MRINGBUF_ENTRY, CONFIG_MRINGBUF_SIZE);
}

void mtrace_close() {
    ringbuf_delete(rb);
}

void mtrace_write(word_t addr, word_t data, word_t strb, bool is_write, bool ifetch) {
    char msg[MSG_LEN];
    bool in_range = (addr >= CONFIG_MTRACE_START) && (addr <= CONFIG_MTRACE_END);
    if (!ifetch && in_range) {
        snprintf(msg, MSG_LEN, "%s: at addr: 0x%08x. data: 0x%08x. strb: 0x%x (@0x%08x)",
                is_write ? "write" : " read", addr, data, strb, dpi_mem_access_pc);
    #ifdef CONFIG_MTRACE_WRITE_LOG
        fprintf(mtrace_fp, "%s\n", msg);
        fflush(mtrace_fp);
    #endif
        ringbuf_write(rb, msg);
    }
}

void mtrace_print() {
    Log("Memory sequence to error instruction (Dump from mringbuf):\n");
    ringbuf_print(rb);
}

#undef MSG_LEN

#endif
