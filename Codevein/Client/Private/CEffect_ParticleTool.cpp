#include "CEffect_ParticleTool.h"
#include "C2DEffectParticle.h"
#include "CGameInstance.h"
#include "CParticle.h"
#include "CEffectGroup.h"

CEffect_ParticleTool::CEffect_ParticleTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CIMGUI(_pDevice, _pContext)
{
}

HRESULT CEffect_ParticleTool::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_pName = "CEffect_ParticleTool";
	return S_OK;
}

HRESULT CEffect_ParticleTool::NativeConstruct_Tool()
{
	if (FAILED(__super::NativeConstruct_Tool()))
		return E_FAIL;

	if(nullptr == m_p2DEffectParticle)
	{
		m_p2DEffectParticle = C2DEffectParticle::Create(m_pDevice, m_pContext);
	}
	m_p2DEffectParticle->Set_Active(true);
	
	if (nullptr == m_pParticle)
	{
		m_pParticle = CParticle::Create(m_pDevice, m_pContext);
	}
	m_pParticle->Set_Active(true);

	if (nullptr == m_pEffect)
	{
		m_pEffect = CEffectGroup::Create(m_pDevice, m_pContext);
	}
	m_pEffect->Set_Active(true);

	if (nullptr == m_pTexture)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstnace)
		{
			m_pTexture = static_cast<CTexture*>(pGameInstnace->Clone_Component(LEVEL_STATIC, PROTO_COMP_TEXTURE_PARTICLEGROUP));
		}
		END_SINGLETON(pGameInstnace);
	}

	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		_matrix TransformationMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PATHS Path{ L"Game", L"Effect", L"FBX_Converter" };
		FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);

		for (auto& iter : pFileGroup->vecFileDataList)
		{
			CModel* pModel = CModel::Create(m_pDevice, m_pContext,
				CModel::TYPE_NONANIM, pFileGroup, &iter, XMMatrixScaling(0.01f, 0.01f, 0.01f));

			m_ModelList.push_back(pModel);
		}
	}
	END_SINGLETON(pGameInstance);


	return S_OK;
}

HRESULT CEffect_ParticleTool::Tick(const _double& _dDeltaSeconds)
{
	if (FAILED(__super::Tick(_dDeltaSeconds)))
		return E_FAIL;

	if (true == m_bAllRender)
	{

		if (nullptr != m_p2DEffectParticle)
		{
			m_p2DEffectParticle->Tick(_dDeltaSeconds);
			m_p2DEffectParticle->Late_Tick(_dDeltaSeconds);
		}
		if (nullptr != m_pParticle)
		{
			m_pParticle->Tick(_dDeltaSeconds);
			m_pParticle->Late_Tick(_dDeltaSeconds);
		}
		if (nullptr != m_pEffect)
		{
			m_pEffect->Tick(_dDeltaSeconds);
			m_pEffect->Late_Tick(_dDeltaSeconds);
		}
	}
	else
	{
		if (true == m_bRender_2DEffect)
		{
			if (nullptr != m_p2DEffectParticle)
			{
				m_p2DEffectParticle->Tick(_dDeltaSeconds);
				m_p2DEffectParticle->Late_Tick(_dDeltaSeconds);
			}
		}
		if (true == m_bRender_Particle)
		{
			if (nullptr != m_pParticle)
			{
				m_pParticle->Tick(_dDeltaSeconds);
				m_pParticle->Late_Tick(_dDeltaSeconds);
			}
		}
		if (true == m_bRender_Effect)
		{
			if (nullptr != m_pEffect)
			{
				m_pEffect->Tick(_dDeltaSeconds);
				m_pEffect->Late_Tick(_dDeltaSeconds);
			}
		}
	}
	return S_OK;
}

HRESULT CEffect_ParticleTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Show_Window();
	return S_OK;
}

