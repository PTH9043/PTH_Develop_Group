#pragma once
#include "CModelObjects.h"

BEGIN(Engine)
class UModel;
class UShaderConstantBuffer;
END

BEGIN(Client)
/*
@ Date: 2024-04-23, Writer: 이성현
@ Explain
- 맵을 이루는 방들을 취급하는 클래스
*/
class CRooms final : public CModelObjects {
public:
	typedef struct tagRoomDesc {
		_wstring	_wsRoomName{};
	}ROOMDESC;
	CRooms(CSHPTRREF<UDevice> _spDevice,
		const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CRooms(const CRooms& _rhs);
	DESTRUCTOR(CRooms)
	CLONE_MACRO(CRooms, "CRooms::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;


	const _float3& GetRoomCenterPos() const { return m_vRoomCenterPos; }
protected:
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	_wstring	m_wsRoomTag;
	_float3     m_vRoomCenterPos;

};

END