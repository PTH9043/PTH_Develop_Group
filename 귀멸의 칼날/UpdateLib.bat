

// // xcopy /옵션 .복사할 파일이 들어가있는 경로      .파일들을 복사할 경로 

xcopy /y/s	.\Engine\Public\*.*				.\Reference\Headers\
xcopy /y/s	.\Engine\Bin\*.lib				.\Reference\Librarys\

xcopy /y/s	.\Engine\Bin\*.dll				.\Client\Bin\
xcopy /y/s	.\Engine\Bin\*.dll				.\Tool\Bin\

REM xcopy /y/s	.\Engine\Bin\ShaderFiles\*.hlsl .\Client\Bin\ShaderFiles\
REM xcopy /y/s	.\Engine\Bin\ShaderFiles\*.hlsl .\Tool\Bin\ShaderFiles\

xcopy /y/s	.\Engine\Bin\Font\*.ttf .\Client\Bin\Font\
xcopy /y/s	.\Engine\Bin\Font\*.ttf .\Tool\Bin\Font\

xcopy /y/s	.\Engine\ThirdPartyLib\*.Lib	.\Reference\Librarys\
