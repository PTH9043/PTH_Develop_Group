
#pragma once
#include"Client_Defines.h"
#include "UI_Base.h"
#include"UI_HP.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Lines final : public CUI_Base
{
public:
	enum Type {TEXTURE_LINES, TEXTURE_NAME, TEXTURE_END};

protected:CUI_Lines(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_Lines(const CUI_Lines& _rhs);
		 virtual ~CUI_Lines() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;

public:
	void Find_Texture();
	void Render_Time(const _double& _dTimeDelta);

	
	_uint Get_Type() { return m_iType; }
	void Set_TextureIndex(_int _iIndex) { TextIndex = _iIndex; }


private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;
	
	map<_wstring, _uint> m_Lines_Texture;
	
	//ด๋ป็
	_int TextIndex = 0;
	_double TextTimeAcc = 0.0;
	_int TextTime = 0;
	_bool TextRender = false;
	_int MaxText = 0;

	_int PlayerIndex = 0;
	_bool StartDelay = false;

	_uint m_iType = TEXTURE_END;

	_double BossLines = 0.0;
	_int BossDealy = 0;

	CUI_HP* Start = nullptr;
	
	_tchar* m_PlayerName = nullptr;
	_tchar* m_Player2Name = nullptr;
	_tchar m_LinesName[MAX_PATH] = L"";

	CTransform* m_pNameTransform = nullptr;

	_int SoundTime = 0;
	_int SoundTotalTime = 0;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Lines* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END


