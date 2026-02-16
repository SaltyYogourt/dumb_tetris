CC = gcc

BUILD_DIR := .build
SRC_DIR := ./src

TARGET := main

SRCS			= $(wildcard $(SRC_DIR)/*.c)
OBJS			= $(addprefix $(BUILD_DIR)/, $(notdir $(SRCS:.c=.o)))
DEPS			= $(OBJS:.o=.d)


COMPILER_FLAGS = -Wall -Wextra
LINKER_FLAGS = -lSDL3
OBJ_NAME = main


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LINKER_FLAGS) -lSDL3 -MMD -MP -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LINKER_FLAGS) $^ -o $@


.PHONY: all clean run rebuild release debug

all: $(TARGET)

release: all

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: all

#3 all : $(SRCS)
#3 	$(CC) $(SRCS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BUILD_DIR)/$(OBJ_NAME)

