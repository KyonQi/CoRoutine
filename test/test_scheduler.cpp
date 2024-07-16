#include "../log/log.h"
#include "../thread.h"
#include "../fiber.h"
#include "../scheduler.h"

using namespace std;

static int m_close_log = 0;

/**
 * @brief 测试协程主动yield让出执行权，协程应当自己重新添加到调度器的任务队列中
 */
void test_fiber1() {
    LOG_INFO("%s", "Test Fiber1 Begin");
    //在协程yield之前，应当自己重新添加回任务队列
    //必须在yield之前，因为之后就不可能添加了
    Scheduler::GetThis()->schedule(Fiber::GetThis()); 
    
    LOG_INFO("test_fiber1:: %s", "before test_fiber1 yield");
    Fiber::GetThis()->yield(); //注意yield时候才会将当前上下文存储并swap，所以可以提前schedule
    LOG_INFO("test_fiber1:: %s", "after test_fiber1 yield");

    LOG_INFO("%s", "Test Fiber1 End");
}

/**
 * @brief 测试睡眠对线程的影响
 */
void test_fiber2() {
    LOG_INFO("%s", "Test Fiber2 Begin");
    //一个线程同一时间只能运行一个协程，因此睡眠会影响整个线程的协程调度，
    //线程在这3s内不会调度新协程
    LOG_DEBUG("test_fiber2 Fiber %d SLEEP 3s", Fiber::GetFiberId());
    sleep(3);
    LOG_INFO("%s", "Test Fiber2 End");
}

void test_fiber3() {
    LOG_INFO("%s", "Test Fiber3 Begin");
    LOG_INFO("%s", "Test Fiber3 End");
}

/**
 * @brief 当任务指定执行线程时的情况
 */
void test_fiber5() {
    static int count = 0;
    LOG_INFO("Test Fiber5 Begin, i = %d", count);
    LOG_INFO("Test Fiber5 End, i = %d", count);
    count++;
}

void test_fiber4() {
    LOG_INFO("%s", "Test Fiber4 Begin");
    
    for (int i = 0; i < 3; ++i) {
        Scheduler::GetThis()->schedule(test_fiber5, Thread::GetThreadId());
    }

    LOG_DEBUG("Test Fiber4 Schedule 3 tasks for %d", Thread::GetThreadId());

    LOG_INFO("%s", "Test Fiber4 End");
}

int main () {
    Log::get_instance()->init("./SchedulerLog", 0, 2000, 800000, 800);
    LOG_DEBUG("%s", "Scheduler Test Begin");
    
    // Scheduler sc; //主线程初始化调度器

    Scheduler sc(3, false); // 创建额外的线程池使用调度器，不使用use_caller
    sc.schedule(test_fiber1); //添加函数调度任务
    sc.schedule(test_fiber2);

    //添加协程调度任务
    Fiber::ptr fiber(new Fiber(&test_fiber3));
    sc.schedule(fiber);

    //start方法创建调度线程,开始调度
    sc.start();

    //开始后往调度器添加调度任务
    sc.schedule(test_fiber4);

    //停止调度
    sc.stop();
    
    LOG_DEBUG("%s", "Scheduler Test End");
    sleep(1);
    return 0;
}