#include "CutSceneTex.h"
#include "GameInstance.h"

CCutSceneTex::CCutSceneTex(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef_DV(m_pDevice);
    Safe_AddRef_DV(m_pContext);
}


HRESULT CCutSceneTex::NativeConstruct(VOIDDATAS* _pDatas)
{
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    return S_OK;
}

_bool CCutSceneTex::Render_Texture_Move(CShader* _pShader, CTexture* _pTexture, CVIBuffer_Rect* _pRectBuffer, const _float4x4& _WorldMatrix)
{
    if (nullptr == _pShader || nullptr == _pTexture || nullptr == _pRectBuffer || false == m_tCutSceneTexInfo.bMoveTexture)
        return false;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        // View Proj
        _float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
        _float4x4  ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
        // View Matrix
        if (FAILED(_pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return false;
        }
        // ProjectMatrix
        if (FAILED(_pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return false;
        }
    }
    RELEASE_INSTANCE(CGameInstance);

    // World matrix
    _float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
    WorldMatrix = CMethod::XMMatrixMultiple4x4_P(MATRIXES4X4{ _WorldMatrix, WorldMatrix });
    WorldMatrix = CMethod::XMMatrixTranspos4x4_P(WorldMatrix);
    if (FAILED(_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
    {
        return false;
    }
    if (FAILED(_pTexture->SetUp_ConstantTable_Index(_pShader,
        "g_DiffuseTexture", m_tCutSceneTexInfo.iRenderTextureIndex)))
    {
        return false;
    }
    // Rendering
    _pShader->Begin(m_tCutSceneTexInfo.iShaderPassIndex);
    _pRectBuffer->Render();
    return true;
}

_bool CCutSceneTex::Render_Screen(const _float4x4& _WorldMatrix, const _float4x4& _ViewMatirx, const _float4x4& _ProjMatrix, CShader* _pShader, CTexture* _pTexture, CVIBuffer_Rect* _pRectBuffer)
{
    if (nullptr == _pShader || nullptr == _pTexture || nullptr == _pRectBuffer || true == m_tCutSceneTexInfo.bMoveTexture)
        return false;

    // World Matrix 
    if (FAILED(_pShader->Set_RawValue("g_WorldMatrix", &_WorldMatrix, sizeof(_float4x4))))
        return false;

    // View Matrix
    if (FAILED(_pShader->Set_RawValue("g_ViewMatrix", &_ViewMatirx, sizeof(_float4x4))))
        return false;

    // ProjectMatrix
    if (FAILED(_pShader->Set_RawValue("g_ProjMatrix", &_ProjMatrix, sizeof(_float4x4))))
        return false;

    if (FAILED(_pTexture->SetUp_ConstantTable_Index(_pShader,
        "g_DiffuseTexture", m_tCutSceneTexInfo.iRenderTextureIndex)))
        return false;

    // Rendering
    _pShader->Begin(m_tCutSceneTexInfo.iShaderPassIndex);
    _pRectBuffer->Render();
    return true;
}

void CCutSceneTex::Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
{
    OUTMATRIX OutMatrix;
    ReadFile(_hFile, &OutMatrix, sizeof(OUTMATRIX), _pByte, nullptr);
    ReadFile(_hFile, &m_tCutSceneTexInfo, sizeof(CUTSCENETEXINFO), _pByte, nullptr);
    ReadFile(_hFile, &m_fCurTime, sizeof(_float), _pByte, nullptr);
    if (nullptr != m_pTransformCom)
    {
        m_pTransformCom->Set_Pos(OutMatrix.vPos.Get_Vector());
        m_pTransformCom->Rotate_Turn(OutMatrix.vRot);
        m_pTransformCom->Set_Scale(OutMatrix.vScale);
    }
}

CCutSceneTex* CCutSceneTex::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas)
{
    CCutSceneTex* pInstance = new CCutSceneTex(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_pDatas)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCutSceneTex_Tool::Create To Failed");
#endif
    }
    return pInstance;
}

void CCutSceneTex::Free()
{
    Safe_Release_DV(m_pDevice);
    Safe_Release_DV(m_pContext);
    Safe_Release(m_pTransformCom);
}
