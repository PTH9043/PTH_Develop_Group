#pragma once
#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UCommand;
/*
Device, GpuCommand를 가지고 있는 클래스이다.
*/


class  UObject : public UBase {
public:
	UObject(CSHPTRREF<UDevice> _spDevice);
	UObject(const UObject& _rhs);
	virtual ~UObject() = default;
public:
	virtual void Free() override PURE;
	virtual HRESULT NativeConstruct() PURE;
#ifdef _USE_IMGUI
	virtual void ShowObjectInfo();
#endif
protected:
	CSHPTRREF<UDevice> GetDevice() const { return m_spDevice; }
private:
	SHPTR<UDevice>				m_spDevice;
	//SHPTR<UCommand>				m_spCommand;
};

END
