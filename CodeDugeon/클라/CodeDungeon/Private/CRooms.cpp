#include "ClientDefines.h"
#include "CRooms.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UTransform.h"
#include "UMethod.h"
#include "UModel.h"

CRooms::CRooms(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CModelObjects(_spDevice, _wstrLayer, _eCloneType)
	, m_wsRoomTag{}, m_vRoomCenterPos{ }
{
}

CRooms::CRooms(const CRooms& _rhs) :
	CModelObjects(_rhs)
	, m_wsRoomTag{}, m_vRoomCenterPos{ }
{
}

void CRooms::Free()
{
}

HRESULT CRooms::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CRooms::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	ROOMDESC tDesc = UMethod::ConvertTemplate_Index<ROOMDESC>(_vecDatas, 0);
	m_wsRoomTag = tDesc._wsRoomName;

	_wstring ModelProtoTag = L"Proto_Res_Model_";
	ModelProtoTag.append(m_wsRoomTag);

	SetModel(ModelProtoTag);

	UCollider::COLLIDERDESC collDesc;

	collDesc.vTranslation = _float3(GetModel()->GetCenterPos());
	collDesc.vScale = _float3(1.f, 1.f, 1.f);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &collDesc }));
	Collider->SetScaleToFitModel(GetModel()->GetMinVertexPos(), GetModel()->GetMaxVertexPos());
	Collider->SetTransform(GetTransform());

	_wstring mainColliderTag = L"Rooms";
	AddColliderInContainer(mainColliderTag, Collider);

	m_vRoomCenterPos = Collider->GetCurPos();
	SetOutline(false);

	return S_OK;
}

void CRooms::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}


