CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

test_fiber: test_fiber.cpp ./log/log.cpp ./fiber.cpp
	$(CXX) -o test_fiber  $^ $(CXXFLAGS) -lpthread

clean:
	rm  -r test_fiber
