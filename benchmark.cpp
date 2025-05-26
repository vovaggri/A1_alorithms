#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;
using Clock = chrono::high_resolution_clock;

// Глобальный счётчик посимвольных сравнений
static long long comparisonCount = 0;

// Лексикографическое сравнение с учётом счётчика
bool lexCompareCounted(const string &a, const string &b) {
  size_t m = min(a.size(), b.size());
  for (size_t i = 0; i < m; ++i) {
    ++comparisonCount;
    if (a[i] != b[i]) return a[i] < b[i];
  }
  ++comparisonCount;
  return a.size() < b.size();
}

// charAt для String QuickSort, считает сравнения
inline int charAtCounted(const string &s, int d) {
  if (d < (int)s.size()) {
    ++comparisonCount;
    return (unsigned char)s[d];
  } else {
    return -1;
  }
}

// 3-way QuickSort по символам
void quick3way_rec(vector<string> &a, int lo, int hi, int d) {
  if (lo >= hi) return;
  int lt = lo, gt = hi;
  int v = charAtCounted(a[lo], d);
  int i = lo + 1;
  while (i <= gt) {
    int c = charAtCounted(a[i], d);
    if (c < v)      swap(a[lt++], a[i++]);
    else if (c > v) swap(a[i], a[gt--]);
    else            i++;
  }
  quick3way_rec(a, lo, lt - 1, d);
  if (v >= 0) quick3way_rec(a, lt, gt, d + 1);
  quick3way_rec(a, gt + 1, hi, d);
}

void sortQuick3Way(vector<string> &v) {
  if (!v.empty()) quick3way_rec(v, 0, v.size() - 1, 0);
}

// LCP для MergeSort, считает сравнения
int lcpCounted(const string &a, const string &b) {
  int n = min(a.size(), b.size()), i = 0;
  while (i < n) {
    ++comparisonCount;
    if (a[i] != b[i]) break;
    ++i;
  }
  return i;
}

bool lcpCompare(const string &a, const string &b) {
  int k = lcpCounted(a, b);
  if (k == (int)a.size() && k == (int)b.size()) return false;
  if (k == (int)a.size()) return true;
  if (k == (int)b.size()) return false;
  ++comparisonCount;
  return a[k] < b[k];
}

// MergeSort с LCP-сравнением
void mergeLCP(vector<string> &a, vector<string> &tmp, int l, int m, int r) {
  int i = l, j = m, p = l;
  while (i < m && j < r) {
    if (lcpCompare(a[i], a[j])) tmp[p++] = move(a[i++]);
    else                        tmp[p++] = move(a[j++]);
  }
  while (i < m) tmp[p++] = move(a[i++]);
  while (j < r) tmp[p++] = move(a[j++]);
  for (int k = l; k < r; ++k) a[k] = move(tmp[k]);
}

void mergeLCP_rec(vector<string> &a, vector<string> &tmp, int l, int r) {
  if (r - l <= 1) return;
  int m = (l + r) >> 1;
  mergeLCP_rec(a, tmp, l, m);
  mergeLCP_rec(a, tmp, m, r);
  mergeLCP(a, tmp, l, m, r);
}

void sortMergeLCP(vector<string> &v) {
  vector<string> tmp(v.size());
  mergeLCP_rec(v, tmp, 0, v.size());
}

// MSD Radix Sort без переключения
static const int R = 256;
static const int M = R + 1;
inline int charAtMSD(const string &s, int d) {
  return (d < (int)s.size() ? (unsigned char)s[d] + 1 : 0);
}

void msdRadix_rec(vector<string> &a, vector<string> &aux,
                  int lo, int hi, int d, bool useQS) {
  int len = hi - lo;
  if (len <= 1) return;
  if (useQS && len < 74) {  // порог переключения
    quick3way_rec(a, lo, hi - 1, d);
    return;
  }
  // 1) частоты
  vector<int> count(M + 1);
  for (int i = lo; i < hi; ++i) {
    count[charAtMSD(a[i], d)]++;
  }
  // 2) префиксные суммы → start
  vector<int> start(M + 1);
  start[0] = 0;
  for (int r = 1; r <= M; ++r) {
    start[r] = start[r - 1] + count[r - 1];
  }
  // 3) распределяем в aux
  for (int i = lo; i < hi; ++i) {
    int c = charAtMSD(a[i], d);
    aux[lo + start[c]++] = move(a[i]);
  }
  // 4) копируем обратно
  for (int i = lo; i < hi; ++i) {
    a[i] = move(aux[i]);
  }
  // 5) рекурсивно для корзин
  int offset = lo;
  for (int r = 0; r <= M; ++r) {
    int sz = count[r];
    if (sz > 1) {
      msdRadix_rec(a, aux, offset, offset + sz, d + 1, useQS);
    }
    offset += sz;
  }
}

