#include "CCubeCollision.h"
#include "CTransform.h"

CCubeCollision::CCubeCollision(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCollision(_pDevice, _pContext)
{
}

CCubeCollision::CCubeCollision(const CCubeCollision& _rhs)
    : CCollision(_rhs)
{
}

HRESULT CCubeCollision::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CCubeCollision::NativeConstruct_Clone(void* _pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;
    return S_OK;
}

_bool CCubeCollision::Is_CollisionPos(const _fvector& _v3MousePos)
{


    return _bool();
}

_bool CCubeCollision::Is_CollisionOtherComp(CCollision* _pCollision)
{
    if (nullptr == _pCollision && nullptr == _pCollision->Get_Transform())
        return false;

    if (nullptr == m_pTransform)
        return false;

    return Is_CollisionOtherComp(_pCollision->Get_Type(), _pCollision->Get_Transform()->Get_Pos(), _pCollision->Get_Size());
}

_bool CCubeCollision::Is_CollisionOtherComp(const TYPE& _eType, const _fvector _v3OtherPos, const _fvector& _v3OtherSize)
{
    if (nullptr == m_pTransform)
        return false;

    if (TYPE_SPEHRE == _eType)
    {

    }
    else  if (TYPE_CUBE == _eType)
    {
        _fvector v3Pos = XMLoadFloat3(&m_v3Pos);
        _fvector v3Size = XMLoadFloat3(&m_v3Size) / 2.f, v3OtherSize = _v3OtherSize / 2.f;

        _fvector _v3Max = v3Pos + v3Size, _v3Min = v3Pos - v3Size;
        _fvector _v3OtherMax = _v3OtherPos + v3OtherSize, _v3OtherMin = _v3OtherPos - v3OtherSize;

        _float3 v3Min, v3Max, v3OtherMax, v3OtherMin;

        XMStoreFloat3(&v3Min, _v3Min);
        XMStoreFloat3(&v3Max, _v3Max);
        XMStoreFloat3(&v3OtherMin, _v3OtherMin);
        XMStoreFloat3(&v3OtherMax, _v3OtherMax);

        if (v3Min.x <= v3OtherMax.x && v3Min.y <= v3OtherMax.y && v3Min.z <= v3OtherMax.z)
        {
            if (v3Max.x >= v3OtherMin.x && v3Max.y >= v3OtherMin.y && v3Max.z >= v3OtherMin.z)
            {
                return true;
            }
        }
    }
    return false;
}

_bool CCubeCollision::Is_CollisionOBB(CCollision* _pCollision)
{
    if (nullptr == _pCollision && nullptr == _pCollision->Get_Transform())
        return false;

    if (nullptr == m_pTransform)
        return false;

    return Is_CollisionOBB(_pCollision->Get_Type(), _pCollision->Get_Transform(), _pCollision->Get_Size());
}

_bool CCubeCollision::Is_CollisionOBB(const TYPE& _eType, class CTransform* _pOtherTransform, const _fvector& _v3Size)
{
    if (nullptr == _pOtherTransform)
        return false;

    if (TYPE_SPEHRE == _eType)
    {

    }
    else  if (TYPE_CUBE == _eType)
    {
        _vector v3DisPos = XMLoadFloat3(&m_v3Pos) - _pOtherTransform->Get_Right();

        _float3 v3OtherMSize;
        XMStoreFloat3(&v3OtherMSize, _v3Size);

        _vector v3SelfRight = m_pTransform->Get_Right();
        _vector v3SelfUp = m_pTransform->Get_Up();
        _vector v3SelfLook = m_pTransform->Get_Look();

        _vector v3OtherRight = _pOtherTransform->Get_Right();
        _vector v3OtherUp = _pOtherTransform->Get_Up();
        _vector v3OtherLook = _pOtherTransform->Get_Look();

        v3SelfRight = XMVector3Normalize(v3SelfRight);
        v3SelfUp = XMVector3Normalize(v3SelfUp);
        v3SelfLook = XMVector3Normalize(v3SelfLook);
        v3OtherRight = XMVector3Normalize(v3OtherRight);
        v3OtherUp = XMVector3Normalize(v3OtherUp);
        v3OtherLook = XMVector3Normalize(v3OtherLook);

        _vector v3Size = XMLoadFloat3(&m_v3Size) / 2.f, v3OtherSize = _v3Size / 2.f;
        if (false == Is_Check_Axis(v3SelfRight, v3DisPos, XYZDIR{ v3SelfRight * m_v3Size.x, v3SelfUp * m_v3Size.y, v3SelfLook * m_v3Size.z },
            XYZDIR{ v3OtherRight * v3OtherMSize.x, v3OtherUp * v3OtherMSize.y, v3OtherLook * v3OtherMSize.z }))
            return false;
        	// Self Up 축을 기준으로 검사
		if (false == Is_Check_Axis(v3SelfUp, v3DisPos, XYZDIR{ v3SelfRight * m_v3Size.x, v3SelfUp * m_v3Size.y, v3SelfLook * m_v3Size.z },
            XYZDIR{ v3OtherRight * v3OtherMSize.x, v3OtherUp * v3OtherMSize.y, v3OtherLook * v3OtherMSize.z }))
            return false;
		// Self Look 축을 기준으로 검사
		if (false == Is_Check_Axis(v3SelfLook, v3DisPos, XYZDIR{ v3SelfRight * m_v3Size.x, v3SelfUp * m_v3Size.y, v3SelfLook * m_v3Size.z },
            XYZDIR{ v3OtherRight * v3OtherMSize.x, v3OtherUp * v3OtherMSize.y, v3OtherLook * v3OtherMSize.z }))
            return false;

		// Other Right 축을 기준으로 검사
		if (false == Is_Check_Axis(v3OtherRight, v3DisPos, XYZDIR{ v3SelfRight * m_v3Size.x, v3SelfUp * m_v3Size.y, v3SelfLook * m_v3Size.z },
            XYZDIR{ v3OtherRight * v3OtherMSize.x, v3OtherUp * v3OtherMSize.y, v3OtherLook * v3OtherMSize.z }))
            return false;
		// Other Up 축을 기준으로 검사
		if (false == Is_Check_Axis(v3OtherUp, v3DisPos, XYZDIR{ v3SelfRight * m_v3Size.x, v3SelfUp * m_v3Size.y, v3SelfLook * m_v3Size.z },
            XYZDIR{ v3OtherRight * v3OtherMSize.x, v3OtherUp * v3OtherMSize.y, v3OtherLook * v3OtherMSize.z }))
            return false;
		// Other Look 축을 기준으로 검사
		if (false == Is_Check_Axis(v3OtherLook, v3DisPos, XYZDIR{ v3SelfRight * m_v3Size.x, v3SelfUp * m_v3Size.y, v3SelfLook * m_v3Size.z },
            XYZDIR{ v3OtherRight * v3OtherMSize.x, v3OtherUp * v3OtherMSize.y, v3OtherLook * v3OtherMSize.z }))
            return false;

        return true;
    }
    return false;
}

_bool CCubeCollision::Is_Check_Axis(const _cvector& _v3Axis, const _cvector& _v3Dist, 
    const XYZDIR& _selfDir, const XYZDIR& _otherDir)
{
    _float fSelfLength = fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _selfDir.v3Right)))) +
        fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _selfDir.v3Up)))) +
        fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _selfDir.v3Look))));
    _float fOtherLength = fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _otherDir.v3Right)))) +
        fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _otherDir.v3Up)))) +
        fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _otherDir.v3Look))));

    _float fCheckLength = fabs(XMVectorGetX(XMVector3LengthSq(XMVector3Dot(_v3Axis, _v3Dist))));

    if (fSelfLength + fOtherLength > fCheckLength)
        return true;

    return false;
}

CCubeCollision* CCubeCollision::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCubeCollision* pInstance = new CCubeCollision(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCubeCollision::Create To Failed");
    }
    return pInstance;
}

CComponent* CCubeCollision::Clone(void* _pArg)
{
    CCubeCollision* pInstance = new CCubeCollision(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCubeCollision::Clone To Failed");
    }
    return pInstance;
}

void CCubeCollision::Free()
{
    __super::Free();
}
