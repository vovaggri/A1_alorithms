#include <iostream>
#include <vector>
#include <string>

using namespace std;

static const int R = 255;
static const int M = R + 2;

inline int charAt(const string &s, int d) {
  return (d < static_cast<int>(s.size())
          ? static_cast<unsigned char>(s[d]) + 1
          : 0);
}

void msdRadixSort(vector<string> &a, vector<string> &aux,
                  int lo, int hi, int d)
{
  if (hi - lo <= 1) return;

  // count[ c+1 ] — количество строк с кодом c
  vector<int> count(M+1, 0);
  for (int i = lo; i < hi; ++i) {
    int c = charAt(a[i], d);
    count[c + 1]++;
  }
  // Префиксная сумма: теперь count[c] — относительный стартовый индекс для кода c
  for (int r = 0; r < M; ++r) {
    count[r + 1] += count[r];
  }
  // Запомнить старты для стабильного распределения
  vector<int> start(M);
  for (int r = 0; r < M; ++r) {
    start[r] = count[r];
  }
  // Распределяем в aux
  for (int i = lo; i < hi; ++i) {
    int c = charAt(a[i], d);
    aux[start[c]++] = std::move(a[i]);
  }
  // Копируем обратно в a[lo..hi)
  for (int i = 0; i < hi - lo; ++i) {
    a[lo + i] = std::move(aux[i]);
  }
  // Рекурсивно сортируем по каждому коду > 0 (0 — это "конец строки")
  for (int r = 1; r < M; ++r) {
    int bucketLo = lo + count[r - 1];
    int bucketHi = lo + count[r];
    if (bucketHi - bucketLo > 1) {
      msdRadixSort(a, aux, bucketLo, bucketHi, d + 1);
    }
  }
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n)) return 0;
  string tmp;
  getline(cin, tmp);

  vector<string> a(n), aux(n);
  for (int i = 0; i < n; ++i) {
    getline(cin, a[i]);
  }

  msdRadixSort(a, aux, 0, n, 0);

  for (auto &s : a) {
    cout << s << "\n";
  }
  return 0;
}