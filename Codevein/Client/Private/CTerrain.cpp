#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CTerrain.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CCubeObj.h"

CTerrain::CTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject_Client(_pDevice, _pContext)
{
}

CTerrain::CTerrain(const CTerrain& _rhs)
    : CGameObject_Client(_rhs)
{
}

void CTerrain::Push_TerrainObjects(CGameObject* _pObjects)
{
    if (nullptr == _pObjects)
        return;

    auto iter = find_if(m_TerrainObjects.begin(), m_TerrainObjects.end(), [&](auto& value)
    {
        if (value == _pObjects)
            return true;

        return false;
    });

    if (iter != m_TerrainObjects.end())
        return;

    m_TerrainObjects.push_back(_pObjects);
}

HRESULT CTerrain::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        TERRIANSAVEDESC tData = *static_cast<TERRIANSAVEDESC*>(pArg);

        m_pVertices = tData.pVertices;
        m_tTerraInDesc.iNumVerticesX = tData.iNumVerticesX;
        m_tTerraInDesc.iNumVerticesZ = tData.iNumVerticesZ;
        m_tGobDesc = tData.tGobData;
        m_iTerrainIndex = tData.iNavIndex;
        m_pNavigation = CNavigation::Create(m_pDevice, m_pContext, tData.strPath.c_str());
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Reset_TransformData()))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(Ready_CubeObjects()))
        return E_FAIL;
#endif

    return S_OK;
}

void CTerrain::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);

}

void CTerrain::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
    {
        if (true == pImGuiManager->Is_RenderingTerrain())
        {
            if (nullptr != m_pRenderCom)
                m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
        }
    }
    END_SINGLETON(pImGuiManager);
}

HRESULT CTerrain::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    m_pShaderCom->Begin(1);
    m_pVITerrainCom->Render();

#ifdef _DEBUG
    if (true == m_bShow)
    {
        for (auto& iter : m_CubeObjs)
            iter->Render();
    }
#endif

#ifdef _DEBUG
    if (nullptr != m_pNavigation)
        m_pNavigation->Render();
#endif
    return S_OK;
}

HRESULT CTerrain::Ready_Component()
{
    LEVEL eLevel = LEVEL_END;
    // Get Level 
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        eLevel = (LEVEL)pGameInstance->Get_CurLevel();
    }
    END_SINGLETON(pGameInstance);

    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXNORTEX, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
        return E_FAIL;

    CVIBuffer_Terrain::TERRAINDESC tTerrainDesc;
    tTerrainDesc.iNumVerticiesX = m_tTerraInDesc.iNumVerticesX;
    tTerrainDesc.iNumVerticiesZ = m_tTerraInDesc.iNumVerticesZ;

    m_pVITerrainCom = CVIBuffer_Terrain::Create_ToLoadData(m_pDevice, m_pContext, tTerrainDesc, m_pVertices);
    if (nullptr == m_pVITerrainCom)
        return E_FAIL;

    if (nullptr == m_pVITerrainCom)
        return E_FAIL;

    m_Components.insert(pair<const _tchar*, CComponent*>(COM_VIBUFFER, m_pVITerrainCom));
    Safe_AddRef(m_pVITerrainCom);
    return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    const LIGHTDESC* pLightDesc = nullptr;

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        pLightDesc = pGameInstance->Get_LightDesc();
    }
    END_SINGLETON(pGameInstance);
    {
        // Texture Mapping And Matrix Calculate
        {
            if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW),
                sizeof(_float4x4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ),
                sizeof(_float4x4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Set_RawValue("g_UvValue", &(m_fUvValue), sizeof(_float))))
                return E_FAIL;

        }
        // Light
        {
            if (nullptr != pLightDesc)
            {
                if (pLightDesc->eLightType == tagLightDesc::TYPE_DIRECTIONAL)
                {
                    if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->v4Direction, sizeof(_float3))))
                        return E_FAIL;
                }

                if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->v4Diffuse, sizeof(_float4))))
                    return E_FAIL;
                if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->v4Ambient, sizeof(_float4))))
                    return E_FAIL;
                if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->v4Specular, sizeof(_float4))))
                    return E_FAIL;
                if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float3))))
                    return E_FAIL;
            }
        }
        // Modify Value
        {
            if (nullptr != m_tModifyDesc.pModifyTexture)
            {
                if (FAILED(m_pShaderCom->Set_SRV("g_BrushTexture", m_tModifyDesc.pModifyTexture)))
                    return E_FAIL;

                if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &m_tModifyDesc.v3Pos, sizeof(_float3))))
                    return E_FAIL;

                _float2 v2Radius = m_tModifyDesc.v2Radius;
                if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushSize", &m_tModifyDesc.v2Radius, sizeof(_float2))))
                    return E_FAIL;
            }
        }
    }

    return S_OK;
}

