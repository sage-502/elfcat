# ELF 구조(x86 64bit 기준)

ELF는 Linux와 Unix에서 사용한는 실행 파일의 포맷이다.

별도의 확장자가 존재하지 않으며, 
* 실행 파일(Executable)
* 라이브러리(.so)
* 오브젝트 파일(.o)
을 모두 포함한다.

이 문서에서는 ELF의 구조를 알아본다.

---

## 1. 개요

ELF 파일의 기본 구조는 다음과 같다.

```
+---------------------------+
| ELF Header                |
+---------------------------+
| Program Header Table      |
+---------------------------+
| Section Header Table      |
+---------------------------+
| Section Data (.text 등)   |
+---------------------------+
```

ELF는 단순한 바이너리 파일이 아니라,
**여러 개의 구조체와 데이터 영역이 조합된 구조화된 포맷**이다.

각 영역은 서로 다른 목적을 가지며,
특히 **실행 관점과 분석 관점이 분리되어 있다는 점이 특징이다.**

> **노트 ─ Segment vs Section**
> 
> | 구분 | Segment (Program Header) | Section (Section Header) |
> | -- | ------------------------ | ------------------------ |
> | 기준 | 실행                       | 분석                       |
> | 사용 | loader                   | 분석 도구                    |
> | 구조 | 메모리 배치                   | 파일 구성                    |

이제부터는 `elf.h`에 구현되어 있는 실제 구조체들을 보며 각 영역에 정의되어 있는 필드를 알아본다.

---

## 2. 각 영역 별 주요 필드

### 2.1 ELF Header

#### 구조체
``` c
typedef struct
{
	unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
	Elf64_Half	e_type;			/* Object file type */
	Elf64_Half	e_machine;		/* Architecture */
	Elf64_Word	e_version;		/* Object file version */
	Elf64_Addr	e_entry;		/* Entry point virtual address */
	Elf64_Off	e_phoff;		/* Program header table file offset */
	Elf64_Off	e_shoff;		/* Section header table file offset */
	Elf64_Word	e_flags;		/* Processor-specific flags */
	Elf64_Half	e_ehsize;		/* ELF header size in bytes */
	Elf64_Half	e_phentsize;		/* Program header table entry size */
	Elf64_Half	e_phnum;		/* Program header table entry count */
	Elf64_Half	e_shentsize;		/* Section header table entry size */
	Elf64_Half	e_shnum;		/* Section header table entry count */
	Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;
```

#### 주요 필드

| 필드 | 의미 | 설명 |
| ---- | ---- | ---- |
| e_ident | ELF인지 판별 + 기본 정보 | 매직넘버(0~3), 32bit/64bit(4), 엔디안(5), ELF 버전(6) : 총 16bytes |
| e_entry | 프로그램 시작 주소 | |
| e_phoff | Program Header 시작 위치 (offset) | ProgramHeader = base + e_phoff |
| e_shoff | Section Header 시작 위치 (offset) | SectionHeader = base + e_shoff |
| e_phnum | Program Header 개수 | 메모리에 로드할 단위(세그먼트) 개수 |
| e_shnum | Section Header 개수 | 파일 내부 구성 요소(섹션) 개수 |
| e_shstrndx | 섹션 이름 페이블 위치 | Section Header 내부 배열(`shstr`)에서 `.shstrtab`이 있는 인덱스 |


### 2.2 Program Header

#### 구조체
``` c
typedef struct
{
	Elf64_Word	p_type;			/* Segment type */
	Elf64_Word	p_flags;		/* Segment flags */
	Elf64_Off	p_offset;		/* Segment file offset */
	Elf64_Addr	p_vaddr;		/* Segment virtual address */
	Elf64_Addr	p_paddr;		/* Segment physical address */
	Elf64_Xword	p_filesz;		/* Segment size in file */
	Elf64_Xword	p_memsz;		/* Segment size in memory */
	Elf64_Xword	p_align;		/* Segment alignment */
} Elf64_Phdr;
```

#### 주요 필드

| 필드       | 의미         | 설명                                                     |
| -------- | ---------- | ------------------------------------------------------ |
| p_type   | Segment 타입 | PT_LOAD(메모리 로드), PT_DYNAMIC(동적 링킹), PT_INTERP(인터프리터) 등 |
| p_flags  | Segment 권한 | PF_R(Read), PF_W(Write), PF_X(Execute)                 |
| p_offset | 파일에서 위치    | file_address = file_base + p_offset                    |
| p_vaddr  | 메모리 주소     | memory_address = base + p_vaddr                        |
| p_filesz | 파일에서 크기    | 실제 파일에 존재하는 데이터 크기                        |
| p_memsz  | 메모리에서 크기   | 메모리에 할당되는 크기 (보통 p_memsz ≥ p_filesz)        |
| p_align  | 정렬 단위      | 메모리 정렬 기준 (보통 0x1000, 페이지 단위)               |


### 2.3 Section Header

### 구조체

``` c
typedef struct
{
	Elf64_Word	sh_name;		/* Section name (string tbl index) */
	Elf64_Word	sh_type;		/* Section type */
	Elf64_Xword	sh_flags;		/* Section flags */
	Elf64_Addr	sh_addr;		/* Section virtual addr at execution */
	Elf64_Off	sh_offset;		/* Section file offset */
	Elf64_Xword	sh_size;		/* Section size in bytes */
	Elf64_Word	sh_link;		/* Link to another section */
	Elf64_Word	sh_info;		/* Additional section information */
	Elf64_Xword	sh_addralign;		/* Section alignment */
	Elf64_Xword	sh_entsize;		/* Entry size if section holds table */
} Elf64_Shdr;
```

#### 주요 필드

| 필드           | 의미             | 설명                                   |
| ------------ | -------------- | -------------------------------------------------|
| sh_name      | 섹션 이름 (offset) | 문자열이 아니라 `.shstrtab`에서의 offset             |
| sh_type      | 섹션 타입          | SHT_PROGBITS(.text), SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS(.bss) 등 |
| sh_flags     | 섹션 속성          | SHF_ALLOC(메모리 로드), SHF_WRITE(쓰기), SHF_EXECINSTR(실행)             |
| sh_addr      | 메모리 주소         | 실행 시 해당 Section이 올라가는 주소             |
| sh_offset    | 파일 위치 (offset) | section_data = base + sh_offset               |
| sh_size      | 섹션 크기          | 해당 Section의 전체 크기                       |
| sh_link      | 다른 섹션 참조       | 예: `.symtab` → `.strtab` 연결                  |
| sh_info      | 추가 정보          | Section별로 의미 다름 (보통 인덱스/정보값)           |
| sh_addralign | 정렬 단위          | 메모리 정렬 기준                                  |
| sh_entsize   | 엔트리 크기         | 테이블형 Section에서 각 entry 크기 (ex: symbol size)     |

### 2.4 Section Data



---

## References

- GNU C Library (glibc), elf.h
  https://www.gnu.org/software/libc/
