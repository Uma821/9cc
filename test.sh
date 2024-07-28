#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }
int add6(int a, int b, int c, int d, int e, int f) {
  return a+b+c+d+e+f;
}
EOF

assert() {
  expected="$1"
  input="$2"

  echo "$input" > tmp.c
  ./9cc tmp.c > tmp.s
  cc -static -o tmp tmp.s tmp2.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

#assert 1 'int main(){ int q = 4294967295,p=4294967295,o=2147483648*2;return add(p,q)+o==q+p;}'
#assert 1 'int main(){ int q = 4294967295,p=4294967295,o=2147483648*2;return add(p,q)+o==q+p;}'
#assert 1 'int main(){ int q = 2147483647,p=2147483647, v= add(p,q);return v == p+q;}'
#assert 1 'int main(){ int q = 2147483648,p=2147483648, v= add(p,q);v=v/2;q=q/2;return v==q;return v == p+q;}'


assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'

assert 0 'int main() { return 0==1; }'
assert 1 'int main() { return 42==42; }'
assert 1 'int main() { return 0!=1; }'
assert 0 'int main() { return 42!=42; }'

assert 1 'int main() { return 0<1; }'
assert 0 'int main() { return 1<1; }'
assert 0 'int main() { return 2<1; }'
assert 1 'int main() { return 0<=1; }'
assert 1 'int main() { return 1<=1; }'
assert 0 'int main() { return 2<=1; }'

assert 1 'int main() { return 1>0; }'
assert 0 'int main() { return 1>1; }'
assert 0 'int main() { return 1>2; }'
assert 1 'int main() { return 1>=0; }'
assert 1 'int main() { return 1>=1; }'
assert 0 'int main() { return 1>=2; }'

assert 3 'int main() { int a; a=3; return a; }'
assert 3 'int main() { int a=3; return a; }'
assert 8 'int main() { int a=3; int z=5; return a+z; }'

assert 1 'int main() { return 1; 2; 3; }'
assert 2 'int main() { 1; return 2; 3; }'
assert 3 'int main() { 1; 2; return 3; }'

assert 3 'int main() { int a=3; return a; }'
assert 8 'int main() { int a=3; int z=5; return a+z; }'
assert 6 'int main() { int a; int b; a=b=3; return a+b; }'
assert 3 'int main() { int foo=3; return foo; }'
assert 8 'int main() { int foo123=3; int bar=5; return foo123+bar; }'

assert 3 'int main( ){ {1; {2;} return 3;} }'
assert 5 'int main( ){ ;;; return 5; }'

assert 3 'int main(){int a = 1; if(a) return 3; return 4;}'
assert 3 'int main(){ if (0) return 2; return 3; }'
assert 3 'int main(){ if (1-1) return 2; return 3; }'
assert 2 'int main(){ if (1) return 2; return 3; }'
assert 2 'int main(){ if (2-1) return 2; return 3; }'
assert 4 'int main(){ if (0) { 1; 2; return 3; } else { return 4; } }'
assert 3 'int main(){ if (1) { 1; 2; return 3; } else { return 4; } }'

assert 55 'int main() { int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'
assert 3 'int main() { for (;;) return 3; return 5; }'

assert 10 'int main() { int i=0; while(i<10) i=i+1; return i; }'

assert 3 'int main() { int x=3; return *&x; }'
assert 3 'int main() { int x=3; int *y=&x; int **z=&y; return **z; }'
assert 5 'int main() { int x=3; int y=5; return *(&x+1); }'
assert 3 'int main() { int x=3; int y=5; return *(&y-1); }'
assert 5 'int main() { int x=3; int y=5; return *(&x-(-1)); }'
assert 5 'int main() { int x=3; int *y=&x; *y=5; return x; }'
assert 7 'int main() { int x=3; int y=5; *(&x+1)=7; return y; }'
assert 7 'int main() { int x=3; int y=5; *(&y-2+1)=7; return x; }'
assert 5 'int main() { int x=3; return (&x+2)-&x+3; }'
assert 8 'int main() { int x, y; x=3; y=5; return x+y; }'
assert 8 'int main() { int x=3, y=5; return x+y; }'

assert 3 'int main() { return ret3(); }'
assert 5 'int main() { return ret5(); }'
assert 8 'int main() { return add(3, 5); }'
assert 2 'int main() { return sub(5, 3); }'
assert 21 'int main() { return add6(1,2,3,4,5,6); }'
assert 66 'int main() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }'
assert 136 'int main() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); }'

assert 32 'int main() { return ret32(); } int ret32() { return 32; }'
assert 7 'int main() { return add2(3,4); } int add2(int x, int y) { return x+y; }'
assert 1 'int main() { return sub2(4,3); } int sub2(int x, int y) { return x-y; }'
assert 55 'int main() {
  return fib(9);
}
int fib(int x) {
  if (x<=1) return 1;
  return fib(x-1) + fib(x-2);
}'

