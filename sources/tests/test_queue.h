#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <string>

#include "queue.h"

using namespace mtq;

void test_queue()
{
    std::cout << "[INFO] QueueTest is running...\n";
    // Test 1
    {
        Queue<int> q;
        q.emplace(4);
        q.emplace(6);
        assert(q.take_first() == 4);
        assert(q.back() == 6);
        q.emplace(3);
        assert(q.take_first() == 6);
        q.pop();
        assert(q.empty());

        std::cout << "[+] Test 1 passed\n";
    }

    // Test 2
    {
        Queue<int> q;
        q.emplace(1);
        q.emplace(2);
        q.emplace(3);
        assert(q.take_first() == 1);
        assert(q.front() == 2);
        assert(q.back() == 3);

        std::cout << "[+] Test 2 passed\n";
    }

    // Test 3
    {
        Queue<int> q, w;
        q.emplace(1);
        q.emplace(2);
        w.emplace(4);

        q = w;
        q.pop();

        assert(q.empty());
        assert(!w.empty());

        std::cout << "[+] Test 3 passed\n";
    }

    // Test 4
    {
        Queue<int> q;
        q.emplace(4);
        Queue<int> w(q);
        w.pop();
        assert(!q.empty());
        w.emplace(3);
        w.move_to(q);
        assert(q.back() == 3 && q.size() == 2);
        Queue<int> dq(std::move(q));
        assert(dq.back() == 3);

        std::cout << "[+] Test 4 passed\n";
    }

    // Test 5
    {
        auto q = new Queue<int>[10];
        for (int i = 0; i < 1000; ++i)
            q[i % 10].emplace(i);

        for (int i = 999; i > 899; --i) {
            assert(q[i % 10].take_first() == ((999 - i) / 10 * 10 + i % 10));
            assert(q[i % 10].back() == (990 + i % 10));
        }

        for (int i = 0; i < 1000; ++i) {
            q[i % 10].clear();
            assert(q[i % 10].empty());
        }

        delete[] q;
        std::cout << "[+] Test 5 passed\n";
    }

    // Test 6
    {
        auto q = new Queue<std::vector<int>>[10];
        for (int k = 2; k; --k) {
            for (int i = 0; i < 1000; ++i) {
                std::vector<int> temp(1, i);
                q[i % 10].emplace(temp);
                assert(q[i % 10].back().back() == i);
            }

            for (int i = 999; i > 899; --i) {
                assert(
                    q[i % 10].take_first().back() == ((999 - i) / 10 * 10 + i % 10));
                assert(q[i % 10].back().back() == (990 + i % 10));
            }

            for (int i = 0; i < 10; ++i) {
                q[i].clear();
                assert(q[i].empty());
            }
        }

        delete[] q;
        std::cout << "[+] Test 6 passed\n";
    }

    // Test 7
    {
        Queue<std::string> q;
        q.emplace("first");
        q.emplace("second");
        Queue<std::string> w(q);

        assert(q.front() != q.back());
        assert(q.front() == "first");
        assert(q.back() == "second");
        q.clear();

        std::string_view s = "safe";
        q.emplace(std::string(s));
        assert(s == "safe");

        std::cout << "[+] Test 7 passed\n";
    }

    // Test 8
    {
        Queue<int> q = {1, 2, 3, 4, 5};

        Queue<int> w = {2};
        w.move_to(q);
        assert(q.back() == 2 && q.size() == 6);

        q.move_to(w);
        assert(q.size() == 0);
        assert(w.size() == 6);

        while (!w.empty())
            w.pop();

        std::cout << "[+] Test 8 passed\n";
    }

    // Test 9
    {
        Queue<int> q = {1, 2, 3, 4, 5};
        int i = 1;
        for (auto x : q) {
            assert(x == i);
            ++i;
        }

        std::cout << "[+] Test 9 passed\n";
    }

    // Test 10
    {
        Queue<std::vector<int>> w;

        for (int i = 0; i < 8; ++i)
            w.emplace(40);

        // do not use emplace in such cases
        w.push(w.front());
        assert(w.front() == w.back());

        Queue<int> q = {1, 2, 3, 4, 5, 6, 7, 8};
        q.push(q.back());

        // there is no moving
        q.push(std::move(q.front()));
        assert(q.take_first() == q.back());
        assert(q.size() == 9);

        std::cout << "[+] Test 10 passed\n";
    }

    std::cout << "[OK] All tests passed\n" << std::endl;
}