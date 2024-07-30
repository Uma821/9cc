int assert(int expected, int actual) {
  if(expected == actual) {
    printf(" => %ld\n", actual);
    return 0;
  }
  printf(" => %ld(%lx) expected, but got %ld(%lx)\n", expected, expected, actual, actual);
  exit(1);
}

int ret_val1() { return 0; }
int ret_val2() { return 42; }
int ret_val3() { return 5+20-4; }
int ret_val4() { return  12 + 34 - 5 ; }
int ret_val5() { return 5+6*7; }
int ret_val6() { return 5*(9-6); }
int ret_val7() { return (3+5)/2; }
int ret_val8() { return -10+20; }
int ret_val9() { return 13%5; }

int ret_eq1() { return 0==1; }
int ret_eq2() { return 42==42; }
int ret_eq3() { return 0!=1; }
int ret_eq4() { return 42!=42; }

int ret_less1() { return 0<1; }
int ret_less2() { return 1<1; }
int ret_less3() { return 2<1; }
int ret_less4() { return 0<=1; }
int ret_less5() { return 1<=1; }
int ret_less6() { return 2<=1; }

int ret_greater1() { return 1>0; }
int ret_greater2() { return 1>1; }
int ret_greater3() { return 1>2; }
int ret_greater4() { return 1>=0; }
int ret_greater5() { return 1>=1; }
int ret_greater6() { return 1>=2; }

int ret_lvar1() { int a; a=3; return a; }
int ret_lvar2() { int a=3; return a; }
int ret_lvar3() { int a=3; int z=5; return a+z; }

int ret_test1() { return 1; 2; 3; }
int ret_test2() { 1; return 2; 3; }
int ret_test3() { 1; 2; return 3; }

int lvar_init1() { int a=3; return a; }
int lvar_init2() { int a=3; int z=5; return a+z; }
int lvar_init3() { int a; int b; a=b=3; return a+b; }
int lvar_init4() { int foo=3; return foo; }
int lvar_init5() { int foo123=3; int bar=5; return foo123+bar; }

// int boundary_int_test_1(){ int q = 4294967295,p=4294967295,o=2147483648*2;return add(p,q)+o==q+p;}
// int boundary_int_test_2(){ int q = 2147483647,p=2147483647, v= add(p,q);return v == p+q;}
// int boundary_int_test_3(){ int q = 2147483648,p=2147483648, v= add(p,q);v=v/2;q=q/2;return v==q;return v == p+q;}

int stmt_test1( ){ {1; {2;} return 3;} }
int stmt_test2( ){ ;;; return 5; }

int if_test1(){int a = 1; if(a) return 3; return 4;}
int if_test2(){ if (0) return 2; return 3; }
int if_test3(){ if (1-1) return 2; return 3; }
int if_test4(){ if (1) return 2; return 3; }
int if_test5(){ if (2-1) return 2; return 3; }
int if_test6(){ if (0) { 1; 2; return 3; } else { return 4; } }
int if_test7(){ if (1) { 1; 2; return 3; } else { return 4; } }

int loop_test1() { int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }
int loop_test2() { for (;;) return 3; return 5; }
int loop_test3() { int i=0; while(i<10) i=i+1; return i; }

int ptr_test1() { int x=3; return *&x; }
int ptr_test2() { int x=3; int *y=&x; int **z=&y; return **z; }
int ptr_test3() { int x=3; int y=5; return *(&x+1); }
int ptr_test4() { int x=3; int y=5; return *(&y-1); }
int ptr_test5() { int x=3; int y=5; return *(&x-(-1)); }
int ptr_test6() { int x=3; int *y=&x; *y=5; return x; }
int ptr_test7() { int x=3; int y=5; *(&x+1)=7; return y; }
int ptr_test8() { int x=3; int y=5; *(&y-2+1)=7; return x; }
int ptr_test9() { int x=3; return (&x+2)-&x+3; }
int ptr_test10() { int x, y; x=3; y=5; return x+y; }
int ptr_test11() { int x=3, y=5; return x+y; }

