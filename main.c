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
  Function *prog = parse();

  codegen(prog);
  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  return 0;
}
