/* ------------------------------------------------------------------------------------------------
 * Copyright (c) 2023. Heqing Huang (feipenghhq@gmail.com)
 *
 * Project: NRC
 * Author: Heqing Huang
 * Date Created: 12/19/2023
 *
 * ------------------------------------------------------------------------------------------------
 *  Core_sTop class: Provide environment for Core_s CPU design
 * ------------------------------------------------------------------------------------------------
 */

#ifndef __CORES_ENV_H__
#define __CORES_ENV_H__

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcore_s.h"
#include "common.h"
#include "Top.h"

class Core_sTop: public Top {

private:
    Vcore_s *top;
    const int reset_cycle = 10;

public:
    Core_sTop(int argc, char *argv[], const char *name, bool trace);
    ~Core_sTop();

    virtual void init_trace(const char *name, int level);
    virtual void reset();
    virtual void clk_tick();
    virtual bool run(int step);
    virtual void report();
};

#endif
