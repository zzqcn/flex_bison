#pragma once

#include <string>

enum {
  CMP_EQ,
  CMP_NE,
  CMP_LT,
  CMP_GT,
  CMP_LE,
  CMP_GE,
  CMP_OR,
  CMP_AND,
};

void yyerror(const char* s);

struct ast {
  int type;
};

struct ast_lr : public ast {
  ast *left;
  ast *right;
};

struct ast_cmp : public ast {
  int key;
  int val;
  std::string str;
};

ast* ast_new_lr(int type, ast* left, ast* right);
ast* ast_new_cmp(int type, int key, int val);
ast* ast_new_cmp(int type, int key, const char* str);
void ast_free(ast* p);
void ast_show(ast* p);
