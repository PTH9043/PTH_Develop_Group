#pragma once
#include "CIMGUI.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)
class CStageManager;
class CRegion;
class CGameObject_Client;
class CCubeObj;

class CNavigationTool final : public CIMGUI
{
public:
	enum SELECT_OBJ
	{
		SEL_1, SEL_2, SEL_3, SEL_END
	};

	typedef struct tagSelectCell
	{
		tagSelectCell() = default;
		tagSelectCell(const _float& _fLength, const _uint& _iIndex, const _float3& _v3Pos)
			: fLength(_fLength), iIndex(_iIndex), v3Pos(_v3Pos)
		{}
		_float			fLength = 0.f;
		_uint			iIndex = 0;
		_float3		v3Pos = _float3(0.f, 0.f, 0.f);
	}SELCELL;
protected:
	explicit CNavigationTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CNavigationTool() = default;
public:
	const _bool& Is_RenderWireFrame() const { return m_bRenderWireFrame; }
	const _bool& Is_NavigationDebugColor() const { return m_bNavigationDebugColor; }
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_Tool() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds) override;
	virtual HRESULT Render() override;
	virtual HRESULT Release_Tool() override;
protected:
	virtual void Show_Window() override;
protected:
	// Modify Navigation
	void Modfiy_Navigation(CRegion* _pStage);
	// Reset Data
	void Reset_Nav_Data(CRegion* _pStage);
protected:
	CStageManager*							m_pStageManager = nullptr;
	CRenderer*										m_pRenderer = nullptr;
	vector<_float3	>								m_vecPosList;
	_uint													m_iCellIndex = 0;
private:
	CCubeObj*										m_pCubePosArr[SEL_END] = { nullptr };
	_uint													m_iRegionIndex = 0;
	_uint													m_iCreateRegionIndex = 0;
	_uint													m_iSelIndex = SEL_1;
	_bool													m_bSelEnd = false;
private:
	_bool													m_bNavigationModify = false;
	_bool													m_bAllRender = false;
	// Render Index
	_bool													m_bRenderWireFrame = false;
	_bool													m_bNavigationDebugColor = false;
public:
	static CNavigationTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END