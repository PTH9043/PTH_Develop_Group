#include "CMapTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "CGameObject_Client.h"
#include "CTerrain.h"
#include "CCubeObj.h"
#include "CCell.h"

CMapTool::CMapTool(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: CIMGUI(_pDevice, _pContext)
{
}

HRESULT CMapTool::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	// Name
	m_pName = "MapTool";
	m_bOpen = false;
	return S_OK;
}

HRESULT CMapTool::NativeConstruct_Tool()
{
	if (FAILED(__super::NativeConstruct_Tool()))
		return E_FAIL;

	for(_uint i = 0; i < SEL_END; ++i)
		m_pCubePosArr[i] = CCubeObj::Create(m_pDevice, m_pContext, XMLoadFloat3(&_float3(0.f, 0.f, 0.f)), i);
	return S_OK;
}

HRESULT CMapTool::Tick(const _double & _dDeltaSeconds)
{
	if (FAILED(__super::Tick(_dDeltaSeconds)))
		return E_FAIL;

	Pick_Terrain();
	return S_OK;
}

HRESULT CMapTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Show_Window();
#ifdef _DEBUG
	if (true == m_bSelEnd)
		for (_uint i = 0; i < m_iSelIndex; ++i)
			m_pCubePosArr[i]->Render();
#endif

	return S_OK;
}

HRESULT CMapTool::Release_Tool()
{
	for (_uint i = 0; i < SEL_END; ++i)
		Safe_Release(m_pCubePosArr[i]);
	Safe_Release(m_pDebugTexture);
	return S_OK;
}

void CMapTool::Reset()
{
	m_pSelectObject = nullptr;
	m_v3PickPos = _float3(0.f, 0.f, 0.f);
	m_v2Range = _float2(0.f, 0.f);
	// Select Mode
	m_bClicked = false;
	m_bModifyCubeMode = false;
	m_iSelectTextureIndex = 0;
	m_bModifyNavigation = false;
	m_v3ModifPosition = _float3(0.f, 0.f, 0.f);
	m_vecPosList.clear();
	m_iCellIndex = 0;
	m_iSelIndex = SEL_1;
	m_bSelEnd = false;
}

void CMapTool::Pick_Terrain()
{
	if (false == m_bOpen || true == m_bModifyNavigation)
		return;
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		m_bClicked = pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L);
		if (true == m_bClicked)
		{
			m_bClicked = true;
			BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
			{
				_float3 v3Pos;
				for (auto& iter : pGameInstance->Find_GameObjectList(LEVEL_TOOL, LAYER_TERRAIN))
				{
					static_cast<CGameObject_Client*>(iter)->Is_Picking(&v3Pos);
				}
			}
			END_SINGLETON(pImGuiManager);
			CPicking::PICKINGDESC* pPickDesc = pGameInstance->Get_Picking_GOB();
			if (nullptr != pPickDesc)
			{
				m_pSelectObject = static_cast<CGameObject_Client*>(pPickDesc->pGameObject);
				m_v3PickPos = pPickDesc->v3PickingPos;
			}
			Reset_Nav_Data();
		}
	}
	END_SINGLETON(pGameInstance);
}

void CMapTool::Show_Window()
{
	ImGui::Begin(m_pName, &m_bOpen);
	{
		if (true == m_bOpen)
		{
			CTerrain* pTerrain = static_cast<CTerrain*>(m_pSelectObject);
			if (nullptr != pTerrain)
			{
				ImGui::Checkbox("Navigation Modify", &m_bModifyNavigation);
#ifdef _DEBUG
				ImGui::Checkbox("ModifyCube", &m_bModifyCubeMode);
#endif
				if (true == m_bModifyNavigation)
				{
					m_bModifyCubeMode = false;
					Modfiy_Navigation(pTerrain);
					Show_Cell_Navigation(pTerrain);
				}
				else
				{
					if (false == m_bModifyCubeMode)
					{
						Select_Debug_Img();
						if (nullptr != m_pSelectObject)
						{
							if (nullptr != pTerrain)
							{
								ImGui::PushItemWidth(200);
								{
									ImGui::NewLine();
									_float Position[2] = { m_v3PickPos.x, m_v3PickPos.z };
									ImGui::InputFloat2("Position", Position, "%.1f");
									if (0 == m_iSelectTextureIndex)
									{
										ImGui::InputFloat("Range	", &m_v2Range.x, 0.f, 0.f, "%.1f");
										m_v2Range = _float2(m_v2Range.x, m_v2Range.x);
									}
									else if (1 == m_iSelectTextureIndex)
									{
										_float Range[2] = { m_v2Range.x, m_v2Range.y };
										ImGui::InputFloat2("Range	", Range, "%.1f");
										m_v2Range = _float2(Range[0], Range[1]);
									}

									m_v3PickPos = _float3(Position[0], 0.f, Position[1]);

									BEGIN_SINGLETON(CGameInstance, pGameInstance)
									{
										if (true == pGameInstance->Get_DIMBtnPressing(CInput_Device::DIMB_R))
										{
											pTerrain->Is_Picking(&m_v3PickPos);
											pGameInstance->Get_Picking_GOB();
										}
									}
									END_SINGLETON(pGameInstance);

									if (nullptr != m_pDebugTexture)
									{
										pTerrain->Setting_Modify_Value(m_pDebugTexture->Find_Texture(m_iSelectTextureIndex), m_v3PickPos, m_v2Range);
										if (ImGui::CollapsingHeader("Height_Modify", ImGuiTreeNodeFlags_OpenOnArrow))
										{
											Modify_Selected_Terrain(pTerrain);
											Reset_Selected_Terrain(pTerrain);
										}
									}
								}
								ImGui::PopItemWidth();
							}
						}
					}
					else
					{
#ifdef _DEBUG
						if (nullptr != m_pSelectObject)
						{
							if (nullptr != pTerrain)
							{
								pTerrain->Show_CubeObjects();
								pTerrain->Control_CubeObjsts();
								ImGui::InputFloat("Control Pos", &m_v3ModifPosition.y, 0.f, 0.f, "%.1f");
								if (ImGui::Button("Modify"))
								{
									pTerrain->Modfiy_TerrainHeight(m_v3ModifPosition.y);
								}
								if (ImGui::Button("Reset"))
								{
									pTerrain->Clear_TerrainHeight();
								}
							}
						}
#endif
					}
				}
			}
		}
	}
	ImGui::End();
}

