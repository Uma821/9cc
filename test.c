int assert(int expected, int actual) {
  if(expected == actual) {
    printf(" => %ld\n", actual);
    return 0;
  }
  printf(" => %ld(0x%lx) expected, but got %ld(0x%lx)\n", expected, expected, actual, actual);
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
int ret_val10() { return (3-5)/2; }
int ret_val11() { return 4*(6-9); }
int ret_val12() { return (7-4)/-2; }

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
int loop_test4() { int j=0; for (int i=0; i<=10; ++i) j=i+j; return j; }

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
int sizeof_test6() { int a; return sizeof(int); }
int sizeof_test7() { return sizeof(int*); }
int sizeof_test8() { int a; return sizeof(char[2]); }
int sizeof_test9() { return sizeof(int[5]); }
int sizeof_test10() { return sizeof(int*[5]); }
int sizeof_test11() { return sizeof(char*[10]); }

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

int gvar_test_a; int gvar_test1() { gvar_test_a=1; return gvar_test_a; }
int gvar_test_fuga[5]; int gvar_test2() { int i=0; return gvar_test_fuga[4]; }
int gvar_test_hoge[5]; int gvar_test3() { int i=0; while(i<5) { gvar_test_hoge[i]=i+1; i=i+1; } return gvar_test_hoge[4]; }
int gvar_test_piyo[10]; int gvar_test4() { int i=1; while(i<10) { gvar_test_piyo[i]=2*gvar_test_piyo[i-1]+1; i=i+1; } return gvar_test_piyo[4]; }
int *gvar_test_foo; int gvar_test5() { int x[2]; *x=-6; gvar_test_foo=x+1; *gvar_test_foo=x[0]+3; return gvar_test_foo[0] - gvar_test_foo[-1] + x[1]; }
int *gvar_test_bar; int gvar_test6() { int x[2]; *x=-6; gvar_test_bar=x+1; *gvar_test_bar=x[0]+3; return - gvar_test_bar[0] - gvar_test_bar[-1] + x[1]; }

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

int grobal_var_init_a=1; int grobal_var_init1() { return grobal_var_init_a; }
char *grobal_var_init_x = "abc"; int grobal_var_init2() { return grobal_var_init_x[0]; }
int grobal_var_init_b[3] = {0, 1, 2}; int grobal_var_init3() { return grobal_var_init_b[0]+grobal_var_init_b[1]+grobal_var_init_b[2]; }
int grobal_var_init_c[5] = {0, 1, 2}; int grobal_var_init4() { return grobal_var_init_c[0]+grobal_var_init_c[1]+grobal_var_init_c[2]+grobal_var_init_c[3]+grobal_var_init_c[4]; }
char grobal_var_init_d[7] = "foobar"; int grobal_var_init5() { return grobal_var_init_d[0]+grobal_var_init_d[1]+grobal_var_init_d[2]+grobal_var_init_d[3]+grobal_var_init_d[4]+grobal_var_init_d[5]+grobal_var_init_d[6]; }
char grobal_var_init_e[7] = "piyo"; int grobal_var_init6() { return grobal_var_init_e[4]+grobal_var_init_e[5]+grobal_var_init_e[6]; }
char grobal_var_init_f[10] = "abcdefg"; int grobal_var_init7() { return sizeof(grobal_var_init_f); }

int lvar_array_init1() { int a[2] = {1, 2}; int *p; p = a; return *p + *(p + 1); }
int lvar_array_init2() { int c[5] = {0, 1, 2}; return c[0]+c[1]+c[2]+c[3]+c[4]; }
int lvar_array_init3() { char d[7] = "foobar"; return d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]; }
int lvar_array_init4() { char e[7] = "piyo"; return e[4]+e[5]+e[6]; }
int lvar_array_init5() { char f[10] = "abcdefg"; return sizeof(f); }

int compound_assign1() { int a=1, b=2; a += b; return a; }
int compound_assign2() { int a=2, b=1; a -= b; return a; }
int compound_assign3() { int a=2, b=3; a *= b; return a; }
int compound_assign4() { int a=10, b=4; a /= b; return a; }
int compound_assign5() { int a=10, b=3; a %= b; return a; }
int compound_assign6() { int a[2] = {1, 2}; int *p=a; p+=1; return *p; }
int compound_assign7() { char d[7] = "foobar"; char *p=d+1,*q; q=&p[0]+3;q+=1; return *q; }
int compound_assign8() { int a[2] = {1, 2}; int *p=a+1; p-=1; return p[0]; }
int compound_assign9() { char d[7] = "foobar"; char *p=d+1,*q; q=&p[0]-1;q-=-2; return *q; }
// int compound_assign10() { int a[3] = {0, 1, 2}; int *p=&a[2]; p-=a; return p; }
// int compound_assign11() { char d[7] = "foobar"; char *p=d+8; p-=d; return p; }

