#include <string>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>
#include "cref.h"
#include "cref_lex.h"

using namespace std;

#define ENABLE_TRACE 0

#if ENABLE_TRACE
#  define TRACE(fmt, ...)                  \
    do {                                   \
      fprintf(stdout, fmt, ##__VA_ARGS__); \
    } while (0)
#  define ERROR(fmt, ...)                  \
    do {                                   \
      fprintf(stderr, fmt, ##__VA_ARGS__); \
    } while (0)
#else
#  define TRACE(fmt, ...) \
    do {                  \
    } while (0)
#  define ERROR(fmt, ...)                  \
    do {                                   \
      fprintf(stderr, fmt, ##__VA_ARGS__); \
    } while (0)
#endif

struct ref {
  std::string file_name;
  int flags;
  int lineno;
};

struct symbol {
  std::string name;
  std::vector<ref> refs;
};

struct cfile {
  std::string file_name;
  FILE *fp;
  YY_BUFFER_STATE bs;
  int lineno;
};

vector<symbol> g_symbols;
stack<cfile> g_stack;

int defining;
const char *cur_filename;

struct symbol_finder {
  symbol_finder(const char *s) : name(s) {}
  bool operator()(const symbol &sym) { return (0 == sym.name.compare(name)); }
  const char *name;
};

struct ref_finder {
  ref_finder(const string &fn, int ln) : file_name(fn), lineno(ln) {}
  bool operator()(const ref &r) { return (r.file_name == file_name && r.lineno == lineno); }
  string file_name;
  int lineno;
};

static symbol &find_symbol(char *word) {
  auto si = find_if(g_symbols.begin(), g_symbols.end(), symbol_finder(word));
  if (si == g_symbols.end()) {
    symbol sym;
    sym.name = word;
    g_symbols.push_back(sym);
    TRACE("add symbol %s\n", sym.name.c_str());
    return g_symbols.back();
  }
  { return *si; }
}

void add_ref(int lineno, const char *filename, char *word, int flags) {
  symbol &sym = find_symbol(word);

  auto ri = find_if(sym.refs.cbegin(), sym.refs.cend(), ref_finder(filename, lineno));
  if (ri == sym.refs.cend()) {
    ref r;
    r.file_name = filename;
    r.lineno = lineno;
    r.flags = flags;
    TRACE("  add %s's ref %s %s %d\n", sym.name.c_str(), r.file_name.c_str(), flags ? "*" : " ",
           r.lineno);
    sym.refs.push_back(r);
  }
}

static void print_refs() {
  for (auto iter = g_symbols.cbegin(); iter != g_symbols.cend(); ++iter) {
    printf("%s (%zu)\n", iter->name.c_str(), iter->refs.size());
    for (auto jter = iter->refs.cbegin(); jter != iter->refs.cend(); ++jter) {
      printf("   %s:%d", jter->file_name.c_str(), jter->lineno);
      if (jter->flags)
        printf("*");
      printf("\n");
    }
  }
}

int new_file(const char *fn) {
  FILE *fp;

  fp = fopen(fn, "r");
  if (!fp)
    return 1;

  if (!g_stack.empty())
    g_stack.top().lineno = yylineno;

  cfile cf;
  cf.file_name = fn;
  cf.fp = fp;
  cf.bs = yy_create_buffer(fp, YY_BUF_SIZE);
  yy_switch_to_buffer(cf.bs);
  g_stack.push(cf);

  yylineno = 1;
  cur_filename = fn;
  return 1;
}

int pop_file(void) {
  if (g_stack.empty())
    return 0;

  cfile &cf = g_stack.top();
  fclose(cf.fp);
  yy_delete_buffer(cf.bs);
  g_stack.pop();

  if (g_stack.empty())
    return 0;
  cf = g_stack.top();
  yy_switch_to_buffer(cf.bs);
  yylineno = cf.lineno;
  cur_filename = cf.file_name.c_str();
  return 1;
}

int main(int argc, char **argv) {
  int i;

  if (argc < 2) {
    fprintf(stderr, "need filename\n");
    return 1;
  }

  for (i = 1; i < argc; i++) {
    if (new_file(argv[i]))
      yylex();
  }

  print_refs();

  return 0;
}
