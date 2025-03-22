#pragma once
#include "UUserInterface.h"

BEGIN(Engine)
class UTexGroup;
END

BEGIN(Client)

class CImageUI final : public UUserInterface {
public:
	CImageUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CImageUI(const CImageUI& _rhs);
	DESTRUCTOR(CImageUI)
public:
	CLONE_MACRO(CImageUI, "CImageUI::Clone To Failed")
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	// Ready 
	virtual HRESULT ReadyRectTransform(const VOIDDATAS& _vecDatas) override;

	// UUserInterface을(를) 통해 상속됨
	HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass) override;
	void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	virtual void Free() override;
private:
	// UI Texture Group 
	SHPTR<UTexGroup>			m_spUITextureGroup;
};


END
