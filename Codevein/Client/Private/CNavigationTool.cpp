#include "CNavigationTool.h"
#include "CData_Manager.h"
#include "CCubeObj.h"
#include "CGameInstance.h"
#include "CCell.h"

CNavigationTool::CNavigationTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CIMGUI(_pDevice, _pContext)
{
}

HRESULT CNavigationTool::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    // Name
    m_pName = "NavigationTool";
    m_bOpen = false;
    return S_OK;
}

HRESULT CNavigationTool::NativeConstruct_Tool()
{
    if (FAILED(__super::NativeConstruct_Tool()))
        return E_FAIL;

	BEGIN_SINGLETON(CData_Manager, pInstance);
	{
		m_pStageManager = pInstance->Get_StageMgr();
		Safe_AddRef(m_pStageManager);
	}
	END_SINGLETON(pInstance);

    for (_uint i = 0; i < SEL_END; ++i)
        m_pCubePosArr[i] = CCubeObj::Create(m_pDevice, m_pContext, XMLoadFloat3(&_float3(0.f, 0.f, 0.f)), i);

	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		m_pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_RENDERER));
	}
	END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CNavigationTool::Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;

	if (nullptr != m_pStageManager->Get_Stage())
		m_pStageManager->Get_Stage()->Update_Region();

#ifdef _DEBUG
	if (false == m_bAllRender)
	{
		if (nullptr != m_pStageManager->Get_Stage())
			m_pStageManager->Get_Stage()->Render(m_iRegionIndex);
	}
	else
	{
		if (nullptr != m_pStageManager->Get_Stage())
			m_pStageManager->Get_Stage()->All_Render();
	}

	if (true == m_bSelEnd)
		for (_uint i = 0; i < m_iSelIndex; ++i)
			m_pCubePosArr[i]->Add_Render(m_pRenderer);

#endif
    return S_OK;
}

HRESULT CNavigationTool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

HRESULT CNavigationTool::Release_Tool()
{
    for (_uint i = 0; i < SEL_END; ++i)
        Safe_Release(m_pCubePosArr[i]);

	Safe_Release(m_pStageManager);
	Safe_Release(m_pRenderer);
    return S_OK;
}

void CNavigationTool::Show_Window()
{
    ImGui::Begin(m_pName, &m_bOpen);
    {
        ImGui::Checkbox("Navigation Modify", &m_bNavigationModify);
        if (true == m_bNavigationModify)
        {
			ImGui::Checkbox("Navigation_WireFrame", &m_bRenderWireFrame);
			ImGui::Checkbox("Navigation_DrawNav", &m_bNavigationDebugColor);
			ImGui::Checkbox("Navigation_AllRender", &m_bAllRender);
			ImGui::InputInt("Region Index", (_int*)&m_iCreateRegionIndex);
			m_pStageManager->Get_Stage()->Create_Region(m_iCreateRegionIndex);

			if (m_pStageManager->Get_Stage()->Get_RegionList().size() > 0)
			{
				_uint iSelect = m_pStageManager->Get_Stage()->Select_Region();
				if (INVALID_MINUS_STAGEVALUE != iSelect)
					m_iRegionIndex = iSelect;
				m_pStageManager->Get_Stage()->Delete_Region(m_iRegionIndex);
				Modfiy_Navigation(m_pStageManager->Get_Stage()->Get_Region(m_iRegionIndex));
				m_pStageManager->Get_Stage()->Control_Collider(m_iRegionIndex);
				m_pStageManager->Get_Stage()->Modify_Cells(m_iRegionIndex);
				m_pStageManager->Get_Stage()->Show_Cells(m_iRegionIndex);
			}
        }
    }
	ImGui::End();
}

void CNavigationTool::Modfiy_Navigation(CRegion* _pStage)
{
    if (nullptr == _pStage)
        return;

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		_float2 MousePos = pGameInstance->Get_MousePosition();
		if (MousePos.x > 0 && MousePos.x < g_iWinSizeX)
		{
			if (MousePos.y > 0 && MousePos.y < g_iWinSizeY)
			{
				_float3 v3Pos;
				if (true == pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
				{
					CNavigation* _pNav = _pStage->Get_Navigation();
					m_pStageManager->Get_Stage()->Is_Picking(m_iRegionIndex);

					CPicking::PICKINGDESC* tDesc = pGameInstance->Get_Picking_GOB();
					if (nullptr == tDesc)
					{
						END_SINGLETON(pGameInstance);
						return;
					}
					if (nullptr == tDesc->pGameObject)
					{
						END_SINGLETON(pGameInstance);
						return;
					}

					v3Pos = tDesc->v3PickingPos;
					vector<CCell*> Cells;
					if (nullptr != _pNav)
						Cells = *_pNav->Get_Cells();

					for (auto& iter : Cells)
					{
						for (_uint i = 0; i < CCell::POINT_END; ++i)
						{
							_float3 Point = iter->Get_PointV3((CCell::POINT)i);

							if (v3Pos.y >= Point.y - 0.1f && v3Pos.y <= Point.y + 0.1f)
							{
								if (v3Pos.x >= Point.x - 0.5f && v3Pos.x <= Point.x + 0.5f)
								{
									if (v3Pos.z >= Point.z - 0.5f && v3Pos.z <= Point.z + 0.5f)
									{
										v3Pos = Point;
									}
								}
							}
						}
					}

					m_vecPosList.push_back(v3Pos);

					if (2 >= (_uint)m_vecPosList.size())
					{
						m_pCubePosArr[m_iSelIndex++]->Set_Position(XMLoadFloat3(&v3Pos));
						m_bSelEnd = true;
					}
					else
					{
						_float3 vPos[3] = { m_vecPosList[SEL_1], m_vecPosList[SEL_2], m_vecPosList[SEL_3] };
						_pStage->Add_Cell(CCell::Create(m_pDevice, m_pContext, vPos, ++m_iCellIndex));
						m_vecPosList.clear();
						m_iSelIndex = SEL_1;
					}
                }
			}
		}
	}
	END_SINGLETON(pGameInstance);
}

void CNavigationTool::Reset_Nav_Data(CRegion* _pStage)
{
	if (nullptr == _pStage)
		return;

}

CNavigationTool* CNavigationTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNavigationTool* pInstance = new CNavigationTool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CNavigationTool::Create To Failed");
    }
    return pInstance;
}

void CNavigationTool::Free()
{
	for (_uint i = 0; i < SEL_END; ++i)
		Safe_Release(m_pCubePosArr[i]);


    Safe_Release(m_pStageManager);
	Safe_Release(m_pRenderer);
    __super::Free();
}