HRESULT CEffect_ParticleTool::Release_Tool()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_p2DEffectParticle);
	Safe_Release(m_pParticle);
	for (auto& iter : m_ModelList)
		Safe_Release(iter);
	return S_OK;
}

void CEffect_ParticleTool::Show_Window()
{
	if (nullptr == m_p2DEffectParticle)
		return;
	ImVec2 InitPos = ImVec2{ static_cast<_float>(g_iWinSizeX + 50),  50.f };
	ImGui::SetNextWindowPos(InitPos, ImGuiCond_Appearing);
//	ImGui::SetNextWindowSize(ImVec2(100.f, 100.f), ImGuiCond_Appearing);
	ImGui::Begin(m_pName, &m_bOpen);
	{

		ImGui::Checkbox("All_Render", &m_bAllRender);
		ImGui::Checkbox("Render_2DEffect", &m_bRender_2DEffect);
		ImGui::Checkbox("Render_Particle", &m_bRender_Particle); 
		ImGui::Checkbox("Render_Effect", &m_bRender_Effect);

		ImGui::Checkbox("Active2DEffectParitlceTool", &m_bActive2DEffectParitlceTool);
		ImGui::Checkbox("ActiveParitlceTool", &m_bActivePaticleTool);
		ImGui::Checkbox("ActiveEffectTool", &m_bActiveEffectTool);
		
		_float fArr[] = {
m_fRanRandomForceMultiply , m_fRanRandomAccelerationMultiply ,
 m_fRanJumpValueMultiply ,
 m_fRanJumpTimeMultiply ,
 m_fRanJumpTimeFastMultiply,
 m_fRanParticleColorMultiply,
 m_fRanBlendColorMultiply ,
 m_fRanBlendSpeedMultiply,
 m_fRanRotSpeedMultiply ,
 m_fRanvDirectionMultiply ,
 m_fRanSizeMultiply ,
 m_fRanInitPosMultiplyX ,
 m_fRanLifeTime,
 m_fRanDirectionMultiplyY,
 m_fRanDirecitonMultiplyZ,
 m_fRanInitPosMultiplyY,
 m_fRanInitPosMultiplyZ,
		};
		
		if (false == m_bActiveEffectTool)
		{
			if (ImGui::CollapsingHeader("RandomMultipleValue"))
			{
				ImGui::DragFloat("RanRandomForceMultiply", &m_fRanRandomForceMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanRandomAccelerationMultiply", &m_fRanRandomAccelerationMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanJumpValueMultiply", &m_fRanJumpValueMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanJumpTimeMultiply", &m_fRanJumpTimeMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanJumpTimeFastMultiply", &m_fRanJumpTimeFastMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanParticleColorMultiply", &m_fRanParticleColorMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanBlendSpeedMultiply", &m_fRanBlendSpeedMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanRotSpeedMultiply", &m_fRanRotSpeedMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanvDirectionMultiplyX", &m_fRanvDirectionMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanvDirectionMultiplyY", &m_fRanDirectionMultiplyY, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanvDirectionMultiplyZ", &m_fRanDirecitonMultiplyZ, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanSizeMultiply", &m_fRanSizeMultiply, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("fRanInitPosMultiplyX", &m_fRanInitPosMultiplyX, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("fRanInitPosMultiplyY", &m_fRanInitPosMultiplyY, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("fRanInitPosMultiplyZ", &m_fRanInitPosMultiplyZ, 0.001f, -100000.f, 100000.f);
				ImGui::DragFloat("RanLifeTime", &m_fRanLifeTime, 0.001f, -100000.f, 100000.f);
			}
		}
		if (true == m_bActive2DEffectParitlceTool && false == m_bActivePaticleTool && false == m_bActiveEffectTool)
		{
			m_p2DEffectParticle->Modify_Value(m_pTexture, &m_iSelectIndex, &m_iSelectInstanceIndex, &m_v3Pos, fArr,
				&m_iCreateCount, &m_tParticleEffectDesc);
		
			if (ImGui::Button("Reset Effect"))
			{
				Safe_Release(m_p2DEffectParticle);
				m_p2DEffectParticle = C2DEffectParticle::Create(m_pDevice, m_pContext);
				m_p2DEffectParticle->Set_Active(true);
			}
			ImGui::InputText("LoadName", m_p2DEffectLoadName, sizeof(_char) * MAX_PATH);
			if (ImGui::Button("Load"))
			{
				string str = m_p2DEffectLoadName;
				_wstring Convert;
				Convert.assign(str.begin(), str.end());

				_wstring strFilePath = L"..\\..\\Resource\\Json\\2DEffect\\";
				strFilePath.append(Convert);
				strFilePath.append(L".bin");
				
				Safe_Release(m_p2DEffectParticle);
				m_p2DEffectParticle = C2DEffectParticle::Create_LoadFile(m_pDevice, m_pContext, strFilePath);
			}
		}
		else if (false == m_bActive2DEffectParitlceTool && true == m_bActivePaticleTool && false == m_bActiveEffectTool)
		{
			m_pParticle->Modify_Value(m_pTexture, &m_iSelectIndex, &m_iSelectInstanceIndex, &m_v3Pos, fArr,
				&m_iCreateCount, &m_tParticleEffectDesc);

			if (ImGui::Button("Reset Effect"))
			{
				Safe_Release(m_pParticle);
				m_pParticle = CParticle::Create(m_pDevice, m_pContext);
			}
			ImGui::InputText("LoadName", m_pParticleLoadName, sizeof(_char) * MAX_PATH);
			if (ImGui::Button("Load"))
			{
				string str = m_pParticleLoadName;
				_wstring Convert;
				Convert.assign(str.begin(), str.end());


				_wstring strFilePath = L"..\\..\\Resource\\Json\\Particle\\";
				strFilePath.append(Convert);
				strFilePath.append(L".bin");
				Safe_Release(m_pParticle);
				m_pParticle = CParticle::Create_LoadFile(m_pDevice, m_pContext, strFilePath);
				m_pParticle->Set_Active(true);
			}
		}
		else if (false == m_bActive2DEffectParitlceTool && false == m_bActivePaticleTool && true == m_bActiveEffectTool)
		{
			if(nullptr != m_pEffect)
				m_pEffect->Modify_Value(&m_ModelList, &m_iSelectInstanceindex_EffectTool, &m_iSelectIndex_EffectTool, m_pParticle);

			if (ImGui::Button("Reset Effect"))
			{
				Safe_Release(m_pEffect);
				m_pEffect = CEffectGroup::Create(m_pDevice, m_pContext);
			}
			ImGui::InputText("LoadName", m_pEffectLoadName, sizeof(_char) * MAX_PATH);
			if (ImGui::Button("Load"))
			{
				string str = m_pEffectLoadName;
				_wstring Convert;
				Convert.assign(str.begin(), str.end());
				_wstring strFilePath = L"..\\..\\Resource\\Json\\Effect\\";
				strFilePath.append(Convert);
				strFilePath.append(L".bin");
				Safe_Release(m_pEffect);
				m_pEffect = CEffectGroup::Create_LoadFile(m_pDevice, m_pContext, strFilePath);
			}
		}
		if (false == m_bActiveEffectTool)
		{
			if (ImGui::Button("Reset SetValue"))
			{
				m_tParticleEffectDesc = EFFECTPARTICLEDESC();
			}
		}
	}
	ImGui::End();
}

CEffect_ParticleTool* CEffect_ParticleTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect_ParticleTool* pInstance = new CEffect_ParticleTool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectTool::Create To Failed");
	}
	return pInstance;
}

void CEffect_ParticleTool::Free()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pParticle);
	Safe_Release(m_p2DEffectParticle);
	Safe_Release(m_pEffect);
	for (auto& iter : m_ModelList)
		Safe_Release(iter);
	__super::Free(); 
}
