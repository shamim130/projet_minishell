#ifndef ALIAS_H
#define ALIAS_H

const char *alias_get(const char *name);
int alias_set(const char *name, const char *value);
void alias_print_all(void);

#endif
