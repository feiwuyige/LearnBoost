# 变量定义
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -lws2_32 -lmswsock -pthread

# 定义目标文件和源文件
TARGET := AsyncServer        # 生成的可执行文件名
SRCS := Session.cpp AsyncServer.cpp  # 源文件
OBJS := $(SRCS:.cpp=.o)  # 自动生成中间的 .o 文件


# 生成可执行文件
all: $(TARGET)

# $@ 代表当前规则中的目标文件
# $^ 当前规则中的所有依赖文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# 编译源文件为目标文件
# % 表示通配符，表示任意匹配的文件
# $< 表示第一个依赖文件
%.o: %.cpp Session.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

# 清理编译生成的文件
clean:
	rm -f $(TARGET) $(OBJS)

# 伪目标，不生成文件
.PHONY: all clean