#pragma region CLIENT_TOOL

_bool CTerrain::Extern_EditValue()
{
    BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
    {
        _bool bActive = false;
        if (ImGui::TreeNodeEx("Terrain"))
        {
            ImGui::PushItemWidth(200);
            Extern_GobData();
            Reset_Data();
            ImGui::InputInt("TerrianX", (int*)&m_tTerraInDesc.iNumVerticesX);
            ImGui::InputInt("TerrianZ", (int*)&m_tTerraInDesc.iNumVerticesZ);
            bActive = ImGui::Button("Create");
            ImGui::PopItemWidth();
            ImGui::TreePop();
            END_SINGLETON(pImGuiManager);
            return bActive;
        }
    }
    END_SINGLETON(pImGuiManager);
    return false;
}

_bool CTerrain::Extern_HierakeyView(const string& _str)
{
    if (nullptr == m_pTransformCom)
        return false;
    BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
    {
        if (true == pImGuiManager->Is_RenderingTerrain())
        {
            _bool bActive = false;
            if (ImGui::TreeNodeEx(_str.c_str()))
            {
                ImGui::PushItemWidth(200);
                Extern_GobData();
                Reset_Data();
                bActive = ImGui::Button("Delete");
                ImGui::TreePop();
                END_SINGLETON(pImGuiManager);
                return bActive;
            }
        }
    }
    END_SINGLETON(pImGuiManager);
    return false;
}

HRESULT CTerrain::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        TERRIANSAVEDESC tData;
        tData.iNumVertices = m_tTerraInDesc.iNumVerticesX * m_tTerraInDesc.iNumVerticesZ;
        tData.iNumVerticesX = m_tTerraInDesc.iNumVerticesX;
        tData.iNumVerticesZ = m_tTerraInDesc.iNumVerticesZ;
        tData.pVertices = m_pVertices;
        tData.tGobData = m_tGobDesc;
        tData.iNavIndex = m_iTerrainIndex++;
        if (nullptr != m_pNavigation)
        {
            tData.pNavigation = m_pNavigation;
            m_pNavigation->Set_CurIndex(m_iTerrainIndex);
        }
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CTerrain::Delete_Self()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        pGameInstance->Add_DeleteObject(LEVEL_TOOL, Get_Layer(), this);
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CTerrain::Save_Data()
{
    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        TERRIANSAVEDESC tData;
        tData.iNumVertices = m_tTerraInDesc.iNumVerticesX * m_tTerraInDesc.iNumVerticesZ;
        tData.iNumVerticesX = m_tTerraInDesc.iNumVerticesX;
        tData.iNumVerticesZ = m_tTerraInDesc.iNumVerticesZ;
        tData.pVertices = m_pVITerrainCom->Get_VerticesPos();
        tData.tGobData = m_tGobDesc;
        tData.iNavIndex = m_iTerrainIndex;
        tData.pNavigation = m_pNavigation;
        // Save 
        pDataManager->Get_WorldMgr()->Insert_TerrainData(tData);
    }
    END_SINGLETON(pDataManager);
    return S_OK;
}

HRESULT CTerrain::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
            , Get_Proto(), _pData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CTerrain::Is_Picking(_float3* _pOut)
{
    _bool bIsPicking = false;
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        bIsPicking = pGameInstance->Picking_Mesh(this, m_pTransformCom, m_pVITerrainCom, _pOut);
    }
    END_SINGLETON(pGameInstance);
    return bIsPicking;
}

