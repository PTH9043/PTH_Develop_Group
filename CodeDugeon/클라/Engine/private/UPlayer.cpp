#include "EngineDefine.h"
#include "UPlayer.h"
#include "UMethod.h"
#include "UTransform.h"
#include "UCamera.h"
#include "UAnimationController.h"
#include "UCell.h"
#include "UNavigation.h"

UPlayer::UPlayer(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCharacter(_spDevice, _wstrLayer, _eCloneType),
	m_spFollowCamera{nullptr},
	m_bisJumping{false},
	m_bisFalling{false},
	m_bisFallOver{false}
{
}

UPlayer::UPlayer(const UPlayer& _rhs) : 
	UCharacter(_rhs),
	m_spFollowCamera{ nullptr },
	m_bisJumping{ false },
	m_bisFalling{ false },
	m_bisFallOver{ false }
{
}

void UPlayer::Free()
{
}

HRESULT UPlayer::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UPlayer::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);
	// VOIDDATAS에 CharacterDesc 0번, PlayerDesc 1번에 값을 채워야한다. 
	SetPawnType(PAWN_PLAYER);

	if (_Datas.size() >= 2)
	{
		PLAYERDESC PlayerDesc = UMethod::ConvertTemplate_Index<PLAYERDESC>(_Datas, PLAYERDESCORDER);
		m_spFollowCamera = PlayerDesc.spFollowCamera;
	}

	return S_OK;
}

void UPlayer::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
}

void UPlayer::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);

}

HRESULT UPlayer::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT UPlayer::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}
HRESULT UPlayer::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}
void UPlayer::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

void UPlayer::FollowCameraMove(const _float3& _vPlayerToDistancePosition, const _double& _TimeElapsed)
{
	RETURN_CHECK(true == IsNetworkConnected(), ;);
	RETURN_CHECK(nullptr == m_spFollowCamera, ;);
    // 카메라의 목표 위치를 계산.
    _float3 vTargetPosition = DirectX::XMVector3Transform(_vPlayerToDistancePosition, GetTransform()->GetWorldMatrix());

    // 카메라의 현재 위치.
    _float3 vCurrentPosition = m_spFollowCamera->GetTransform()->GetPos();

    // 카메라의 이동 방향을 계산.
    _float3 vDirection = vTargetPosition - vCurrentPosition;
    _float3 f3Distance = DirectX::XMVector3Length(vDirection);
    _float fDistance = f3Distance.x;
    _float fTimeLag = 0.05f;

    vDirection.Normalize();
    // 딜레이를 적용한 거리를 계산.
	_float fTimeLagScale = (fTimeLag) ? static_cast<_float>(_TimeElapsed) * (1.0f / fTimeLag) : 1.0f;
	_float fAdjustedDistance = fDistance * fTimeLagScale;

    // 카메라 이동 거리를 제한.
    if (fAdjustedDistance > fDistance) fAdjustedDistance = fDistance;
    if (fDistance < 0.01f) fAdjustedDistance = fDistance;

    // 적용된 이동 거리에 따라 카메라를 이동.
    if (fAdjustedDistance > 0)
    {
        _float3 vNewPosition = vCurrentPosition + vDirection * fAdjustedDistance;;
        m_spFollowCamera->GetTransform()->SetPos(vNewPosition);
    }
    m_spFollowCamera->GetTransform()->LookAt(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 7.f, GetTransform()->GetPos().z));


}

void UPlayer::JumpState(const _double& _dTimeDelta)
{
	RETURN_CHECK(true == IsNetworkConnected(), ;);
	// Region 
	{
		SHPTR<UNavigation> spNavigation = GetCurrentNavi();
		_float3 vPosition{ GetTransform()->GetPos() };
		SHPTR<UCell> PrevCell = spNavigation->GetCurCell();
		SHPTR<UCell> newCell{};
		_double speed = _dTimeDelta * 3.f;

		//예외처리. 전방으로 점프하면서 떨어질 때 아래가 낭떠러지면 두 상태가 전부 true가 되면서 얼어붙음.
		//이를 방지하기 위해 둘 다 true일 때 falling을 false로 함으로써 점프를 마저 하도록 처리. 
		if (m_bisJumping && m_bisFalling)
		{
			m_bisFalling = false;
		}

		//기본 중력 낙하. 중력이 활성화되야만 실행
		//점프하여 올라가고 있을 땐 비활성화
		if (!m_bisJumping && m_bisFalling)
		{
			GetTransform()->GravityFall(speed);
			vPosition = GetTransform()->GetPos();
		}

		//점프를 하고 있다면 점프 움직임 수행
		if (!m_bisFalling && m_bisJumping)
		{
			GetTransform()->JumpMovement(speed);
			vPosition = GetTransform()->GetPos();
		}
		if (GetTransform()->GetJumpVelocity().y < 0.f) //최대 높이까지 점프했다면 
		{
			//점프(상승) 해제, 낙하 수행
		
			m_bisJumping = false;
			m_bisFalling = true;
			
			GetTransform()->DisableJump();
		}


		//셀의 끝에 다다랐을 때
		if (false == spNavigation->IsMove(vPosition, REF_OUT newCell))
		{
			//낭떠러지 셀이 아닐 경우
			if (!spNavigation->GetCurCell()->GetJumpableState())
			{
				if (GetElapsedTime() < 70)
				{
					// 움직이려는 위치가 셀 밖에 있는 경우 현재 셀의 가장 가까운 선 위의 점으로 조정
					// 슬라이딩 벡터의 효과를 낼 수 있음
					_float3 closestPoint = spNavigation->ClampPositionToCell(vPosition);
					GetTransform()->SetPos(_float3(closestPoint.x, vPosition.y, closestPoint.z));
					vPosition = GetTransform()->GetPos();
				}
			}
			//낭떠러지 셀인 경우
			else
			{
				//낭떠러지 셀에서 밖으로 갔을 때, 중력 활성화(낙하)
				//공중에서 발 아래의 셀을 찾고, 현재 셀로 설정함
				newCell = spNavigation->FindCell(vPosition);
				
				m_bisFalling = true;
				
			}
		}

		if (m_bisFalling && spNavigation->GetCurCell() != nullptr)
		{
			//현재 셀보다 아래로 떨어지면
			_float curCellCenterY = spNavigation->GetCurCell()->GetHeightAtXZ(vPosition.x, vPosition.z);
			if (vPosition.y < curCellCenterY)
			{
				//낙하 종료vPosition
				m_bisFalling = false;
				GetTransform()->DisableGravity();
				GetTransform()->SetPos(_float3(vPosition.x, curCellCenterY, vPosition.z));
				vPosition = GetTransform()->GetPos();
			}
			
		}

		//떨어지고 있지 않을 때, 그리고 점프하여 올라가고 있지 않을 때 셀 위의 높이를 탐
		if (!m_bisFalling && !m_bisJumping)
		{		
			spNavigation->ComputeHeight(GetTransform());
		}
	}
}

