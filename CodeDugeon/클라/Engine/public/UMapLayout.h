#pragma once
#include "UComponent.h"

BEGIN(Engine)
class UPawn;
class UTransform;

class UMapLayout : public UComponent {
public:
	typedef struct ObjectDesc {
		_string		_sModelName{};
		_float4x4		_mWorldMatrix{ _float4x4::Identity };

	}OBJDESC;

	typedef struct MobsDesc {
		_string		_sAnimModelName{};
		_string		_sAnimName{};
		_float4x4		_mWorldMatrix{ _float4x4::Identity };

	}MOBDESC;
	using MOBDATA = std::pair<_string, _string>;
	using MAPOBJECTS = VECTOR<OBJDESC>;
	using MAPMOBS = VECTOR<MOBDESC>;
	using MAPOBJECTSCONTAINER = UNORMAP<_string, MAPOBJECTS>; //<방 이름, 데이터>
	using GUARDSSCONTAINER = UNORMAP<_string, MAPOBJECTS>;
	using MAPMOBSCONTAINER = UNORMAP<_string, MAPMOBS>;
	
public:
	UMapLayout(CSHPTRREF <UDevice> _spDevice);
	UMapLayout(const UMapLayout& _rhs);
	DESTRUCTOR(UMapLayout)
	CLONE_MACRO(UMapLayout, "MapLayout::Clone To Failed")
	// UComponent을(를) 통해 상속됨
	void Free() override;
	HRESULT NativeConstruct() override;
	HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;

	_bool SaveMapObjects(const _wstring& _wstrPath);
	_bool SaveMapMobs(const _wstring& _wstrPath);
	_bool LoadMapObjects();
	_bool LoadMapMobs();
	_bool LoadMapGuards();
	
	void AddtoMapContainer(const _string& _RoomName, MAPOBJECTS& _ObjData);
	void AddtoMobsContainer(const _string& _MobName, MAPMOBS& MobData);

	CSHPTRREF<MAPOBJECTSCONTAINER> GetMapObjectsContainer() { return m_spMapObjectsContainer; };
	CSHPTRREF<MAPMOBSCONTAINER> GetMapMobsContainer() { return m_spMapMobsContainer; };
	CSHPTRREF<GUARDSSCONTAINER> GetGuardsContainer() { return m_spGuardsContainer; };
private:
	SHPTR<MAPOBJECTSCONTAINER> m_spMapObjectsContainer;
	SHPTR<MAPMOBSCONTAINER> m_spMapMobsContainer;
	SHPTR< GUARDSSCONTAINER> m_spGuardsContainer;

};


END;
