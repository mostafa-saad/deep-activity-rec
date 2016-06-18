USE_CPU := -DCPU_ONLY=0

# TODO: Update next 2 directories
CAFFE_LSTM_DIR := <SomePath1>/caffe-lstm
DLIB_DIR := <SomePath2>/dlib

RM := rm -rf
CC := g++
CC_OPTIONS = -std=gnu++0x -Wall -c -fmessage-length=0 -O3 $(USE_CPU)
CFLAGS = -fPIC $(CC_OPTIONS)

# TODO: Add/Remove if needed (e.g. Opencv directories)
INCS_DIRS := -I$(CAFFE_LSTM_DIR)/include -I$(CAFFE_LSTM_DIR)/build/src -I$(DLIB_DIR)
#             -I<SomePath3>/LIB/OPENCV/3.0.0-CUDA65/include    \
#             -I/usr/include/openblas    \
#             -I/usr/local/cuda-6.5/include    \
#             -I<SomePath3>/LIB/BOOST/1.57.0/include    \
#             -I<SomePath3>/LIB/GLOG/0.3.3/include    \
#             -I<SomePath3>/LANG/PYTHON/2.7.6-SYSTEM/include/python2.7

LIBS_DIRS := -L$(DLIB_DIR) -L$(CAFFE_LSTM_DIR)/build/lib
#             -L<SomePath3>/LIB/OPENCV/3.0.0-CUDA65/lib    \
#             -L/usr/lib    \
#             -L/usr/local/cuda-6.5/lib64    \
#             -L<SomePath3>/LANG/PYTHON/2.7.6-SYSTEM/lib    \
#             -L<SomePath3>/LIB/GLOG/0.3.3/lib    \
#             -L<SomePath3>/LIB/BOOST/1.57.0/lib    \
#             -L/cs/vml2/msibrahi/workspaces/software/dlib/examples/build/dlib_build

LIBS := -lboost_system -lboost_filesystem -lboost_chrono -lboost_python            \
        -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml                \
        -lpython2.7 -lleveldb -lprotobuf -lgflags -lglog -pthread -lcaffe -ldlib

############################################################


SRC_BASE := src
SRC_BASE_OUT = release
APP_BASE := apps
APP_BASE_OUT = apps-release

SRCS := $(wildcard  $(SRC_BASE)/*.cpp)
OBJS := $(addprefix $(SRC_BASE_OUT)/, $(patsubst %.cpp,%.o,$(notdir $(SRCS))))
DEPS := $(addprefix $(SRC_BASE_OUT)/, $(patsubst %.cpp,%.d,$(notdir $(SRCS))))

ifneq ($(MAKECMDGOALS),clean)
	ifneq ($(strip $(DEPS)),)
		-include $(DEPS)
	endif
endif

TARGET1 = exePhase1_2
TARGET2 = exePhase3
TARGET3 = exePhase4

all:
	mkdir -p $(SRC_BASE_OUT)
	mkdir -p $(APP_BASE_OUT) 
	$(MAKE) $(MAKEFILE) $(TARGET1)
	$(MAKE) $(MAKEFILE) $(TARGET2)
	$(MAKE) $(MAKEFILE) $(TARGET3)

$(SRC_BASE_OUT)/%.o: $(SRC_BASE)/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) $(CFLAGS) $(INCS_DIRS) -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(APP_BASE_OUT)/%.o: $(APP_BASE)/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CC) $(CFLAGS) $(INCS_DIRS) -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TARGET1): $(OBJS) $(APP_BASE_OUT)/$(TARGET1).o
	@echo 'Building TARGET1: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LIBS_DIRS) -o $(TARGET1) $(SRC_BASE_OUT)/*.o $(APP_BASE_OUT)/$(TARGET1).o $(LIBS)
	@echo 'Finished building TARGET1: $@'
	@echo ' '
	
$(TARGET2): $(OBJS) $(APP_BASE_OUT)/$(TARGET2).o
	@echo 'Building TARGET2: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LIBS_DIRS) -o $(TARGET2) $(SRC_BASE_OUT)/*.o $(APP_BASE_OUT)/$(TARGET2).o $(LIBS)
	@echo 'Finished building TARGET2: $@'
	@echo ' '
	
$(TARGET3): $(OBJS) $(APP_BASE_OUT)/$(TARGET3).o
	@echo 'Building TARGET3: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CC) $(LIBS_DIRS) -o $(TARGET3) $(SRC_BASE_OUT)/*.o $(APP_BASE_OUT)/$(TARGET3).o $(LIBS)
	@echo 'Finished building TARGET3: $@'
	@echo ' '	

clean:
	-$(RM) $(SRC_BASE_OUT)
	-$(RM) $(APP_BASE_OUT)
	-$(RM) $(TARGET1)
	-$(RM) $(TARGET2)
	-$(RM) $(TARGET3)

.PHONY: clean all
