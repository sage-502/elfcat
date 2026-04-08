#include <stdio.h>
#include "elf_parser.h"
#include "vuln.h"

int main(int argc, char **argv)
{
    elf_t elf;
    vuln_t v;

    if (argc != 2)
    {
        printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    if (parse_elf(argv[1], &elf) != 0)
    {
        printf("parse_elf failed\n");
        return 1;
    }

    v = analyze_vulnerability(&elf);
    print_vuln(v);

    free_elf(&elf);
    return 0;
}
