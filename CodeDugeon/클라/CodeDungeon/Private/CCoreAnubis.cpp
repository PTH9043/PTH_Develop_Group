#include "ClientDefines.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UTransform.h"
#include "UMethod.h"
#include "CCoreAnubis.h"
#include "UPawn.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "UModel.h"

CCoreAnubis::CCoreAnubis(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CModelObjects(_spDevice, _wstrLayer, _eCloneType)
{
}

CCoreAnubis::CCoreAnubis(const CCoreAnubis& _rhs)
	: CModelObjects(_rhs)
{
}

void CCoreAnubis::Free()
{
}

HRESULT CCoreAnubis::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CCoreAnubis::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	SetModel(L"Proto_Res_Model_anubishead_FBX.bin");

	COREANUBISNDESC tBarsDesc = UMethod::ConvertTemplate_Index<COREANUBISNDESC>(_vecDatas, 0);
	GetTransform()->SetNewWorldMtx(tBarsDesc._Worldm);
	SetPawnType(PAWNTYPE::PAWN_STATICOBJ);
	SetActive(false);

	SetIfOutlineScale(true);

	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(-10.f, 10.f, 15.f);
	tDesc.vScale = _float3(12.5, 12.5, 12.5);
	SHPTR<UCollider> Collider1 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_SPHERECOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"ForInteractionCoreAnubis";
	AddColliderInContainer(mainColliderTag, Collider1);


	return S_OK;
}

void CCoreAnubis::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTransform(GetTransform()->GetPos(), GetTransform()->GetQuaternion());
	}

	if (GetInteractionState())
	{
	}
}

void CCoreAnubis::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
}

HRESULT CCoreAnubis::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	return S_OK;
}

HRESULT CCoreAnubis::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	return S_OK;
}
HRESULT CCoreAnubis::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	__super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
	return S_OK;
}

void CCoreAnubis::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

