CXX = g++
CFLAGS = -std=c++11 -O2 -Wall -g 

TARGET = server
OBJS = ../src/log/*.cpp  ../src/buffer/*.cpp \
       ../src/threadpool/*.cpp ../src/server/*.cpp \
       ../src/utils/*.cpp  ../src/server.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../bin/$(TARGET)  -pthread

clean:
	rm -rf ../bin/$(OBJS) $(TARGET)