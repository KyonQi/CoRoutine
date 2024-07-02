/**
 * @file test_fiber.cpp
 * @brief 协程1.0测试版本
 * @version 0.1
 * @date 2024-06-25
 */
#include "fiber.h"
#include "log/log.h"
#include "thread.h"

#include <vector>

using namespace std; 

static int m_close_log = 0;

void run_in_fiber2() {
    LOG_DEBUG("%s", "run_in_fiber2 begin");
    LOG_DEBUG("%s", "run_in_fiber2 end");
}

void run_in_fiber() {
    LOG_INFO("%s", "run_in_fiber begin");

    LOG_DEBUG("%s", "before run_in_fiber yield");
    Fiber::GetThis()->yield();
    LOG_DEBUG("%s", "after run_in_fiber yield");

    LOG_INFO("%s", "run_in_fiber end");
}

void test_fiber() {
    LOG_INFO("%s", "test_fiber begin");
    
    Fiber::GetThis(); //初始化线程主协程 这里很重要

    Fiber::ptr fiber(new Fiber(run_in_fiber, 0)); //线程主协程，stack_size应当为0
    LOG_DEBUG("user count is: %ld", fiber.use_count()); //1

    LOG_DEBUG("%s", "Before test_fiber resume");
    fiber->resume();
    LOG_DEBUG("%s", "After test fiber resume");

    LOG_DEBUG("user count is: %ld", fiber.use_count()); //3
    LOG_DEBUG("fiber status: %d", fiber->getState()); //READY

    LOG_DEBUG("before test_fiber resume again");
    fiber->resume();
    LOG_DEBUG("after test_fiber resume again");

    LOG_DEBUG("user count is: %ld", fiber.use_count()); //1
    LOG_DEBUG("fiber status: %d", fiber->getState()); //TERM

    fiber->reset(run_in_fiber2);
    fiber->resume();

    LOG_DEBUG("user count is: %ld", fiber.use_count()); //1

    LOG_INFO("%s", "test_fiber end");
}



int main () {
    Log::get_instance()->init("./ServerLog", 0, 2000, 800000, 800);
    LOG_INFO("%s", "TEST BEGIN");

    vector<Thread::ptr> thrs; //智能指针数组
    for (int i = 0; i < 2; ++i) {
        thrs.push_back(Thread::ptr
        (new Thread(&test_fiber, "thread_" + to_string(i))));
    }

    for (auto i : thrs) {
        i->join();
    }

    LOG_INFO("%s", "TEST END");
    sleep(2);
    return 0;
}