void CTerrain::Reset_Data()
{
    __super::Reset_Data();
}

#pragma endregion CLIENT_TOOL

#pragma region EXTERN_MAPTOOL

HRESULT CTerrain::Setting_Modify_Value(ID3D11ShaderResourceView* _pDubgTexture, const _float3& _v3Pos, const _float2& _v2Radius)
{
    if (nullptr == _pDubgTexture && nullptr == m_pVITerrainCom)
        return E_FAIL;

    m_tModifyDesc.pModifyTexture = _pDubgTexture;
    m_tModifyDesc.v3Pos = _v3Pos;
    m_tModifyDesc.v2Radius = _v2Radius;
    XMStoreFloat2(&m_tModifyDesc.v2Radius, XMLoadFloat2(&m_tModifyDesc.v2Radius) / 2.f);
    return S_OK;
}

HRESULT CTerrain::Modify_Terrain(const _fvector& _v3Pos, const _uint& _iJudge)
{
    if (nullptr == m_pVITerrainCom || nullptr == m_pTransformCom)
        return E_FAIL;

    if (nullptr == m_pVITerrainCom->Get_VB())
        return E_FAIL;

    _vector vYPos = _v3Pos;
    //  vYPos /= 10.f;

    D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
    ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    HRESULT		hr = 0;

    ID3D11Buffer* pVB = m_pVITerrainCom->Get_VB();
    hr = m_pContext->Map(pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

    // Check E_FAIL
    if (FAILED(hr))
        return E_FAIL;
    // Get Vertices
    VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(MappedSubResource.pData);
    if (nullptr == pVertices)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }

    _float3 v3ModifyPos = m_tModifyDesc.v3Pos;
    _float2 v2Radius = m_tModifyDesc.v2Radius;
    if (0 == _iJudge)
    {
        for (_uint i = 0; i < m_pVITerrainCom->Get_NumVertices(); ++i)
        {
            _float3 v3VerticesPos = m_pVITerrainCom->Get_VerticesPos()[i];
            _vector vPos = XMVector3TransformCoord(XMLoadFloat3(&v3VerticesPos), m_pTransformCom->Get_WorldMatrix());

            _float fRadius = XMVectorGetX(XMVector3Length(vPos - XMLoadFloat3(&v3ModifyPos)));
            if (fRadius < v2Radius.x)
            {
                XMStoreFloat3(&v3VerticesPos, XMLoadFloat3(&v3VerticesPos) + vYPos);
                pVertices[i].v3Position = v3VerticesPos;
                m_pVITerrainCom->Get_VerticesPos()[i] = pVertices[i].v3Position;
            }
        }
    }
    else if (1 == _iJudge)
    {
        for (_uint i = 0; i < m_pVITerrainCom->Get_NumVertices(); ++i)
        {
            _float3 v3VerticesPos = m_pVITerrainCom->Get_VerticesPos()[i];
            _vector vPos = XMVector3TransformCoord(XMLoadFloat3(&v3VerticesPos), m_pTransformCom->Get_WorldMatrix());
            XMStoreFloat3(&v3VerticesPos, vPos);
            if (v3ModifyPos.x - v2Radius.x <= v3VerticesPos.x && v3VerticesPos.x <= v3ModifyPos.x + v2Radius.x &&
                v3ModifyPos.z - v2Radius.y <= v3VerticesPos.z && v3VerticesPos.z <= v3ModifyPos.z + v2Radius.y)
            {
                XMStoreFloat3(&v3VerticesPos, XMLoadFloat3(&m_pVITerrainCom->Get_VerticesPos()[i]) + vYPos);
                pVertices[i].v3Position = v3VerticesPos;
                m_pVITerrainCom->Get_VerticesPos()[i] = pVertices[i].v3Position;
            }
        }
    }

    void* pData = m_pVITerrainCom->ReCalculate_Value(pVertices);
    if (nullptr == pData)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }
    pVertices = static_cast<VTXNORTEX*>(pData);

    m_pVertices = m_pVITerrainCom->Get_VerticesPos();
    m_pContext->Unmap(pVB, 0);
    return S_OK;
}

