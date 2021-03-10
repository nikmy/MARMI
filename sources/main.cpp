#ifdef QUEUE_TEST
#include "test_queue.h"
#endif  // QUEUE_TEST

#ifdef GENERICS_TEST
#include "test_generics.h"
#endif  // GENERICS_TEST

#include <iostream>

int main() {
    std::cout << "[INFO] Looking up for tests...\n" << std::endl;

#ifdef QUEUE_TEST
    std::cout << "[INFO] + Found test: QueueTest\n";
    test_queue();
#endif

#ifdef GENERICS_TEST
    std::cout << "[INFO] + Found test: GenericsTest\n";
    test_generics();
#endif

    std::cout << "[INFO] No more tests\n" << std::endl;
}