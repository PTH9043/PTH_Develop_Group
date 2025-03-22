#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)
 
class ENGINE_DLL CVIBuffer_Tail  final : public CVIBuffer
{
public:
	typedef struct tagVIBuffer_TailDesc
	{
		_float fShowTime = 0.f;
	}BUFFERTAILDESC;
private:
	explicit CVIBuffer_Tail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Tail(const CVIBuffer_Tail& _rhs);
	virtual ~CVIBuffer_Tail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	// Tail 업데이트
	void Tail_Update(const _double& _dTimeDelta, const _uint& _iMatrixCount, _matrix* _vecMatrix);
	// 리셋 Value
	void Reset_Value();
private:
	HRESULT Create_Vertex_Tail();
	HRESULT Create_Index_Tail();

private:
	_uint					m_iRenderVertexCount = 0;
	CUSTIMER			m_tShowTimer = CUSTIMER(0.5f);
public:
	static CVIBuffer_Tail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END