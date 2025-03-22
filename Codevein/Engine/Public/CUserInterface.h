#pragma once
#include "CGameObject.h"
#include "CRectTransform.h"

BEGIN(Engine)


class ENGINE_DLL CUserInterface abstract : public CGameObject
{
public:
	typedef struct tagUIDesc
	{
		tagUIDesc() = default;
		tagUIDesc(const _wstring& _strName, const _float2& _v2Pos, const _float2& _v2Size, const _float& _fZBufferOrder)
			: strImgName(_strName), v2Pos(_v2Pos), v2Size(_v2Size), fZBufferOrder(_fZBufferOrder)
		{}
		_wstring	strImgName;
		_float2		v2Pos;
		_float2		v2Size;
		_float			fZBufferOrder;
	}UIDESC;
protected:
	explicit CUserInterface(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CUserInterface(const CGameObject& _Prototype);
	virtual ~CUserInterface() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Clone(void* pArg);
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT  Render();
protected:
	HRESULT Ready_RectTransform(const _tchar* _pTag, CRectTransform::RECTDESC* _pDesc = nullptr);
	HRESULT Ready_RectTransform(const _tchar* _pTag, CRectTransform::RECTDESC* _pDesc, CRectTransform** _pRectTransform);
	// Mouse	On	Rect
	_bool Is_MouseOnRect();
protected:
	// Rect Transform
	CRectTransform*		m_pRectTransformCom = nullptr;
	UIDESC							m_tUIDesc;
public:
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END