int funcall_test1() { return ret3(); }
int funcall_test2() { return ret5(); }
int funcall_test3() { return add(3, 5); }
int funcall_test4() { return sub(5, 3); }
int funcall_test5() { return add6(1,2,3,4,5,6); }
int funcall_test6() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }
int funcall_test7() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); }

int fun_def_call_test1() { return ret32(); } int ret32() { return 32; }
int fun_def_call_test2() { return add2(3,4); } int add2(int x, int y) { return x+y; }
int fun_def_call_test3() { return sub2(4,3); } int sub2(int x, int y) { return x-y; }
int fun_def_call_test4() {
  fib(9);
}
int fib(int x) {
  if (x<=1) return 1;
  return fib(x-1) + fib(x-2);
}

int sizeof_test1() { int x; return sizeof(x); }
int sizeof_test2() { int x; return sizeof x; }
int sizeof_test3() { int *x; return sizeof(x); }
int sizeof_test4() { int x=1; return sizeof(x=2); }
int sizeof_test5() { int x=1; sizeof(x=2); return x; }

int array_test1() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }
int array_test2() { int x[5], *y; *x=4; *(x+4)=8; y=x; return *y + *(y+4); }
int array_test3() { int x[5], *y; *x=17; *(x+4)=1; y=x+4; return *(y-4) - *y; }
int array_test4() { int x[6]; x[5]=21; return *(5+x); }
int array_test5() { int x[10]; x[9]=71; return 9[x]; }
int array_test6() { int x[10]; *(x+0)=1; *(x+1)=1+*(x+0); *(x+2)=*(x+1)+1; x[3]=1+*(x+2); x[4]=*(x+3)+1; x[5]=1+x[4]; x[6]=x[5]+1; 7[x]=1+x[6]; x[8]=7[x]+1; 9[x]=1+8[x]; return 9[x]; }
int array_test7() { int x[5], *y; *x=-9; *(x-(-4))=-23; y=x+4; return *(y-4) - *y; }
int array_test8() { int x[3]; x[1]=5; *(x-(-2))=7; return (x+2) - x; }
int array_test9() { int x[10], y[2]; return y - x; }
int array_test10() { int x[10], *y; *x=13; y=&x[0]+4; *y=*(x+6)=3; return y[-4] + x[4] + y[2]; }

int gvar_pre_a; int gvar_pre1() { return 0; }
int *gvar_pre_b; int gvar_pre2(){ if (1) return 2; return 3; }
int gvar_pre_hoge[10]; int gvar_pre3() { int i=0; while(i<10) i=i+1; return i; }
int *gvar_pre_piyo[10]; int gvar_pre4() { int *x; return sizeof(x); }

int gvar_test1_a; int gvar_test1() { gvar_test1_a=1; return gvar_test1_a; }
int gvar_test2_fuga[5]; int gvar_test2() { int i=0; return gvar_test2_fuga[4]; }
int gvar_test3_fuga[5]; int gvar_test3() { int i=0; while(i<5) { gvar_test3_fuga[i]=i+1; i=i+1; } return gvar_test3_fuga[4]; }
int gvar_test4_fuga[10]; int gvar_test4() { int i=1; while(i<10) { gvar_test4_fuga[i]=2*gvar_test4_fuga[i-1]+1; i=i+1; } return gvar_test4_fuga[4]; }
int *gvar_test5_foo; int gvar_test5() { int x[2]; *x=-6; gvar_test5_foo=x+1; *gvar_test5_foo=x[0]+3; return gvar_test5_foo[0] - gvar_test5_foo[-1] + x[1]; }
int *gvar_test6_foo; int gvar_test6() { int x[2]; *x=-6; gvar_test6_foo=x+1; *gvar_test6_foo=x[0]+3; return - gvar_test6_foo[0] - gvar_test6_foo[-1] + x[1]; }