HRESULT CTerrain::Reset_ToModify_Terrain()
{
    if (nullptr == m_pVITerrainCom || nullptr == m_pTransformCom)
        return E_FAIL;

    if (nullptr == m_pVITerrainCom->Get_VB())
        return E_FAIL;

    D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
    ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    HRESULT		hr = 0;

    ID3D11Buffer* pVB = m_pVITerrainCom->Get_VB();
    hr = m_pContext->Map(pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);
    // Check E_FAIL
    if (FAILED(hr))
        return E_FAIL;
    // Get Vertices
    VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(MappedSubResource.pData);
    if (nullptr == pVertices)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }

    for (_uint i = 0; i < m_tTerraInDesc.iNumVerticesZ; ++i)
    {
        for (_uint j = 0; j < m_tTerraInDesc.iNumVerticesX; ++j)
        {
            _uint		iIndex = i * m_tTerraInDesc.iNumVerticesX + j;
            pVertices[iIndex].v3Position = _float3(m_pVITerrainCom->Get_VerticesPos()[iIndex].x, 0.f, m_pVITerrainCom->Get_VerticesPos()[iIndex].z);
            m_pVITerrainCom->Get_VerticesPos()[iIndex] = pVertices[iIndex].v3Position;
        }
    }

    void* pData = m_pVITerrainCom->ReCalculate_Value(pVertices);
    if (nullptr == pData)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }
    pVertices = static_cast<VTXNORTEX*>(pVertices);

    m_pVertices = m_pVITerrainCom->Get_VerticesPos();
    m_pContext->Unmap(pVB, 0);
    return S_OK;
}

HRESULT CTerrain::Add_Cell(CCell* _pCell)
{
    if (nullptr == m_pNavigation)
        m_pNavigation = CNavigation::Create(m_pDevice, m_pContext);

    m_pNavigation->Add_Cells(_pCell);
    return S_OK;
}

HRESULT CTerrain::Delete_Cell(const _uint& _iIndex)
{
    if (nullptr == m_pNavigation)
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Clear_Cell()
{
    return E_NOTIMPL;
}

#ifdef _DEBUG
HRESULT CTerrain::Ready_CubeObjects()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        for (_uint i = 0; i < m_tTerraInDesc.iNumVerticesZ * m_tTerraInDesc.iNumVerticesX; ++i)
        {
            _vector Pos = XMVector3TransformCoord(XMLoadFloat3(&m_pVITerrainCom->Get_VerticesPos()[i]),
                m_pTransformCom->Get_WorldMatrix());

            CCubeObj* pObjects = CCubeObj::Create(m_pDevice, m_pContext, Pos, i);
            m_CubeObjs.push_back(pObjects);
        }
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}
HRESULT CTerrain::Show_CubeObjects()
{
    ImGui::Checkbox("Active", &m_bShow);

    if (true == m_bShow)
    {
        for (auto& iter : m_CubeObjs)
            iter->Set_Position(XMLoadFloat3(&m_pVITerrainCom->Get_VerticesPos()[iter->Get_Index()]));
    }

    if (ImGui::Button("Selected_Reset"))
    {
        for (auto& iter : m_SelectCubeObjs)
            iter->Set_Color(XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 1.f)));
        m_SelectCubeObjs.clear();
    }
    return S_OK;
}

