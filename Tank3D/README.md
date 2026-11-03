# 3D 탱크

개인 3D 게임 과제입니다. 탱크의 부품을 부모·자식 관계로 연결하고, 3D 정점을 화면 좌표로 변환해 GDI 선분으로 그렸습니다.

## 작업한 내용

- 탱크의 이동과 회전, 포탑 회전, 포신 각도 조절, 발사·재장전을 별도 객체로 나누었습니다.
- 포탑 → 포신 → 발사 장치 순서로 변환을 연결하고, 위치·회전·크기 중 부모로부터 적용할 값을 선택할 수 있도록 했습니다.
- 부모 변환 참조에는 `weak_ptr`을 사용했습니다.
- 화면과 깊이 범위를 기준으로 선분을 선택하고 탱크와 지형을 그렸습니다.

## 관련 코드

- [탱크 조작](./DirectX12_Project/CTank.cpp)
- [변환과 부모 연결](./DirectX12_Project/CTransform.cpp)
- [포탑 회전](./DirectX12_Project/CControlHead.cpp)
- [포신 각도](./DirectX12_Project/CPosin.cpp)
- [발사와 재장전](./DirectX12_Project/CShooter.cpp)

C++, WinAPI/GDI와 DirectXMath를 사용했습니다. 프로젝트 폴더 이름은 DirectX12_Project이지만 렌더링은 GDI로 처리합니다. [프로젝트 시연 영상](https://youtu.be/JX8UwgPkC4c)

[Visual Studio 솔루션](./DirectX12_Project.sln)과 소스가 포함되어 있습니다. 빌드·실행은 문서 정리 시 다시 확인하지 않았습니다.
