#pragma once
#include "UVIBuffer.h"

BEGIN(Engine)

class  UVIBufferTrail : public UVIBuffer {
public:
	/*typedef struct TrailBufferDesc
	{
		_float4x4* pStartMatrix;
		_float4x4* pEndMatrix;
		_float4x4 PivotMatrix;
		_float fReviseStart;
		_float fReviseEnd;
	}TRAILBUFFERDESC;*/
	typedef struct TrailVertexCnt {
		_int iMaxVertexCount;
	}TRAILVTXCOUNT;
public:
	UVIBufferTrail(CSHPTRREF <UDevice> _spDevice, const VIBUFFERTYPE _eBufferType);
	UVIBufferTrail(const UVIBufferTrail& _rhs);
	DESTRUCTOR(UVIBufferTrail)
public:
	CLONE_MACRO(UVIBufferTrail, "UVIBufferTrail::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	//HRESULT Update(_double TimeDelta, _float4x4 WorldMatrix, _bool bUpdate, _bool& bShow);
	virtual HRESULT Render(CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommands, const _uint& _VerticesCnt = 0)override;
private:
	//TRAILBUFFERDESC m_tTrailBifferDesc;
	TRAILVTXCOUNT m_tTrailMaxCountDesc;
	_bool m_bStart = false;
	_uint m_iCount = 0;
	_uint m_iCatmullRomIndex[4]{};
	_uint m_iCatmullRomCount = 4;
	_double m_EndTime = 0.3, m_EndTimeAcc = 0.0;
	UINT8* m_pBufferDataBegin;
private:
	_float4x4 m_StartMatrix, m_EndMatrix;
public:
	void SetVertices(VECTOR<VTXDEFAULT>& pVertices, _int iVertexCount);
};

END