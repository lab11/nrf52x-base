# nRF application makefile

PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52840
SDK_VERSION = 15
SOFTDEVICE_MODEL = s132


TENSORFLOW_ACTUALPATH=/home/ajay/Desktop/MastersProject/tensorflowlite/tensorflow

MAKEFILE_DIR := tensorflow/lite/micro/tools/make

INCLUDES := \
 . \
 $(MAKEFILE_DIR)/downloads/ \
 $(MAKEFILE_DIR)/downloads/gemmlowp \
 $(MAKEFILE_DIR)/downloads/flatbuffers/include \
 ./third_party \
 ./tensorflow/lite/micro/tools/make/downloads/gcc_embedded//arm-none-eabi \
 ./tensorflow/lite/micro/tools/make/downloads/kissfft

# Source and header files
#APP_HEADER_PATHS += .
#APP_HEADER_PATHS += ./third_party/gemmlowp
#APP_HEADER_PATHS += ./third_party/flatbuffers/include
APP_HEADER_PATHS += $(INCLUDES)


APP_SOURCE_PATHS += .
#APP_SOURCE_PATHS += ./tensorflow/lite/experimental/micro\
./tensorflow/lite/c\
./tensorflow/lite/core/api\
./tensorflow/lite/kernels\
./tensorflow/lite/experimental/micro/kernels\
./tensorflow/lite/kernels/internal/\

#TF_SOURCE_PATHS = /tensorflow/lite/micro/
#APP_SOURCE_PATHS += $(addprefix $(TENSORFLOW_ACTUALPATH), $(TF_SOURCE_PATHS))
APP_SOURCE_PATHS += ./tensorflow/lite/micro/ ./tensorflow/lite/micro/kernels/ ./tensorflow/lite/micro/examples/hello_world/ ./tensorflow/lite/core/api/ ./tensorflow/lite/kernels/internal/ ./tensorflow/lite/micro/memory_planner/ ./tensorflow/lite/c ./tensorflow/lite/kernels 

