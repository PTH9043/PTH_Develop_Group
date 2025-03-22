#pragma once

#include "UBase.h"

BEGIN(Engine)
class UActor;
class UPawn;
class UTransform;
class UVIBuffer;
class UGameInstance;
class UGrid;
class UCollider;


using COLLIDERCONTAINER = UNORMAP<_wstring, SHPTR<UCollider>>;

struct PICKINGDESC {
	PICKINGDESC() = default;
	PICKINGDESC(CSHPTRREF<UPawn> _spPawn, const _float3& _vPickPos, const _float _fDist, _bool _bSuccess)
		: spPawn(_spPawn), vPickPos(_vPickPos), fDist(_fDist), bPickingSuccess(_bSuccess)
	{}
	SHPTR<UPawn>	spPawn{ nullptr };
	_float3			vPickPos{ 0, 0, 0 };
	_float			fDist{ 0 };
	_bool			bPickingSuccess{ false };
};

struct WAITCHECKPAWN {
	SHPTR<UPawn>			spPawn{ nullptr };
	SHPTR<UVIBuffer>		spVIBuffer{ nullptr };


	_bool operator == (const WAITCHECKPAWN& _stPawn) const
	{
		if (spPawn == _stPawn.spPawn && spVIBuffer == _stPawn.spVIBuffer)
			return true;

		return false;
	}
};

struct MAINGRID {
	SHPTR<UGrid> spGrid{ nullptr };
	_bool operator == (const MAINGRID& _stGrid) const
	{
		if (spGrid == _stGrid.spGrid)
			return true;

		return false;
	}
};

END

namespace std
{
	// Atomic 함수 재정의
	template<>
	struct hash<Engine::WAITCHECKPAWN> {
		size_t operator()(WAITCHECKPAWN ptr) const {
			return std::hash<WAITCHECKPAWN*>()(&ptr);
		}
	};
	template<>
	struct equal_to<Engine::WAITCHECKPAWN> {
		bool operator()(const Engine::WAITCHECKPAWN& lhs, const Engine::WAITCHECKPAWN& rhs) const {
			return lhs == rhs;
		}
	};
}

BEGIN(Engine)

class UPicking : public UBase {
public:
	UPicking();
	NO_COPY(UPicking)
	DESTRUCTOR(UPicking)
public:
	virtual void Free() override;
	HRESULT ReadyPickingDesc(CSHPTRREF<GRAPHICDESC> _spGraphicDesc);
	void CastRayInWorldSpace(UGameInstance* _pGameInstance);
	void AddPickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer);
	void DeletePickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer);
	void AddPickingGrid(const MAINGRID& _stGrid);
	SHPTR<UPawn> GetPickingPawn();
	const PICKINGDESC GetPickDesc();
	_bool PickingMesh(const _float3& _RayPos, const _float3& _RayDir, CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer, _float* _pDist, _float3* _pOut);
	_bool PickingOnGrid(CSHPTRREF<UGrid> _spGrid, _float* _pDist, _float3* _pOut);
	_bool PickingCollider(const _float3& _RayPos, const _float3& _RayDir, CSHPTRREF<UPawn> _spPawn, _float* _pDist);
private:
	_bool IsPickingCheck(const _float3& _vLocalRay, const _float3& _vDirRay, const _float3& _vPos1,
		const _float3& _vPos2, const _float3& _vPos3, const _float4x4& _mWorldMatrix, _float* _pDist, _float3* _pOut);
	// Add Picking Desc
	void AddPickingObject(const PICKINGDESC& _stDesc);
private:
	using PICKINGLIST = std::list<PICKINGDESC>;
	using WAITCHECKPAWNLIST = UNORSET<WAITCHECKPAWN>;
	// // Dats
	_float2					m_vWindowSize;
	RECT					m_stClientRect;
	_float3					m_vRayPos;
	_float3					m_vRayDir;
	PICKINGLIST				m_lsPickingList;
	PICKINGDESC				m_stPickingDesc;
	WAITCHECKPAWNLIST		m_WaitCheckPawnList;
	MAINGRID				m_spMainGrid;
	_bool					m_isMouseInScreen;
};

END

