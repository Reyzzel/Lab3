#include <iostream>
#include <syncstream>
#include <array>
#include <barrier>
#include <list>
#include <map>
#include <vector>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const int nt = 5;

void f(char set_name, int id_action) {
	osyncstream(cout) << "З набору " << set_name << " виконано дію " << id_action << endl;
}

static const array<string, nt> thread_tasks = {
    "aaacccccfggggjj",
    "aaacccddgggg jj",
    "bbbdddddhhhi jj",
    "bbbeeeeehhhi jj",
    "bbbfffff iiii  "
};

void StartProcessing(const array<string, nt> &thread_tasks, map<char, int> &action_counters, barrier<> &phase_sync) {
    list<jthread> threads;

    vector<vector<pair<char, int> > > set_actions(nt, vector<pair<char, int> >());

    for (int i = 0; i < nt; ++i) {
        for (const char &set_name: thread_tasks[i]) {
            set_actions[i].emplace_back(set_name, action_counters[set_name]++);
        }
    }

    for (int i = 0; i < nt; ++i) {
        threads.emplace_back([set_actions, i, &phase_sync]() {
            for (const auto [set_name, id_action]: set_actions[i]) {
                if (set_name != ' ') {
                    f(set_name, id_action);
                }
                phase_sync.arrive_and_wait();
            }
        });
    }
}

void perform_computation() {
    barrier phase_sync(nt);
    map<char, int> action_counters;

    for (char c = 'a'; c <= 'j'; ++c) {
        action_counters[c] = 1;
    }

    StartProcessing(thread_tasks, action_counters, phase_sync);
}

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    cout << "\xEF\xBB\xBF";
    cout << "Обчислення розпочато\n";
    perform_computation();
    std::cout << "Обчислення завершено\n";

    return 0;
}