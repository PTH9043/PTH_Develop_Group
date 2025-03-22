#include "CoreDefines.h"
#include "AMainLoop.h"
#include "AService.h"
#include "AGameTimer.h"
#include "ACoreInstance.h"
#include "AMonster.h"
#include "AAnimator.h"
#include "AAnimController.h"

namespace Core {

	AMainLoop::AMainLoop(OBJCON_CONSTRUCTOR, Asio::io_context& _context, SHPTR<AService> _spServerService) :
		ACoreObject(OBJCON_CONDATA), m_spGameTimer{ Create<AGameTimer>() }, 
		m_SteadyTimer{ _context }, m_wpServerService{_spServerService}
	{

	}

	void AMainLoop::RegisterTimer(_int _iTime)
	{
		m_SteadyTimer.expires_from_now(std::chrono::microseconds(_iTime));
		// 타이머의 비동기 대기 설정
		m_SteadyTimer.async_wait(std::bind(&AMainLoop::TimerThread, this, std::placeholders::_1));
	}

	void AMainLoop::TimerThread(const boost::system::error_code& _error)
	{
		m_spGameTimer->Tick();
		SHPTR<ACoreInstance> spCoreInstance = GetCoreInstance();
		SHPTR<AService> spServerService = m_wpServerService.lock();
		{
			_double dTimeDelta = m_spGameTimer->GetDeltaTime();
			LIST<SHPTR<AMonster>> aliveMonster;
			{
				// Mob을 찾음 
				for (auto& iter : spServerService->GetMobObjContainer())
				{
					SHPTR<AMonster> spMonster = iter.second;

					if (true == spMonster->IsPermanentDisable())
						continue;

					if (true == spMonster->IsActive())
					{
						aliveMonster.push_back(spMonster);
					}
				}

				if (0 >= aliveMonster.size())
				{
					RegisterTimer(10);
					return;
				}
			}
			// Animation 활성 
			{
				for (auto& iter : aliveMonster)
				{
					iter->Tick(dTimeDelta);
				}
				// Collision 
				spCoreInstance->CollisionSituation(dTimeDelta);
			}
		}
		RegisterTimer(0);
	}

	void AMainLoop::Free()
	{
	}

}