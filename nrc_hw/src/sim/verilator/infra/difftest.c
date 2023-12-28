// ------------------------------------------------------------------------------------------------
// Copyright (c) 2023. Heqing Huang (feipenghhq@gmail.com)
//
// Project: NRC
// Author: Heqing Huang
// Date Created: 12/27/2023
// ------------------------------------------------------------------------------------------------
// difftest: Diff test
// ------------------------------------------------------------------------------------------------

#include <dlfcn.h>
#include "common.h"
#include "config.h"

#ifdef CONFIG_DIFFTEST

// ------------------------------------
// Function prototype, global variable
// ------------------------------------

byte_t *mem_ptr();
void init_ref(size_t mem_size);
const char *reg_id2str(int id);

static void *lib = NULL;

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

typedef void (*difftest_init_t)(int port);
typedef void (*difftest_memcpy_t) (uint32_t addr, void *buf, size_t n, bool direction);
typedef void (*difftest_exec_t)(uint64_t n);
typedef void (*difftest_regcpy_t)(void *reg, bool direction);

// ------------------------------------
// Functions
// ------------------------------------

void init_difftest(char *ref, size_t mem_size) {
    lib = dlopen(ref, RTLD_NOW);
    Check(lib, "Failed to open shared lib: %s. %s.", ref, dlerror());
    init_ref(mem_size);
}

#define load_from_so(name) \
    name##_t name; \
    name = (name##_t) dlsym(lib, #name); \
    Check(name, "Failed to load from shared lib")

/**
 * initialize the reference model
 */
void init_ref(size_t mem_size) {
    // difftest_init
    load_from_so(difftest_init);
    difftest_init(0);
    // copy the DUT memory to REF memory
    load_from_so(difftest_memcpy);
    difftest_memcpy(MEM_OFFSET, mem_ptr(), mem_size, DIFFTEST_TO_REF);
}

void ref_exec(uint64_t n) {
    load_from_so(difftest_exec);
    difftest_exec(n);
}

bool difftest_compare(word_t *dut_reg) {
    bool pass = true;
    word_t ref_reg[NUM_REG];
    load_from_so(difftest_regcpy);
    difftest_regcpy(ref_reg, DIFFTEST_TO_DUT);
    for (int i = 0; i < NUM_REG; i++) {
      if (ref_reg[i] != dut_reg[i]) {
        log_err("difftest: Register Value mismatch on reg %s.\nRef: 0x%08x. Dut: 0x%08x",
          reg_id2str(i), ref_reg[i], dut_reg[i]);
        pass = false;
      }
    }
    return pass;
}

#endif
