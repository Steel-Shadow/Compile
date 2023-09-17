# 编译器和编译选项
CC = gcc
CFLAGS = -m64 -Wall -Wextra -g 
# clang改为 --target=x86_64-w64-mingw32

# 目标文件和可执行文件
TARGET = test.exe
OBJS = test.o libsysy\libsysy.o

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

# 编译每个源文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	del $(TARGET) $(OBJS)
# 定义伪目标，用于防止与同名文件冲突
.PHONY: all clean
