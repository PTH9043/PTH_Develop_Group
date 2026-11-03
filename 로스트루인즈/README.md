# 로스트 루인즈 모작

혼자 만든 WinAPI 게임 모작입니다. 아이템을 얻고 사용하는 인벤토리와 활·화살 기능을 기술소개서에서 설명했습니다.

## 작업한 내용

- 같은 아이템을 한 슬롯에 모아 수량을 표시하고, 선택한 슬롯과 상세 정보를 바꾸는 인벤토리를 만들었습니다.
- 장비와 아이템의 선택·장착·사용을 플레이어 상태와 연결했습니다.
- 키를 누르는 동안 활의 발사 속도를 조절하고, 손을 떼면 캐릭터의 방향으로 화살을 생성하도록 했습니다.
- 화살의 이동·낙하·충돌과 피격 처리를 분리하고, 한 화살이 같은 대상에 중복 피해를 주지 않도록 상태를 관리했습니다.

## 관련 코드

- [인벤토리](./PTH_Framework/PTH_Framework/CInventory.cpp)
- [인벤토리 슬롯](./PTH_Framework/PTH_Framework/CInvenSlot.cpp)
- [활](./PTH_Framework/PTH_Framework/CBow.cpp)
- [화살](./PTH_Framework/PTH_Framework/CArrow.cpp)

C++, WinAPI와 Visual Studio 2015를 사용했습니다. [프로젝트 시연 영상](https://youtu.be/nmEQ2ILw3QU)

원본 프레임워크와 리소스의 구성은 프로젝트 폴더를 참고해 주세요. 문서 정리 시 빌드·실행은 다시 확인하지 않았습니다.
