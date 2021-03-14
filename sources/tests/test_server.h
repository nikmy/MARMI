#pragma once

#include <iostream>
#include <thread>

#include "progress_bar.h"
#include "echo_server.h"

using namespace server;

void test_server()
{
    std::cout << "[INFO] ServerTest is running...\n";

    ProgressBar bar(100);
    BDRequestCounter checker{};
    EchoServer& server = GetEchoServer(checker);

    std::cout << "[+] Launching server..." << std::endl;

    server.start();

    std::cout << "[+] Server is running, wait 2 minutes..." << std::endl;

    for (int i = 0; i < 100; ++i) {
        bar.make_progress();
        bar.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

    bar.make_progress();
    bar.update();

    std::cout << std::endl;

    server.stop();

    std::cout << "[+] Server shutdown normally" << std::endl;
    std::cout << "[!] " << checker.get_all()
              << " requests have been processed, "
              << checker.get_all_ignored()
              << " have been ignored\n";
    if (checker.is_balanced()) {
        std::cout << "[OK] Test passed\n" << std::endl;
    }
    else {
        std::cerr << "[-] Test failed.\n" << std::endl;
    }
}