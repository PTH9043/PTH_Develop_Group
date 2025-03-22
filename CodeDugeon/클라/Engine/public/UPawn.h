#pragma once
#include "UActor.h"

BEGIN(Engine)
class URenderer;
class UShader;
class UCollider;

using COLLIDERCONTAINER = UNORMAP<_wstring, SHPTR<UCollider>>;

class UPawn  abstract : public UActor{
public:
	UPawn(CSHPTRREF<UDevice> _spDevice,  const _wstring & _wstrLayer, const CLONETYPE & _eCloneType, const BACKINGTYPE _eBackingType = BACKINGTYPE::STATIC,
		PAWNTYPE _ePawnType = PAWNTYPE::PAWN_ECT);
	UPawn(const UPawn& _rhs);
	DESTRUCTOR(UPawn)
public:
	//	CSHPTRREF<URenderer> GetRenderer() const;
		CSHPTRREF<UShader> GetShader() const;
		CSHPTRREF<UShader> GetShadowShader() const;
		CSHPTRREF<UShader> GetNorPosShader() const;
		CSHPTRREF<UShader> GetOutlineShader() const;

		const 	VECTOR<_uint>& GetRenderingDatas() const { return m_vecRenderingDatas; }
	#ifdef _USE_DEBUGGING
		const std::atomic<_bool>& IsDebugRenderingType() const { return m_isDebugRenderingType; }
	#endif 
		const COLLIDERCONTAINER& GetColliderContainer() const { return m_ColliderContainer; }
	public:
		virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _vecDatas) override PURE;
		virtual void Free() override PURE;
		virtual HRESULT NativeConstruct() override PURE;
		virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override PURE;
		bool IsHit(CSHPTRREF<UPawn> _spEnemy, const _double& _dTimeDelta);
		// Hit 

		void AddRenderGroup(const RENDERID _iRenderID);
		void AddShadowRenderGroup(const RENDERID _iRenderID);
		void AddOutlineRenderGroup(const RENDERID _iRenderID);
		void AddNorPosRenderGroup(const RENDERID _iRenderID);
public: /* get set*/
		void SetPawnType(PAWNTYPE _newType) { m_ePawnType = _newType; }
		const PAWNTYPE GetPawnType() const { return m_ePawnType; }
protected:
		// Tick, LateTick, Render
		virtual void TickActive(const _double& _dTimeDelta) override PURE;
		virtual void LateTickActive(const _double& _dTimeDelta) override PURE;
		virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override PURE;
		virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override PURE;
		virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor,_bool _pass = true) override PURE;
		// Damaged
		virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) PURE;
		// Add Collider
		void AddColliderInContainer(const _wstring& _wstrTag ,const _wstring& _wstrColliderProto, const _float3& _vColliderPos, const _float3& _vColliderScale);
		void AddColliderInContainer(const _wstring& _wstrTag, CSHPTRREF<UCollider> _spCollider);
		// Add Shader
		void AddShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag = RES_SHADER, const VOIDDATAS& _vecDatas = VOIDDATAS{});
		void AddShadowShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag = RES_SHADER, const VOIDDATAS& _vecDatas = VOIDDATAS{});
		void AddOutlineShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag = RES_SHADER, const VOIDDATAS& _vecDatas = VOIDDATAS{});
		void AddNorPosShader(const _wstring& _wstrProtoTag, const _wstring& _wstrTag = RES_SHADER, const VOIDDATAS& _vecDatas = VOIDDATAS{});

		void UpdateCollision();
	#ifdef _USE_DEBUGGING
		void AddDebugRenderGroup(const DEBUGRENDERID _iRenderID);
	#endif
	#ifdef _USE_IMGUI
	public:
		virtual void ShowObjectInfo() override;
	#endif
protected: /* get set*/
	// Get Renderer
	CSHPTRREF<URenderer> GetRenderer() const { return m_spRenderer; }
private:
		SHPTR<URenderer>		m_spRenderer;
		SHPTR<UShader>			m_spShader;
		SHPTR<UShader>			m_spShadowShader;
		SHPTR<UShader>			m_spNorPosShader;
		SHPTR<UShader>			m_spOutlineShader;

		COLLIDERCONTAINER		m_ColliderContainer;
	#ifdef _USE_DEBUGGING
		VECTOR<_uint>				m_vecRenderingDatas;
		_bool									m_isDebugRenderingType;
	#endif 
		PAWNTYPE						m_ePawnType;
};

END