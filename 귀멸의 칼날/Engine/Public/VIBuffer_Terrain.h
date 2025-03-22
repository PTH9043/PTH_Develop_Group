#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct tagTerrianDesc
	{
		_uint iNumVerticiesX = 0;
		_uint iNumVerticiesZ = 0;
	}TERRAINDESC;
protected:
	explicit CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TERRAINDESC& _tTerrianDesc);
	explicit CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& _rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	const TERRAINDESC& Get_TerrainDesc() const { return m_tTerrainDesc; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(const _tchar* _pMapFilePath);
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT NativeConstruct_Prototype(const TERRAINDESC& _tTerrianDesc, _float3* _pVertices);
	virtual HRESULT Render();
	// ReCalculate Value
	virtual void* ReCalculate_Value(void* _pData) override;
private:
	// Terrian Desc
	TERRAINDESC m_tTerrainDesc;
public:
	static CVIBuffer_Terrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TERRAINDESC& _tTerrianDesc);
	static CVIBuffer_Terrain* Create_MapFile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pMapFilePath);
	static CVIBuffer_Terrain* Create_ToLoadData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TERRAINDESC& _tTerrianDesc, _float3* _pVertices);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END