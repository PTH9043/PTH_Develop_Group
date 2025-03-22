#include "CVIBuffer_Rect_Instance.h"
#include "CGameInstance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& _rhs)
    : CVIBuffer(_rhs), m_pVBInstance(_rhs.m_pVBInstance), m_iStrideInstance(_rhs.m_iStrideInstance), m_iNumInstance(_rhs.m_iNumInstance)
    , m_RectInstanceList(_rhs.m_RectInstanceList)
{
    Create_VertexBuffer_Instance(m_iNumInstance);
}

void CVIBuffer_Rect_Instance::Get_OriginPositionData(VTXTEX** _ppOut)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    *_ppOut = ((VTXTEX*)SubResource.pData);

    m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Rect_Instance::Set_OriginPositionData(VTXTEX* _ppInstert)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    ((VTXTEX*)SubResource.pData)[0] = _ppInstert[0];
    ((VTXTEX*)SubResource.pData)[1] = _ppInstert[1];
    ((VTXTEX*)SubResource.pData)[2] = _ppInstert[2];
    ((VTXTEX*)SubResource.pData)[3] = _ppInstert[3];

    m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Rect_Instance::Get_Data(VTXRECTINSTANCE** _ppData, const _uint& _iIndex)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    *_ppData = &((VTXRECTINSTANCE*)SubResource.pData)[_iIndex];

    m_pContext->Unmap(m_pVBInstance, 0);
}
void CVIBuffer_Rect_Instance::Set_Data(VTXRECTINSTANCE* _pData, const _uint& _iIndex)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    ((VTXRECTINSTANCE*)SubResource.pData)[_iIndex] = *_pData;

    m_pContext->Unmap(m_pVBInstance, 0);
}

EFFECTPARTICLEDESC* CVIBuffer_Rect_Instance::Get_InstanceData(const _uint& _iIndex)
{
    if (m_RectInstanceList.size() <= _iIndex)
        return nullptr;

    return &m_RectInstanceList[_iIndex];
}