int incr_decr1() { int a=10; ++a; return a++; }
int incr_decr2() { char a=10; ++a; return a++; }
int incr_decr3() { int a[2], *b; b=a+5; return b++-a; }
int incr_decr4() { char a[2], *b; b=a+5; return b++-a; }
int incr_decr5() { int a=10; a++; return ++a; }
int incr_decr6() { char a=10; a++; return ++a; }
int incr_decr7() { int a[2], *b; b=a+5; return ++b-a; }
int incr_decr8() { char a[2], *b; b=a+5; return ++b-a; }

int exclamation1() { return !0; }
int exclamation2() { return !42; }
int exclamation3() { return !(5+20-4); }
int exclamation4() { return  !12 + 34 - 5 ; }
int exclamation5() { return !5+6*7; }
int exclamation6() { return 5*!(9-9); }
int exclamation7() { return !(3+5)/2; }
int exclamation8() { return !-10+20; }
int exclamation9() { return !13%5; }
int exclamation10() { int a=0; return !a; }
int exclamation11() { char *x = "abc"; return x[!0]; }

struct A {
  char mem1;
  int mem2;
  int mem3;
  char mem4;
};
struct B {
  int mem1;
  int mem2;
  char mem3;
  char mem4;
};
struct C {
  int mem1;
  struct B mem2;
  char mem3;
  struct B *mem4;
};

int struct_test1() { struct A a; a.mem1 = 1; a.mem2 = 2; a.mem3 = 3; a.mem4 = 4; return a.mem1; }
int struct_test2() { struct B b; b.mem1 = 1; b.mem2 = 2; b.mem3 = 3; b.mem4 = 4; return b.mem1; }
int struct_test3() { struct A aa[1], *p[1]; int a[3] = {0, 1, 2}; aa->mem2; return sizeof(struct A);; }
int struct_test4() { struct C cc; cc.mem2.mem1 = 2; cc.mem2.mem2 = 1; cc.mem3 = 10;; return cc.mem2.mem1+cc.mem2.mem2; }
int struct_test5() { struct C cc, *pcc; pcc = &cc;(*pcc).mem2.mem1 = 10; pcc->mem2.mem2 = 10; cc.mem3 = 40;; return cc.mem2.mem1+(*pcc).mem2.mem2; }
int struct_test6() { struct C cc, *pcc; pcc = &cc;pcc->mem2.mem2 = 5; (*pcc).mem2.mem1 = -4; (*pcc).mem4 = &(*pcc).mem2; cc.mem3 = -3;; return cc.mem2.mem1+pcc->mem4->mem2; }

char integer_promotion1() {
    char a = 30;
    return (a * a) / 25; // 36
}
char integer_promotion2() {
    char a = 30;
    char b = a * a; // 900 ==> -124 (mod 256)
    return b / 25; // -4
}

