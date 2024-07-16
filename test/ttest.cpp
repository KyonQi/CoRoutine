// #include <iostream>
// #include <memory>
// using namespace std;

// class Person {
// public:
//     int val;

// public:
//     Person(int v) : val(v) {
//         cout << "Val is: " << val  << " Created"<< endl;
//     }
//     ~Person() {
//         cout << val << "Person is DEAD" << endl;
//     }
// };

// class TestSwap {
// public:
//     shared_ptr<Person> pp;
//     TestSwap(shared_ptr<Person> p) {
//         cout << "Function User Counter is : " << p.use_count() << endl;
//         pp.swap(p);
//         if (!p) cout << "FUCK P" << endl;
//         cout << "pp's val is " << pp->val << endl;
//     }
//     TestSwap(shared_ptr<Person>* p) {
//         pp.swap(*p);
//     } 
// };

// int main() {
//     shared_ptr<Person> p1 = make_shared<Person> (10);
//     shared_ptr<Person> p2 = make_shared<Person> (20);
//     TestSwap tsw2(p2);
//     if (p2) cout << "FUCK p2" << endl;
//     cout << "Counter is : " << tsw2.pp.use_count() << endl;

//     TestSwap tsw1(&p1);
//     cout << "Counter is : " << tsw1.pp.use_count() << endl;
//     return 0;
// }
#include <memory>
#include <iostream>

class Fiber {
public:
    using ptr = std::shared_ptr<Fiber>;
    Fiber() { std::cout << "Fiber created\n"; }
    ~Fiber() { std::cout << "Fiber destroyed\n"; }
};

class ScheduleTask {
public:
    ScheduleTask(Fiber::ptr f, int thr) {
        fiber.swap(f);
        thread = thr;
    }

    void print() {
        if (fiber) {
            std::cout << "Fiber is not null, thread: " << thread << "\n";
        } else {
            std::cout << "Fiber is null, thread: " << thread << "\n";
        }
    }

private:
    Fiber::ptr fiber;
    int thread;
};

int main() {
    Fiber::ptr fiber = std::make_shared<Fiber>();
    std::cout << "Initial reference count: " << fiber.use_count() << "\n"; // 输出 1
    ScheduleTask task(fiber, 1);
    std::cout << "After passing to ScheduleTask, reference count: " << fiber.use_count() << "\n"; // 输出 1
    task.print();
    return 0;
}
