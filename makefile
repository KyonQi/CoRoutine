CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

test_scheduler: ./test/test_scheduler.cpp ./log/log.cpp ./fiber.cpp ./scheduler.cpp
	$(CXX) -o test_scheduler  $^ $(CXXFLAGS) -lpthread

clean:
	rm  -r test_scheduler
