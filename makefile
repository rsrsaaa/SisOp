CXX = g++
CXXFLAGS = -std=c++11 -pthread
LDFLAGS = -lpthread
SOURCES = Client.cpp Discovery.cpp global.cpp Interface.cpp Management.cpp Monitoring.cpp Server.cpp sleep_server.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = sleep_server

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
