#pragma once
#include "ModelDefines.h"
#include "CBase.h"
BEGIN(Model)

class CIMGUI_MD : public CBase
{
public:
	typedef struct tagWindowDesc
	{
		tagWindowDesc() = default;
		tagWindowDesc(const char* _pName, const _int& _imGuiFlags, const _float2& _v2StartPos = _float2(0.f, 0.f), const _bool& _bEnable = true)
			: pName(_pName), imGuiWindowFlags(_imGuiFlags), v2Pos(_v2StartPos), bEnable(_bEnable)
		{}
		const char*					 pName = nullptr;
		_int									imGuiWindowFlags;
		_float2							v2Pos;
		_bool								bEnable;
	}WINDESC;
protected:
	explicit CIMGUI_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CIMGUI_MD() = default;
public:
	const _bool& Is_Open() const { return m_bOpen; }
	void Set_Open(const _bool& _bOpen) { m_bOpen = _bOpen; }
	// Get Name
	const char* Get_Name() const { return m_pName; }
public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Render();
protected:
	virtual void Show_Window() = 0;
protected:
	// Device, Context
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	// Open
	_bool										m_bOpen = false;
	const char*							m_pName = nullptr;
public:
	virtual void Free() override;
};

END