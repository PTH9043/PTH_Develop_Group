#pragma once
#include "UComponent.h"

BEGIN(Engine)
class UTransform;

class URectTransform : public UComponent{
public:
	typedef struct tagRectTransformDesc
	{
		SHPTR<UTransform>	 pTransform{nullptr};
		// Projection Matrix
		_float4x4						ProjMatrix = _float4x4::Identity;

		_float								iWindowX{ 0 }, iWindowY{ 0 };
		// Pos, Size
		_float2							vPos{};
		_float2							vSize{};
		_float								fRatio{ 0.f };
	}RECTDESC;
public:
	URectTransform(CSHPTRREF<UDevice> _spDevice);
	URectTransform(const URectTransform& _rhs);
	DESTRUCTOR(URectTransform)
public:
	const RECTDESC& GetRectTransformDesc() const { return m_stRectDesc; }
	const _float2& GetMinPos() const { return m_vMinPos; }
	const _float2& GetMaxPos() const { return m_vMaxPos; }
public:
	CLONE_MACRO(URectTransform, "URectTransform::Clone To Failed")
	virtual void Free() override;
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
public:
	void ChangeSize(const _uint _iWinX, const _uint _iWinY);
	// Update Ratio
	void UpdateRatio(const _float _fRatio);
	// Update Pos
	void UpdatePos(const _float2& _vPos);
	// Upate Size
	void UpdateSize(const _float2& _vSize);
	// Judge Mouse On Rect
	_bool IsMouseOnRect(const _float2& _vMousePos);
private:
	// Resize MinMax
	void ResizeMinMax();
	// Compute Rect Size
	void ComputeRectSize();
private:
	// Rect Transform 
	RECTDESC										m_stRectDesc;
	// Min Max
	_float2											m_vMinPos;
	_float2											m_vMaxPos;
};

END
