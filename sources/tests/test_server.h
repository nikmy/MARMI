#pragma once

#include <iostream>
#include <thread>

#include "progress_bar.h"
#include "echo_server.h"

using namespace server;

void test_server()
{
    std::cout << "[INFO] ServerTest is running..." << std::endl;

    ProgressBar bar(100);

    BDRequestCounter checker{};
    EchoServer& server = GetEchoServer(checker);

    std::cout << "[+] Launching server..." << std::endl;

    server.start();

    std::cout << "[+] Server is running, wait a minute..." << std::endl;

    for (int i = 0; i < 100; ++i) {
        bar.make_progress();
        bar.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }

    std::cout << std::endl << "[+] Stopping server..." << std::endl;

    server.stop();

    std::cout << "[+] Server stopped normally" << std::endl;

    std::cout << "[+] " << checker.get_all_ignored()
              << " requests are pending" << std::endl;

    bar.assign(100);

    std::cout << "[+] Restarting server..." << std::endl;

    server.start();

    std::cout << "[+] Server is running, wait a minute..." << std::endl;

    for (int i = 0; i < 100; ++i) {
        bar.make_progress();
        bar.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }

    std::cout << std::endl << "[+] Shutdown server..." << std::endl;

    server.shutdown();

    std::cout << "[+] Server shutdown normally" << std::endl;

    std::cout << "[!] " << checker.get_all()
              << " requests have been processed, "
              << checker.get_all_ignored()
              << " ignored\n";

    if (checker.get_all_ignored()) {
        std::cerr << "[-] Test failed.\n" << std::endl;
    }
    else {
        std::cout << "[OK] Test passed\n" << std::endl;
    }
}