void CMapTool::Modify_Selected_Terrain(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return;

	if (m_v3ModifPosition.y >= 25.f)
		m_v3ModifPosition.y = 25.f;

	ImGui::InputFloat("Control Pos", &m_v3ModifPosition.y, 0.f, 0.f, "%.1f");
	_float3 v3ModifyPosition = _float3(0.f, m_v3ModifPosition.y, 0.f);
	if (ImGui::Button("Modify"))
	{
		_pTerrain->Modify_Terrain(XMLoadFloat3(&v3ModifyPosition), m_iSelectTextureIndex);
	}

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		if (true == pGameInstance->Get_DIKeyDown(DIK_UP))
		{
			m_v3ModifPosition.y += 0.1f;
			_pTerrain->Modify_Terrain(XMLoadFloat3(&v3ModifyPosition), m_iSelectTextureIndex);
		}
		else if (true == pGameInstance->Get_DIKeyDown(DIK_DOWN))
		{
			m_v3ModifPosition.y -= 0.1f;
			_pTerrain->Modify_Terrain(XMLoadFloat3(&v3ModifyPosition), m_iSelectTextureIndex);
		}
	}
	END_SINGLETON(pGameInstance);
}

void CMapTool::Reset_Selected_Terrain(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return;

	if (ImGui::Button("Reset"))
	{
		_pTerrain->Reset_ToModify_Terrain();
	}
}

// Select Debug Image 
void CMapTool::Select_Debug_Img()
{
	if (nullptr == m_pDebugTexture)
		return;
	// Range Restrict
	if (0 > m_iSelectTextureIndex)
		m_iSelectTextureIndex = 0;
	if (m_iSelectTextureIndex >= m_pDebugTexture->Get_TextureSize())
		m_iSelectTextureIndex = m_pDebugTexture->Get_TextureSize() - 1;
	// Get Name
	string str;
	_wstring TextureStr = m_pDebugTexture->Get_TextureName(m_iSelectTextureIndex);
	str.assign(TextureStr.begin(), TextureStr.end());
	ImGui::Text(str.c_str());
	// ImGui Set Image
	ID3D11ShaderResourceView* pTexture = m_pDebugTexture->Find_Texture(m_iSelectTextureIndex);
	if (nullptr != pTexture)
	{
		ImGuiID iID = ImGui::GetID("Image");
		if (ImGui::BeginChildFrame(iID, ImVec2(50.f, 50.f), ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Image((void*)pTexture, ImVec2(40.f, 40.f));
			ImGui::EndChildFrame();
		}
	}
	ImGui::InputInt("Index", (int*)&m_iSelectTextureIndex);
}

void CMapTool::Modfiy_Navigation(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
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
					CNavigation* _pNav = _pTerrain->Get_Navigation();
					if (false == _pTerrain->Is_Picking(&v3Pos))
					{
						END_SINGLETON(pGameInstance);
						return;
					}
					else
					{
						vector<CCell*> Cells;
						if (nullptr != _pNav)
							Cells = *_pNav->Get_Cells();

						for (auto& iter : Cells)
						{
							for(_uint i = 0; i < CCell::POINT_END; ++i)
							{
								_float3 Point = iter->Get_PointV3((CCell::POINT)i);
								if (v3Pos.x >= Point.x - 0.5f && v3Pos.x <= Point.x + 0.5f)
								{
									if (v3Pos.z >= Point.z - 0.5f && v3Pos.z <= Point.z + 0.5f)
									{
										v3Pos = Point;
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
							_pTerrain->Add_Cell(CCell::Create(m_pDevice, m_pContext, vPos, ++m_iCellIndex));
							m_vecPosList.clear();
							m_iSelIndex = SEL_1;
						}
					}
				}
			}
		}
	}
	END_SINGLETON(pGameInstance);
}

