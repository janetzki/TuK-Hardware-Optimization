#include <bits/stdc++.h>

using namespace std;

class Benchmarker {
private:
    int storeType;
    int dataType;
    int columnSize;

private:
    static void printHeader() {
        cout << "store_type,data_type,column_size,bandwidth\n";
    }

    void printAttributes() {
        if (storeType == 0) {
            cout << "column_store,";
        } else {
            cout << "row_store,";
        }
        cout << "uint" << dataType << "_t," << columnSize << ",";
    }

    template <typename T>
    void benchmarkColumnLayout() {
        vector<T> memoryWaster(columnSize, 0);
        uint64_t counter = 0;

        auto startTime = chrono::high_resolution_clock::now();
        for (auto it = memoryWaster.begin(); it != memoryWaster.end(); ++it) {
            if (*it == 0) {
                counter++;
            }
        }
        auto endTime = chrono::high_resolution_clock::now();

        uint64_t nanoSeconds = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count();
        cout << sizeof(T) * columnSize / nanoSeconds << '\n';
        assert(counter == columnSize);
    }

    template <typename T>
    void benchmarkRowLayout() {
        vector<vector<T>> memoryWaster(columnSize, vector<T>(100, 0));
        uint64_t counter = 0;

        clock_t begin = clock();
        for (auto it = memoryWaster.begin(); it != memoryWaster.end(); ++it) {
            if (it->at(0) == 0) {
                counter++;
            }
        }
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << sizeof(T) * columnSize / elapsed_secs / 1e9 << '\n';
        assert(counter == columnSize);
    }

    template <typename T>
    void benchmark() {
        if (storeType == 0) {
            return benchmarkColumnLayout<T>();
        } else {
            return benchmarkRowLayout<T>();
        }
    }

public:
    void runAllBenchmarks() {
        printHeader();
        for (storeType = 0; storeType <= 1; storeType++) {
            for (dataType = 8; dataType <= 64; dataType *= 2) {
                for (columnSize = 1; columnSize <= 1e7; columnSize *= 10) {
                    printAttributes();
                    switch (dataType) {
                        case 8:
                            benchmark<uint8_t>();
                            break;
                        case 16:
                            benchmark<uint16_t>();
                            break;
                        case 32:
                            benchmark<uint32_t>();
                            break;
                        case 64:
                            benchmark<uint64_t>();
                            break;
                        default:
                            cerr << "Invalid value" << endl;
                    }
                }
            }
        }
        cout.flush();
    }
};

int main(int argc, char *argv[]) {
    Benchmarker *benchmarker = new Benchmarker;
    benchmarker->runAllBenchmarks();
}
