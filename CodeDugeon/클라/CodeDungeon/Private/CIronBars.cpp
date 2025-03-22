#include "ClientDefines.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UTransform.h"
#include "UMethod.h"
#include "CIronBars.h"
#include "UPawn.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "UModel.h"

CIronBars::CIronBars(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CModelObjects(_spDevice, _wstrLayer, _eCloneType)
{
}

CIronBars::CIronBars(const CIronBars& _rhs)
	: CModelObjects(_rhs)
{
}

void CIronBars::Free()
{
}

HRESULT CIronBars::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CIronBars::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);


	SetModel(L"Proto_Res_Model_Bars_FBX.bin");

	IRONBARSDESC tBarsDesc = UMethod::ConvertTemplate_Index<IRONBARSDESC>(_vecDatas, 0);
	GetTransform()->SetNewWorldMtx(tBarsDesc._Worldm);
	m_f3OriginPos = GetTransform()->GetPos();

	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(410, 350, 0);
	SHPTR<UCollider> Collider1 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";
	AddColliderInContainer(mainColliderTag, Collider1);


	UCollider::COLLIDERDESC tDesc2;
	tDesc2.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc2.vScale = _float3(410, 350, 100);
	SHPTR<UCollider> Collider2 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc2 }));
	_wstring subColliderTag = L"ForInteractionBars";
	AddColliderInContainer(subColliderTag, Collider2);


	SetPawnType(PAWNTYPE::PAWN_STATICOBJ);
	SetIfOutlineScale(false);
	/*SetOutline(true);*/
	return S_OK;
}

void CIronBars::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTranslate(GetModel()->GetCenterPos());
		Containers.second->SetTransform(GetTransform());
	}

	if(GetInteractionState())
	{
		if (GetTransform()->GetPos().y - m_f3OriginPos.y < 30)
			GetTransform()->TranslateDir(_float3(0, 1, 0), _dTimeDelta, 3);
		else
			SetActive(false);
	}
}


void CIronBars::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
	//for (auto& Colliders : GetColliderContainer())
	//{
	//	if(Colliders.first == L"ForInteraction")
	//		Colliders.second->AddRenderer(RENDERID::RI_NONALPHA_LAST);
	//}
}

HRESULT CIronBars::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);
	return S_OK;
}

HRESULT CIronBars::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	return S_OK;
}
HRESULT CIronBars::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	__super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
	return S_OK;
}

void CIronBars::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

