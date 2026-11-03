#include "CBlood.h"
#include "CMethod.h"
#include "CGameInstance.h"
#include "CCube.h"
#include "CTransform.h"
#include<random>

shared_ptr<CActor> CBlood::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CBlood> pInstance = CSmartPointerFactory<CBlood>::CreateShared();
	if (FAILED(pInstance->NativeConstruct(_tDatas))) {
		Safe_Free(pInstance);
		DEBUG_MSG("CBlood::Failed To CloneActor");
	}
	return pInstance;
}

void CBlood::Free()
{
	for (auto& iter : m_BloodList) {
		shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		pGameInstance->Add_DeleteActor(LAYER_OBJECT, iter);
	}
	__super::Free();
}

HRESULT CBlood::NativeConstruct()
{
	return S_OK;
}

HRESULT CBlood::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas)))
		return E_FAIL;

	if (_tDatas.size() > 0) {
		BLOODDESC tDesc = CMethod::ConvertTemplate_PopBack<BLOODDESC>(&_tDatas);
		m_BloodList.reserve(tDesc.iBloodCnt);
		m_BloodDirList.reserve(tDesc.iBloodCnt);
		ChageConstValue(BLOOD_CNT, tDesc.iBloodCnt);

		shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		CCube::CUBEDESC tCubeDecs;
		tCubeDecs.vColor = _float3(255.f, 10.f, 10.f);

		int n = 100;
		std::random_device rd;  
		std::mt19937 gen(rd()); 
		std::uniform_int_distribution<> dis(0, n - 1); 

		for (_uint i = 0; i < BLOOD_CNT; ++i) {
			shared_ptr<CCube> pCube = static_pointer_cast<CCube>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_CUBE, LAYER_OBJECT,
				VOIDDATAS{ &tCubeDecs }));

			pCube->Get_Transform()->Set_ParentsTransform(Get_Transform());
			pCube->Get_Transform()->Set_Scale(_float3(0.25f, 0.25f, 0.25f));
			pCube->Set_Active(true);
			m_BloodList.push_back(pCube);
			_float fRandX = (_float)dis(gen);
			_float fRandY = (_float)dis(gen);
			_float fRandZ = (_float)dis(gen);

			_float3 vDir = _float3(fRandX, fRandY, fRandZ);
			vDir.NormalizeSelf();
			m_BloodDirList.push_back(vDir);
		}
	}
	Set_Active(true);
	return S_OK;
}

HRESULT CBlood::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlood::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	for (_uint i = 0; i < BLOOD_CNT; ++i) {
		m_BloodList[i]->Get_Transform()->Translate_Dir(m_BloodDirList[i].Get_Vector(), _dTimeDelta, 10.f);
	}
	m_dBloodTimer += _dTimeDelta;
	if (m_dBloodTimer >= BLOOD_TIMER) {
		shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		pGameInstance->Add_DeleteActor(LAYER_BLOOD, ThisShared<CActor>());
		Set_Active(false);
	}
	return S_OK;
}

HRESULT CBlood::Late_Tick(const _double& _dTimeDelta)
{
	return S_OK;
}

HRESULT CBlood::Render(const HDC& _hDC)
{
	return S_OK;
}