void CMapTool::Show_Cell_Navigation(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return;
	CNavigation* _pNav = _pTerrain->Get_Navigation();
	if (nullptr == _pNav)
		return;

	vector<CCell*>* Cells = _pNav->Get_Cells();
	for (vector<CCell*>::iterator it = Cells->begin(); it != Cells->end(); ++it)
	{
		char TEXT[MAX_PATH] = {};
		sprintf_s(TEXT, "Cell: %d", (*it)->Get_Index());
		if (ImGui::TreeNodeEx(TEXT, ImGuiTreeNodeFlags_DefaultOpen))
		{
			{
				_float3 v3Pos = (*it)->Get_PointV3(CCell::POINT_A);
				sprintf_s(TEXT, "PA: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
				ImGui::Text(TEXT);
			}
			{
				_float3 v3Pos = (*it)->Get_PointV3(CCell::POINT_B);
				sprintf_s(TEXT, "PB: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
				ImGui::Text(TEXT);
			}
			{
				_float3 v3Pos = (*it)->Get_PointV3(CCell::POINT_C);
				sprintf_s(TEXT, "PC: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
				ImGui::Text(TEXT);
			}

			if (ImGui::Button("Delete"))
			{
				Safe_Release((*it));
				Cells->erase(it);
				break;
			}
			ImGui::TreePop();
		}
	}
}

void CMapTool::Reset_Nav_Data()
{
	m_iSelIndex = SEL_1;
	m_bSelEnd = false;
	m_iCellIndex = 0;
}

CMapTool * CMapTool::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
{
	CMapTool* pInstance = new CMapTool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMapTool::Create To Failed");
	}
	return pInstance;
}

void CMapTool::Free()
{
	for (_uint i = 0; i < SEL_END; ++i)
		Safe_Release(m_pCubePosArr[i]);
	Safe_Release(m_pDebugTexture);
	__super::Free();
}


/*
void CMapTool::Make_HeightTexture(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return;

	char szFile[MAX_PATH];
	strcpy_s(szFile, m_strHeightTextureName.c_str());
	ImGui::InputText("Name", szFile, sizeof(char) * MAX_PATH);
	m_strHeightTextureName = szFile;
	if (ImGui::Button("Create  Height"))
	{
		if (FAILED(Create_HeighTexture(_pTerrain)))
			return;
	}
}

void CMapTool::Make_FilterTexture(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return;

	char szFile[MAX_PATH];
	strcpy_s(szFile, m_strFilterName.c_str());
	ImGui::InputText("Name", szFile, sizeof(char) * MAX_PATH);
	m_strFilterName = szFile;

	if (ImGui::Button("Create  Height"))
	{
		string path = "..\\..\\Resource\\Game\\Terrain\\Textures\\Filter\\";
		path.append(m_strFilterName);
		path.append(".dds");
		if (FAILED(_pTerrain->Create_FilterTexture(path.c_str())))
			return;
	}
}

HRESULT CMapTool::Create_HeighTexture(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return E_FAIL;
	// Texture
	ID3D11Texture2D* pTexture2D = nullptr;
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	// SaveDesc
	CTerrain::TERRIANSAVEDESC tSaveDesc = _pTerrain->Get_TerrainDesc();
	// TextureDesc
	TextureDesc.Width = tSaveDesc.iNumVerticesX;
	TextureDesc.Height = tSaveDesc.iNumVerticesZ;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;
	// Pixel
	_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);
	for (_uint i = 0; i < TextureDesc.Width * TextureDesc.Height; ++i)
	{
		_float3 v3Pos = tSaveDesc.pVertices[i];
		pPixel[i] = D3DCOLOR_ARGB(255, 255, 255, 255);
	}
	// Sub Source Data
	D3D11_SUBRESOURCE_DATA			SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = (_uint)(sqrtf((_float)TextureDesc.Width * TextureDesc.Height)) * sizeof(_ulong);
	// Create Texture 2D
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture2D)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = m_pContext->Map(pTexture2D, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	if (FAILED(hr))
	{
		Safe_Release(pTexture2D);
		Safe_Delete_Array(pPixel);
		return E_FAIL;
	}

	_ulong		dwByte = 0;
	_ulong		iIndex = 0;

	_ulong* _pPixel = (_ulong*)MappedSubResource.pData;
	for (int i = tSaveDesc.iNumVerticesZ - 1; i >= 0; --i)
	{
		for (int j = tSaveDesc.iNumVerticesX - 1; j >= 0; --j)
		{
			_uint iIndex = i * tSaveDesc.iNumVerticesX + j;
			_uint iValue = (_uint)tSaveDesc.pVertices[iIndex].y;

			if (iValue >= 255)
				iValue = 255;
			else if (iValue > 0)
				int a = 0;

			((_ulong*)MappedSubResource.pData)[iIndex] = D3DCOLOR_ARGB(255, iValue, iValue, iValue);
		}
	}

	m_pContext->Unmap(pTexture2D, 0);
	// Path
	string path = "..\\..\\Resource\\Game\\Terrain\\Textures\\Height\\";
	path.append(m_strHeightTextureName);
	path.append(".bmp");
	// RePath
	wstring _RePath;
	_RePath.assign(path.begin(), path.end());
	// Save WICTextureToFile
	if (FAILED(DirectX:: SaveWICTextureToFile(m_pContext, pTexture2D, GUID_ContainerFormatBmp,_RePath.c_str())))
		return E_FAIL;
	// Safe
	Safe_Release(pTexture2D);
	Safe_Delete_Array(pPixel);
	tSaveDesc.strSaveHeightBmpPath = path;
	_pTerrain->Set_TerrainDesc(tSaveDesc);
	return S_OK;
}

HRESULT CMapTool::Modify_FilterTexture(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return E_FAIL;

	if (true == m_bClicked)
	{
		if (FAILED(_pTerrain->Modify_FilterTexture()))
			return E_FAIL;
	}
	return S_OK;
}
*/

/*
		if (pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X) || pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
					{
						if (true == _pTerrain->Is_Picking(&v3Pos))
						{
							CNavigation* _pNav = _pTerrain->Get_Navigation();
							if (nullptr != _pNav)
							{
								if (0 < (_uint)_pNav->Get_Cells().size())
								{
									vector<CCell*> Cells = _pNav->Get_Cells();
									vector<SELCELL> SelCell;
									for (_uint i = 0; i < m_iCellIndex; ++i)
									{
										_float3 vPosArr[CCell::POINT_END] =
										{
										 Cells[(_uint)(i)]->Get_PointV3(CCell::POINT_A),
										 Cells[(_uint)(i)]->Get_PointV3(CCell::POINT_B),
										 Cells[(_uint)(i)]->Get_PointV3(CCell::POINT_C)
										};

										_float fLength1 = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&v3Pos) - XMLoadFloat3(&vPosArr[CCell::POINT_A])));
										_float fLength2 = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&v3Pos) - XMLoadFloat3(&vPosArr[CCell::POINT_B])));
										_float fLength3 = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&v3Pos) - XMLoadFloat3(&vPosArr[CCell::POINT_C])));

										{
											auto iter = find_if(SelCell.begin(), SelCell.end(), [&](SELCELL p1)
											{
												if (p1.fLength == fLength1)
													return true;

												return false;
											});

											if(iter == SelCell.end())
												SelCell.push_back(SELCELL(fLength1, CCell::POINT_A, Cells[(_uint)(i)]->Get_PointV3(CCell::POINT_A)));
										}

										{
											auto iter = find_if(SelCell.begin(), SelCell.end(), [&](SELCELL p1)
											{
												if (p1.fLength == fLength2)
													return true;

												return false;
											});

											if (iter == SelCell.end())
												SelCell.push_back(SELCELL(fLength2, CCell::POINT_B, Cells[(_uint)(i)]->Get_PointV3(CCell::POINT_B)));
										}

										{
											auto iter = find_if(SelCell.begin(), SelCell.end(), [&](SELCELL p1)
											{
												if (p1.fLength == fLength3)
													return true;

												return false;
											});

											if (iter == SelCell.end())
												SelCell.push_back(SELCELL(fLength3, CCell::POINT_C, Cells[(_uint)(i)]->Get_PointV3(CCell::POINT_C)));
										}

										sort(SelCell.begin(), SelCell.end(), [&](SELCELL t1, SELCELL t2)
										{
											return t1.fLength < t2.fLength;
										});
									}
									if ((_uint)SelCell.size() >= 2)
									{
										m_pCubePosArr[SEL_1]->Set_Position(XMLoadFloat3(&SelCell[SEL_1].v3Pos));
										m_pCubePosArr[SEL_2]->Set_Position(XMLoadFloat3(&SelCell[SEL_2].v3Pos));
										m_vecPosList.clear();
										m_vecPosList.push_back(SelCell[SEL_1].v3Pos);
										m_vecPosList.push_back(SelCell[SEL_2].v3Pos);
									}
								}
							}
						}
					}
*/