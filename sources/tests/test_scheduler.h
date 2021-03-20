#pragma once

#include <iostream>
#include <thread>

#include "progress_bar.h"
#include "task_scheduler.h"

using namespace ts;

void test_scheduler()
{
    std::cout << "[INFO] SchedulerTest is running..." << std::endl;

    ProgressBar bar(100);
    bar.update();

    TaskScheduler& scheduler = GetScheduler();

    scheduler.launch();
    for (int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        bar.make_progress();
        bar.update();
    }
    scheduler.shutdown();

    std::cout << "\n[OK] Test finished\n" << std::endl;
}