#include "Item.h"

BEGIN(Engine)
class CVIBuffer_Trail;
END

BEGIN(Client)

class CSword : public CItem
{
public:
	typedef struct tagSwordDesc
	{
		ITEMDESC tItemDesc;
		_string		strStartBoneName = "";
		_string		strFinishBoneName = "";
	}SWORDDESC;
protected:
	explicit CSword(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CSword(const CSword& _rhs);
	virtual ~CSword() = default;

public:
	void Set_TrailActive(_bool bActive);
	//  Change Bind Matrix
	void Change_Bind_Matrix(CHierarchyNode* _pNode);

	void Change_Trail_Texture(const _wstring& _strTextureName);
	void Return_Trail_Texture();
	void Active_TrailObject(const _bool& _bActive);
	void Update_TrailMatrix(const _float& _fStartValue, const _float& _fEndValue);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual HRESULT	RenderLight() override;
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;

protected:
	// Trail Object
	class CTrailObject*		m_pTrailObject = nullptr;
	CTransform*				m_pSocketTransform = nullptr;
public:
	static  CSword* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END