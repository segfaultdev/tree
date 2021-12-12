#ifndef __CFG_CONFIG_H__
#define __CFG_CONFIG_H__

#include <stdint.h>
#include <stdio.h>

int cfg_strcmp(const char *str_1, const char *str_2);
int cfg_atoi(const char *str);

ssize_t cfg_find_raw(FILE *file, const char *name);

int cfg_find_str(FILE *file, const char *name, char *ptr);
int cfg_find_int(FILE *file, const char *name, int *ptr);

int cfg_edit_str(FILE *file, const char *name, const char *ptr);
int cfg_edit_int(FILE *file, const char *name, int value);

#endif