char char_pointer_to[20] = "akasatana";
int *pointer_to_int() { return &gvar_test_a; }
char *pointer_to_char() { return char_pointer_to+5; }

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
  printf("int ret_val10() { return (3-5)/2; }");
  assert(-1, ret_val10());
  printf("int ret_val11() { return 4*(6-9); }");
  assert(-12, ret_val11());
  printf("int ret_val12() { return (7-4)/-2; }");
  assert(-1, ret_val12());

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
  printf("int loop_test4() { int j=0; for (int i=0; i<=10; ++i) j=i+j; return j; }");
  assert(55, loop_test4());

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
  printf("int sizeof_test6() { int a; return sizeof(int); }");
  assert(8, sizeof_test6());
  printf("int sizeof_test7() { return sizeof(int*); }");
  assert(8, sizeof_test7());
  printf("int sizeof_test8() { int a; return sizeof(char[2]); }");
  assert(2, sizeof_test8());
  printf("int sizeof_test9() { return sizeof(int[5]); }");
  assert(40, sizeof_test9());
  printf("int sizeof_test10() { return sizeof(int*[5]); }");
  assert(40, sizeof_test10());
  printf("int sizeof_test11() { return sizeof(char*[10]); }");
  assert(80, sizeof_test11());

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

  printf("int a; int gvar_test1() { a=1; return a; }");
  assert(1, gvar_test1());
  printf("int fuga[5]; int gvar_test2() { int i=0; return fuga[4]; }");
  assert(0, gvar_test2());
  printf("int hoge[5]; int gvar_test3() { int i=0; while(i<5) { hoge[i]=i+1; i=i+1; } return hoge[4]; }");
  assert(5, gvar_test3());
  printf("int piyo[10]; int gvar_test4() { int i=1; while(i<10) { piyo[i]=2*piyo[i-1]+1; i=i+1; } return piyo[4]; }");
  assert(15, gvar_test4());
  printf("int *foo; int gvar_test5() { int x[2]; *x=-6; foo=x+1; *foo=x[0]+3; return foo[0] - foo[-1] + x[1]; }");
  assert(0, gvar_test5());
  printf("int *bar; int gvar_test6() { int x[2]; *x=-6; bar=x+1; *bar=x[0]+3; return - bar[0] - bar[-1] + x[1]; }");
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

  printf("int a=1; int grobal_var_init1() { return a; }");
  assert(1, grobal_var_init1());
  printf("char *x = \"abc\"; int grobal_var_init2() { return x[0]; }");
  assert(97, grobal_var_init2());
  printf("int b[3] = {0, 1, 2}; int grobal_var_init3() { return b[0]+b[1]+b[2]; }");
  assert(3, grobal_var_init3());
  printf("int c[5] = {0, 1, 2}; int grobal_var_init4() { return c[0]+c[1]+c[2]+c[3]+c[4]; }");
  assert(3, grobal_var_init4());
  printf("char d[7] = \"foobar\"; int grobal_var_init5() { return d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]; }");
  assert(633, grobal_var_init5());
  printf("char e[7] = \"piyo\"; int grobal_var_init6() { return e[4]+e[5]+e[6]; }");
  assert(0, grobal_var_init6());
  printf("char f[10] = \"abcdefg\"; int grobal_var_init7() { return sizeof(f); }");
  assert(10, grobal_var_init7());

  printf("int lvar_array_init1() { int a[2] = {1, 2}; int *p = a; return *p + *(p + 1); }");
  assert(3, lvar_array_init1());
  printf("int lvar_array_init2() { int c[5] = {0, 1, 2}; return c[0]+c[1]+c[2]+c[3]+c[4]; }");
  assert(3, lvar_array_init2());
  printf("int lvar_array_init3() { char d[7] = \"foobar\"; return d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]; }");
  assert(633, lvar_array_init3());
  printf("int lvar_array_init4() { char e[7] = \"piyo\"; return e[4]+e[5]+e[6]; }");
  assert(0, lvar_array_init4());
  printf("int lvar_array_init5() { char f[10] = \"abcdefg\"; return sizeof(f); }");
  assert(10, lvar_array_init5());

  printf("int compound_assign1() { int a=1, b=2; a += b; return a; }");
  assert(3, compound_assign1());
  printf("int compound_assign2() { int a=2, b=1; a -= b; return a; }");
  assert(1, compound_assign2());
  printf("int compound_assign3() { int a=2, b=3; a *= b; return a; }");
  assert(6, compound_assign3());
  printf("int compound_assign4() { int a=10, b=4; a /= b; return a; }");
  assert(2, compound_assign4());
  printf("int compound_assign5() { int a=10, b=3; a %= b; return a; }");
  assert(1, compound_assign5());
  printf("int compound_assign6() { int a[2] = {1, 2}; int *p=a; p+=1; return p; }");
  assert(2, compound_assign6());
  printf("int compound_assign7() { char d[7] = \"foobar\"; char *p=d+1,*q; q=&p[0]+3;q+=1; return *q; }");
  assert(114, compound_assign7());
  printf("int compound_assign8() { int a[2] = {1, 2}; int *p=a+1; p-=1; return p[0]; }");
  assert(1, compound_assign8());
  printf("int compound_assign9() { char d[7] = \"foobar\"; char *p=d+1,*q; q=&p[0]-1;q-=-2; return *q; }");
  assert(111, compound_assign9());
  // printf("int compound_assign10() { int a[3] = {0, 1, 2}; int *p=&a[2]; p-=a; return p; }");
  // assert(2, compound_assign10());
  // printf("int compound_assign11() { char d[7] = \"foobar\"; char *p=d+8; p-=d; return p; }");
  // assert(8, compound_assign11());

  printf("int incr_decr1() { int a=10; ++a; return a++; }");
  assert(11, incr_decr1());
  printf("int incr_decr2() { char a=10; ++a; return a++; }");
  assert(11, incr_decr2());
  printf("int incr_decr3() { int a[2], *b; b=a+5; return b++-a; }");
  assert(5, incr_decr3());
  printf("int incr_decr4() { char a[2], *b; b=a+5; return b++-a; }");
  assert(5, incr_decr4());
  printf("int incr_decr5() { int a=10; a++; return ++a; }");
  assert(12, incr_decr5());
  printf("int incr_decr6() { char a=10; a++; return ++a; }");
  assert(12, incr_decr6());
  printf("int incr_decr7() { int a[2], *b; b=a+5; return ++b-a; }");
  assert(6, incr_decr7());
  printf("int incr_decr8() { char a[2], *b; b=a+5; return ++b-a; }");
  assert(6, incr_decr8());

  printf("int exclamation1() { return !0; }");
  assert(1, exclamation1());
  printf("int exclamation2() { return !42; }");
  assert(0, exclamation2());
  printf("int exclamation3() { return !(5+20-4); }");
  assert(0, exclamation3());
  printf("int exclamation4() { return  !12 + 34 - 5 ; }");
  assert(29, exclamation4());
  printf("int exclamation5() { return !5+6*7; }");
  assert(42, exclamation5());
  printf("int exclamation6() { return 5*!(9-9); }");
  assert(5, exclamation6());
  printf("int exclamation7() { return !(3+5)/2; }");
  assert(0, exclamation7());
  printf("int exclamation8() { return !-10+20; }");
  assert(20, exclamation8());
  printf("int exclamation9() { return !13%5; }");
  assert(0, exclamation9());
  printf("int exclamation10() { int a=0; return !a; }");
  assert(1, exclamation10());
  printf("int exclamation11() { char *x = \"abc\"; return x[!0]; }");
  assert(98, exclamation11());

  printf("_Alignof(int)");
  assert(8, _Alignof(int));
  printf("_Alignof(int[4])");
  assert(8, _Alignof(int[4]));
  printf("_Alignof(int*)");
  assert(8, _Alignof(int*));
  printf("_Alignof(char)");
  assert(1, _Alignof(char));
  printf("_Alignof(char[5])");
  assert(1, _Alignof(char[5]));
  printf("_Alignof(char*)");
  assert(8, _Alignof(char*));
  printf("_Alignof(struct A)");
  assert(8, _Alignof(struct A));
  printf("sizeof(struct A)");
  assert(32, sizeof(struct A));
  printf("sizeof(struct B)");
  assert(24, sizeof(struct B));

  printf("offsetof(struct A,mem1)");
  assert(0, offsetof(struct A,mem1));
  printf("offsetof(struct A,mem2)");
  assert(8, offsetof(struct A,mem2));
  printf("offsetof(struct A,mem3)");
  assert(16, offsetof(struct A,mem3));
  printf("offsetof(struct A,mem4)");
  assert(24, offsetof(struct A,mem4));
  printf("offsetof(struct B,mem4)");
  assert(17, offsetof(struct B,mem4));

  printf("int struct_test1() { struct A a; a.mem1 = 1; a.mem2 = 2; a.mem3 = 3; a.mem4 = 4; return a.mem1; }");
  assert(1, struct_test1());
  printf("int struct_test2() { struct B b; b.mem1 = 1; b.mem2 = 2; b.mem3 = 3; b.mem4 = 4; return b.mam1; }");
  assert(1, struct_test2());
  printf("int struct_test3() { struct A aa[1], *p[1]; int a[3] = {0, 1, 2}; aa->mem2; return sizeof(struct A);; }");
  assert(32, struct_test3());
  printf("int struct_test4() { struct C cc; cc.mem2.mem1 = 2; cc.mem2.mem2 = 1; cc.mem3 = 10;; return cc.mem2.mem1+cc.mem2.mem2; }");
  assert(3, struct_test4());
  printf("int struct_test5() { struct C cc, *pcc; pcc = &cc;(*pcc).mem2.mem1 = 10; pcc->mem2.mem2 = 10; cc.mem3 = 40;; return cc.mem2.mem1+(*pcc).mem2.mem2; }");
  assert(20, struct_test5());
  printf("int struct_test6() { struct C cc, *pcc; pcc = &cc;pcc->mem2.mem2 = 5; (*pcc).mem2.mem1 = -4; (*pcc).mem4 = &(*pcc).mem2; cc.mem3 = -3;; return cc.mem2.mem1+pcc->mem4->mem2; }");
  assert(1, struct_test6());

  printf("char integer_promotion1() { char a = 30; return (a * a) / 25; }");
  assert(36, integer_promotion1());
  printf("char integer_promotion2() { char a = 30; char b = a * a; return b / 25; }");
  assert(-4, integer_promotion2());

  printf("int *pointer_to_int() { return &a; }");
  int *pointer_to_int_a = pointer_to_int();
  assert(1, *pointer_to_int_a);
  printf("char char_pointer_to[20] = \"akasatana\"; int *pointer_to_char() { return char_pointer_to+10; }");
  char *pointer_to_char_a = pointer_to_char();
  assert(116, *pointer_to_char_a);
}