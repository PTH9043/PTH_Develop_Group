# 귀멸의 칼날 히노카미 혈풍담 모작

6명이 함께 개발한 DirectX 11 게임 모작입니다. 2022년 10월부터 12월까지 작업했고, 저는 공통 프레임워크와 컷신 제작 도구를 맡았습니다. 저장소에는 팀이 함께 만든 코드가 포함되어 있습니다.

## 담당한 작업

- 제공받은 프레임워크에 파일 관리와 공통 기능을 추가하고, 편집 도구와 게임에서 같은 형식의 데이터를 사용하도록 연결했습니다.
- ImGui로 캐릭터와 카메라를 배치하고, 카메라 경로와 애니메이션을 편집해 결과를 확인하는 컷신 도구를 만들었습니다.
- 컷신 설정을 바이너리 파일로 저장하고 불러와, 게임에서 트리거 이름으로 선택해 재생하도록 연결했습니다.
- 저장한 연출이 캐릭터의 실제 위치와 방향에 맞춰 재생되도록 변환을 적용했습니다.

## 관련 코드

- [컷신 편집 화면](./Tool/Private/CutSceneViewer_Tool.cpp)
- [카메라 편집](./Tool/Private/CutSceneCamera_Tool.cpp)
- [컷신 등록과 선택](./Client/Private/CutScene_Manager.cpp)
- [게임에서 컷신 재생](./Client/Private/CutSceneModul.cpp)

C++, DirectX 11, ImGui, Assimp, FMOD와 Visual Studio 2019를 사용했습니다. [프로젝트 시연 영상](https://youtu.be/OlgaruBwfow)

[Visual Studio 솔루션](./Framework.sln)을 열기 전에 참조 라이브러리와 리소스 경로를 확인해 주세요. 이 문서를 정리하면서 새로 빌드하거나 실행하지는 않았습니다.
