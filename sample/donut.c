int mul(int a, int b) {
  return a * b / 10000;
}

int k;

int update2(int *cos, int *sin, int cos_delta, int sin_delta) {
  int new_cos = mul(*cos, cos_delta) - mul(*sin, sin_delta);
  int new_sin = mul(*sin, cos_delta) + mul(*cos, sin_delta);
  *cos = new_cos;
  *sin = new_sin;
  return 0;
}

int main() {
  int zTimes10000[1760];
  char b[1760];
  printf("%c[2J", 27);
  int cosATimes10000 = 10000; int sinATimes10000 = 0;
  int cosBTimes10000 = 10000; int sinBTimes10000 = 0;
  for (;;) {
    memset(b, 32, 1760 * sizeof(*b));
    memset(zTimes10000, 0, 1760 * sizeof(*zTimes10000));
    int sinthetaTimes10000 = 0; int costhetaTimes10000 = 10000;
    int theta_times_14;
    for (theta_times_14 = 0; theta_times_14 < 88; ){
      int sinphiTimes10000 = 0; int cosphiTimes10000 = 10000;
      int phi_times_50;
      for (phi_times_50 = 0; phi_times_50 < 314; ) {
        int hTimes10000 = costhetaTimes10000 + 20000;
        int DTimes10000 = 100000000 / (
          mul(mul(sinphiTimes10000, hTimes10000), sinATimes10000) 
          + mul(sinthetaTimes10000, cosATimes10000)
          + 50000);
        int tTimes10000 = mul(mul(sinphiTimes10000, cosATimes10000), hTimes10000) - mul(sinthetaTimes10000, sinATimes10000);
        int x = 40 + 30 * mul(DTimes10000, mul(mul(cosphiTimes10000, cosBTimes10000), hTimes10000) - mul(tTimes10000 , sinBTimes10000)) / 10000,
            y = 12 + 15 * mul(DTimes10000, mul(mul(cosphiTimes10000, sinBTimes10000), hTimes10000) + mul(tTimes10000 , cosBTimes10000)) / 10000, 
            o = x + 80 * y,
            N = 8 * (
                mul(mul(sinthetaTimes10000, sinATimes10000) - mul(mul(sinphiTimes10000, cosATimes10000), costhetaTimes10000), cosBTimes10000)
                - mul(mul(sinphiTimes10000, sinATimes10000), costhetaTimes10000)
                - mul(sinthetaTimes10000, cosATimes10000)
                - mul(mul(cosphiTimes10000, sinBTimes10000), costhetaTimes10000)
              ) / 10000;
        if (22 > y && y > 0 && x > 0 && 80 > x && DTimes10000 > zTimes10000[o]) {
          zTimes10000[o] = DTimes10000;
          if (N >= 1)
          b[o] = ".,-~:;=!*#$@"[N];
          else
          b[o] = ".,-~:;=!*#$@"[0];
        }
        phi_times_50 += 1;
        update2(&cosphiTimes10000, &sinphiTimes10000, 9998, 200);
      }
      theta_times_14 += 1;
      update2(&costhetaTimes10000, &sinthetaTimes10000, 9974 + theta_times_14 % 2, 714);
    }
    printf("%c[H", 27);
    for (k = 0; 1761 > k; k+=1)
      if (k % 80)
      putchar(b[k]);
      else
      putchar(10);

    update2(&cosATimes10000, &sinATimes10000, 9992, 400);
    update2(&cosBTimes10000, &sinBTimes10000, 9998, 200);
    usleep(50000);
  }
}
