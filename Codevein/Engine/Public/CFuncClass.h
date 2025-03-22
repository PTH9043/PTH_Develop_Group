#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CFuncClass
{
public:
	static _float Dot_DirToSelfLook(const _fvector& _vLook, const _fvector& _vDir);
	static _float Dot_DirToSelfRight(const _fvector& _vRight, const _fvector& _vDir);
	static _float Dot_DirToSelfUp(const _fvector& _vUp, const _fvector& _vDir);
	static _float Atan2_Angle(const _fvector& _vPos);

	static _bool Same_Text(const _string& _strText, const _string& _strFindText);
	static _bool Same_Text(const _wstring& _strText, const _wstring& _strFindText);
};

END