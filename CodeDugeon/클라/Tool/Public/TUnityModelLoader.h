#pragma once

BEGIN(Tool)

/*
@ Date: 2024-03-18, Writer: 박태현
@ Explain
- Unity에서 뽑아온 데이터들을 Export하는 클래스 
*/
class TUnityModelLoader {
public:

	static void LoadUnityAnimFBXData(CSHPTRREF<FILEGROUP> _spFileGroup,
		CSHPTRREF<FILEDATA> _spFileData, IN _wstring& _wstrConvertLoadPath);

};

END