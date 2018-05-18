#include <bits/stdc++.h>

using namespace std;

template <typename T>
void benchmarkColumnLayout(uint64_t length) {
    vector<T> memoryWaster(length, 0);
    uint64_t counter = 0;

    clock_t begin = clock();
    for (auto it = memoryWaster.begin(); it != memoryWaster.end(); ++it) {
        if (*it == 0) {
            counter++;
        }
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << sizeof(T) * length / elapsed_secs / 1e9 << " GB/s" << endl;
    cout << elapsed_secs << " s" << endl;
    cout << counter << " should equal " << length << endl;
    assert(counter == length);
}

template <typename T>
void benchmarkRowLayout(uint64_t length) {
    vector<vector<T>> memoryWaster(length, vector<T>(100, 0));
    uint64_t counter = 0;

    clock_t begin = clock();
    for (auto it = memoryWaster.begin(); it != memoryWaster.end(); ++it) {
        if (it->at(0) == 0) {
            counter++;
        }
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << sizeof(T) * length / elapsed_secs / 1e9 << " GB/s" << endl;
    cout << elapsed_secs << endl;
    cout << counter << " should equal " << length << endl;
    assert(counter == length);
}

template <typename T>
void benchmark(uint64_t length, bool useColumnLayout) {
    if (useColumnLayout) {
        return benchmarkColumnLayout<T>(length);
    } else {
        return benchmarkRowLayout<T>(length);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        cerr << "Usage: ./program length int_type [row]" << endl;
        cerr << "Example: ./program 1000000 64 1" << endl;
        return 1;
    }
    const uint64_t length = atoi(argv[0]);
    const int int_type = atoi(argv[1]);
    bool useColumnLayout = true;

    if (argc == 3) {
        useColumnLayout = atoi(argv[2]) == 1;
    }

    switch(int_type) {
        case 8:
            benchmark<uint8_t>(length, useColumnLayout);
            break;
        case 16:
            benchmark<uint16_t>(length, useColumnLayout);
            break;
        case 32:
            benchmark<uint32_t>(length, useColumnLayout);
            break;
        case 64:
            benchmark<uint64_t>(length, useColumnLayout);
            break;
        default:
            cerr << "Unvalid value" << endl;
    }
    vector<uint64_t> memoryWaster;
}