char char_test_x[3]; char char_test_y[3]; int char_test1() { char_test_x[0] = char_test_y[1]; return 0; }
int char_test2() { char x[3]; x[0] = -1; x[1] = 2; return x[0]; }
char char_test_z[3]; int char_test3() { char_test_x[0] = -1; char_test_x[1] = 2; int y; y = 4; return char_test_x[0] + y; }
int char_test4() { char x[3]; x[1] = 2; x[0] = -1; int y; y = 4; return x[0] + y; }
char char_test4_x[3]; int char_test5() { char_test4_x[0] = -1; return char_test4_x[1] == 0; }
int char_test6() { char x[3]; x[0] = -1; return &x[2]-&x[0]; }

int string_literal_test1() { char *x = "abc"; return x[0]; }
int string_literal_test2() { return "ABC"[0]; }
int string_literal_test3() { return 1["ABC"]; }
int string_literal_test4() { return *"abc"; }
int string_literal_test5() { char *x; x = "abc"; return x[2]-x[0]; }
int string_literal_test6() { return sizeof("abcdefg"); }

int comment_test1() {
  int //b=256876;
  a = 5;
  return a;
}
int comment_test2() {
  int a=10, b=2;
  int c = a//*
  // */ b
  ;
  return c;
}
int comment_test3() { /*commentです
*/ return 1; }

int logical_op1() { return 0||0; }
int logical_op2() { return 2||0; }
int logical_op3() { return 0||1; }
int logical_op4() { return 99||ret16(); } int ret16() { printf("呼び出さないはず\n"); return 16; }
int logical_op5() { return 0&&0; }
int logical_op6() { return 7&&0; }
int logical_op7() { return 0&&ret8(); } int ret8() { printf("呼び出さないはず\n"); return 8; }
int logical_op8() { return 99&&ret3(); }
int logical_op9() { int a=ret0(); return a || (3 && ret5()); } int ret0() { return 0; }

