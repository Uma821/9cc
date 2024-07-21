#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トークナイズしてパースする
  // 結果はcodeに保存される
  user_input = argv[1];
  tokenize();
  Program *prog = parse();

  codegen(prog);
  return 0;
}
