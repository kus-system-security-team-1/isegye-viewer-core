# isegye-viewer-core 

submodule of [./isegye-viewer](https://github.com/kus-system-security-team-1/isegye-viewer-core) repository.

[1. Coding Convention](##coding-convention)

[2. Commit Convention](##commit-convention)

[3. Directory Structure](##directory-structure)

[4. Branch Flow](##branch-flow)

## Coding Convention

C++ Code style은 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)를 기반으로 함.
그에 따라 [cpplint](https://github.com/cpplint/cpplint) 를 사용해 code check를 꼭 해주세요

## Commit Convention

commit message의 형식

```
<type>(<scope>): <short summary> <-- message header
<BLANK LINE>
<body> <-- message body
<BLANK LINE>
<footer> <-- message footer
```

### message header

commit message를 작성할 시, 다음 메시지 헤더에서 `scope` 를 제외한 `type`과 `short summary` 만 사용함. 

```
커밋 메시지 헤더
<type>(<scope>): <short summary>
  │       │             │
  │       │             └─⫸ 명령문, 현재 시제로 작성합니다. 대문자를 사용하지 않으며, 마침표로 끝내지 않습니다.
  │       │
  │       └─⫸ Commit Scope: animations|bazel|benchpress|common|compiler|compiler-cli|core|
  │                          elements|forms|http|language-service|localize|platform-browser|
  │                          platform-browser-dynamic|platform-server|router|service-worker|
  │                          upgrade|zone.js|packaging|changelog|dev-infra|docs-infra|migrations|
  │                          ngcc|ve
  │
  └─⫸ Commit Type: build|docs|feat|fix|perf|refactor|test
The <type> and <summary> fields are mandatory, the (<scope>) field is optional.
```

- `type`
```
build: Changes that affect the build system or external dependencies (example scopes: gulp, broccoli, npm)
ci: Changes to our CI configuration files and scripts (examples: CircleCi, SauceLabs)
docs: Documentation only changes
feat: A new feature
fix: A bug fix
perf: A code change that improves performance
refactor: A code change that neither fixes a bug nor adds a feature
test: Adding missing tests or correcting existing tests
```

- `short summary`
```
현재 시제로 작성 : change (o) , changed, changes (x)
마침표 붙이지 말기 (.)
첫글자를 소문자로 써야함
```

### message body
- 명령문, 현재 시제로 작성 권장
- 코드를 변경했을 시, 변경 이유와 변경 젼과 차이점 설명

### message footer
- 선택사항임
- issue tracker ID 명시할 때 작성
- 유형: #이슈 번호 형식으로 작성
- 여러 개의 이슈 번호는 쉼표(,) 로 구분
- `Fixes`: 이슈 수정중 (아직 해결되지 않은 경우)
- `Resolves`: 이슈를 해결했을 때 사용
- `Ref`: 참고할 이슈가 있을 때 사용
- ``Related to``: 해당 커밋에 관련된 이슈번호 (아직 해결되지 않은 경우)

```
ex) 
Fixes: #45 Related to: #34, #23
```


### 예시
```
feat: "로그인 기능 구현"

로그인 시 JWT 발급

Resolves: #111
Ref: #122
related to: #30, #50
```

```
fix: prevent racing of requests

Introduce a request id and a reference to latest request. Dismiss
incoming responses other than from latest request.

Remove timeouts which were used to mitigate the racing issue but are
obsolete now.

Reviewed-by: Z
Refs: #123
```

출처 : [Angular commit message format](https://github.com/angular/angular/blob/main/CONTRIBUTING.md#commit-message-format)

## Directory Structure

```
include/ - 헤더파일 위치
lib/ - 외부 라이브러리 위치
src/ - 소스코드 파일
test/ - test 코드 파일
bin/ - 빌드된 실행 파일
```

## Branch flow

[github-flow](https://www.heropy.dev/p/6hdJi6) 사용