void sortMSDRadix(vector<string> &v) {
  vector<string> aux(v.size());
  msdRadix_rec(v, aux, 0, v.size(), 0, false);
}

void sortMSDRadixQS(vector<string> &v) {
  vector<string> aux(v.size());
  msdRadix_rec(v, aux, 0, v.size(), 0, true);
}

// Генератор строк (корректный порядок полей)
class StringGenerator {
public:
    StringGenerator()
            : alpha(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "!@#%:;^&*()-"
    ),
              rng(random_device{}()),
              lenDist(10, 200),
              charDist(0, static_cast<int>(alpha.size()) - 1)
    {}

    vector<string> generateRandom(int n) {
      vector<string> v; v.reserve(n);
      for (int i = 0; i < n; ++i) v.push_back(randString());
      return v;
    }

    vector<string> generateReverse(int n) {
      auto v = generateRandom(n);
      sort(v.begin(), v.end());
      reverse(v.begin(), v.end());
      return v;
    }

    vector<string> generateAlmost(int n, int swaps) {
      auto v = generateRandom(n);
      sort(v.begin(), v.end());
      uniform_int_distribution<int> idx(0, n - 1);
      for (int i = 0; i < swaps; ++i) {
        int a = idx(rng), b = idx(rng);
        swap(v[a], v[b]);
      }
      return v;
    }

private:
    string randString() {
      int L = lenDist(rng);
      string s; s.reserve(L);
      for (int i = 0; i < L; ++i)
        s.push_back(alpha[charDist(rng)]);
      return s;
    }

    const string alpha;
    mt19937_64 rng;
    uniform_int_distribution<int> lenDist, charDist;
};

// Результат одного замера
struct TestResult {
    string algorithm, dataType;
    int n;
    double timeMs;
    long long comparisons;
};

int main(){
  vector<int> sizes;
  for(int x = 100; x <= 3000; x += 100)
    sizes.push_back(x);
  vector<string> types = {"random","reverse","almost"};

  StringGenerator gen;
  vector<TestResult> results;

  for (int n : sizes) {
    for (auto &tp : types) {
      // Генерация
      vector<string> data;
      if (tp == "random")  data = gen.generateRandom(n);
      if (tp == "reverse") data = gen.generateReverse(n);
      if (tp == "almost")  data = gen.generateAlmost(n, 10);

      // 1) std::sort
      comparisonCount = 0;
      {
        auto v = data;
        auto t0 = Clock::now();
        sort(v.begin(), v.end(), lexCompareCounted);
        auto t1 = Clock::now();
        results.push_back({"std_sort", tp, n,
                           chrono::duration<double, milli>(t1 - t0).count(),
                           comparisonCount});
      }

      // 2) std::stable_sort
      comparisonCount = 0;
      {
        auto v = data;
        auto t0 = Clock::now();
        stable_sort(v.begin(), v.end(), lexCompareCounted);
        auto t1 = Clock::now();
        results.push_back({"stable_sort", tp, n,
                           chrono::duration<double, milli>(t1 - t0).count(),
                           comparisonCount});
      }

      // 3) MergeSort с LCP
      comparisonCount = 0;
      {
        auto v = data;
        auto t0 = Clock::now();
        sortMergeLCP(v);
        auto t1 = Clock::now();
        results.push_back({"merge_lcp", tp, n,
                           chrono::duration<double, milli>(t1 - t0).count(),
                           comparisonCount});
      }

      // 4) 3-way QuickSort
      comparisonCount = 0;
      {
        auto v = data;
        auto t0 = Clock::now();
        sortQuick3Way(v);
        auto t1 = Clock::now();
        results.push_back({"quick3way", tp, n,
                           chrono::duration<double, milli>(t1 - t0).count(),
                           comparisonCount});
      }

      // 5) MSD Radix Sort
      comparisonCount = 0;
      {
        auto v = data;
        auto t0 = Clock::now();
        sortMSDRadix(v);
        auto t1 = Clock::now();
        results.push_back({"msd_radix", tp, n,
                           chrono::duration<double, milli>(t1 - t0).count(),
                           comparisonCount});
      }

      // 6) MSD Radix + QuickSwitch
      comparisonCount = 0;
      {
        auto v = data;
        auto t0 = Clock::now();
        sortMSDRadixQS(v);
        auto t1 = Clock::now();
        results.push_back({"msd_radix_qs", tp, n,
                           chrono::duration<double, milli>(t1 - t0).count(),
                           comparisonCount});
      }
    }
  }

  // Сохранение CSV
  ofstream out("results.csv");
  out << "algorithm,dataType,n,timeMs,comparisons\n";
  for (auto &r : results) {
    out
            << r.algorithm << ','
            << r.dataType  << ','
            << r.n         << ','
            << r.timeMs    << ','
            << r.comparisons
            << "\n";
  }
  cout << "Done\n";
  return 0;
}
