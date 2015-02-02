CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror -MMD -g
LINKER = $(CXX)
LDFLAGS =

TARGET = main
SOURCES = main.cc $(wildcard **/*.cc)
OBJECTS = $(SOURCES:.cc=.o)

TEST_TARGET = test
TEST_SOURCES = test.cc $(wildcard **/*.cc)
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)

DEPS = $(OBJECTS:.o=.d)
ODIR = ./objects

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(LDFLAGS) $(OBJECTS) -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	$(LINKER) $(LDFLAGS) $(TEST_OBJECTS) -o $@

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

-include $(DEPS)