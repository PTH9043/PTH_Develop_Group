# ServerFramework

Code Dungeon 졸업 프로젝트의 서버 코드입니다. 저는 Boost.Asio와 Protocol Buffers를 이용한 서버 통신과 공통 세션·플레이어 메시지 처리를 맡았습니다.

패킷의 종류와 크기를 확인해 수신 데이터를 처리하고, 나누어 도착한 데이터는 버퍼에 남겼다가 이어서 처리했습니다. 패킷 처리 후 남은 데이터를 옮기는 위치의 오류로 데이터가 누락되는 문제도 수정했습니다. 크기별 메모리 풀과 STL 할당기도 사용했습니다.

- [공통 세션과 수신 버퍼](./Core/Private/ASession.cpp)
- [플레이어 메시지 처리](./Server/Private/CPlayerSession.cpp)
- [메모리 풀](./Core/Public/Memory.h) · [STL 할당기](./Core/Public/Allocator.h)
- [서버 솔루션](./ServerFramework.sln) · [클라이언트 코드](../클라) · [프로젝트 시연 영상](https://youtu.be/hQBBsoL_ETs)
- [프로젝트 소개와 다른 공개 코드](https://github.com/PTH9043/GraduateProject)

이 폴더에는 팀원이 작성한 코드도 포함되어 있습니다. 2026년의 추가 통신 검증은 별도 재현 빌드에서 진행한 작업입니다.
