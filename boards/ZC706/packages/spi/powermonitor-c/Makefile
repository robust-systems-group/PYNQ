
CC			:= gcc
CXX 		:= g++

SRC_DIR     := src
BUILD_DIR   := .build
INCLUDE_DIR := include
TEST_DIR 	:= tests
PYTHON_DIR 	:= python-tests
BIN_DIR 	:= bin

# various src definitions for easily building tests or the main app
_HEADERS 	:= INA229.h PowerMonitor.pb.h PowerMonitorChannel.h PowerDataStreamer.h PowerMonitor.h 
HEADERS		:= $(patsubst %, $(INCLUDE_DIR)/%, $(_HEADERS))

_DRIVER_SRC		:= INA229.c
DRIVER_SRC		:= $(patsubst %, $(SRC_DIR)/%, $(_DRIVER_SRC))

_APP_SRC    := main.cpp
APP_SRC     := $(patsubst %, $(SRC_DIR)/%, $(_APP_SRC))

_PROTO_SRC	:= PowerMonitor.pb.cc
PROTO_SRC	:= $(patsubst %, $(SRC_DIR)/%, $(_PROTO_SRC))

_TEST_SRC 	:= socket-test.cpp 
TEST_SRC 	:= $(patsubst %, $(TEST_DIR)/%, $(_TEST_SRC))

# used to build the driver
DRIVER_OBJS = $(BUILD_DIR)/$(_DRIVER_SRC:.c=.o)

# used for building main app
OBJS		+= $(BUILD_DIR)/$(_APP_SRC:.cpp=.o)

# used to build tests
TEST_OBJS	+= $(BUILD_DIR)/$(_TEST_SRC:.cpp=.o)

# used to build protobuf
PROTO_OBJS  = $(BUILD_DIR)/$(_PROTO_SRC:.cc=.o)

TARGET 		:= $(BIN_DIR)/powermonitor
TEST_TARGET := $(BIN_DIR)/socket-test

PROTOBUF_CFLAGS := -DNOMINMAX -lprotobuf -labsl_log_internal_check_op -labsl_leak_check -labsl_die_if_null -labsl_log_internal_conditions -labsl_log_internal_message -labsl_examine_stack -labsl_log_internal_format   -labsl_log_internal_log_sink_set -labsl_log_sink -labsl_log_entry -labsl_flags -labsl_flags_internal -labsl_flags_marshalling -labsl_flags_reflection -labsl_flags_private_handle_accessor -labsl_flags_commandlineflag -labsl_flags_commandlineflag_internal -labsl_flags_config -labsl_flags_program_name -labsl_log_initialize -labsl_log_globals -labsl_log_internal_globals -labsl_hash -labsl_city -labsl_low_level_hash -labsl_raw_hash_set -labsl_hashtablez_sampler -labsl_statusor -labsl_status -labsl_cord -labsl_cordz_info -labsl_cord_internal -labsl_cordz_functions -labsl_exponential_biased -labsl_cordz_handle  -labsl_bad_optional_access -labsl_str_format_internal -labsl_strerror -labsl_synchronization -labsl_graphcycles_internal -labsl_stacktrace -labsl_symbolize -labsl_debugging_internal -labsl_demangle_internal -labsl_malloc_internal -labsl_time -labsl_civil_time -labsl_time_zone -labsl_bad_variant_access -labsl_strings -labsl_strings_internal -lrt -labsl_base -labsl_spinlock_wait -labsl_int128 -labsl_throw_delegate -labsl_raw_logging_internal -labsl_log_severity 
LIBS := m protobuf
CPPFLAGS    := -g -I $(INCLUDE_DIR) -MMD -MP -Wall -O0 -std=c++17
CFLAGS := -Wall -Wextra -std=c99 -I $(INCLUDE_DIR) -MMD -MP 
LDFLAGS     := 

LDLIBS      := libina229.a

dir_guard = @mkdir -p $(@D)

.PHONY: clean variables  test setup
.SILENT:

####################################### end_5 ####
all: setup  PowerMonitor_pb2.py $(PROTO_SRC) $(TARGET)

setup:
	mkdir -pv .build
	mkdir -pv bin

PowerMonitor_pb2.py: PowerMonitor.proto
	protoc --python_out=$(PYTHON_DIR) $<

$(PROTO_SRC):
	protoc --cpp_out=. PowerMonitor.proto
	mv PowerMonitor.pb.cc $(SRC_DIR)
	mv PowerMonitor.pb.h $(INCLUDE_DIR)

$(DRIVER_OBJS):$(DRIVER_SRC)
	$(CC) $(CFLAGS) -c $^ -o $@

$(TEST_OBJS):$(TEST_SRC) 
	$(CXX) $(CPPFLAGS) -c $^ -o $@

$(PROTO_OBJS):$(PROTO_SRC)
	$(CXX) $(CPPFLAGS) -c $^ -o $@

$(OBJS):$(APP_SRC)
	$(CXX) $(CPPFLAGS) -c $^ -o $@

$(BUILD_DIR)/PowerDataStreamer.o:$(SRC_DIR)/PowerDataStreamer.cpp 
	$(CXX) $(CPPFLAGS) -c $^ -o $@

$(TARGET): $(DRIVER_OBJS) $(PROTO_OBJS) $(OBJS) $(BUILD_DIR)/PowerMonitor.o $(BUILD_DIR)/PowerMonitorChannel.o $(BUILD_DIR)/PowerDataStreamer.o
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(PROTOBUF_CFLAGS)

$(TEST_TARGET):$(DRIVER_OBJS) $(PROTO_OBJS) $(TEST_OBJS) $(BUILD_DIR)/PowerMonitor.o $(BUILD_DIR)/PowerDataStreamer.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(PROTOBUF_CFLAGS)

$(BUILD_DIR)/PowerMonitorChannel.o:$(SRC_DIR)/PowerMonitorChannel.cpp 	
	$(CXX) $(CPPFLAGS) -c $^ -o $@

$(BUILD_DIR)/PowerMonitor.o:$(SRC_DIR)/PowerMonitor.cpp
	$(CXX) $(CPPFLAGS) -c $^ -o $@

#$(BUILD_DIR)/test-channels.o:$(TEST_DIR)/test-channels.cpp
#	$(CXX) $(CPPFLAGS) -c $^ -o $@

#$(BIN_DIR)/test-channels: $(DRIVER_OBJS) $(BUILD_DIR)/PowerMonitorChannel.o $(BUILD_DIR)/test-channels.o
#	$(CXX) $(LDFLAGS) -o $@ $^ 

$(BIN_DIR)/ina229: $(DRIVER_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

tests:$(TEST_TARGET) PowerMonitor_pb2.py
	@echo Tests built!

clean:
	rm -f $(SRC_DIR)/PowerMonitor.pb.cc $(INCLUDE_DIR)/PowerMonitor.pb.h $(PYTHON_DIR)/PowerMonitor_pb2.py
	rm -f $(BUILD_DIR)/*
	rm -f $(BIN_DIR)/*
