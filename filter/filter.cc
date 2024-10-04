#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include "filter.h"
#include "filter_lex.h"
#include "filter_syn.h"

using namespace std;

extern int yydebug;

struct staff {
  unsigned id;
  std::string name;
  unsigned age;
  std::string lang;
};

vector<staff> g_staffs;

void yyerror(const char *s) {
  fprintf(stderr, "error: %s\n", s);
}

ast *ast_new_lr(int type, ast *left, ast *right) {
  ast_lr *p = new ast_lr;
  p->type = type;
  p->left = left;
  p->right = right;
  return p;
}

ast *ast_new_cmp(int type, int key, int val) {
  ast_cmp *p = new ast_cmp;
  p->type = type;
  p->key = key;
  p->val = val;
  return p;
}

ast *ast_new_cmp(int type, int key, const char *str) {
  ast_cmp *p = new ast_cmp;
  p->type = type;
  p->key = key;
  p->val = 0;
  p->str = str;
  return p;
}

void ast_free(ast *p) {
  if (NULL == p)
    return;

  switch (p->type) {
  case CMP_AND:
  case CMP_OR: {
    ast_lr *cmp = (ast_lr *)p;
    ast_free(cmp->right);
    ast_free(cmp->left);
  } break;

  default:
    break;
  }

  delete p;
}

static const char *ast_key2str(int key) {
  switch (key) {
  case ID:
    return "ID";
  case NAME:
    return "NAME";
  case AGE:
    return "AGE";
  case LANG:
    return "LANG";
  default:
    return "/";
  }
}

static const char *ast_type2str(int type) {
  switch (type) {
  case CMP_EQ:
    return "==";
  case CMP_NE:
    return "!=";
  case CMP_LT:
    return "<";
  case CMP_GT:
    return ">";
  case CMP_LE:
    return "<=";
  case CMP_GE:
    return ">=";
  case CMP_OR:
    return "||";
  case CMP_AND:
    return "&&";
  default:
    return "/";
  }
}

static bool ast_eval_val(unsigned a, int op, unsigned b) {
  switch (op) {
  case CMP_EQ:
    return (a == b);
  case CMP_NE:
    return (a != b);
  case CMP_LT:
    return (a < b);
  case CMP_GT:
    return (a > b);
  case CMP_LE:
    return (a <= b);
  case CMP_GE:
    return (a >= b);
  default:
    return false;
  }
}

static bool ast_eval_string(const string &s1, int op, const string &s2) {
  switch (op) {
  case CMP_EQ:
    return (s1.compare(s2) == 0);
  case CMP_NE:
    return (s1.compare(s2) != 0);
  case CMP_LT:
    return (s1.compare(s2) < 0);
  case CMP_GT:
    return (s1.compare(s2) > 0);
  default:
    return false;
  }
}

static bool ast_eval_cmp(ast_cmp *p, const staff &stf) {
  switch (p->key) {
  case ID:
    return ast_eval_val(stf.id, p->type, p->val);
  case AGE:
    return ast_eval_val(stf.age, p->type, p->val);
  case NAME:
    return ast_eval_string(stf.name, p->type, p->str);
  case LANG:
    return ast_eval_string(stf.lang, p->type, p->str);
  default:
    return false;
  }
}

bool ast_eval(ast *tree, const staff &stf) {
  ast_cmp *cmp;
  ast_lr *lr;
  bool bl, br;

  if (nullptr == tree)
    return false;

  switch (tree->type) {
  case CMP_EQ:
  case CMP_NE:
  case CMP_LT:
  case CMP_GT:
  case CMP_LE:
  case CMP_GE:
    cmp = (ast_cmp *)tree;
    // printf("%s %s ", ast_key2str(cmp->key), ast_type2str(cmp->type));
    // if (cmp->val != 0)
    //   printf("%d\n", cmp->val);
    // else
    //   printf("%s\n", cmp->str.c_str());
    // break;
    return ast_eval_cmp(cmp, stf);

  case CMP_OR:
    // printf("LOGIC(%s)\n", ast_type2str(tree->type));
    lr = (ast_lr *)tree;
    bl = ast_eval(lr->left, stf);
    br = ast_eval(lr->right, stf);
    return (bl || br);
  case CMP_AND:
    // printf("LOGIC(%s)\n", ast_type2str(tree->type));
    lr = (ast_lr *)tree;
    bl = ast_eval(lr->left, stf);
    br = ast_eval(lr->right, stf);
    return (bl && br);

  default:
    break;
  }

  return false;
}

void ast_show(ast *p) {
  printf("-----------------------\n");
  if (nullptr == p) {
    for (auto stf : g_staffs) {
      printf("%3u %-10s %2u %-8s\n", stf.id, stf.name.c_str(), stf.age, stf.lang.c_str());
    }
  } else {
    for (auto stf : g_staffs) {
      if (ast_eval(p, stf)) {
        printf("%3u %-10s %2u %-8s\n", stf.id, stf.name.c_str(), stf.age, stf.lang.c_str());
      }
    }
  }
  printf("-----------------------\n");
}

/* split string into tokens */
static int strsplit_line(char *str, unsigned str_len, char **tokens, unsigned max_tokens,
                         char delim) {
  int tok = 0;
  unsigned i, tok_start = 1; /* first token is right at start of string */

  if (str == NULL || tokens == NULL)
    goto error;

  for (i = 0; i < str_len; i++) {
    if (str[i] == '\0' || str[i] == '\n')
      break;
    if (tok >= max_tokens)
      goto error;

    if (tok_start) {
      tok_start = 0;
      tokens[tok++] = &str[i];
    }
    if (str[i] == delim) {
      str[i] = '\0';
      tok_start = 1;
    }
  }

  return tok;

error:
  return -1;
}

int read_data(const char *path) {
#define BUF_LEN 64
  FILE *fp;
  char buf[BUF_LEN];
  char *tokens[4];
  size_t len;
  staff stf;

  fp = fopen(path, "r");
  if (NULL == fp) {
    fprintf(stderr, "open file %s failed\n", path);
    return -1;
  }

  fgets(buf, BUF_LEN, fp);
  while (fgets(buf, BUF_LEN, fp)) {
    len = strlen(buf);
    if (len < 2)
      continue;
    buf[len - 1] = 0;
    int n = strsplit_line(buf, len, tokens, 5, ',');
    if (n < 4)
      continue;
    stf.id = stoul(tokens[0]);
    stf.name = tokens[1];
    stf.age = stoul(tokens[2]);
    stf.lang = tokens[3];
    // printf("%u,%s,%u,%s", stf.id, stf.name.c_str(), stf.age, stf.lang.c_str());
    g_staffs.push_back(stf);
  }
  printf("read %zu staffs\n", g_staffs.size());

  fclose(fp);
  return 0;
#undef BUF_LEN
}

int main(int argc, char **argv) {
  int ret;
  char buf[1024], *s;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    return 1;
  }

  ret = read_data(argv[1]);
  if (ret != 0) {
    fprintf(stderr, "read file %s failed\n", argv[1]);
    return 1;
  }

  printf("FLT>");
  while (s = fgets(buf, 1024, stdin)) {
    YY_BUFFER_STATE bs;
    if (buf[0] == '\n')
      goto loop;

    bs = yy_scan_string(buf);
    // yydebug = 1;
    yyparse();
    yy_delete_buffer(bs);

  loop:
    printf("FLT>");
  }

  return 0;
}
