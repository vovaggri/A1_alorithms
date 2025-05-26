#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

using namespace std;

// Возвращает числовое значение символа на позиции d, или -1, если строка короче (терминирующий символ)
int charAt(const string &s, int d) {
  return (d < static_cast<int>(s.size()) ? static_cast<unsigned char>(s[d]) : -1);
}

// 3-х-путевая быстрая сортировка строк на подмассиве [lo..hi], начиная с символа d
void quickSort3way(vector<string> &a, int lo, int hi, int d) {
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
  // Рекурсивно сортируем каждую из трёх частей
  quickSort3way(a, lo, lt - 1, d);
  if (v >= 0) {
    quickSort3way(a, lt, gt, d + 1);
  }
  quickSort3way(a, gt + 1, hi, d);
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n)) return 0;
  string line;
  // Сброс остатка после числа
  getline(cin, line);

  vector<string> a(n);
  for (int i = 0; i < n; ++i) {
    getline(cin, a[i]);
  }

  // Чтобы избежать худшего случая, слегка перемешиваем перед сортировкой
  {
    random_device rd;
    mt19937 gen(rd());
    shuffle(a.begin(), a.end(), gen);
  }

  if (n > 0) {
    quickSort3way(a, 0, n - 1, 0);
  }

  for (auto &s : a) {
    cout << s << "\n";
  }
  return 0;
}
