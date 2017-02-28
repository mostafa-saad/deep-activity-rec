################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../apps/exePhase3.cpp 

OBJS += \
./apps/exePhase3.o 

CPP_DEPS += \
./apps/exePhase3.d 


# Each subdirectory must supply rules for building sources it contributes
apps/%.o: ../apps/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/rcg/software/Linux/RHEL/6/x86_64/LIB/OPENCV/3.0.0-CUDA65/include -I/usr/include/openblas -I/cs/vml2/msibrahi/workspaces/caffe-lstm/include -I/cs/vml2/msibrahi/workspaces/caffe-lstm/build/src -I/cs/vml2/msibrahi/workspaces/software/dlib -I/usr/local/cuda-6.5/include -I/rcg/software/Linux/RHEL/6/x86_64/LIB/BOOST/1.57.0/include -I/rcg/software/Linux/RHEL/6/x86_64/LIB/GLOG/0.3.3/include -I/rcg/software/Linux/RHEL/6/x86_64/LANG/PYTHON/2.7.6-SYSTEM/include/python2.7 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


