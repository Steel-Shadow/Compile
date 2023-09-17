# 编译器
CC = clang

ifneq ($(CC), cl)
    # CFLAGS = -g
endif

# 设置不同编译器的编译选项
ifeq ($(CC), cl)
    CFLAGS += /Wall
else ifeq ($(CC), clang)
    CFLAGS += -Wextra --target=x86_64-w64-mingw32
else ifeq ($(CC), gcc)
# -Wall
    CFLAGS += -Wextra
else
    $(error Unsupported compiler: $(CC))
endif


# 目标文件和可执行文件
TARGET = testfile1.exe
OBJS = testfile1.o libsysy\libsysy.o

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
