# 전체 구조

```c
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

parse_elf()         → elf_t
analyze_mitigation() → mitigation_t
analyze_vuln()       → vuln_t

**mitigation 분석 파트**

```
입력:  elf_t
출력: mitigation_t
```

## elf_t

```c
typedef struct s_elf
{
    Elf64_Ehdr ehdr;

    Elf64_Phdr *phdrs;
    int phnum;

    Elf64_Shdr *shdrs;
    int shnum;

    char *shstrtab;

    unsigned char *data;
    size_t size;

} elf_t;
```

## 전체 흐름(main 함수)

```c
int main(int argc, char **argv)
{
    elf_t *elf = parse_elf(argv[1]);

    mitigation_t m = analyze_mitigation(elf);

    vuln_t v = analyze_vulnerability(elf);

    print_result(m, v);
    
    elf_free(elf);

    return 0;
}
```

## mitigation_t

```c
typedef struct s_mitigation
{
    int nx;
    int pie;
    int relro;   // 0: none, 1: partial, 2: full
    int canary;

} mitigation_t;
```

## 함수 시그니처

```c
mitigation_t analyze_mitigation(elf_t *elf)
{
    mitigation_t m;

    m.nx = check_nx(elf);
    m.pie = check_pie(elf);
    m.relro = check_relro(elf);
    m.canary = check_canary(elf);

    return m;
}
```

##  출력 함수
`print.h`:  void print_result(mitigation_t m, vuln_t v) 프로토타입</br>

```c
void print_vuln(vuln_t v);
void print_result(mitigation_t m);
```

### mitigation 출력
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
## **해야할 것**

```
NX / PIE / RELRO / Canary 판별
```
# Mitigation Analysis (mitigation.c)
mitigation.c는 ELF 바이너리에 적용된 대표적인 보호 기법(mitigation)을 정적으로 분석하는 모듈이다.
이 모듈은 ELF 파일의 Program Header, Section Header, Dynamic Section, Symbol Table 등을 분석하여 다음 보호 기법의 활성화 여부를 판별한다.
```
NX (Non-Executable Stack)
PIE (Position Independent Executable)
RELRO (Relocation Read-Only)
Stack Canary
```
분석은 실행(runtime)이 아니라 ELF 구조 자체를 해석하는 방식으로 이루어진다.
즉, 바이너리를 실행하지 않고도 보안 설정 상태를 확인할 수 있다.
```
Overall Architecture
ELF File
   ↓
parse_elf()
   ↓
elf_t
   ├── ELF Header
   ├── Program Header
   ├── Section Header
   ├── Raw Data
   ↓
analyze_mitigation()
   ↓
mitigation_t
analyze_mitigation()
```
역할

모든 mitigation 검사 함수를 호출하여 최종 결과 구조체(mitigation_t)를 반환한다.

mitigation_t analyze_mitigation(elf_t *elf)
동작 과정
```
check_nx()
check_pie()
check_relro()
check_canary()
```
각 함수는 독립적으로 ELF 구조를 분석하며, 결과는 mitigation_t에 저장된다.

## 1. NX (Non-Executable Stack)
목적

NX는 스택 영역에서 코드 실행을 차단하는 보호 기법이다.
현재 `check_nx()`는:

1.Program Header 존재 여부 확인</br>
2.모든 Program Header 순회</br>
3.PT_GNU_STACK 세그먼트 탐색</br>
4.p_flags에서 PF_X(Execute 권한) 검사</br>
5.스택 실행 가능 여부를 기반으로 NX Enabled/Disabled 판별</br>

을 수행한다.

분석 방식

NX 여부는 Program Header의 PT_GNU_STACK 세그먼트를 통해 확인한다.

`if (elf->phdrs[i].p_type == PT_GNU_STACK)`
핵심 필드
`p_flags`

이 값은 세그먼트 권한을 의미한다.

판별 로직
```
if (elf->phdrs[i].p_flags & PF_X)
    return 0;
else
    return 1;
```
PF_X 존재</br>
실행 가능 스택- NX Disabled
PF_X 없음</br>
실행 불가능 스택
NX Enabled

## 2. PIE (Position Independent Executable)
목적

PIE는 바이너리를 메모리의 임의 주소에 로딩할 수 있도록 만드는 보호 기법이다.
현재 `check_pie()`는:

1.ELF Header 존재 여부 확인</br>
2.ELF Header의 e_type 값 확인</br>
3.ELF 타입이 ET_DYN인지 검사</br>
4.PIE 여부 판별</br>

을 수행한다.

분석 방식

PIE 여부는 ELF Header의 e_type 값을 통해 판별한다.

`elf->ehdr.e_type`
핵심 값
값	의미
```
ET_EXEC	일반 실행 파일
ET_DYN	공유 라이브러리
```
PIE판별 로직
```
if (elf->ehdr.e_type == ET_DYN)
    return 1;
