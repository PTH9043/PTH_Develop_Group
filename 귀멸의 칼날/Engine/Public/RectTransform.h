#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRectTransform final : public CComponent
{
public:
	typedef struct tagRectTransformDesc
	{
		// Projection Matrix
		_float4x4 ProjMatrix;
		// Pos, Size
		_float2 v2Pos;
		_float2 v2Size;
	}RECTDESC;
private:
	explicit CRectTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CRectTransform(const CRectTransform& _rhs);
	virtual ~CRectTransform() = default;
public:
	const RECTDESC& Get_RectDesc() const { return m_tRectTransformDesc; }
	void Set_RectDesc(const RECTDESC& _tRectDesc) { this->m_tRectTransformDesc = _tRectDesc; 		Resize_MinMax(); }
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	// Update Rect
	void Update_Rect(class CTransform* _pTransform, const _uint& _iWinSizeX, const _uint& _iWinSizeY, const _float& _fRatio);
	// Judge Mouse On Rect
	_bool Is_MouseOnRect(const _float2& _v2MousePos);
private:
	// Resize MinMax
	void Resize_MinMax();
private:
	// Rect Transform Desc
	RECTDESC m_tRectTransformDesc;

	_float2		 m_v2MinPos;
	_float2		m_v2MaxPos;
public:
	static CRectTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END