int main() {

  printf("int ret_val1() { return 0; }");
  assert(0, ret_val1());
  printf("int ret_val2() { return 42; }");
  assert(42, ret_val2());
  printf("int ret_val3() { return 5+20-4; }");
  assert(21, ret_val3());
  printf("int ret_val4() { return  12 + 34 - 5 ; }");
  assert(41, ret_val4());
  printf("int ret_val5() { return 5+6*7; }");
  assert(47, ret_val5());
  printf("int ret_val6() { return 5*(9-6); }");
  assert(15, ret_val6());
  printf("int ret_val7() { return (3+5)/2; }");
  assert(4, ret_val7());
  printf("int ret_val8() { return -10+20; }");
  assert(10, ret_val8());
  printf("int ret_val9() { return 13%5; }");
  assert(3, ret_val9());

  printf("int ret_eq1() { return 0==1; }");
  assert(0, ret_eq1());
  printf("int ret_eq2() { return 42==42; }");
  assert(1, ret_eq2());
  printf("int ret_eq3() { return 0!=1; }");
  assert(1, ret_eq3());
  printf("int ret_eq4() { return 42!=42; }");
  assert(0, ret_eq4());

  printf("int ret_less1() { return 0<1; }");
  assert(1, ret_less1());
  printf("int ret_less2() { return 1<1; }");
  assert(0, ret_less2());
  printf("int ret_less3() { return 2<1; }");
  assert(0, ret_less3());
  printf("int ret_less4() { return 0<=1; }");
  assert(1, ret_less4());
  printf("int ret_less5() { return 1<=1; }");
  assert(1, ret_less5());
  printf("int ret_less6() { return 2<=1; }");
  assert(0, ret_less6());

  printf("int ret_greater1() { return 1>0; }");
  assert(1, ret_greater1());
  printf("int ret_greater2() { return 1>1; }");
  assert(0, ret_greater2());
  printf("int ret_greater3() { return 1>2; }");
  assert(0, ret_greater3());
  printf("int ret_greater4() { return 1>=0; }");
  assert(1, ret_greater4());
  printf("int ret_greater5() { return 1>=1; }");
  assert(1, ret_greater5());
  printf("int ret_greater6() { return 1>=2; }");
  assert(0, ret_greater6());

  printf("int ret_lvar1() { int a; a=3; return a; }");
  assert(3, ret_lvar1());
  printf("int ret_lvar2() { int a=3; return a; }");
  assert(3, ret_lvar2());
  printf("int ret_lvar3() { int a=3; int z=5; return a+z; }");
  assert(8, ret_lvar3());

  // printf("int main(){ int q = 4294967295,p=4294967295,o=2147483648*2;return add(p,q)+o==q+p;}");
  // assert(1, boundary_int_test_1());
  // printf("int main(){ int q = 2147483647,p=2147483647, v= add(p,q);return v == p+q;}");
  // assert(1, boundary_int_test_2());
  // printf("int main(){ int q = 2147483648,p=2147483648, v= add(p,q);v=v/2;q=q/2;return v==q;return v == p+q;}");
  // assert(1, boundary_int_test_3());

  printf("int ret_test1() { return 1; 2; 3; }");
  assert(1, ret_test1());
  printf("int ret_test2() { 1; return 2; 3; }");
  assert(2, ret_test2());
  printf("int ret_test3() { 1; 2; return 3; }");
  assert(3, ret_test3());

  printf("int lvar_init1() { int a=3; return a; }");
  assert(3, lvar_init1());
  printf("int lvar_init2() { int a=3; int z=5; return a+z; }");
  assert(8, lvar_init2());
  printf("int lvar_init3() { int a; int b; a=b=3; return a+b; }");
  assert(6, lvar_init3());
  printf("int lvar_init4() { int foo=3; return foo; }");
  assert(3, lvar_init4());
  printf("int lvar_init5() { int foo123=3; int bar=5; return foo123+bar; }");
  assert(8, lvar_init5());

  printf("int stmt_test1( ){ {1; {2;} return 3;} }");
  assert(3, stmt_test1());
  printf("int stmt_test2( ){ ;;; return 5; }");
  assert(5, stmt_test2());

  printf("int if_test1(){int a = 1; if(a) return 3; return 4;}");
  assert(3, if_test1());
  printf("int if_test2(){ if (0) return 2; return 3; }");
  assert(3, if_test2());
  printf("int if_test3(){ if (1-1) return 2; return 3; }");
  assert(3, if_test3());
  printf("int if_test4(){ if (1) return 2; return 3; }");
  assert(2, if_test4());
  printf("int if_test5(){ if (2-1) return 2; return 3; }");
  assert(2, if_test5());
  printf("int if_test6(){ if (0) { 1; 2; return 3; } else { return 4; } }");
  assert(4, if_test6());
  printf("int if_test7(){ if (1) { 1; 2; return 3; } else { return 4; } }");
  assert(3, if_test7());

  printf("int loop_test1() { int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }");
  assert(55, loop_test1());
  printf("int loop_test2() { for (;;) return 3; return 5; }");
  assert(3, loop_test2());
  printf("int loop_test3() { int i=0; while(i<10) i=i+1; return i; }");
  assert(10, loop_test3());

  printf("int ptr_test1() { int x=3; return *&x; }");
  assert(3, ptr_test1());
  printf("int ptr_test2() { int x=3; int *y=&x; int **z=&y; return **z; }");
  assert(3, ptr_test2());
  printf("int ptr_test3() { int x=3; int y=5; return *(&x+1); }");
  assert(5, ptr_test3());
  printf("int ptr_test4() { int x=3; int y=5; return *(&y-1); }");
  assert(3, ptr_test4());
  printf("int ptr_test5() { int x=3; int y=5; return *(&x-(-1)); }");
  assert(5, ptr_test5());
  printf("int ptr_test6() { int x=3; int *y=&x; *y=5; return x; }");
  assert(5, ptr_test6());
  printf("int ptr_test7() { int x=3; int y=5; *(&x+1)=7; return y; }");
  assert(7, ptr_test7());
  printf("int ptr_test8() { int x=3; int y=5; *(&y-2+1)=7; return x; }");
  assert(7, ptr_test8());
  printf("int ptr_test9() { int x=3; return (&x+2)-&x+3; }");
  assert(5, ptr_test9());
  printf("int ptr_test10) { int x, y; x=3; y=5; return x+y; }");
  assert(8, ptr_test10());
  printf("int ptr_test11 { int x=3, y=5; return x+y; }");
  assert(8, ptr_test11());

  printf("int funcall_test1() { return ret3(); }");
  assert(3, funcall_test1());
  printf("int funcall_test2() { return ret5(); }");
  assert(5, funcall_test2());
  printf("int funcall_test3() { return add(3, 5); }");
  assert(8, funcall_test3());
  printf("int funcall_test4() { return sub(5, 3); }");
  assert(2, funcall_test4());
  printf("int funcall_test5() { return add6(1,2,3,4,5,6); }");
  assert(21, funcall_test5());
  printf("int funcall_test6() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }");
  assert(66, funcall_test6());
  printf("int funcall_test7() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); }");
  assert(136, funcall_test7());

  printf("int fun_def_call_test1() { return ret32(); } int ret32() { return 32; }");
  assert(32, fun_def_call_test1());
  printf("int fun_def_call_test2() { return add2(3,4); } int add2(int x, int y) { return x+y; }");
  assert(7, fun_def_call_test2());
  printf("int fun_def_call_test3() { return sub2(4,3); } int sub2(int x, int y) { return x-y; }");
  assert(1, fun_def_call_test3());
  printf("int fun_def_call_test4() {\n  return fib(9);\n}\nint fib(int x) {\n  if (x<=1) return 1;\n  return fib(x-1) + fib(x-2);\n}");
  assert(55, fun_def_call_test4());

  printf("int sizeof_test1() { int x; return sizeof(x); }");
  assert(8, sizeof_test1());
  printf("int sizeof_test2() { int x; return sizeof x; }");
  assert(8, sizeof_test2());
  printf("int sizeof_test3() { int *x; return sizeof(x); }");
  assert(8, sizeof_test3());
  printf("int sizeof_test4() { int x=1; return sizeof(x=2); }");
  assert(8, sizeof_test4());
  printf("int sizeof_test5() { int x=1; sizeof(x=2); return x; }");
  assert(1, sizeof_test5());

  printf("int array_test1() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }");
  assert(3, array_test1());
  printf("int array_test2() { int x[5], *y; *x=4; *(x+4)=8; y=x; return *y + *(y+4); }");
  assert(12, array_test2());
  printf("int array_test3() { int x[5], *y; *x=17; *(x+4)=1; y=x+4; return *(y-4) - *y; }");
  assert(16, array_test3());
  printf("int array_test4() { int x[6]; x[5]=21; return *(5+x); }");
  assert(21, array_test4());
  printf("int array_test5() { int x[10]; x[9]=71; return 9[x]; }");
  assert(71, array_test5());
  printf("int array_test6() { int x[10]; *(x+0)=1; *(x+1)=1+*(x+0); *(x+2)=*(x+1)+1; x[3]=1+*(x+2); x[4]=*(x+3)+1; x[5]=1+x[4]; x[6]=x[5]+1; 7[x]=1+x[6]; x[8]=7[x]+1; 9[x]=1+8[x]; return 9[x]; }");
  assert(10, array_test6());
  printf("int array_test7() { int x[5], *y; *x=-9; *(x-(-4))=-23; y=x+4; return *(y-4) - *y; }");
  assert(14, array_test7());
  printf("int array_test8() { int x[3]; x[1]=5; *(x-(-2))=7; return (x+2) - x; }");
  assert(2, array_test8());
  printf("int array_test9() { int x[10], y[2]; return y - x; }");
  assert(10, array_test9());
  printf("int array_test10() { int x[10], *y; *x=13; y=&x[0]+4; *y=*(x+6)=3; return y[-4] + x[4] + y[2]; }");
  assert(19, array_test10());

  printf("int a; int gvar_pre1() { return 0; }");
  assert(0, gvar_pre1());
  printf("int *b; int gvar_pre2(){ if (1) return 2; return 3; }");
  assert(2, gvar_pre2());
  printf("int hoge[10]; int gvar_pre3() { int i=0; while(i<10) i=i+1; return i; }");
  assert(10, gvar_pre3());
  printf("int *piyo[10]; int gvar_pre4() { int *x; return sizeof(x); }");
  assert(8, gvar_pre4());

  printf("int a; int gvar_test1() { a=1; return a; }");
  assert(1, gvar_test1());
  printf("int fuga[5]; int gvar_test2() { int i=0; return fuga[4]; }");
  assert(0, gvar_test2());
  printf("int fuga[5]; int gvar_test3() { int i=0; while(i<5) { fuga[i]=i+1; i=i+1; } return fuga[4]; }");
  assert(5, gvar_test3());
  printf("int fuga[10]; int gvar_test4() { int i=1; while(i<10) { fuga[i]=2*fuga[i-1]+1; i=i+1; } return fuga[4]; }");
  assert(15, gvar_test4());
  printf("int *foo; int gvar_test5() { int x[2]; *x=-6; foo=x+1; *foo=x[0]+3; return foo[0] - foo[-1] + x[1]; }");
  assert(0, gvar_test5());
  printf("int *foo; int gvar_test6() { int x[2]; *x=-6; foo=x+1; *foo=x[0]+3; return - foo[0] - foo[-1] + x[1]; }");
  assert(6, gvar_test6());

  printf("char x[3]; char y[3]; int char_test1() { x[0] = y[1]; return 0; }");
  assert(0, char_test1());
  printf("int char_test2() { char x[3]; x[0] = -1; x[1] = 2; return x[0]; }");
  assert(-1, char_test2());
  printf("char x[3]; int char_test3() { x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y; }");
  assert(3, char_test3());
  printf("int char_test4() { char x[3]; x[1] = 2; x[0] = -1; int y; y = 4; return x[0] + y; }");
  assert(3, char_test4());
  printf("char x[3]; int char_test5() { x[0] = -1; return x[1] == 0; }");
  assert(1, char_test5());
  printf("int char_test6() { char x[3]; x[0] = -1; return &x[2]-&x[0]; }");
  assert(2, char_test6());

  printf("int string_literal_test1() { char *x = \"abc\"; return x[0]; }");
  assert(97, string_literal_test1());
  printf("int string_literal_test2() { return \"ABC\"[0]; }");
  assert(65, string_literal_test2());
  printf("int string_literal_test3() { return 1[\"ABC\"]; }");
  assert(66, string_literal_test3());
  printf("int string_literal_test4() { return *\"abc\"; }");
  assert(97, string_literal_test4());
  printf("int string_literal_test5() { char *x; x = \"abc\"; return x[2]-x[0]; }");
  assert(2, string_literal_test5());
  printf("int string_literal_test6() { return sizeof(\"abcdefg\"); }");
  assert(8, string_literal_test6());

  printf("int comment_test1() {\n  int //b=256876;\n  a = 5;\n  return a;\n}");
  assert(5, comment_test1());
  printf("int comment_test2() {\n  int a=10, b=2;\n  int c = a//*\n  // */ b\n  ;\n  return c;\n}");
  assert(10, comment_test2());
  printf("int comment_test3() { /*commentです\n*/ return 1; }");
  assert(1, comment_test3());

  printf("int logical_op1() { return 0||0; }");
  assert(0, logical_op1());
  printf("int logical_op2() { return 2||0; }");
  assert(1, logical_op2());
  printf("int logical_op3() { return 0||1; }");
  assert(1, logical_op3());
  printf("int logical_op4() { return 99||ret16(); } int ret16() { printf(\"呼び出さないはず\\n\"); return 16; }");
  assert(1, logical_op4());
  printf("int logical_op5() { return 0&&0; }");
  assert(0, logical_op5());
  printf("int logical_op6() { return 7&&0; }");
  assert(0, logical_op6());
  printf("int logical_op7() { return 0&&ret8(); } int ret8() { printf(\"呼び出さないはず\\n\"); return 8; }");
  assert(0, logical_op7());
  printf("int logical_op8() { return 99&&ret3(); }");
  assert(1, logical_op8());
  printf("int logical_op9() { int a=ret0(); return a || (3 && ret5()); } int ret0() { return 0; }");
  assert(1, logical_op9());
}