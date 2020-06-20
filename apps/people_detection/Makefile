# nRF application makefile

PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52840
SDK_VERSION = 15
SOFTDEVICE_MODEL = s140
#USE_MBR=0

#SDK_VARS += __STACK_SIZE=0x10000 __HEAP_SIZE=0x25000

MAKEFILE_DIR := tensorflow/lite/micro/tools/make

INCLUDES := \
 . \
 $(MAKEFILE_DIR)/downloads/ \
 $(MAKEFILE_DIR)/downloads/gemmlowp \
 $(MAKEFILE_DIR)/downloads/flatbuffers/include \
 ./third_party \
 ./tensorflow/lite/micro/tools/make/downloads/gcc_embedded//arm-none-eabi \
 ./tensorflow/lite/micro/tools/make/downloads/kissfft\
 ./tensorflow/lite/micro/tools/make/downloads/person_model_int8/
 
# Source and header files
#APP_HEADER_PATHS += .
#APP_HEADER_PATHS += ./third_party/gemmlowp
#APP_HEADER_PATHS += ./third_party/flatbuffers/include
APP_HEADER_PATHS += $(INCLUDES)


APP_SOURCE_PATHS += .

#TF_SOURCE_PATHS = /tensorflow/lite/micro/
#APP_SOURCE_PATHS += $(addprefix $(TENSORFLOW_ACTUALPATH), $(TF_SOURCE_PATHS))
#APP_SOURCE_PATHS_HELLO_WORLD = ./tensorflow/lite/micro/ ./tensorflow/lite/micro/kernels/ ./tensorflow/lite/micro/examples/hello_world/ ./tensorflow/lite/core/api/ ./tensorflow/lite/kernels/internal/ ./tensorflow/lite/micro/memory_planner/ ./tensorflow/lite/c ./tensorflow/lite/kernels
 
APP_SOURCE_PATHS_PERSON_DETECTION = \
	./tensorflow/lite/micro/\
	./tensorflow/lite/micro/kernels/\
	./tensorflow/lite/core/api/\
	./tensorflow/lite/kernels/internal/\
	./tensorflow/lite/micro/memory_planner/\
	./tensorflow/lite/c\
	./tensorflow/lite/kernels\
	./tensorflow/lite/micro/examples/person_detection/\
	./tensorflow/lite/micro/tools/make/downloads/person_model_int8/\
	./tensorflow/lite/micro/tools/make/downloads/person_model_int8/images/96x96/\
	./tensorflow/lite/micro/tools/make/downloads/person_model_int8/images/192x192/\
	./tensorflow/lite/micro/tools/make/downloads/person_model_int8/images/48x48/\
	./tensorflow/lite/micro/tools/make/downloads/person_model_int8/images/120x120/\
	./tensorflow/lite/micro/tools/make/downloads/person_model_int8/images/72x72/\

APP_SOURCE_PATHS += $(APP_SOURCE_PATHS_PERSON_DETECTION)

