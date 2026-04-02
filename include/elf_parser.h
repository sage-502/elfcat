#ifndef ELF_PARSER_H
# define ELF_PARSER_H

# include <elf.h>
# include <stddef.h>

/*
** ELF 파일 전체 상태를 담는 컨테이너
*/
typedef struct s_elf
{
    /* ===== ELF Header ===== */
    Elf64_Ehdr      ehdr;

    /* ===== Program Header ===== */
    Elf64_Phdr      *phdrs;
    int             phnum;

    /* ===== Section Header ===== */
    Elf64_Shdr      *shdrs;
    int             shnum;

    /* ===== Section Header String Table ===== */
    char            *shstrtab;

    /* ===== Raw file data ===== */
    unsigned char   *data;
    size_t          size;

}   elf_t;


/* ========================= */
/*        PARSER API         */
/* ========================= */

/*
** ELF 파일을 파싱하여 elf_t 구조체로 반환
** 실패 시 NULL 반환
*/
elf_t  *parse_elf(const char *filename);

/*
** elf_t 내부 동적 메모리 해제
*/
void    free_elf(elf_t *elf);


/* ========================= */
/*      UTILITY (옵션)       */
/* ========================= */

/*
** ELF 파일인지 검증 (Magic Number 체크)
*/
int     is_elf(elf_t *elf);

#endif