HRESULT CTerrain::Control_CubeObjsts()
{
    if (true == m_bShow)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance);
        {
            if (true == pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_R))
            {
                BEGIN_SINGLETON(CGameInstance, pGameInstance);
                _vector vCamPos = pGameInstance->Get_CamPosition();
                END_SINGLETON(pGameInstance);
                CCubeObj* _pObj = nullptr;
                list<MODIFYHEIGHTDESC> m_lsList;
                for (auto& iter : m_CubeObjs)
                {
                    Safe_AddRef(iter);
                    _float3 v3Pos;
                    if (true == iter->Is_Picking(&v3Pos))
                    {
                        _float fLength = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&v3Pos) - vCamPos));
                        m_lsList.push_back(MODIFYHEIGHTDESC(iter, fLength));
                    }
                    Safe_Release(iter);
                }

                m_lsList.sort([&](MODIFYHEIGHTDESC t1, MODIFYHEIGHTDESC t2)
                {
                    return t1.fLenght < t2.fLenght;
                });

                _pObj = m_lsList.front().pObj;

                for (list<CCubeObj*>::iterator it = m_SelectCubeObjs.begin(); it != m_SelectCubeObjs.end();)
                {
                    Safe_AddRef(*it);
                    if ((*it) == _pObj)
                    {
                        _pObj->Set_Color(XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 1.f)));
                        Safe_Release(*it);
                        m_SelectCubeObjs.erase(it);
                        return E_FAIL;
                    }
                    else
                    {
                        Safe_Release(*it);
                        ++it;
                    }
                }

                _pObj->Set_Color(XMLoadFloat4(&_float4(0.f, 0.f, 1.f, 1.f)));
                m_SelectCubeObjs.push_back(_pObj);
            }
        }
        END_SINGLETON(pGameInstance);
    }
    return S_OK;
}

HRESULT CTerrain::Modfiy_TerrainHeight(const _float& _yPos)
{
    if (m_SelectCubeObjs.size() <= 0)
        return E_FAIL;

    D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
    ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    HRESULT		hr = 0;

    ID3D11Buffer* pVB = m_pVITerrainCom->Get_VB();
    hr = m_pContext->Map(pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

    // Check E_FAIL
    if (FAILED(hr))
        return E_FAIL;

    // Get Vertices
    VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(MappedSubResource.pData);
    if (nullptr == pVertices)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }

    for (auto& iter : m_SelectCubeObjs)
    {
        _float3 v3VerticesPos = m_pVITerrainCom->Get_VerticesPos()[iter->Get_Index()];
        pVertices[iter->Get_Index()].v3Position = _float3(v3VerticesPos.x, _yPos, v3VerticesPos.z);
        m_pVITerrainCom->Get_VerticesPos()[iter->Get_Index()] = pVertices[iter->Get_Index()].v3Position;
        iter->Set_Position(XMLoadFloat3(&pVertices[iter->Get_Index()].v3Position));
    }

    void* pData = m_pVITerrainCom->ReCalculate_Value(pVertices);
    if (nullptr == pData)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }
    pVertices = static_cast<VTXNORTEX*>(pData);

    m_pVertices = m_pVITerrainCom->Get_VerticesPos();
    m_pContext->Unmap(pVB, 0);
    return S_OK;
}
HRESULT CTerrain::Clear_TerrainHeight()
{
    if (m_SelectCubeObjs.size() <= 0)
        return E_FAIL;

    //  vYPos /= 10.f;

    D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
    ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    HRESULT		hr = 0;

    ID3D11Buffer* pVB = m_pVITerrainCom->Get_VB();
    hr = m_pContext->Map(pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

    // Check E_FAIL
    if (FAILED(hr))
        return E_FAIL;

    // Get Vertices
    VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(MappedSubResource.pData);
    if (nullptr == pVertices)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }

    for (auto& iter : m_SelectCubeObjs)
    {
        _float3 v3VerticesPos = m_pVITerrainCom->Get_VerticesPos()[iter->Get_Index()];
        pVertices[iter->Get_Index()].v3Position = _float3(v3VerticesPos.x, 0.f, v3VerticesPos.z);
        m_pVITerrainCom->Get_VerticesPos()[iter->Get_Index()] = pVertices[iter->Get_Index()].v3Position;
        iter->Set_Position(XMLoadFloat3(&pVertices[iter->Get_Index()].v3Position));
    }

    void* pData = m_pVITerrainCom->ReCalculate_Value(pVertices);
    if (nullptr == pData)
    {
        m_pContext->Unmap(pVB, 0);
        return E_FAIL;
    }
    pVertices = static_cast<VTXNORTEX*>(pData);

    m_pVertices = m_pVITerrainCom->Get_VerticesPos();
    m_pContext->Unmap(pVB, 0);
    return S_OK;
}
#endif

#pragma endregion EXTERN_MAPTOOL

CTerrain* CTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTerrain* pInstance = new CTerrain(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CTerrain::Create To Failed");
    }
    return pInstance;
}

