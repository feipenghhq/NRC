# ------------------------------------------------------------------------------------------------
# Copyright (c) 2023. Heqing Huang (feipenghhq@gmail.com)
#
# Project: NRC
# Author: Heqing Huang
# Date Created: 12/16/2023
# ------------------------------------------------------------------------------------------------

# path
_PATH = $(shell realpath src/sim/verilator)

# Folder
_DIR = testbench memory include

# Verilator cpp/c source file
CPP_SRCS += $(foreach folder,$(_DIR), $(shell find $(_PATH)/$(folder) -name "*.cpp"))
CPP_SRCS += $(foreach folder,$(_DIR), $(shell find $(_PATH)/$(folder) -name "*.c"))

# main function file
CPP_SRCS += $(_PATH)/tests/$(TOP)_main.cpp

# Verilator include directory
CPP_INCS += $(foreach folder,$(_DIR), $(addprefix $(_PATH)/,$(folder)))
