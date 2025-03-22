#pragma once
#include "UPawn.h"

BEGIN(Engine)
class URectTransform;
class UVIBufferRect;

class UUserInterface abstract : public UPawn{
public:
	typedef struct tagUIDesc
	{
		tagUIDesc() = default;
		tagUIDesc(const _wstring& _strImgName, const _wstring& _shaderName, const _wstring& _drawOrder, const _float2& _v2Pos, const _float2& _v2Size, const _float& _fZBufferOrder)
			: strImgName(_strImgName), _shaderName(_shaderName), DrawOrder(_drawOrder), v2Pos(_v2Pos), v2Size(_v2Size), fZBufferOrder(_fZBufferOrder)
		{}
		_float2		v2Pos;
		_float2		v2Size;
		_float			fZBufferOrder{ 0.f };
		_wstring			DrawOrder{ L"" };
		_wstring		strImgName{ L"" };
		_wstring		_shaderName{ L"" };
	}UIDESC;
public:
	UUserInterface(CSHPTRREF<UDevice> _spDevice, 	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UUserInterface(const UUserInterface& _rhs);
	DESTRUCTOR(UUserInterface)
public:
	virtual void Free() override PURE;
	virtual HRESULT NativeConstruct() override PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override PURE;
	// Update Ratio
	void UpdateRatio(const _float _fRatio);
	// Update Pos
	void UpdatePos(const _float2& _vPos);
	// Upate Size
	void UpdateSize(const _float2& _vSize);
	// Mouse	On	Rect
	_bool IsMouseOnRect();

	static void EnableRender() { s_AllDisable = true; }
	static void DisableRender() { s_AllDisable = false; }

	static _bool IsAllRender() { return s_AllDisable; }
public:
	CSHPTRREF<URectTransform> GetRectTransform() const { return m_spRectTransform; }
	CSHPTRREF<UVIBufferRect> GetVIBufferRect() const { return m_spVIBufferRect; }
	const UIDESC& GetUIDesc() const { return m_stUIDesc; }

	void SetImageName(const _wstring& _wstrImgName) { this->m_stUIDesc.strImgName = _wstrImgName; }
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override PURE;
	virtual void LateTickActive(const _double& _dTimeDelta) override PURE;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand , CSHPTRREF<UTableDescriptor> _spTableDescriptor) override PURE;
	// Ready 
	virtual HRESULT ReadyRectTransform(const VOIDDATAS& _vecDatas) PURE;
protected:
	// Ready Rect Transform
	HRESULT CreateRectTrasnform(void* _pDesc);
private:
	SHPTR< URectTransform>		m_spRectTransform;
	SHPTR< UVIBufferRect>		m_spVIBufferRect;
	UIDESC										m_stUIDesc;

	static	_bool								s_AllDisable;
};

END
