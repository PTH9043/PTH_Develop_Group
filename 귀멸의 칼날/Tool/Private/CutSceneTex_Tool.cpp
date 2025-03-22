#include "CutSceneTex_Tool.h"
#include "GameInstance.h"

CCutSceneTex_Tool::CCutSceneTex_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef_DV(m_pDevice);
    Safe_AddRef_DV(m_pContext);
}
                           

HRESULT CCutSceneTex_Tool::NativeConstruct(VOIDDATAS* _pDatas)
{
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    return S_OK;
}

void CCutSceneTex_Tool::Show_CutSceneTexture(const _float& _fTotalTime)
{
    if (nullptr != m_pTransformCom)
    {
        OUTMATRIX outMatrix = CMethod::Out_Matrix(m_pTransformCom->Get_WorldFloat4x4());

        _bool bPos = ImGui::DragFloat3("Texture_Position", &outMatrix.vPos.x, 0.5f, -1000.f, 1000.f);
        _bool bRot = ImGui::DragFloat3("Texture_Rotation", &outMatrix.vRot.x, 0.5f, -1000.f, 1000.f);
        _bool bScale = ImGui::DragFloat3("Texture_Scale", &outMatrix.vScale.x, 0.5f, -1000.f, 1000.f);

        if (true == bPos)
            m_pTransformCom->Set_Pos(outMatrix.vPos.Get_Vector());
        if (true == bRot)
            m_pTransformCom->Rotate_Turn(outMatrix.vRot);
        if (true == bScale)
            m_pTransformCom->Set_Scale(outMatrix.vScale);
    }

    ImGui::Checkbox("Move Texture", &m_tCutSceneTexInfo.bMoveTexture);
    ImGui::DragInt("Texture Index", (_int*)&m_tCutSceneTexInfo.iRenderTextureIndex);
    ImGui::DragInt("Shader Pass", (_int*)&m_tCutSceneTexInfo.iShaderPassIndex);
    ImGui::DragFloat("Texture Time", &m_fCurTime, 0.01f, 0.f, _fTotalTime);
}

_bool CCutSceneTex_Tool::Render_Texture_Move(CShader* _pShader, CTexture* _pTexture, CVIBuffer_Rect* _pRectBuffer)
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
    WorldMatrix = CMethod::XMMatrix4x4SetFloat3(CMethod::XMMatrix4x4GetFloat3(WorldMatrix, MATROW_POS)
        , MATROW_POS, WorldMatrix);
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

_bool CCutSceneTex_Tool::Render_Screen(const _float4x4& _WorldMatrix, const _float4x4& _ViewMatirx, const _float4x4& _ProjMatrix,
    CShader* _pShader, CTexture* _pTexture, CVIBuffer_Rect* _pRectBuffer)
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

void CCutSceneTex_Tool::Draw_TexturePosition(CGameInstance* _pGameInstance)
{
    _float4 vColor = _float4(0.2f, 0.5f, 1.f, 1.f);
    OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pTransformCom->Get_WorldFloat4x4());
    BoundingOrientedBox bBox1 = BoundingOrientedBox(OutMatrix.vPos, OutMatrix.vScale, OutMatrix.vQuaternionRot);
    _pGameInstance->Draw(bBox1, CDrawing::VIEW_3D, vColor.Get_Vector());
}

void CCutSceneTex_Tool::Save_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
{
    OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pTransformCom->Get_WorldMatrix());
    WriteFile(_hFile, &OutMatrix, sizeof(OUTMATRIX), _pByte, nullptr);
    WriteFile(_hFile, &m_tCutSceneTexInfo, sizeof(CUTSCENETEXINFO), _pByte, nullptr);
    WriteFile(_hFile, &m_fCurTime, sizeof(_float), _pByte, nullptr);
}

void CCutSceneTex_Tool::Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
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

CCutSceneTex_Tool* CCutSceneTex_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas)
{
    CCutSceneTex_Tool* pInstance = new CCutSceneTex_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_pDatas)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCutSceneTex_Tool::Create To Failed");
#endif
    }
	return pInstance;
}

void CCutSceneTex_Tool::Free()
{
    Safe_Release_DV(m_pDevice);
    Safe_Release_DV(m_pContext);
    Safe_Release(m_pTransformCom);
}
