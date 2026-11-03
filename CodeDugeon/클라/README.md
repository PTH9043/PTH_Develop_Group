# Code Dungeon 클라이언트

세 명이 함께 개발한 졸업 프로젝트의 DirectX12 클라이언트와 편집 도구입니다. 저는 공통 프레임워크, 리소스 관리와 애니메이션·장비 부착 도구를 맡았습니다.

객체 생성 조건을 제한하고 공통 기능을 사용할 수 있도록 구성했습니다. 애니메이션 이벤트를 편집·저장해 게임에서 사용하고, 본 변환을 이용해 장비 위치와 회전을 갱신하도록 했습니다. 스마트 포인터는 교수님께서 제공해 주신 코드를 바탕으로 확장했습니다.

- [애니메이션 편집 도구](./Tool/Private/TAnimControlView.cpp)
- [공통 객체 코드](./Engine/public/UObject.h)
- [클라이언트 솔루션](./CodeDungeon.sln) · [서버 코드](../서버) · [프로젝트 시연 영상](https://youtu.be/hQBBsoL_ETs)
- [프로젝트 소개와 다른 공개 코드](https://github.com/PTH9043/GraduateProject)

이 폴더에는 팀원이 작성한 코드도 포함되어 있습니다. 공개 저장소의 코드 버전은 위에 연결한 GraduateProject 저장소와 일부 다릅니다.
