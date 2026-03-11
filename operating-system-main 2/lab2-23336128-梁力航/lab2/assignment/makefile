# 修正后的Makefile
TARGET = test    # 修正拼写错误

AS = nasm
CXX = g++

ASM_FILE = $(wildcard *.asm)
CXX_FILE = $(wildcard *.cpp)

ALL_OBJ += $(ASM_FILE:%.asm=%.o)
ALL_OBJ += $(CXX_FILE:%.cpp=%.o)

# 添加-m32保证32位编译
run:
	@rm -rf *.o
	@nasm -f elf32 -g $(ASM_FILE)  # 添加调试信息
	@g++ -m32 -g -c $(CXX_FILE)
	@g++ -m32 -o $(TARGET) $(ALL_OBJ) -no-pie # 禁用地址随机化方便调试
	@./$(TARGET)

clean:
	@rm -rf *.o $(TARGET)