APP_SOURCES = $(notdir $(wildcard ./*.c))
APP_SOURCES += $(notdir $(wildcard ./*.cc))
 
TF_SOURCES_PERSON_DETECTION = \
	tensorflow/lite/micro/simple_memory_allocator.cc\
	tensorflow/lite/micro/memory_helpers.cc\
	tensorflow/lite/micro/micro_utils.cc\
	tensorflow/lite/micro/micro_error_reporter.cc\
	tensorflow/lite/micro/debug_log_numbers.cc\
	tensorflow/lite/micro/micro_allocator.cc\
	tensorflow/lite/micro/micro_optional_debug_tools.cc\
	tensorflow/lite/micro/debug_log.cc\
	tensorflow/lite/micro/test_helpers.cc\
	tensorflow/lite/micro/micro_interpreter.cc\
	tensorflow/lite/micro/kernels/logical.cc\
	tensorflow/lite/micro/kernels/elementwise.cc\
	tensorflow/lite/micro/kernels/comparisons.cc\
	tensorflow/lite/micro/kernels/depthwise_conv.cc\
	tensorflow/lite/micro/kernels/split.cc\
	tensorflow/lite/micro/kernels/logistic.cc\
	tensorflow/lite/micro/kernels/strided_slice.cc\
	tensorflow/lite/micro/kernels/prelu.cc\
	tensorflow/lite/micro/kernels/softmax.cc\
	tensorflow/lite/micro/kernels/dequantize.cc\
	tensorflow/lite/micro/kernels/pack.cc\
	tensorflow/lite/micro/kernels/activations.cc\
	tensorflow/lite/micro/kernels/ceil.cc\
	tensorflow/lite/micro/kernels/arg_min_max.cc\
	tensorflow/lite/micro/kernels/pad.cc\
	tensorflow/lite/micro/kernels/conv.cc\
	tensorflow/lite/micro/kernels/unpack.cc\
	tensorflow/lite/micro/kernels/fully_connected.cc\
	tensorflow/lite/micro/kernels/add.cc\
	tensorflow/lite/micro/kernels/pooling.cc\
	tensorflow/lite/micro/kernels/floor.cc\
	tensorflow/lite/micro/kernels/neg.cc\
	tensorflow/lite/micro/kernels/svdf.cc\
	tensorflow/lite/micro/kernels/concatenation.cc\
	tensorflow/lite/micro/kernels/mul.cc\
	tensorflow/lite/micro/kernels/round.cc\
	tensorflow/lite/micro/kernels/quantize.cc\
	tensorflow/lite/micro/kernels/all_ops_resolver.cc\
	tensorflow/lite/micro/kernels/maximum_minimum.cc\
	tensorflow/lite/micro/kernels/reshape.cc\
	tensorflow/lite/micro/kernels/reduce.cc\
	tensorflow/lite/micro/memory_planner/linear_memory_planner.cc\
	tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc\
	tensorflow/lite/c/common.c\
	tensorflow/lite/core/api/error_reporter.cc\
	tensorflow/lite/core/api/flatbuffer_conversions.cc\
	tensorflow/lite/core/api/op_resolver.cc\
	tensorflow/lite/core/api/tensor_utils.cc\
	tensorflow/lite/kernels/internal/quantization_util.cc\
	tensorflow/lite/kernels/kernel_util.cc\
	tensorflow/lite/micro/examples/person_detection/detection_responder.cc\
	tensorflow/lite/micro/examples/person_detection/image_provider.cc\
	tensorflow/lite/micro/examples/person_detection/main.cc\
	tensorflow/lite/micro/examples/person_detection/main_functions.cc\
	tensorflow/lite/micro/examples/person_detection/model_settings.cc\
	tensorflow/lite/micro/examples/person_detection/person_detect_model_data.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/96x96/person_image_data.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/96x96/no_person_image_data.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/96x96/man_image_data.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/48x48/lady_image_data_48.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/48x48/pineapple_image_data_48.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/48x48/man_image_data_48.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/48x48/indian_image_data_48.cc\
	tensorflow/lite/micro/tools/make/downloads/person_image_int8/images/48x48/meninshower_image_data_48.cc\
	tensorflow/lite/micro/examples/person_detection/virtual_timer.c 


APP_SOURCES += $(addprefix ./, $(TF_SOURCES_PERSON_DETECTION))


# Path to base of nRF52-base repo
NRF_BASE_DIR = ../../

# Include board Makefile (if any)
include ../../boards/nrf52840dk/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)make/AppMakefile.mk

list:
	@echo "PROJECT FILE: $(PROJECT_NAME)" 
	@echo "PROJECT NAME: $(OUTPUT_NAME)"
	@echo "SOFT DEVICE MODEL: $(SOFTDEVICE_MODEL)" 
	@echo "DETAILS: $(addsuffix _sdk$(SDK_VERSION)_$(SOFTDEVICE_MODEL), $(PROJECT_NAME))"
	@echo "USE MBR: $(USE_MBR)"
	@echo "MBR PATH: $(MBR_PATH)" 


