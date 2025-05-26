#include <iostream>
#include <vector>
#include <string>
#include <random>

using namespace std;

constexpr int THRESHOLD = 74;

static const int R = 255;
static const int M = R + 2;

inline int charAt(const string &s, int d) {
  return (d < static_cast<int>(s.size())
          ? static_cast<unsigned char>(s[d]) + 1
          : 0);
}

// 3-way quick sort по символу d на подмассиве a[lo..hi] включительно
void quick3way(vector<string> &a, int lo, int hi, int d) {
  if (lo >= hi) return;
  int lt = lo, gt = hi;
  int v = charAt(a[lo], d);
  int i = lo + 1;
  while (i <= gt) {
    int c = charAt(a[i], d);
    if (c < v) {
      swap(a[lt++], a[i++]);
    } else if (c > v) {
      swap(a[i], a[gt--]);
    } else {
      ++i;
    }
  }
  quick3way(a, lo, lt - 1, d);
  if (v != 0)
    quick3way(a, lt, gt, d + 1);
  quick3way(a, gt + 1, hi, d);
}

// MSD radix sort + переключение на quick3way
void msd_radix_qs(vector<string> &a,
                  vector<string> &aux,
                  int lo, int hi, int d)
{
  int len = hi - lo;
  if (len <= 1) return;
  if (len < THRESHOLD) {
    // Переключаемся на 3-way quick sort
    quick3way(a, lo, hi - 1, d);
    return;
  }
  // Подсчёт по кодам
  // count[r] = число строк с кодом < r
  vector<int> count(M+1, 0);
  for (int i = lo; i < hi; ++i) {
    int c = charAt(a[i], d);
    count[c + 1]++;
  }
  for (int r = 0; r < M; ++r) {
    count[r + 1] += count[r];
  }
  // Сохраняем границы корзин
  vector<int> start(M);
  for (int r = 0; r < M; ++r) {
    start[r] = count[r];
  }
  // Распределение в aux
  for (int i = lo; i < hi; ++i) {
    int c = charAt(a[i], d);
    aux[lo + count[c]++] = move(a[i]);
  }
  // Копирование обратно
  for (int i = lo; i < hi; ++i) {
    a[i] = move(aux[i]);
  }
  // Рекурсивно сортируем каждый ненулевой код
  for (int r = 1; r < M; ++r) {
    int bucketLo = lo + start[r - 1];
    int bucketHi = lo + count[r - 1];
    bucketLo = lo + start[r];
    bucketHi = lo + count[r];
    if (bucketHi - bucketLo > 1) {
      msd_radix_qs(a, aux, bucketLo, bucketHi, d + 1);
    }
  }
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n)) return 0;
  string line;
  getline(cin, line);

  vector<string> a(n), aux(n);
  for (int i = 0; i < n; ++i) {
    getline(cin, a[i]);
  }

  // Перемешаем, чтобы защититься от вырожденных случаев quick sort
  {
    random_device rd;
    mt19937 gen(rd());
    shuffle(a.begin(), a.end(), gen);
  }

  msd_radix_qs(a, aux, 0, n, 0);

  for (auto &s : a) {
    cout << s << "\n";
  }
  return 0;
}
