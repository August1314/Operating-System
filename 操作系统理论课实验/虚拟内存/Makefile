CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDE = -I./include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 目标可执行文件
TARGET = $(BIN_DIR)/vm_manager

# 源文件和目标文件
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# 创建必要的目录
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 编译源文件生成目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# 清理目标文件和可执行文件
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# 运行程序
run: $(TARGET)
	$(TARGET) addresses.txt

.PHONY: all clean run 