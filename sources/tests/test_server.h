#pragma once

#include <iostream>
#include <thread>

#include "progress_bar.h"
#include "echo_server.h"

using namespace server;

void test_server()
{
    std::cout << "[INFO] ServerTest is running...\n";

    ProgressBar      bar(100);
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

    std::cout << "[+] Server stopped normally" << std::endl;

    std::cout << "[+] " << server.request_queue_.size()
              << " requests saved" << std::endl;

    server.stop();

    bar.assign(100);

    std::cout << "[+] Restarting server..." << std::endl;

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

    std::cout << "[!] " << checker.get_all()
              << " requests have been processed, "
              << server.request_queue_.size()
              << " saved, "
              << checker.get_all_ignored() - server.request_queue_.size()
              << " ignored\n";

    if (checker.get_all_ignored() - server.request_queue_.size()) {
        std::cerr << "[-] Test failed.\n" << std::endl;
    }
    else {
        std::cout << "[OK] Test passed\n" << std::endl;
    }
}