APP_SOURCES = $(notdir $(wildcard ./*.c))
APP_SOURCES += $(notdir $(wildcard ./*.cc))
#APP_SOURCES += ./tensorflow/lite/experimental/micro/micro_error_reporter.cc\
./tensorflow/lite/experimental/micro/micro_mutable_op_resolver.cc\
./tensorflow/lite/experimental/micro/simple_tensor_allocator.cc\
./tensorflow/lite/experimental/micro/debug_log.cc\
./tensorflow/lite/experimental/micro/debug_log_numbers.cc\
./tensorflow/lite/experimental/micro/micro_interpreter.cc\
./tensorflow/lite/experimental/micro/kernels/depthwise_conv.cc\
./tensorflow/lite/experimental/micro/kernels/softmax.cc\
./tensorflow/lite/experimental/micro/kernels/all_ops_resolver.cc\
./tensorflow/lite/experimental/micro/kernels/fully_connected.cc\
./tensorflow/lite/c/c_api_internal.c\
./tensorflow/lite/core/api/error_reporter.cc\
./tensorflow/lite/core/api/flatbuffer_conversions.cc\
./tensorflow/lite/core/api/op_resolver.cc\
./tensorflow/lite/kernels/kernel_util.cc\
./tensorflow/lite/kernels/internal/quantization_util.cc\
./tensorflow/lite/experimental/micro/simple_tensor_allocator_test.cc
#APP_SOURCES += ./tensorflow/lite/experimental/micro/micro_error_reporter.cc ./tensorflow/lite/experimental/micro/simple_tensor_allocator.cc ./tensorflow/lite/experimental/micro/debug_log.cc ./tensorflow/lite/experimental/micro/debug_log_numbers.cc ./tensorflow/lite/experimental/micro/micro_interpreter.cc ./tensorflow/lite/experimental/micro/kernels/depthwise_conv.cc ./tensorflow/lite/experimental/micro/kernels/softmax.cc ./tensorflow/lite/experimental/micro/kernels/all_ops_resolver.cc ./tensorflow/lite/experimental/micro/kernels/fully_connected.cc ./tensorflow/lite/c/c_api_internal.c ./tensorflow/lite/core/api/error_reporter.cc ./tensorflow/lite/core/api/flatbuffer_conversions.cc ./tensorflow/lite/core/api/op_resolver.cc ./tensorflow/lite/kernels/kernel_util.cc ./tensorflow/lite/kernels/internal/quantization_util.cc  ./tensorflow/lite/experimental/micro/micro_error_reporter_test.cc

#APP_SOURCES += ./tensorflow/lite/micro/micro_error_reporter.cc ./tensorflow/lite/micro/debug_log.cc ./tensorflow/lite/micro/debug_log_numbers.cc ./tensorflow/lite/micro/micro_interpreter.cc ./tensorflow/lite/micro/kernels/depthwise_conv.cc ./tensorflow/lite/micro/kernels/softmax.cc ./tensorflow/lite/micro/kernels/all_ops_resolver.cc ./tensorflow/lite/micro/kernels/fully_connected.cc ./tensorflow/lite/core/api/error_reporter.cc ./tensorflow/lite/core/api/flatbuffer_conversions.cc ./tensorflow/lite/core/api/op_resolver.cc ./tensorflow/lite/kernels/kernel_util.cc ./tensorflow/lite/kernels/internal/quantization_util.cc  ./tensorflow/lite/micro/micro_error_reporter_test.cc

#TF_SOURCES = /tensorflow/lite/micro/micro_error_reporter.cc /tensorflow/lite/micro/micro_interpreter.cc /tensorflow/lite/micro/kernels/all_ops_resolver.cc 
#APP_SOURCES += $(addprefix $(TENSORFLOW_ACTUALPATH), $(TF_SOURCES)) 
#APP_SOURCES += ./tensorflow/lite/micro/examples/hello_world/main.cc ./tensorflow/lite/micro/examples/hello_world/main_functions.cc ./tensorflow/lite/micro/examples/hello_world/output_handler.cc ./tensorflow/lite/micro/examples/hello_world/sine_model_data.cc ./tensorflow/lite/micro/examples/hello_world/constants.cc ./tensorflow/lite/micro/micro_error_reporter.cc ./tensorflow/lite/micro/micro_interpreter.cc ./tensorflow/lite/micro/kernels/all_ops_resolver.cc ./tensorflow/lite/micro/debug_log.cc ./tensorflow/lite/micro/debug_log_numbers.cc ./tensorflow/lite/core/api/error_reporter.cc ./tensorflow/lite/core/api/op_resolver.cc ./tensorflow/lite/kernels/kernel_util.cc ./tensorflow/lite/kernels/internal/quantization_util.cc ./tensorflow/lite/micro/simple_memory_allocator.cc ./tensorflow/lite/micro/memory_helpers.cc ./tensorflow/lite/micro/micro_allocator.cc ./tensorflow/lite/micro/test_helpers.cc ./tensorflow/lite/micro/micro_interpreter.cc ./tensorflow/lite/micro/kernels/logical.cc ./tensorflow/lite/micro/kernels/elementwise.cc ./tensorflow/lite/micro/kernels/comparisons.cc ./tensorflow/lite/micro/kernels/depthwise_conv.cc ./tensorflow/lite/micro/kernels/split.cc ./tensorflow/lite/micro/kernels/logistic.cc ./tensorflow/lite/micro/kernels/strided_slice.cc ./tensorflow/lite/micro/kernels/prelu.cc ./tensorflow/lite/micro/kernels/softmax.cc ./tensorflow/lite/micro/kernels/dequantize.cc ./tensorflow/lite/micro/kernels/pack.cc ./tensorflow/lite/micro/kernels/activations.cc ./tensorflow/lite/micro/kernels/ceil.cc ./tensorflow/lite/micro/kernels/arg_min_max.cc ./tensorflow/lite/micro/kernels/pad.cc ./tensorflow/lite/micro/kernels/conv.cc ./tensorflow/lite/micro/kernels/unpack.cc ./tensorflow/lite/micro/kernels/fully_connected.cc ./tensorflow/lite/micro/kernels/add.cc ./tensorflow/lite/micro/kernels/pooling.cc ./tensorflow/lite/micro/kernels/floor.cc ./tensorflow/lite/micro/kernels/neg.cc ./tensorflow/lite/micro/kernels/svdf.cc ./tensorflow/lite/micro/kernels/concatenation.cc ./tensorflow/lite/micro/kernels/mul.cc ./tensorflow/lite/micro/kernels/round.cc ./tensorflow/lite/micro/kernels/quantize.cc
TF_SOURCES = tensorflow/lite/micro/simple_memory_allocator.cc tensorflow/lite/micro/memory_helpers.cc tensorflow/lite/micro/micro_utils.cc tensorflow/lite/micro/micro_error_reporter.cc tensorflow/lite/micro/debug_log_numbers.cc tensorflow/lite/micro/micro_allocator.cc tensorflow/lite/micro/micro_optional_debug_tools.cc tensorflow/lite/micro/debug_log.cc tensorflow/lite/micro/test_helpers.cc tensorflow/lite/micro/micro_interpreter.cc tensorflow/lite/micro/kernels/logical.cc tensorflow/lite/micro/kernels/elementwise.cc tensorflow/lite/micro/kernels/comparisons.cc tensorflow/lite/micro/kernels/depthwise_conv.cc tensorflow/lite/micro/kernels/split.cc tensorflow/lite/micro/kernels/logistic.cc tensorflow/lite/micro/kernels/strided_slice.cc tensorflow/lite/micro/kernels/prelu.cc tensorflow/lite/micro/kernels/softmax.cc tensorflow/lite/micro/kernels/dequantize.cc tensorflow/lite/micro/kernels/pack.cc tensorflow/lite/micro/kernels/activations.cc tensorflow/lite/micro/kernels/ceil.cc tensorflow/lite/micro/kernels/arg_min_max.cc tensorflow/lite/micro/kernels/pad.cc tensorflow/lite/micro/kernels/conv.cc tensorflow/lite/micro/kernels/unpack.cc tensorflow/lite/micro/kernels/fully_connected.cc tensorflow/lite/micro/kernels/add.cc tensorflow/lite/micro/kernels/pooling.cc tensorflow/lite/micro/kernels/floor.cc tensorflow/lite/micro/kernels/neg.cc tensorflow/lite/micro/kernels/svdf.cc tensorflow/lite/micro/kernels/concatenation.cc tensorflow/lite/micro/kernels/mul.cc tensorflow/lite/micro/kernels/round.cc tensorflow/lite/micro/kernels/quantize.cc tensorflow/lite/micro/kernels/all_ops_resolver.cc tensorflow/lite/micro/kernels/maximum_minimum.cc tensorflow/lite/micro/kernels/reshape.cc tensorflow/lite/micro/kernels/reduce.cc tensorflow/lite/micro/memory_planner/linear_memory_planner.cc tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc tensorflow/lite/c/common.c tensorflow/lite/core/api/error_reporter.cc tensorflow/lite/core/api/flatbuffer_conversions.cc tensorflow/lite/core/api/op_resolver.cc tensorflow/lite/core/api/tensor_utils.cc tensorflow/lite/kernels/internal/quantization_util.cc tensorflow/lite/kernels/kernel_util.cc tensorflow/lite/micro/examples/hello_world/main.cc tensorflow/lite/micro/examples/hello_world/main_functions.cc tensorflow/lite/micro/examples/hello_world/output_handler.cc tensorflow/lite/micro/examples/hello_world/sine_model_data.cc tensorflow/lite/micro/examples/hello_world/constants.cc 
 
APP_SOURCES += $(addprefix ./, $(TF_SOURCES))

#CXXFLAGS += -O3 -DNDEBUG --std=c++11 -g -DTF_LITE_STATIC_MEMORY -fpermissive
#TF_LITE_USE_GLOBAL_ROUND=1

# Path to base of nRF52-base repo
NRF_BASE_DIR = ../../nrf52x-base/

# Include board Makefile (if any)
include ../../boards/buckler_revB/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)make/AppMakefile.mk
#@echo "BUILDDIR: $(BUILDDIR)"
#@echo "ELF: $(ELF)"
#@echo "CFLAGS: $(CFLAGS)"
#@echo "CXXFLAGS: $(CXXFLAGS)"
#@echo "OBJS: $(OBJS)" 
#@echo "VPATH: $(VPATH)"
#@echo "HEADER INCLUDES: $(HEADER_INCLUDES)"
list:
	@echo "PROJECT NAME: $(OUTPUT_NAME)"
	@echo "APP SOURCES: $(APP_SOURCES)"
	@echo "APP SOURCES PATH: $(APP_SOURCE_PATHS)"	
	@echo "APP HEADER PATHS: $(APP_HEADER_PATHS)" 
