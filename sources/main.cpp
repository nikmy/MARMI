#ifdef QUEUE_TEST
#include "test_queue.h"
#endif  // QUEUE_TEST

#ifdef GENERICS_TEST
#include "test_generics.h"
#endif  // GENERICS_TEST

#ifdef SCHEDULER_TEST
#include "test_scheduler.h"
#endif  // SCHEDULER_TEST

#ifdef SERVER_TEST
#include "test_server.h"
#endif  // SERVER_TEST

#include <iostream>

int main() {
    std::cout << "[INFO] Looking up for tests...\n" << std::endl;

#ifdef QUEUE_TEST
    std::cout << "[INFO] + Found test: QueueTest" << std::endl;
    test_queue();
#endif

#ifdef GENERICS_TEST
    std::cout << "[INFO] + Found test: GenericsTest" << std::endl;
    test_generics();
#endif

#ifdef SCHEDULER_TEST
    std::cout << "[INFO] + Found test: SchedulerTest" << std::endl;
    test_scheduler();
#endif

#ifdef SERVER_TEST
    std::cout << "[INFO] + Found test: ServerTest\n";
    test_server();
#endif

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[INFO] No more tests\n";
}