```
PIE 바이너리는 실제로 공유 라이브러리처럼 동작하기 때문에 ELF 타입이 ET_DYN으로 설정된다.

## 3. RELRO (Relocation Read-Only)
목적

RELRO는 GOT(Global Offset Table) overwrite 공격을 방지하기 위한 보호 기법이다.

`check_relro()`는:

1.Program Header에서 PT_GNU_RELRO 탐색</br>
2.Dynamic Section(PT_DYNAMIC) 위치 확인</br>
3.Dynamic Entry 순회</br>
4.BIND_NOW 관련 플래그 탐색</br>
5.Full / Partial / None 판별</br>

RELRO 종류
종류
```
No RELRO -	GOT writable
Partial - RELRO	일부 보호
Full RELRO -	GOT 전체 read-only
```
분석 방식

RELRO는 두 단계를 모두 만족해야 한다.

1단계: PT_GNU_RELRO 존재 확인
```
if (elf->phdrs[i].p_type == PT_GNU_RELRO)
    has_relro = 1;
```
이 세그먼트는 read-only로 전환될 메모리 영역을 의미한다.

2단계: BIND_NOW 확인

Full RELRO는 lazy binding을 제거해야 한다.

이를 위해 Dynamic Section에서 다음 값을 검사한다.

검사 대상
1. DT_BIND_NOW</br>
`if (dyn[i].d_tag == DT_BIND_NOW)`
2. DT_FLAGS</br>
```if (dyn[i].d_tag == DT_FLAGS)
{
    if (dyn[i].d_un.d_val & DF_BIND_NOW)
```
3. DT_FLAGS_1</br>
```
if (dyn[i].d_tag == DT_FLAGS_1)
{
    if (dyn[i].d_un.d_val & DF_1_NOW) 
```


최종 판별
PT_GNU_RELRO 없음:	No RELRO</br>
PT_GNU_RELRO만 존재:	Partial RELRO</br>
PT_GNU_RELRO + BIND_NOW:	Full RELRO</br>

## 4. Stack Canary
목적

Stack Canary는 스택 버퍼 오버플로우를 탐지하기 위한 보호 기법이다.

현재 `check_canary()`는:

1.Section Header와 raw data 존재 여부 확인</br>
2.모든 Section Header 순회</br>
3.Symbol Table(SHT_SYMTAB, SHT_DYNSYM) 탐색</br>
4.연결된 String Table 위치 확인</br>
5.Symbol Entry들을 순회하며 심볼 이름 추출</br>
6.__stack_chk_fail 심볼 존재 여부 검사</br>
7.Canary 활성화 여부 판별</br>

함수 리턴 직전에 Canary 값을 검사하며, 값이 변경되면 프로그램을 강제 종료한다.

함수 종료 시:

__stack_chk_fail()

이 호출되며 프로그램이 종료된다.

분석 방식

Canary는 Symbol Table에서 다음 심볼 존재 여부로 판별한다.

__stack_chk_fail
검사 대상 섹션
```
SHT_DYNSYM
SHT_SYMTAB
```
문자열 테이블 접근

심볼 이름은 직접 저장되지 않고 String Table에 offset 형태로 저장된다.

char *name = strtab + sym[j].st_name;
판별 로직
if (strcmp(name, "__stack_chk_fail") == 0)
    return 1;
Boundary Check

이 코드는 ELF 파싱 과정에서 잘못된 offset 접근으로 인한 segmentation fault를 방지하기 위해 여러 안전 검사를 수행한다.

예시:
```
if (sh.sh_offset + sh.sh_size > elf->size)
    continue;
```
주요 안전 검사
```
NULL 체크	invalid pointer 방지
offset 범위 검사	out-of-bounds 접근 방지
sh_link 검사	잘못된 section index 방지
entsize 검사	division by zero 방지
```

checksec 유사 동작하므로 결과 확인으로 검증 가능

대표적인 ELF 보호기법 분석 도구인 checksec과 유사한 방식으로 동작한다.
