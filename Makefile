CC			= gcc

BUILD_DIR 		:= .build
SRC_DIR			:= ./src
TARGET			:= main

SRCS			= $(wildcard $(SRC_DIR)/*.c)
OBJS			= $(addprefix $(BUILD_DIR)/, $(notdir $(SRCS:.c=.o)))
DEPS			= $(OBJS:.o=.d)

CFLAGS 			:= -Wall -Wextra
OPTIMIZE_FLAGS 		:= -O3
DEBUG_FLAGS 		= -g
DEBUG3_FLAGS 		= -ggdb3 -fno-stack-protector
LINKER_FLAGS 		= -lSDL3 -lSDL3_ttf
OBJ_NAME 		= main

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LINKER_FLAGS) -MMD -MP -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LINKER_FLAGS) $^ -o $@

.PHONY: all clean run rebuild release debug

all: $(TARGET)

release: CFLAGS += $(OPTIMIZE_FLAGS)
release: all

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

debug3: CFLAGS += $(DEBUG3_FLAGS)
debug3: all

clean: 
	rm -rf $(TARGET) $(OBJS)
