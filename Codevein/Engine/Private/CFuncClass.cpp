#include "CFuncClass.h"

_float CFuncClass::Dot_DirToSelfLook(const _fvector& _vLook, const _fvector& _vDir)
{
    _vector vDir = _vDir;
    XMVectorSetY(vDir, 0.f);

    _vector vDot = XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(_vLook));
    return XMVectorGetX(vDot);
}

_float CFuncClass::Dot_DirToSelfRight(const _fvector& _vRight, const _fvector& _vDir)
{
    _vector vDir = _vDir;
    XMVectorSetY(vDir, 0.f);

    _vector vDot = XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(_vRight));
    return XMVectorGetX(vDot);
}

_float CFuncClass::Dot_DirToSelfUp(const _fvector& _vUp, const _fvector& _vDir)
{
    _vector vDot = XMVector3Dot(XMVector3Normalize(_vDir), XMVector3Normalize(_vUp));
    return XMVectorGetX(vDot);
}

_float CFuncClass::Atan2_Angle(const _fvector& _vPos)
{
    _float fAngle = atan2f(XMVectorGetZ(_vPos), XMVectorGetX(_vPos));
    _float fDegree = XMConvertToDegrees(fAngle);
    return fDegree;
}

_bool CFuncClass::Same_Text(const _string& _strText, const _string& _strFindText)
{
    for (_uint k = 0; k < _strFindText.length();)
    {
        _bool bIsTrue = false;
        for (_uint i = 0; i < _strText.length(); ++i)
        {
            if (false == bIsTrue)
            {
                if (_strText[i] == _strFindText[k])
                {
                    bIsTrue = true;
                    ++k;
                }
                else
                    break;
            }
            else
            {
                if (_strText[i] == _strFindText[k])
                {
                    ++k;
                }
                else
                {
                    bIsTrue = false;
                    break;
                }
            }
        }
        if (true == bIsTrue)
            return true;
        ++k;
    }
    return false;
}

_bool CFuncClass::Same_Text(const _wstring& _strText, const _wstring& _strFindText)
{
    for (_uint k = 0; k < _strFindText.length();)
    {
        _bool bIsTrue = false;
        for (_uint i = 0; i < _strText.length(); ++i)
        {
            if (false == bIsTrue)
            {
                if (_strText[i] == _strFindText[k])
                {
                    bIsTrue = true;
                    ++k;
                }
                else
                    break;
            }
            else
            {
                if (_strText[i] == _strFindText[k])
                {
                    ++k;
                }
                else
                {
                    bIsTrue = false;
                    break;
                }
            }
        }
        if (true == bIsTrue)
            return true;
        ++k;
    }
    return false;
}
