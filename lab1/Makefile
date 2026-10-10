CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude

BUILD_DIR = build
SRC_DIR = src

TASKS = 1 3 4 9 10

TARGETS = $(addprefix $(BUILD_DIR)/task, $(addsuffix .out, $(TASKS)))

all: $(BUILD_DIR) $(TARGETS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

define TASK_RULE
$(BUILD_DIR)/task$(1).out: $$(wildcard $(SRC_DIR)/task$(1)/*.c) | $(BUILD_DIR)
	$$(CC) $$(CFLAGS) -o $$@ $$^
endef

$(foreach task,$(TASKS),$(eval $(call TASK_RULE,$(task))))

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean