# 变量定义
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -lws2_32 -pthread

# 目标文件
TARGET_SERVER = server
TARGET_CLIENT = client

# 源文件
SRC_SERVER = server.cpp
SRC_CLIENT = client.cpp

# 编译规则
all: $(TARGET_SERVER) $(TARGET_CLIENT)
# $@ 代表当前规则中的目标文件
# $^ 当前规则中的所有依赖文件
$(TARGET_SERVER): $(SRC_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TARGET_CLIENT): $(SRC_CLIENT)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# 清理规则
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT)

# 重新编译
rebuild: clean all
