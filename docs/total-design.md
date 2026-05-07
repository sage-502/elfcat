
---

# 📄 `total-design.md`

```markdown
# 전체 설계 (Total Design)

## 개요
본 프로젝트는 ELF 파일을 분석하여 취약 함수 사용 여부를 탐지하는 프로그램이다.

## 구성 요소

### 1. ELF Parser (다른 팀원 담당)
- ELF 파일을 메모리 구조체로 파싱
- 섹션/심볼 테이블 접근 제공

### 2. 취약 함수 탐색 모듈 (본인 담당)
- 심볼 테이블 분석
- 위험 함수 탐지

### 3. 출력 모듈
- 결과를 사용자에게 출력

---

## 전체 흐름

```text
ELF 파일
   ↓
parse_elf()
   ↓
elf_t 구조체
   ↓
analyze_vulnerability()
   ↓
vuln_t 결과
   ↓
print_vuln()
