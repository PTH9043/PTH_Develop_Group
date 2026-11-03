# Code Vein 모작

2022년 7월부터 9월까지 혼자 만든 DirectX 11 게임 모작입니다. 게임 화면을 구성하며 맵과 이동 영역, 이펙트를 편집할 수 있는 도구도 만들었습니다.

## 작업한 내용

- 삼각형 셀을 이어 캐릭터가 이동할 영역을 만들고, 이웃 셀을 연결해 이동 가능 여부를 검사했습니다.
- 화면에서 마우스로 객체를 선택해 위치와 회전을 수정하고, 객체를 복제하거나 삭제하는 맵 편집 기능을 만들었습니다.
- 파티클의 위치, 크기, 수명과 반복 여부 등을 편집하고 파일로 저장해 다시 불러오도록 했습니다.
- 플레이어의 위치와 방향을 기준으로 몬스터의 행동과 애니메이션을 전환하도록 구성했습니다.

## 관련 코드

- [이동 영역 편집](./Client/Private/CNavigationTool.cpp)
- [맵 편집 화면](./Client/Private/CMainView.cpp)
- [파티클 편집](./Client/Private/CEffect_ParticleTool.cpp)
- [몬스터 처리](./Client/Private/CMonster.cpp)

C++, DirectX 11, ImGui, Assimp, FMOD와 Visual Studio 2015를 사용했습니다. [프로젝트 시연 영상](https://youtu.be/JrEseMyUdbg)

[Visual Studio 솔루션](./PTH_Framework.sln)을 열기 전에 참조 라이브러리와 리소스 경로를 확인해 주세요. 이번 문서 정리 과정에서 빌드나 실행은 다시 확인하지 않았습니다.
