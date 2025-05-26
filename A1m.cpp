#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>

using namespace std;

// Вычисление длины общего префикса
int lcp(const string &a, const string &b) {
  int n = min(a.size(), b.size());
  int i = 0;
  while (i < n && a[i] == b[i]) ++i;
  return i;
}

// Сравнение строк через LCP
bool lcpCompare(const string &a, const string &b) {
  int k = lcp(a, b);
  if (k == a.size() && k == b.size()) {
    return false;
  } else if (k == a.size()) {
    return true;
  } else if (k == b.size()) {
    return false;
  } else {
    return a[k] < b[k];
  }
}

// Слияние двух уже отсортированных частей [l,m) и [m,r)
void mergeRange(vector<string> &a, vector<string> &tmp, int l, int m, int r) {
  int i = l, j = m, p = l;
  while (i < m && j < r) {
    if (lcpCompare(a[i], a[j]))
      tmp[p++] = move(a[i++]);
    else
      tmp[p++] = move(a[j++]);
  }
  while (i < m) tmp[p++] = move(a[i++]);
  while (j < r) tmp[p++] = move(a[j++]);
  for (int k = l; k < r; ++k)
    a[k] = move(tmp[k]);
}

// Рекурсивный merge sort на отрезке [l,r)
void mergeSortRange(vector<string> &a, vector<string> &tmp, int l, int r) {
  if (r - l <= 1) return;
  int m = l + (r - l) / 2;
  mergeSortRange(a, tmp, l, m);
  mergeSortRange(a, tmp, m, r);
  mergeRange(a, tmp, l, m, r);
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n)) return 0;
  string line;
  // Сброс остатка после числа
  getline(cin, line);

  vector<string> a(n), tmp(n);
  for (int i = 0; i < n; ++i) {
    getline(cin, a[i]);
  }

  mergeSortRange(a, tmp, 0, n);

  for (auto &s : a)
    cout << s << "\n";

  return 0;
}
