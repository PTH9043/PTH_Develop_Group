#include "CMonsterEPController.h"
#include "CEffectParticleGroup.h"
#include "CEffectGroup.h"
#include "CGameInstance.h"
#include "CMonster.h"
#include"C2DEffectParticle.h"
#include "CParticle.h"

CMonsterEPController::CMonsterEPController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMonsterEPController::NativeConstruct(void* _pArg)
{
	if (nullptr != _pArg)
	{
		MONEPCONDESC tDesc = *static_cast<MONEPCONDESC*>(_pArg);

		CTransform* pTransform = static_cast<CTransform*>(tDesc._pMonster->Get_Component(COM_TRANSFORM));

		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			{
				m_pDeadOneParticle = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
					LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_DEAD));
				Safe_AddRef(m_pDeadOneParticle);
				m_pDeadOneParticle->Active(false);
			}

			for (_uint i = 0; i < 3; ++i)
			{
				{
					CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
						LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_BLOOD_1));
					Safe_AddRef(pInstance);
					pInstance->Active(false);
					m_Blood1List.push_back(pInstance);
				}
				{
					CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
						LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_BLOOD_2));
					Safe_AddRef(pInstance);
					pInstance->Active(false);
					m_Blood2List.push_back(pInstance);
				}
				{
					CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
						LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_BLOOD_3));
					Safe_AddRef(pInstance);
					pInstance->Active(false);
					m_Blood3List.push_back(pInstance);
				}
			}
		}
		END_SINGLETON(pGameInstance);
	}

	return S_OK;
}

void CMonsterEPController::Update(const _double& _dDeltaSeconds, CMonster* _pMonster)
{
	if (nullptr != _pMonster)
	{

	}
}

void CMonsterEPController::Show_Blood(_fvector vPos, _fvector vDir)
{
	_uint iRan = rand() % 3;
	if (0 == iRan)
	{
		if (m_iBlood1Index >= 3)
			m_iBlood1Index = 0;
		m_Blood1List[m_iBlood1Index]->Set_Pos(vPos);
		m_Blood1List[m_iBlood1Index]->Set_Look(vDir);
		m_Blood1List[m_iBlood1Index++]->Active(true);
	}
	else if (1 == iRan)
	{
		if (m_iBlood2Index >= 3)
			m_iBlood2Index = 0;
		m_Blood2List[m_iBlood2Index]->Set_Pos(vPos);
		m_Blood2List[m_iBlood2Index]->Set_Look(vDir);
		m_Blood2List[m_iBlood2Index++]->Active(true);
	}
	else if (2 == iRan)
	{
		if (m_iBlood3Index >= 3)
			m_iBlood3Index = 0;
		m_Blood3List[m_iBlood3Index]->Set_Pos(vPos);
		m_Blood3List[m_iBlood3Index]->Set_Look(vDir);
		m_Blood3List[m_iBlood3Index++]->Active(true);
	}
}

void CMonsterEPController::Show_DeadParticle(_fvector _vCenterPos)
{
	if (nullptr != m_pDeadOneParticle)
		m_pDeadOneParticle->Set_Pos(_vCenterPos);

	m_pDeadOneParticle->Active(true);
}

void CMonsterEPController::Stop_DeadParticle_Show()
{
	if (nullptr != m_pDeadOneParticle)
	{
		if (nullptr != m_pDeadOneParticle->Get_2DEffectParticle())
			m_pDeadOneParticle->Get_2DEffectParticle()->Set_Loop(true);

		if (nullptr != m_pDeadOneParticle->Get_Particle())
			m_pDeadOneParticle->Get_Particle()->Set_Loop(true);

		m_pDeadOneParticle->Turn_Off_LifeLoop();
	}
}

CMonsterEPController* CMonsterEPController::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CMonsterEPController* pInstance = new CMonsterEPController(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMonsterEPController::Create To Failed");
	}
	return pInstance;
}

void CMonsterEPController::Free()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		{
			pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, m_pDeadOneParticle);
		}
		for (_uint i = 0; i < 3; ++i)
		{
			pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, m_Blood1List[i]);
			pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, m_Blood2List[i]);
			pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, m_Blood3List[i]);

			Safe_Release(m_Blood1List[i]);
			Safe_Release(m_Blood2List[i]);
			Safe_Release(m_Blood3List[i]);
		}

		
	}
	END_SINGLETON(pGameInstance);
	Safe_Release(m_pDeadOneParticle);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
