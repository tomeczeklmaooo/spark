#ifndef OPERATIONS_H
#define OPERATIONS_H

int execute_alias(const char *name);
int create_alias(const char *name, const char *command);
int remove_alias(const char *name);
int list_aliases();

#endif /* OPERATIONS_H */
