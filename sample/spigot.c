// https://xn--w6q13e505b.jp/program/spigot.html

int base = 10000;     // 基底
int n = 8400;         // 計算項数
int i;                // ループ変数
int temp;             // 一時変数/繰り上がり
int out;              // 出力値
int denom;            // 分母
int numerator[8401];  // 分子
 
int main() {
  for (i = 0; i < n; i += 1) {
    numerator[i] = base / 5;
  }
 
  out = 0;
  for (n = 8400; n > 0; n -= 14) {
    temp = 0;
    for (i = n - 1; i > 0; i -= 1) {
      denom = 2 * i - 1;
      temp = temp * i + numerator[i] * base;
      numerator[i] = temp % denom;
      temp /= denom;
    }
    printf("%04d", out + temp / base);
    out = temp % base;
  }
  return 0;
}
