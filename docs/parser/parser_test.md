# Parser 테스트 결과

## 1. sample ELF 컴파일

`/elfcat/sample/hello.c`
``` c
#include<stdio.h>

int main(){
    printf("Hello, world!");
    return 0;
}
```
64bit로 컴파일

---

## 2. 테스트용 `main`과 `elf_parser.c` 컴파일

```
/elfcat/src$ gcc -I../include -o test elf_parser.c main.c
```

임시로 헤더파일 경로 작성함.

---

## 3. 샘플로 테스트

```
/elfcat/src$ ./test ../sample/hello
=== BASIC INFO ===
Magic: ELF
Size : 15864 bytes

=== ELF HEADER ===
Entry : 0x401050
PHOFF : 0x40
SHOFF : 0x3638
PHNUM : 13
SHNUM : 31

=== PROGRAM HEADER ===
PHDR[0] type: 6
PHDR[1] type: 3
PHDR[2] type: 1
PHDR[3] type: 1
PHDR[4] type: 1
PHDR[5] type: 1
PHDR[6] type: 2
PHDR[7] type: 4
PHDR[8] type: 4
PHDR[9] type: 6474e553
PHDR[10] type: 6474e550
PHDR[11] type: 6474e551
PHDR[12] type: 6474e552

=== SECTION HEADER ===
[0] name: 
[1] name: .interp
[2] name: .note.gnu.property
[3] name: .note.gnu.build-id
[4] name: .note.ABI-tag
[5] name: .gnu.hash
[6] name: .dynsym
[7] name: .dynstr
[8] name: .gnu.version
[9] name: .gnu.version_r
[10] name: .rela.dyn
[11] name: .rela.plt
[12] name: .init
[13] name: .plt
[14] name: .plt.sec
[15] name: .text
[16] name: .fini
[17] name: .rodata
[18] name: .eh_frame_hdr
[19] name: .eh_frame
[20] name: .init_array
[21] name: .fini_array
[22] name: .dynamic
[23] name: .got
[24] name: .got.plt
[25] name: .data
[26] name: .bss
[27] name: .comment
[28] name: .symtab
[29] name: .strtab
[30] name: .shstrtab
```