CGameObject* CTerrain::Clone(void* _pArg)
{
    CTerrain* pInstance = new CTerrain(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CTerrain::Clone To Failed");
    }
    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();

    Safe_Release(m_pNavigation);

#ifdef  _DEBUG
    for (auto& iter : m_CubeObjs)
        Safe_Release(iter);

    m_CubeObjs.clear();
#endif //  _DEBUG

    for (auto& iter : m_TerrainObjects)
        Safe_Release(iter);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVITerrainCom);
    Safe_Release(m_pRenderCom);
}









/*
HRESULT CTerrain::Create_FilterTexture(const _string& _strPath)
{
    ID3D11Texture2D* pTexture2D = nullptr;

    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = m_tTerraInDesc.iNumVerticesX;
    TextureDesc.Height = m_tTerraInDesc.iNumVerticesZ;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    TextureDesc.MiscFlags = 0;

    _ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];
    ZeroMemory(pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);

    for (_uint i = 0; i < TextureDesc.Width * TextureDesc.Height; ++i)
        pPixel[i] = D3DCOLOR_ARGB(255, 0, 0, 0);

    D3D11_SUBRESOURCE_DATA			SubResourceData;
    ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

    SubResourceData.pSysMem = pPixel;
    SubResourceData.SysMemPitch = (_uint)(sqrtf((_float)TextureDesc.Width * TextureDesc.Height)) * sizeof(_ulong);
    // Create Texture 2D

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture2D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &m_pFilterSRV)))
        return E_FAIL;


    D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
    ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    HRESULT hr = m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
    if (FAILED(hr))
    {
        Safe_Release(pTexture2D);
        Safe_Delete_Array(pPixel);
        return E_FAIL;
    }

    _ulong		dwByte = 0;
    _ulong		iIndex = 0;

    for (_uint i = 0; i < m_tTerraInDesc.iNumVerticesZ; ++i)
    {
        for (_uint j = 0; j < MappedSubResource.RowPitch / 4; ++j)
        {
            _uint iIndex = i * MappedSubResource.RowPitch / 4 + j;
            _float3 pos = m_pVertices[iIndex];
            pos.y = pos.y > 255.f ? 255.f : pos.y;

            ((_ulong*)MappedSubResource.pData)[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
        }
    }

    m_pContext->Unmap(pTexture2D, 0);

    _wstring path;
    path.assign(_strPath.begin(), _strPath.end());

    if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, path.c_str())))
        return E_FAIL;

    Safe_Release(pTexture2D);
    Safe_Delete_Array(pPixel);

 //   m_tSaveTerrianDesc.strSaveFilterDDSPath = _strPath;
    return S_OK;
}

HRESULT CTerrain::Modify_FilterTexture()
{
    D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
    ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    ID3D11Texture2D* pTexture2D = nullptr;
    m_pFilterSRV->GetResource((ID3D11Resource**) & pTexture2D);
    HRESULT		hr =  m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

    if (FAILED(hr))
        return E_FAIL;

    _vector vPos = XMLoadFloat3(&m_tModifyDesc.v3Pos);
    XMVector3TransformCoord(vPos, m_pTransformCom->Get_WorldMatrixInv());

    _float3 v3Pos;
    _float2 v2Radius = m_tModifyDesc.v2Radius;
    XMStoreFloat3(&v3Pos, vPos);
    for (_uint i = 0; i < m_tTerraInDesc.iNumVerticesZ; ++i)
    {
        for (_uint j = 0; j < MappedSubResource.RowPitch / 4; ++j)
        {
            _uint iIndex = i * (MappedSubResource.RowPitch / 4) + j;

            _float2 v2Pos = _float2(_float(i), _float(j));
            if (v3Pos.x - v2Radius.x < v2Pos.x && v2Pos.x <= v3Pos.x + v2Radius.x &&
                v3Pos.z - v2Radius.y < v2Pos.y && v2Pos.y <= v3Pos.z + v2Radius.y)
            {
                ((_ulong*)MappedSubResource.pData)[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
            }
        }
    }

    m_pContext->Unmap(pTexture2D, 0);
    return S_OK;
}
*/