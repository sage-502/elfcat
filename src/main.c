#include <stdio.h>
#include "elf_parser.h"

int main(int argc, char **argv)
{
    elf_t *elf;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    elf = parse_elf(argv[1]);
    if (!elf)
    {
        fprintf(stderr, "parse_elf failed\n");
        return 1;
    }

    /* ===== 파일 읽기 검증 ===== */
    printf("=== BASIC INFO ===\n");
    printf("Magic: %.4s\n", elf->data);
    printf("Size : %zu bytes\n", elf->size);

    /* ===== ELF Header 검증 ===== */
    printf("\n=== ELF HEADER ===\n");
    printf("Entry : 0x%lx\n", elf->ehdr.e_entry);
    printf("PHOFF : 0x%lx\n", elf->ehdr.e_phoff);
    printf("SHOFF : 0x%lx\n", elf->ehdr.e_shoff);
    printf("PHNUM : %d\n", elf->ehdr.e_phnum);
    printf("SHNUM : %d\n", elf->ehdr.e_shnum);
    
    printf("\n=== PROGRAM HEADER ===\n");
    for (int i = 0; i < elf->phnum; i++)
    {
        printf("PHDR[%d] type: %x\n", i, elf->phdrs[i].p_type);
    }

    printf("\n=== SECTION HEADER ===\n");
    for (int i = 0; i < elf->shnum; i++)
    {
        printf("[%d] name: %s\n", i, elf->shstrtab + elf->shdrs[i].sh_name);
    }
    
    free_elf(elf);
    
    return 0;
}
