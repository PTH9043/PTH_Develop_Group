#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer
{
public:
	typedef struct tagEffectParticleUVDesc
	{		
		// Draw UvCenterPos
		_float2 vInitUVCenterPos = _float2(0.5f, 0.5f);
		// XY Value
		_float2 vXYDrawValue = _float2(0.5f, 0.5f);
		// PM XY Pos
		_float2 vPMXYPos = _float2(0.f, 0.f);
		// Width Height Value
		_uint iUVWidthValue = 0;
		_uint iUVHeightValue = 0;
		// UvFastValue
		_float	fUVFastValue = 1.f;
		// Result Uv CenterPos
		_float2 vResultUVCenterPos = _float2(0.f, 0.f);
		// Check Width Height Value
		_uint iCheckUVWidthValue = 0;
		_uint iCheckUVHeightValue = 0;
		// Frame Index
		_uint iFrameIndexX = 0;
		_uint iFrameIndexY = 0;
		// UVFPS
		_float fUVFPS = 1.f;
		// UV Time
		_float fUVTime = 0.f;
		// Change UV
		_bool bChangeUV = false;
		// Not Frame
		_bool bNotFrame = false;

		void Update_UV(const _double& _dTimeDelta)
		{
			if (true == bChangeUV)
			{
				if(false == bNotFrame)
				{
					// Uv Time이 1을 넘었을때
					if (fUVTime >= fUVFPS)
					{
						// Width를 넘어갔을 때
						if (++iCheckUVWidthValue >= iUVWidthValue)
						{
							iCheckUVWidthValue = 0;
							if (++iCheckUVHeightValue >= iUVHeightValue)
							{
								// 초기화
								vResultUVCenterPos = vInitUVCenterPos;
								iCheckUVWidthValue = 0;
								iCheckUVHeightValue = 0;
								fUVTime = 0.f;
							}
							else
							{
								vResultUVCenterPos.y += vPMXYPos.y;
							}
						}
						else
						{
							vResultUVCenterPos.x += vPMXYPos.x;
						}

						fUVTime = 0.f;
					}
					fUVTime += (_float)(_dTimeDelta)*fUVFastValue;
				}
			}
		}

		void Change_UV()
		{
			vInitUVCenterPos.x = (_float)(0.5f / iUVWidthValue) * iFrameIndexX;
			vInitUVCenterPos.y = (_float)(0.5f / iUVHeightValue) * iFrameIndexY;
		}

		void ResetData()
		{
			vInitUVCenterPos.x = (_float)(0.5f / iUVWidthValue);
			vInitUVCenterPos.y = (_float)(0.5f / iUVHeightValue);

			vXYDrawValue.x = 0.5f / iUVWidthValue;
			vXYDrawValue.y = 0.5f / iUVHeightValue;
			vPMXYPos.x = 1.f / iUVWidthValue;
			vPMXYPos.y = 1.f / iUVHeightValue;
			vResultUVCenterPos = vInitUVCenterPos;
			iCheckUVWidthValue = 0;
			iCheckUVHeightValue = 0;
		}
	}EPUVDESC;
public:
	const _uint& Get_StrideInstance() const { return m_iStrideInstance; }
	const _uint& Get_NumInstance() const { return m_iNumInstance; }
private:
	explicit CVIBuffer_Rect_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& _rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;
public:
	vector<EFFECTPARTICLEDESC> Get_RectInstanceList_Info() {return m_RectInstanceList;}
	vector<EFFECTPARTICLEDESC> Get_RectInstanceList_Modify() { return m_RectInstanceList; }
	// Get Origin Position Data
	void Get_OriginPositionData(VTXTEX** _ppOut);
	void Set_OriginPositionData(VTXTEX* _ppInstert);

	void Get_Data(VTXRECTINSTANCE** _ppData, const _uint& _iIndex = 0);
	void Set_Data(VTXRECTINSTANCE* _pData, const _uint& _iIndex = 0);

	EFFECTPARTICLEDESC* Get_InstanceData(const _uint& _iIndex);
	void Set_InstnaceData(const EFFECTPARTICLEDESC& _tInfo, const _uint&  _iIndex);

	const EPUVDESC& Get_Data() { return m_tEffectParitlceUVDesc; }
	EPUVDESC& Get_Modify_Data() { return m_tEffectParitlceUVDesc; }
	void Set_Data(const EPUVDESC& _tDesc) { this->m_tEffectParitlceUVDesc = _tDesc; m_tEffectParitlceUVDesc.ResetData(); }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render() override;

	HRESULT Change_Rect_Count(const _uint& _iNumInstance);
	HRESULT Reset_Rect_Count();
	// Delte Instancew
	void Delete_Instance(const _uint& _iIndex);
public:
	void Update(class CTransform* _pTransform, const _double& _dTimeDelta, _vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f), const _bool& _bLookAt = false);
	void Late_Update(class CTransform* _pTransform, const _double& _dTimeDelta);
protected:
	void Update_Dir(class CTransform* _pTransform, const _double& _dTimeDelta, _vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f), const _bool& _bLookAt = false);
public:
	void Reset_Data();
	// Save Load
	void Save(const HANDLE& _hFile, DWORD* _dwByte);
	void Load(const HANDLE& _hFile, DWORD* _dwByte);
protected:
	HRESULT Create_VertexBuffer_Instance();
	HRESULT Create_VertexBuffer_Instance(const _uint& _iNum);
	HRESULT Create_IndexBuffer_Instance(const _uint& _iNum);
private:
	vector<EFFECTPARTICLEDESC>		m_RectInstanceList;
	EPUVDESC												m_tEffectParitlceUVDesc;
	ID3D11Buffer*											m_pVBInstance = nullptr;
	_uint															m_iStrideInstance = 0;
	_uint															m_iNumInstance = 0;
public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};

END