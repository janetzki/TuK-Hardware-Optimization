#include <bits/stdc++.h>
#include "prefetching.cpp"

using namespace std;

class Benchmarker {
private:
    // PRINTED ATTRIBUTES:
    // Either COLUMN_STORE or ROW_STORE
    int storeType;

    // Number of bits in uint data type
    int dataType;

    // Column size in bytes
    int columnSize;

    // Whether prefetcher is enabled
    bool usePrefetcher;

    // Whether to flush the cache between every benchmark
    bool withCacheFlush;

    // Number of columns in the table
    size_t columnCount;

    // UNPRINTED ATTRIBUTES:
    // Number of rows in the table
    size_t rowCount;

    int numberOfRuns = 10;

    const int COLUMN_STORE = 0;
    const int ROW_STORE = 1;

private:
    static void printHeader() {
        cout << "store_type,data_type,column_size,column_count,prefetcher,cache_flush,bandwidth\n";
    }

    void printAttributes() {
        if (storeType == COLUMN_STORE) {
            cout << "column_store,";
        } else {
            cout << "row_store,";
        }
        cout << "uint" << dataType << "_t," << columnSize << "," << columnCount << ","
        << (usePrefetcher ? '1' : '0') << ","
        << (withCacheFlush ? '1' : '0') << ",";
    }

    void clear_cache() {
      std::vector<int> clear = std::vector<int>();
      clear.resize(500 * 1000 * 1000, 42);
      for (uint i = 0; i < clear.size(); i++) {
        clear[i] += 1;
      }
      clear.resize(0);
    }

    int random_int(int min, int max) {
      return std::rand() % (max - min) + min;
    }

    template <typename T>
    std::vector<T> random_vector(size_t element_count) {
      std::vector<T> values = std::vector<T>();
      values.resize(element_count);

      for (size_t i = 0; i < element_count; i++) {
        values[i] = static_cast<T>(random_int(0, 1000));
      }

      return values;
    }

    template <typename T>
    uint64_t benchmarkColumnLayout(std::vector<T>& values) {
        uint64_t counter = 0;
        for (size_t i = 0; i < rowCount; i++) {
            if (values[i] == 0) {
                counter++;
            }
        }
        return counter;
    }

    template <typename T>
    uint64_t benchmarkRowLayout(std::vector<T>& values) {
        uint64_t counter = 0;
        for (size_t i = 0; i < values.size(); i += columnCount) {
            if (values[i] == 0) {
                counter++;
            }
        }
        return counter;
    }

    template <typename T>
    void benchmark() {
      printAttributes();
      dataType = sizeof(T);
      rowCount = columnSize / sizeof(T);
      auto values = random_vector<T>(rowCount * columnCount);
      uint64_t nanoSecondsTotal = 0;
      for (int i = 0; i < numberOfRuns; i++) {
        if (withCacheFlush) {
          clear_cache();
        }
        uint64_t counter;
        auto startTime = chrono::high_resolution_clock::now();
        if (storeType == COLUMN_STORE) {
            counter = benchmarkColumnLayout<T>(values);
        } else if (storeType == ROW_STORE){
            counter =  benchmarkRowLayout<T>(values);
        }
        auto endTime = chrono::high_resolution_clock::now();

        nanoSecondsTotal += chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count();
        assert(counter < rowCount);
      }
      double averageNanoSeconds = nanoSecondsTotal / static_cast<double>(numberOfRuns);
      cout << columnSize / averageNanoSeconds << '\n';
    }

    void configure_prefetcher() {
      auto result = set_prefetch_nhm(ALL_CORES, usePrefetcher);
      if (result < 0) {
          fprintf(stderr, "Unable to access prefetch MSR.\n");
          exit(1);
      }
    }

public:
    void runAllBenchmarks() {
        printHeader();
        auto columnCounts = {10, 50};
        for (auto numberOfColumns : columnCounts) {
          columnCount = numberOfColumns;
          for (int cacheFlush = 0; cacheFlush <= 1; cacheFlush++) {
              withCacheFlush = (cacheFlush == 1);
              for (int prefetcher = 0; prefetcher <= 1; prefetcher++) {
                usePrefetcher = (prefetcher == 1);
                configure_prefetcher();
                for (storeType = 0; storeType <= 1; storeType++) {
                    for (double exp = 3.0; exp <= 9.0; exp += 0.5) {
                        columnSize = static_cast<int>(std::pow(10, exp));
                        benchmark<uint8_t>();
                        benchmark<uint16_t>();
                        benchmark<uint32_t>();
                        benchmark<uint64_t>();
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
