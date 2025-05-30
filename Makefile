CXX = g++
CXXFLAGS = -g -Wall -I./include -fdiagnostics-color=always
LDFLAGS = -lgphoto2 -lgphoto2_port

SRC = src/main.cpp src/camera/camera.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = src/main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/camera/%.o: src/camera/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o src/camera/*.o $(TARGET)

.PHONY: all clean