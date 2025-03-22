#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERADIMINSTOR_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERADIMINSTOR_H

#include "AServerService.h"

BEGIN(Core)
class AJobTimer;

END

BEGIN(Server)
/*
@ Date: 2023-01-02, Writer: 박태현
@ Explain
- Server를 관리하는 관리자 클래스이다. 플레이어 세션들을 총괄
*/
class  CServerAdiminstor final : public Core::AServerService{
public:
	CServerAdiminstor(OBJCON_CONSTRUCTOR, const _string& _strNavigationPath = "");
	NO_COPY(CServerAdiminstor)
	DESTRUCTOR(CServerAdiminstor)
public:
	virtual bool NativeConstruct() override;
	virtual bool Start() override;
protected:
	// UServerService을(를) 통해 상속됨
	virtual void Connect() override;
private:
	void CreateServerMobResourceData();
	void CreateServerMob();

	void CreateMummyAndSarphagousMob(void* _pData, SARCOPHAGUSTYPE _SarcophagusType, SHPTR< AJobTimer> _spMonsterJobTimer);

	template<class T>
	void CreateMonster(void* _pData, SHPTR<AJobTimer> _spMonsterJobTimer)
	{
		SHPTR<T> spMonster = Create<T>(GetCoreInstance(), GiveID(), _spMonsterJobTimer);
		spMonster->Start(VOIDDATAS{ _pData });
		InsertMobObject(spMonster->GetSessionID(), spMonster);
	}
private:
	void Free() override;
};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CSERVERADIMINSTOR_H
