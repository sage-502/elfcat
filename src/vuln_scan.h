#ifndef VULN_SCAN_H
#define VULN_SCAN_H

#include <elf.h>
#include "elf_parser.h"

typedef struct s_danger_func
{
    const char *name;
    const char *category;
    const char *severity;
    const char *description;
} danger_func_t;

Elf64_Shdr *find_section_by_name(elf_t *elf, const char *name);
const danger_func_t *find_danger_func(const char *name);
void scan_symbol_table(elf_t *elf, const char *symsec_name, const char *strsec_name);
void scan_dangerous_functions(elf_t *elf);

#endif
