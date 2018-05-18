#include <bits/stdc++.h>
#include "prefetching.cpp"

using namespace std;

class Benchmarker {
private:
    int storeType;
    int dataType;
    int columnSize;

private:
    static void printHeader() {
        cout << "store_type,data_type,column_size,prefetcher,bandwidth\n";
    }

    void printAttributes(bool usingPrefetcher) {
        if (storeType == 0) {
            cout << "column_store,";
        } else {
            cout << "row_store,";
        }
        cout << "uint" << dataType << "_t," << columnSize << "," << (usingPrefetcher ? '1' : '0') << ",";
    }

    void clear_cache() {
      std::vector<int> clear = std::vector<int>();
      clear.resize(500 * 1000 * 1000, 42);
      for (uint i = 0; i < clear.size(); i++) {
        clear[i] += 1;
      }
      clear.resize(0);
    }

    template <typename T>
    void benchmarkColumnLayout() {
        vector<T> memoryWaster(columnSize, 0);
        uint64_t counter = 0;
        clear_cache();
        auto startTime = chrono::high_resolution_clock::now();
        for (auto it = memoryWaster.begin(); it != memoryWaster.end(); ++it) {
            if (*it == 0) {
                counter++;
            }
        }
        auto endTime = chrono::high_resolution_clock::now();

        uint64_t nanoSeconds = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count();
        cout << sizeof(T) * columnSize / static_cast<double>(nanoSeconds) << '\n';
        assert(counter == columnSize);
    }

    template <typename T>
    void benchmarkRowLayout() {
        vector<vector<T>> memoryWaster(columnSize, vector<T>(100, 0));
        uint64_t counter = 0;
        clear_cache();
        auto startTime = chrono::high_resolution_clock::now();
        for (auto it = memoryWaster.begin(); it != memoryWaster.end(); ++it) {
            if (it->at(0) == 0) {
                counter++;
            }
        }
        auto endTime = chrono::high_resolution_clock::now();

        uint64_t nanoSeconds = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count();
        cout << sizeof(T) * columnSize / static_cast<double>(nanoSeconds) << '\n';
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

    void configure_prefetcher(bool use_prefetcher) {
      auto result = set_prefetch_nhm(ALL_CORES, use_prefetcher);
      if (result < 0) {
          fprintf(stderr, "Unable to access prefetch MSR.\n");
          exit(1);
      }
    }

public:
    void runAllBenchmarks() {
        printHeader();
        for (int prefetcher = 0; prefetcher <= 1; prefetcher++) {
          auto usingPrefetcher = prefetcher == 1;
          configure_prefetcher(usingPrefetcher);
          for (storeType = 0; storeType <= 1; storeType++) {
              for (dataType = 8; dataType <= 64; dataType *= 2) {
                  for (double exp = 3.0; exp <= 7.0; exp += 0.2) {
                      columnSize = static_cast<int>(std::pow(10, exp));
                      printAttributes(usingPrefetcher);
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
        }

        cout.flush();
    }
};


int main(int argc, char *argv[]) {
    Benchmarker *benchmarker = new Benchmarker;
    benchmarker->runAllBenchmarks();
}