void CVIBuffer_Rect_Instance::Set_InstnaceData(const EFFECTPARTICLEDESC& _tInfo, const _uint& _iIndex)
{
    if (m_RectInstanceList.size() <= _iIndex)
        return;
    D3D11_MAPPED_SUBRESOURCE		SubResource;
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    m_RectInstanceList[_iIndex] = _tInfo;
    m_RectInstanceList[_iIndex].Reset();
    ((VTXRECTINSTANCE*)SubResource.pData)[_iIndex].vColor = m_RectInstanceList[_iIndex].vParticleColor;
    ((VTXRECTINSTANCE*)SubResource.pData)[_iIndex].vBlendColor = m_RectInstanceList[_iIndex].vResultBlendColor;

    m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Rect_Instance::NativeConstruct_Prototype()
{
    Create_VertexBuffer_Instance();
    Create_IndexBuffer_Instance(1);
    m_RectInstanceList.resize(1);
    Create_VertexBuffer_Instance(1);
    return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::NativeConstruct_Clone(void* _pArg)
{
    m_bCloned = true;
    return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    ID3D11Buffer* pVertexBuffers[] = {m_pVB, m_pVBInstance };

    _uint				iStrides[] = {  m_iStride,   m_iStrideInstance  };

    _uint				iOffset[] = {    0,   0 };

    m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffset);  
    m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_eTopology);
    m_pContext->DrawIndexedInstanced(6, m_iNumInstance, 0, 0, 0);
   
    return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Change_Rect_Count(const _uint& _iNumInstance)
{
    vector<EFFECTPARTICLEDESC> tRectInstanceInfo;
    _uint iSize = (_uint)m_RectInstanceList.size();

    if (iSize < _iNumInstance)
    {
        tRectInstanceInfo.resize(_iNumInstance);
        for (_uint i = 0; i < iSize; ++i)
        {
            tRectInstanceInfo[i] = m_RectInstanceList[i];
        }
        m_RectInstanceList.clear();
        m_RectInstanceList = tRectInstanceInfo;
    }
    else
    {
        tRectInstanceInfo.resize(_iNumInstance);
        for (_uint i = 0; i < _iNumInstance; ++i)
        {
            tRectInstanceInfo[i] = m_RectInstanceList[i];
        }
        m_RectInstanceList.clear();
        m_RectInstanceList = tRectInstanceInfo;
    }

    if (FAILED(Create_VertexBuffer_Instance(_iNumInstance)))
        return E_FAIL;
    if (FAILED(Create_IndexBuffer_Instance(_iNumInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Reset_Rect_Count()
{
    m_RectInstanceList.clear();
    m_RectInstanceList.resize(1);
    if (FAILED(Create_VertexBuffer_Instance(1)))
        return E_FAIL;
    if (FAILED(Create_IndexBuffer_Instance(1)))
        return E_FAIL;
    return S_OK;
}

void CVIBuffer_Rect_Instance::Delete_Instance(const _uint& _iIndex)
{
    _uint iIndex = 0;
    for (vector< EFFECTPARTICLEDESC>::iterator it = m_RectInstanceList.begin(); it != m_RectInstanceList.end();)
    {
        if (_iIndex == iIndex)
        {
            it = m_RectInstanceList.erase(it);
        }
        else
            ++it;
        ++iIndex;
    }
}

void CVIBuffer_Rect_Instance::Update(class CTransform* _pTransform, const _double& _dTimeDelta, _vector vPlayerPos, const _bool& _bLookAt )
{
    if (nullptr == _pTransform)
        return;

    Update_Dir(_pTransform, _dTimeDelta, vPlayerPos, _bLookAt);
    m_tEffectParitlceUVDesc.Update_UV(_dTimeDelta);

    if (true == m_tEffectParitlceUVDesc.bChangeUV)
    {
        D3D11_MAPPED_SUBRESOURCE		SubResource;

        m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

        _float2 vPos = m_tEffectParitlceUVDesc.vResultUVCenterPos;
        _float2 vSize = m_tEffectParitlceUVDesc.vXYDrawValue;

        ((VTXTEX*)SubResource.pData)[0].v2TexUv = _float2(vPos.x - vSize.x, vPos.y - vSize.y);
        ((VTXTEX*)SubResource.pData)[1].v2TexUv = _float2(vPos.x + vSize.x, vPos.y - vSize.y);
        ((VTXTEX*)SubResource.pData)[2].v2TexUv = _float2(vPos.x + vSize.x, vPos.y + vSize.y);
        ((VTXTEX*)SubResource.pData)[3].v2TexUv = _float2(vPos.x - vSize.x, vPos.y + vSize.y);

        m_pContext->Unmap(m_pVB, 0);
    }
    else
    {
        m_tEffectParitlceUVDesc.ResetData();
    }
}

void CVIBuffer_Rect_Instance::Late_Update(class CTransform* _pTransform, const _double& _dTimeDelta)
{
    if (nullptr == _pTransform)
        return;
}

void CVIBuffer_Rect_Instance::Update_Dir(CTransform* _pTransform, const _double& _dTimeDelta, _vector vPlayerPos, const _bool& _bLookAt)
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

 
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (_uint i = 0; i < m_RectInstanceList.size(); ++i)
    {
        if (m_RectInstanceList[i].bLoop)
        {
            if (m_RectInstanceList[i].fSaveTime > m_RectInstanceList[i].fLifeTime)
            {
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vRight = _float4(m_RectInstanceList[i].vSize.x, 0.f, 0.f, 0.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vUp = _float4(0.f, m_RectInstanceList[i].vSize.y, 0.f, 0.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vLook = _float4(0.f, 0.f, m_RectInstanceList[i].vSize.z, 0.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation = _float4(m_RectInstanceList[i].vInitPos.x, m_RectInstanceList[i].vInitPos.y,
                    m_RectInstanceList[i].vInitPos.z, 1.f);
                ((VTXRECTINSTANCE*)SubResource.pData)[i].vColor = _float4(m_RectInstanceList[i].vParticleColor.x, m_RectInstanceList[i].vParticleColor.y,
                    m_RectInstanceList[i].vParticleColor.z, m_RectInstanceList[i].vParticleColor.w);
                m_RectInstanceList[i].Reset();
            }
        }

        m_RectInstanceList[i].Update_Data(_dTimeDelta);
        _float3 v3Right, v3Up, v3Look, vPos;
        _vector vChanagePos =  m_RectInstanceList[i].Translate_Pos(
            XMLoadFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation), _dTimeDelta);

        _float3 v3Scale;
        v3Scale.x = m_RectInstanceList[i].vSize.x + m_RectInstanceList[i].vResultPmScaleSize.x;
        v3Scale.y = m_RectInstanceList[i].vSize.y + m_RectInstanceList[i].vResultPmScaleSize.y;
        v3Scale.z = m_RectInstanceList[i].vSize.z + m_RectInstanceList[i].vResultPmScaleSize.z;

        _vector		vRight = XMLoadFloat3(&_float3(1.f, 0.f, 0.f)),
            vUp = XMLoadFloat3(&_float3(0.f, 1.f, 0.f)), vLook = XMLoadFloat3(&_float3(0.f, 0.f, 1.f));

        if (true == _bLookAt)
        {
            vLook = XMVector3Normalize(vPlayerPos);
            vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
            vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

            vRight = vRight * v3Scale.x;
            vUp = vUp * v3Scale.y;
            vLook = vLook * v3Scale.z;
        }
        else
        {
            vRight = vRight * v3Scale.x;
            vUp = vUp * v3Scale.y;
            vLook = vLook * v3Scale.z;

            _matrix RotationMatrix;
            RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_RectInstanceList[i].vResultRotAnlge.x),
                XMConvertToRadians(m_RectInstanceList[i].vResultRotAnlge.y),
                XMConvertToRadians(m_RectInstanceList[i].vResultRotAnlge.z));
        
            vRight = XMVector3TransformNormal(vRight, RotationMatrix);
            vUp = XMVector3TransformNormal(vUp, RotationMatrix);
            vLook = XMVector3TransformNormal(vLook, RotationMatrix);
        }

        _matrix Coord = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_RectInstanceList[i].vRotTranslate.x),
            XMConvertToRadians(m_RectInstanceList[i].vRotTranslate.y),
            XMConvertToRadians(m_RectInstanceList[i].vRotTranslate.z));

        vChanagePos += XMVector3TransformNormal(vChanagePos, Coord) * m_RectInstanceList[i].fRotLength ;

        XMStoreFloat3(&v3Right, vRight);
        XMStoreFloat3(&v3Up, vUp);
        XMStoreFloat3(&v3Look, vLook);
        XMStoreFloat3(&vPos, vChanagePos);

        ((VTXRECTINSTANCE*)SubResource.pData)[i].vRight = _float4(v3Right.x, v3Right.y, v3Right.z, 0.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vUp = _float4(v3Up.x, v3Up.y, v3Up.z, 0.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vLook = _float4(v3Look.x, v3Look.y, v3Look.z, 0.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation = _float4(vPos.x, vPos.y, vPos.z, 1.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vBlendColor = _float4(m_RectInstanceList[i].vResultBlendColor.x, m_RectInstanceList[i].vResultBlendColor.y, 
            m_RectInstanceList[i].vResultBlendColor.z, m_RectInstanceList[i].vResultBlendColor.w);
    }
    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Rect_Instance::Reset_Data()
{
    D3D11_MAPPED_SUBRESOURCE		SubResource;

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
    for (_uint i = 0; i < m_RectInstanceList.size(); ++i)
    {
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vRight = _float4(m_RectInstanceList[i].vSize.x, 0.f, 0.f, 0.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vUp = _float4(0.f, m_RectInstanceList[i].vSize.y, 0.f, 0.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vLook = _float4(0.f, 0.f, m_RectInstanceList[i].vSize.z, 0.f);
        ((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation = _float4(m_RectInstanceList[i].vInitPos.x, m_RectInstanceList[i].vInitPos.y,
            m_RectInstanceList[i].vInitPos.z, 1.f);
        m_RectInstanceList[i].Reset();
    }
    m_pContext->Unmap(m_pVBInstance, 0);
    m_tEffectParitlceUVDesc.ResetData();
}

// Save Load
void CVIBuffer_Rect_Instance::Save(const HANDLE& _hFile, DWORD* _dwByte)
{
    if (true == m_bCloned)
        return;

    WriteFile(_hFile, &m_iNumInstance, sizeof(_uint), _dwByte, nullptr);
    WriteFile(_hFile, &m_iStrideInstance, sizeof(_uint), _dwByte, nullptr);

    for (auto& iter : m_RectInstanceList)
    {
        WriteFile(_hFile, &iter, sizeof(EFFECTPARTICLEDESC), _dwByte, nullptr);
    }

    WriteFile(_hFile, &m_tEffectParitlceUVDesc, sizeof(EPUVDESC), _dwByte, nullptr);
}

void CVIBuffer_Rect_Instance::Load(const HANDLE& _hFile, DWORD* _dwByte)
{
    if (true == m_bCloned)
        return;
  
    m_RectInstanceList.clear();
    ReadFile(_hFile, &m_iNumInstance, sizeof(_uint), _dwByte, nullptr);
    ReadFile(_hFile, &m_iStrideInstance, sizeof(_uint), _dwByte, nullptr);

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        EFFECTPARTICLEDESC tDesc;
        ReadFile(_hFile, &tDesc, sizeof(EFFECTPARTICLEDESC), _dwByte, nullptr);

        m_RectInstanceList.push_back(tDesc);
    }

    Create_VertexBuffer_Instance(m_iNumInstance);
    Create_IndexBuffer_Instance(m_iNumInstance);
    ReadFile(_hFile, &m_tEffectParitlceUVDesc, sizeof(EPUVDESC), _dwByte, nullptr);
}

HRESULT CVIBuffer_Rect_Instance::Create_VertexBuffer_Instance()
{
    m_iStride = sizeof(VTXTEX);
    m_iNumVertices = 4;
    m_iNumBuffers = 2;

#pragma region VERTEX_BUFFER

    {
        D3D11_BUFFER_DESC tBufferDesc;
        ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
        tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
        tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        tBufferDesc.StructureByteStride = m_iStride;
        tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        tBufferDesc.MiscFlags = 0;

        VTXTEX* pVertices = new VTXTEX[m_iNumVertices];
        ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

        pVertices[0].v3Position = _float3(-0.5f, 0.5f, 0.f);
        pVertices[0].v2TexUv = _float2(0.0f, 0.f);

        pVertices[1].v3Position = _float3(0.5f, 0.5f, 0.f);
        pVertices[1].v2TexUv = _float2(1.0f, 0.f);

        pVertices[2].v3Position = _float3(0.5f, -0.5f, 0.f);
        pVertices[2].v2TexUv = _float2(1.0f, 1.f);

        pVertices[3].v3Position = _float3(-0.5f, -0.5f, 0.f);
        pVertices[3].v2TexUv = _float2(0.0f, 1.f);

        D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
        ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        tBufferSubResourceData.pSysMem = pVertices;

        if (FAILED(Create_VertexBuffer(tBufferDesc, tBufferSubResourceData)))
            return E_FAIL;

        Safe_Delete_Array(pVertices);
    }

#pragma endregion VERTEX_BUFFER
    return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Create_VertexBuffer_Instance(const _uint& _iNum)
{
    if (true == m_bCloned)
        return E_FAIL;

    if (0 == _iNum)
        return E_FAIL;

    if (nullptr != m_pVBInstance)
        Safe_Release(m_pVBInstance);

#pragma region INSTANCE_BUFFER

    m_iNumInstance = _iNum;
    m_iStrideInstance = sizeof(VTXRECTINSTANCE);

    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
    tBufferDesc.ByteWidth = m_iStrideInstance * m_iNumInstance;
    tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    tBufferDesc.StructureByteStride = m_iStrideInstance;
    tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tBufferDesc.MiscFlags = 0;

    VTXRECTINSTANCE* pInstanceVertices = new VTXRECTINSTANCE[m_iNumInstance];
    ZeroMemory(pInstanceVertices, sizeof(VTXRECTINSTANCE) * m_iNumInstance);

    for (_uint i = 0; i < _iNum; ++i)
    {
        pInstanceVertices[i].vTranslation = _float4(m_RectInstanceList[i].vInitPos.x, m_RectInstanceList[i].vInitPos.y, m_RectInstanceList[i].vInitPos.z, 1.f);
        pInstanceVertices[i].vRight = _float4(m_RectInstanceList[i].vSize.x, 0.f, 0.f, 0.f);
        pInstanceVertices[i].vUp = _float4(0.f, m_RectInstanceList[i].vSize.y, 0.f, 0.f);
        pInstanceVertices[i].vLook = _float4(0.f, 0.f, m_RectInstanceList[i].vSize.z, 0.f);
        pInstanceVertices[i].vColor = m_RectInstanceList[i].vParticleColor;
        pInstanceVertices[i].vBlendColor = m_RectInstanceList[i].vBlendColor;
    }

    D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
    ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    tBufferSubResourceData.pSysMem = pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, &tBufferSubResourceData, &m_pVBInstance)))
        return E_FAIL;

    Safe_Delete_Array(pInstanceVertices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Create_IndexBuffer_Instance(const _uint& _iNum)
{
    if (true == m_bCloned)
        return E_FAIL;

    if (0 == _iNum)
        return E_FAIL;

    if (nullptr != m_pIB)
        Safe_Release(m_pIB);

    m_iNumInstance = _iNum;
    m_iNumPrimitive = 2 * _iNum;
    m_iNumIndicesPerPrimitive = 3;
    m_iBytePerPrimitive = sizeof(FACEINDICES16);
    m_eFormat = DXGI_FORMAT_R16_UINT;
    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region INDEX_BUFFER

    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

    tBufferDesc.ByteWidth = m_iBytePerPrimitive * m_iNumPrimitive;
    tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    tBufferDesc.StructureByteStride = 0;
    tBufferDesc.CPUAccessFlags = 0;
    tBufferDesc.MiscFlags = 0;

    FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitive];
    ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

    _uint			iNumFaces = 0;

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        pIndices[iNumFaces]._1 = 0;
        pIndices[iNumFaces]._2 = 1;
        pIndices[iNumFaces]._3 = 2;
        ++iNumFaces;

        pIndices[iNumFaces]._1 = 0;
        pIndices[iNumFaces]._2 = 2;
        pIndices[iNumFaces]._3 = 3;
        ++iNumFaces;
    }

    D3D11_SUBRESOURCE_DATA tBufferSubResourceData;
    ZeroMemory(&tBufferSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    tBufferSubResourceData.pSysMem = pIndices;

    if (FAILED(__super::Create_IndexBuffer(tBufferDesc, tBufferSubResourceData)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

#pragma endregion INDEX_BUFFER
    return S_OK;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CVIBuffer_Rect_Instance::Create To Failed");
    }
    return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* _pArg)
{
    CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CVIBuffer_Rect_Instance::Create To Failed");
    }
    return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
    Safe_Release(m_pVBInstance);
    __super::Free();
}