assert 8 'int main() { int x; return sizeof(x); }'
assert 8 'int main() { int x; return sizeof x; }'
assert 8 'int main() { int *x; return sizeof(x); }'
assert 8 'int main() { int x=1; return sizeof(x=2); }'
assert 1 'int main() { int x=1; sizeof(x=2); return x; }'

assert 3 'int main() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }'
assert 12 'int main() { int x[5], *y; *x=4; *(x+4)=8; y=x; return *y + *(y+4); }'
assert 16 'int main() { int x[5], *y; *x=17; *(x+4)=1; y=x+4; return *(y-4) - *y; }'
assert 21 'int main() { int x[6]; x[5]=21; return *(5+x); }'
assert 71 'int main() { int x[10]; x[9]=71; return 9[x]; }'
assert 10 'int main() { int x[10]; *(x+0)=1; *(x+1)=1+*(x+0); *(x+2)=*(x+1)+1; x[3]=1+*(x+2); x[4]=*(x+3)+1; x[5]=1+x[4]; x[6]=x[5]+1; 7[x]=1+x[6]; x[8]=7[x]+1; 9[x]=1+8[x]; return 9[x]; }'
assert 14 'int main() { int x[5], *y; *x=-9; *(x-(-4))=-23; y=x+4; return *(y-4) - *y; }'
assert 2 'int main() { int x[3]; x[1]=5; *(x-(-2))=7; return (x+2) - x; }'
assert 10 'int main() { int x[10], y[2]; return y - x; }'
assert 19 'int main() { int x[10], *y; *x=13; y=&x[0]+4; *y=*(x+6)=3; return y[-4] + x[4] + y[2]; }'

assert 0 'int a; int main() { return 0; }'
assert 2 'int *b; int main(){ if (1) return 2; return 3; }'
assert 10 'int hoge[10]; int main() { int i=0; while(i<10) i=i+1; return i; }'
assert 8 'int *piyo[10]; int main() { int *x; return sizeof(x); }'

assert 1 'int a; int main() { a=1; return a; }'
assert 0 'int fuga[5]; int main() { int i=0; return fuga[4]; }'
assert 5 'int fuga[5]; int main() { int i=0; while(i<5) { fuga[i]=i+1; i=i+1; } return fuga[4]; }'
assert 15 'int fuga[10]; int main() { int i=1; while(i<10) { fuga[i]=2*fuga[i-1]+1; i=i+1; } return fuga[4]; }'
assert 0 'int *foo; int main() { int x[2]; *x=-6; foo=x+1; *foo=x[0]+3; return foo[0] - foo[-1] + x[1]; }'
assert 6 'int *foo; int main() { int x[2]; *x=-6; foo=x+1; *foo=x[0]+3; return - foo[0] - foo[-1] + x[1]; }'

assert 0 'char x[3]; char y[3]; int main() { x[0] = y[1]; return 0; }'
assert 3 'char x[3]; int main() { x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y; }'
assert 3 'int main() { char x[3]; x[1] = 2; x[0] = -1; int y; y = 4; return x[0] + y; }'
assert 1 'char x[3]; int main() { x[0] = -1; return x[1] == 0; }'
assert 2 'int main() { char x[3]; x[0] = -1; return &x[2]-&x[0]; }'

assert 97 'int main() { char *x = "abc"; return x[0]; }'
assert 2 'int main() { char *x; x = "abc"; return x[2]-x[0]; }'
assert 8 'int main() { return sizeof("abcdefg"); }'

echo OK
