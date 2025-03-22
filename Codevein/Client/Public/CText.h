#include "CUserInterface.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CText : public CUserInterface
{
protected:
	explicit CText(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CText(const CText& _rhs);
	virtual ~CText() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
private:
	// Setting Constant Table
	HRESULT SetUp_ConstantTable();
	// Ready Component
	HRESULT Ready_Component();
protected:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRenderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIRectCom = nullptr;
public:
	static CText* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END