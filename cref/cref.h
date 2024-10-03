#pragma once

extern int defining;
extern const char *cur_filename;

void add_ref(int lineno, const char *filename, char *word, int flags);
int new_file(const char *fn);
int pop_file(void);
