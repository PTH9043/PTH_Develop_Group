#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CCamera;
END

BEGIN(Client)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager);

private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	CCamera* Get_ActiveCamera();
	HRESULT Add_Camera(const _tchar* tagCamera, CCamera* pCamera);

	void	Set_ActiveCamera(const _tchar* tagCamera);
	void	Change_Level();

private:
	map<const _tchar*, CCamera*> m_CameraArr;

private:
	CCamera* Find_Camera(const _tchar* tagCamera);

public:
	virtual void Free() override;
};

END