#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
public:
	typedef struct tagTrailBufferDesc
	{
		tagTrailBufferDesc() = default;
		tagTrailBufferDesc(_float4x4* _pStartMatrix, _float4x4* _pEndMatrix, _float4x4 _pPivotMatrix)
			:pStartMatrix(_pStartMatrix), pEndMatrix(_pEndMatrix), PivotMatrix(_pPivotMatrix)
		{}

		_float4x4* pStartMatrix = nullptr;
		_float4x4* pEndMatrix = nullptr;
		_float4x4 PivotMatrix;
		_float			fStartValue = 0.f;
		_float			fEndValue = 0.f;
	}TRAILBUFFERDESC;

	typedef struct tagTrailBufferSaves
	{
		tagTrailBufferSaves() = default;
		tagTrailBufferSaves(TRAILBUFFERDESC _tDesc, const _uint& _iVerticesNum)
			: tValue(_tDesc), iVerticesNum(_iVerticesNum)
		{}

		TRAILBUFFERDESC tValue;
		_uint			 iVerticesNum = 0;
	}TRAILBUFFERSAVES;
public:
	explicit CVIBuffer_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Trail(const CVIBuffer_Trail& _rhs);
	virtual ~CVIBuffer_Trail() = default;
public:
	void Change_TrailBufferDesc(TRAILBUFFERDESC*& _pModify);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	HRESULT Update(const _double& _dTimeDelta, const _float4x4& _WorldMatrix, const _bool& _bUpdate, const _bool& _bShow);
public:
	// Start
	TRAILBUFFERDESC		m_tTrailBufferDesc;
	_uint									m_iVertexIndex = 0;
	_uint									m_iEndIndex = 0;
	// 시작 하고자 하는 Bool 
	_bool									m_bStart = false;
	// CatmulRom
	_uint									m_iCatmulRomArr[4] = { 0 };
	_uint									m_iCatmulRomMax = 4;
public:
	static CVIBuffer_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END