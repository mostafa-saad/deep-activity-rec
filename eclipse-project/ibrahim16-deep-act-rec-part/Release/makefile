################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include apps/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ibrahim16-deep-act-rec-part

# Tool invocations
ibrahim16-deep-act-rec-part: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L/rcg/software/Linux/RHEL/6/x86_64/LIB/OPENCV/3.0.0-CUDA65/lib -L/cs/vml2/msibrahi/workspaces/caffe-lstm/build/lib -L/usr/local/lib -L/usr/lib -L/usr/local/cuda-6.5/lib64 -L/rcg/software/Linux/RHEL/6/x86_64/LANG/PYTHON/2.7.6-SYSTEM/lib -L/rcg/software/Linux/RHEL/6/x86_64/LIB/GLOG/0.3.3/lib -L/rcg/software/Linux/RHEL/6/x86_64/LIB/BOOST/1.57.0/lib -o "ibrahim16-deep-act-rec-part" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) ibrahim16-deep-act-rec-part
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
