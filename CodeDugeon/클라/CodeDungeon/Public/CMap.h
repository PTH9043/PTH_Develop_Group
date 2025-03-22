#pragma once
#include "UComponent.h"
#include "UMapLayout.h"
#include "UGuard.h"

BEGIN(ENGINE)
class UStageManager;
class UMapLayout;

END

/*
@ Date: 2024-04-23, Writer: 이성현
@ Explain
- 맵을 취급하는 클래스
*/
BEGIN(Client)
class CRooms;
class CModelObjects;
class CMob;
class CWarriorPlayer;

using ROOMCONTAINER = UNORMAP<_wstring, SHPTR<CRooms>>;
using OBJCONTAINER = VECTOR<SHPTR<CModelObjects>>;
using MOBCONTAINER = VECTOR<SHPTR<CMob>>;

using STATICOBJCONTAINER = UNORMAP<_wstring, OBJCONTAINER>;
using MOBSCONTAINER = UNORMAP<_wstring, MOBCONTAINER>;
using GUARDCONTAINER = UNORMAP<_wstring, SHPTR<UGuard>>;

class CMap : public UComponent
{
public:
	CMap(CSHPTRREF <UDevice> _spDevice);
	CMap(const CMap& _rhs);
	DESTRUCTOR(CMap)

	// UComponent을(를) 통해 상속됨
	void Free() override;
	SHPTR<UCloneObject> Clone(const VOIDDATAS& _tDatas) override;
	HRESULT NativeConstruct() override;
	HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;

	void LoadRooms();
	void LoadStaticObjects();
	void LoadGuards();
	void LoadMobs(CSHPTRREF<CWarriorPlayer> _spPlayer);
	
	CSHPTRREF<ROOMCONTAINER> GetRooms() { return m_spRoomContainer; }
	CSHPTRREF<STATICOBJCONTAINER> GetStaticObjs() { return m_spStaticObjContainer;}
	CSHPTRREF<MOBSCONTAINER> GetMobs() { return m_spMobsContainer; }
	const GUARDCONTAINER& GetGuards() { return m_GuardContainer; }

	void AddLightCount() { m_iLightCount++; }
	const _uint GetLightCount() { return m_iLightCount; }
	const _int GetSarcophagusCount() { return m_iSarcophagusCount; }
private:
	SHPTR<ROOMCONTAINER>			m_spRoomContainer;
	SHPTR<UMapLayout>				m_spMapLayout;
	SHPTR<STATICOBJCONTAINER>		m_spStaticObjContainer;
	GUARDCONTAINER			m_GuardContainer;

	_uint							m_iLightCount;
	_int							m_iSarcophagusCount;

	SHPTR<MOBSCONTAINER>			m_spMobsContainer;

};
END
