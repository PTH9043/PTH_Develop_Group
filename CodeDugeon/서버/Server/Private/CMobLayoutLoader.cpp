#include "ServerDefines.h"
#include "CMobLayoutLoader.h"

namespace Server {

	CMobLayoutLoader::CMobLayoutLoader(const _string& _strPath)
	{
		std::ifstream read{ _strPath, std::ios::binary };
		RETURN_CHECK(true == read.fail(), ;);

		size_t LayOutSize;
		read.read((_char*)&LayOutSize, sizeof(size_t));
		m_MobData.reserve(LayOutSize);

		for (size_t i = 0; i < LayOutSize; ++i)
		{
			MOBDATA MobData;

			size_t ObjectNameSize = 0;
			read.read((_char*)&ObjectNameSize, sizeof(size_t));

			size_t ObjectAnimNameSize = 0;
			read.read((_char*)&ObjectAnimNameSize, sizeof(size_t));

			_string ObjName;
			_string AnimName;
			MobData.strAnimModelName.resize(ObjectNameSize);
			read.read(&MobData.strAnimModelName[0], ObjectNameSize);

			MobData.strAnimName.resize(ObjectAnimNameSize);
			read.read(&MobData.strAnimName[0], ObjectAnimNameSize);

			read.read((_char*) & MobData.mWorldMatrix, sizeof(_float4x4));

			m_MobData.push_back(MobData);
		}
	}

	CMobServerLayoutLoader::CMobServerLayoutLoader(const _string& _strPath)
	{
		std::ifstream read{ _strPath, std::ios::binary };
		ASSERT_CRASH(false == read.fail());

		size_t MobDatsSize = 0;
		read.read((_char*)&MobDatsSize, sizeof(size_t));
		m_MobData.reserve(MobDatsSize);
		for (_int i = 0; i < MobDatsSize; ++i)
		{
			MOBSERVERDATA mobServerData;
			read.read((_char*)&mobServerData, sizeof(MOBSERVERDATA));
			m_MobData.emplace_back(mobServerData);
		}
		int a = 0;
	}
}