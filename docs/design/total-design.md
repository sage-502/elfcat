# 전체 툴 입력 및 반환 설계

이 프로젝트에서는 다음과 같은 필수 기능이 존재한다.

1. ELF 파서
2. Mitigation 분석
3. 취약점 탐지
4. CLI 출력

이 문서에서는 각 기능들이 유기적으로 동작하기 위한 각 함수의 출력 형태와 전체 흐름을 정의한다.

---

## 1. 전체 구조 요약

```
parse_elf()         → elf_t*
analyze_mitigation() → mitigation_t
analyze_vuln()       → vuln_t
```

흐름:

```
[CLI]
  ↓ (파일 입력)
[ELF 파서]
  ↓ elf_t
[Mitigation 분석]
  ↓ mitigation_t
[취약점 탐지]
  ↓ vuln_t
[CLI 출력]
```

---

## 2. 각 기능 별 반환 형태

### 1) ELF 파서 결과 구조체 (`elf_t`)

```c
typedef struct s_elf
{
    /* 기본 헤더 */
    Elf64_Ehdr ehdr;

    /* Program Header */
    Elf64_Phdr *phdrs;
    int phnum;

    /* Section Header */
    Elf64_Shdr *shdrs;
    int shnum;

    /* Section name string table */
    char *shstrtab;

    /* 파일 raw 데이터 */
    unsigned char *data;
    size_t size;

} elf_t;
```

### 2) mitigation 결과 구조체 (`mitigation_t`)

CLI에서 바로 출력 가능한 형태로

```c
typedef struct s_mitigation
{
    int nx;         // 0: disabled, 1: enabled
    int pie;        // 0: no, 1: yes
    int relro;      // 0: none, 1: partial, 2: full
    int canary;     // 0: disabled, 1: enabled

} mitigation_t;
```

#### (옵션) enum 버전

```c
typedef enum e_relro
{
    RELRO_NONE,
    RELRO_PARTIAL,
    RELRO_FULL
} relro_t;
```

### 3) 취약점 결과 구조체 (`vuln_t`)

확장 가능하게 설계

```c
#define MAX_VULN 32

typedef struct s_vuln
{
    int has_gets;
    int has_strcpy;
    int has_rwx_segment;

    /* 확장용 */
    int count;
    char *messages[MAX_VULN];

} vuln_t;
```

message 용도:

```
[!] gets() found
[!] RWX segment detected
```

### 4) 최종 출력용 (CLI용)
`print.h`:  void print_result(mitigation_t m, vuln_t v) 프로토타입</br>

```c
void print_vuln(vuln_t v);
void print_result(mitigation_t m);
```

#### mitigation 출력
`main.c`:  호출</br>
`print.c`: 함수 정의</br>
```c
#include <stdio.h>

void print_mitigation(mitigation_t m)
{
    printf("NX: %s\n", m.nx ? "Enabled" : "Disabled");
    printf("PIE: %s\n", m.pie ? "Enabled" : "Disabled");

    if (m.relro == 0)
        printf("RELRO: No RELRO\n");
    else if (m.relro == 1)
        printf("RELRO: Partial RELRO\n");
    else
        printf("RELRO: Full RELRO\n");

    printf("Canary: %s\n", m.canary ? "Enabled" : "Disabled");
}
```

#### 지정 함수 출력

```c
void print_vuln(vuln_t v)
{
    int i;

    printf("===== Vulnerability Scan Result =====\n");
    printf("has_gets        : %d\n", v.has_gets);
    printf("has_strcpy      : %d\n", v.has_strcpy);
    printf("has_rwx_segment : %d\n", v.has_rwx_segment);
    printf("message count   : %d\n", v.count);

    for (i = 0; i < v.count; i++)
    {
        if (v.messages[i])
            printf("[%d] %s\n", i + 1, v.messages[i]);
    }
}
```
### 5) 메모리 동적 할당 해제 함수

해제 대상:

* `elf->phdrs`
* `elf->shdrs`
* `elf->data`

---

## 3.  함수 시그니처 (확정판)

```c
elf_t *parse_elf(const char *filename);

mitigation_t analyze_mitigation(elf_t *elf);

vuln_t analyze_vulnerability(elf_t *elf);

void print_result(mitigation_t m, vuln_t v);

void free_elf(elf_t *elf);
```

---

## 4. 코드 기준 전체 흐름

```c
int main(int argc, char **argv)
{
    elf_t *elf = parse_elf(argv[1]);

    mitigation_t m = analyze_mitigation(elf);

    vuln_t v = analyze_vulnerability(elf);

    print_result(m, v);

    free_elf(elf);

    return 0;
}
```
