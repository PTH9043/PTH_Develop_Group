#pragma once

#include "Base.h"

BEGIN(Engine)

class CDrawing final : public CBase
{
	DECLARE_SINGLETON(CDrawing)

public:
	enum VIEWTYPE { VIEW_2D, VIEW_3D, VIEW_END };

public:
	CDrawing();
	virtual ~CDrawing() = default;

public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	DrawTriangle(const _fvector vPoint1, const _fvector vPoint2, const _fvector vPoint3, VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT DrawLine(const _fvector vPoint1, const _fvector vPoint2, VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT Draw(const BoundingBox& box, VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT Draw(const BoundingOrientedBox& box, VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT Draw(const BoundingSphere& sphere, VIEWTYPE eType, _fvector color = DirectX::Colors::White);


private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	HWND					m_hWnd = 0;

private:
	_float					m_fWinSizeX, m_fWinSizeY;

private:
	PrimitiveBatch<VertexPositionColor>*			m_pBatch = nullptr;
	BasicEffect*									m_pEffect = nullptr;
	ID3D11InputLayout*								m_pInputLayout = nullptr;

public:
	virtual void	Free() override;
};

END
