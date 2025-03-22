#pragma once
#include "CMob.h"
#include "CMummy.h"
#include "USound.h"

BEGIN(Client)

class CSarcophagus final : public CMob {
public:
	CSarcophagus(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CSarcophagus(const CSarcophagus& _rhs);
	enum SARCOTYPE {
		TYPE_STANDING, TYPE_LYING, TYPE_END
	};
	DESTRUCTOR(CSarcophagus)
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CSarcophagus, "CSarcophagus::CloneToFailed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
	virtual void ReceiveNetworkProcessData(const UProcessedData& _ProcessData);

	void SetSarcophagusType(SARCOTYPE _type) { m_SarcophagusType = _type; }
	SARCOTYPE GetSarcophagusType() { return m_SarcophagusType; }

	void SetOwnerMummy(CSHPTRREF<CMummy> _spMummy) { m_spOwnerMummy = _spMummy; }
	CSHPTRREF<CMummy> GetOwnerMummy() { return m_spOwnerMummy; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;

private:
	SARCOTYPE m_SarcophagusType;
	SHPTR<CMummy> m_spOwnerMummy;

	_double		m_dElapsedTimeForDeath;

	FMOD::Channel* m_pLaying1Channel;
	FMOD::Channel* m_pLaying2Channel;
	FMOD::Channel* m_pStanding1Channel;
	FMOD::Channel* m_pStanding2Channel;
};

END

