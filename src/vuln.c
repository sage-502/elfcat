#include <stdio.h>
#include <string.h>
#include <elf.h>
#include "vuln.h"

static char *dangerous_funcs[] = {
    "gets",
    "strcpy",
    "strcat",
    "sprintf",
    "scanf",
    NULL
};

// 함수 찾기: 주소 → 함수 이름
static const char *find_function_by_addr(elf_t *elf, uint64_t addr)
{
    for (int i = 0; i < elf->shnum; i++) {
        Elf64_Shdr sh = elf->shdrs[i];

        if (sh.sh_type != SHT_SYMTAB && sh.sh_type != SHT_DYNSYM)
            continue;

        Elf64_Sym *symtab = (Elf64_Sym *)(elf->data + sh.sh_offset);
        int count = sh.sh_size / sizeof(Elf64_Sym);

        Elf64_Shdr str_shdr = elf->shdrs[sh.sh_link];
        char *strtab = (char *)(elf->data + str_shdr.sh_offset);

        for (int j = 0; j < count; j++) {
            Elf64_Sym sym = symtab[j];

            if (ELF64_ST_TYPE(sym.st_info) != STT_FUNC)
                continue;

            if (addr >= sym.st_value && addr < sym.st_value + sym.st_size) {
                return strtab + sym.st_name;
            }
        }
    }
    return "unknown";
}

void analyze_vuln(elf_t *elf)
{
    printf("[*] Finding vulnerable function calls...\n");

    for (int i = 0; i < elf->shnum; i++) {
        Elf64_Shdr sh = elf->shdrs[i];

        // relocation section만 처리
        if (sh.sh_type != SHT_RELA && sh.sh_type != SHT_REL)
            continue;

        // 연결된 symbol table
        Elf64_Shdr sym_sh = elf->shdrs[sh.sh_link];
        Elf64_Sym *symtab = (Elf64_Sym *)(elf->data + sym_sh.sh_offset);

        Elf64_Shdr str_sh = elf->shdrs[sym_sh.sh_link];
        char *strtab = (char *)(elf->data + str_sh.sh_offset);

        int count = sh.sh_size / sh.sh_entsize;

        for (int j = 0; j < count; j++) {
            Elf64_Rela *rela = (Elf64_Rela *)(elf->data + sh.sh_offset + j * sizeof(Elf64_Rela));

            int sym_idx = ELF64_R_SYM(rela->r_info);
            char *func_name = strtab + symtab[sym_idx].st_name;

            // 위험 함수인지 검사
            for (int k = 0; dangerous_funcs[k]; k++) {
                if (strcmp(func_name, dangerous_funcs[k]) == 0) {

                    // 호출한 함수 찾기
                    const char *caller = find_function_by_addr(elf, rela->r_offset);

                    printf("[!] %s called in function: %s\n", func_name, caller);
                }
            }
        }
    }
}
