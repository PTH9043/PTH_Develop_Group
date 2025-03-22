#include "ClientDefines.h"
#include "CMob.h"
#include "CSarcophagus.h"
#include "UGameInstance.h"
#include "URenderer.h"
#include "CMainCamera.h"
#include "UTransform.h"
#include "UAnimModel.h"
#include "CMummyAnimController.h"
#include "UMethod.h"
#include "UCollider.h"
#include "UProcessedData.h"

CSarcophagus::CSarcophagus(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CMob(_spDevice, _wstrLayer, _eCloneType), m_SarcophagusType{}, m_dElapsedTimeForDeath{}
{
}

CSarcophagus::CSarcophagus(const CSarcophagus& _rhs)
	: CMob(_rhs), m_SarcophagusType{}, m_dElapsedTimeForDeath{}
{
}

void CSarcophagus::Free()
{
}

HRESULT CSarcophagus::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CSarcophagus::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SetPawnType(PAWNTYPE::PAWN_STATICOBJ);
	SetActivationRange(50);
	SetDeactivationRange(120);
	/*UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(1.f, 1.f, 1.f);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";
	AddColliderInContainer(mainColliderTag, Collider);

	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTranslate(GetAnimModel()->GetCenterPos());
		Containers.second->SetScaleToFitModel(GetAnimModel()->GetMinVertexPos(), GetAnimModel()->GetMaxVertexPos());
	}*/

	return S_OK;
}

void CSarcophagus::ReceiveNetworkProcessData(const UProcessedData& _ProcessData)
{

}

void CSarcophagus::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	_double SarcophagusOpeningSpeed = 40;
	_double LyingSarcophagusTimeArcOpenStart = 50;
	_double LyingSarcophagusTimeArcOpenEnd = 50;
	_double StandingSarcophagusTimeArcOpenEnd = 30;

	GetAnimationController()->Tick(_dTimeDelta);

	if (!GetDeathState())
	{
		if (GetFoundTargetState())
		{
			if (GetSarcophagusType() == SARCOTYPE::TYPE_LYING)
			{
				USound* Layingsound1 = spGameInstance->BringSound(L"CrumblingRock").get();
				USound* Layingsound2 = spGameInstance->BringSound(L"StoneSlide").get();
				if (GetElapsedTime() == 0)
				{
					Layingsound1->PlayWithInputChannel(&m_pLaying1Channel);
					Layingsound2->PlayWithInputChannel(&m_pLaying2Channel);
				}
			}
			else
			{
				USound* Standingsound1 = spGameInstance->BringSound(L"Coffin_InsideImpact").get();
				USound* Standingsound2 = spGameInstance->BringSound(L"DebrisandImpact").get();
				if (GetElapsedTime() == 0)
				{
					Standingsound1->PlayWithInputChannel(&m_pStanding1Channel);
					Standingsound1->PlayWithInputChannel(&m_pStanding2Channel);
				}
			}
			SetElapsedTime(GetElapsedTime() + _dTimeDelta * SarcophagusOpeningSpeed);
		}
		if (GetSarcophagusType() == SARCOTYPE::TYPE_LYING)
		{
			if (GetElapsedTime() < LyingSarcophagusTimeArcOpenEnd)
			{
				GetAnimModel()->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, LyingSarcophagusTimeArcOpenStart + GetElapsedTime());
			}
		}
		else
			if (GetElapsedTime() < StandingSarcophagusTimeArcOpenEnd)
			{
				GetAnimModel()->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, GetElapsedTime());
			}
	}
	else
		GetAnimModel()->UpdateDissolveTImer(_dTimeDelta);


	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTransform(GetTransform());
	}
}

void CSarcophagus::LateTickActive(const _double& _dTimeDelta)
{
	GetRenderer()->AddRenderGroup(RENDERID::RI_NONALPHA_LAST, GetShader(), ThisShared<UPawn>());
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (GetOwnerMummy()->GetDeathState() == true)
	{
		SetDeathState(true);
		m_dElapsedTimeForDeath += _dTimeDelta * 20;
		if (m_dElapsedTimeForDeath >= 100.0)
		{
			SetActive(false);
			spGameInstance->RemoveCollisionPawn(ThisShared<CMob>());
		}
	}
}

HRESULT CSarcophagus::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CSarcophagus::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CSarcophagus::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void CSarcophagus::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
