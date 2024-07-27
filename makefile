CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

test_iomanager: ./test/test_iomanager.cpp ./log/log.cpp ./fiber.cpp ./scheduler.cpp ./iomanager.cpp
	$(CXX) -o test_iomanager  $^ $(CXXFLAGS) -lpthread

clean:
	rm  -r test_iomanager
