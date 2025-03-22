#pragma once
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CGameInstance;
END

BEGIN(Tool)

class CCutSceneTex_Tool final : public CBase
{
public:
	typedef struct tagCutSceneTextureInfo
	{
		tagCutSceneTextureInfo() = default;
		tagCutSceneTextureInfo(const _bool& _bIsMoveTexture,
			const _uint& _iRenderTextureIndex, const _uint& _iShaderPassIndex)
			: 
			bMoveTexture(_bIsMoveTexture),
			iRenderTextureIndex(_iRenderTextureIndex), iShaderPassIndex(_iShaderPassIndex)
		{}
		_bool								bMoveTexture = false;
		_uint								iRenderTextureIndex = 0;
		_uint								iShaderPassIndex = 0;
	}CUTSCENETEXINFO;
private:
	explicit CCutSceneTex_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutSceneTex_Tool() = default;

public:
	const _bool& Is_MoveTexture() const { return m_tCutSceneTexInfo.bMoveTexture; }
	const _float& Get_CurTime() const { return m_fCurTime; }
public:
	// Native Construct 
	HRESULT NativeConstruct(VOIDDATAS* _pDatas);
	// Show CutScene Texture
	void Show_CutSceneTexture(const _float& _fTotalTime);
	// Render Texture
	_bool Render_Texture_Move(CShader* _pShader, CTexture* _pTexture, CVIBuffer_Rect* _pRectBuffer);
	// Render Screen
	_bool Render_Screen( const _float4x4& _WorldMatrix, const _float4x4& _ViewMatirx, const _float4x4& _ProjMatrix,
		CShader* _pShader, CTexture* _pTexture, CVIBuffer_Rect* _pRectBuffer);
	// Draw Value Model
	void Draw_TexturePosition(CGameInstance* _pGameInstance);

	void Save_CutSceneTex(HANDLE& _hFile, DWORD* _pByte);
	void Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte);
private: 
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	// Transform
	CTransform*						m_pTransformCom = nullptr;
	// Texture 
	CUTSCENETEXINFO			m_tCutSceneTexInfo;

	_float										m_fCurTime = 0.f;
public:
	static CCutSceneTex_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas = nullptr);
	virtual void Free() override;
};

END