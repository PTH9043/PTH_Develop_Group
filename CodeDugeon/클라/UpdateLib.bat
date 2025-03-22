

// // xcopy /옵션 .복사할 파일이 들어가있는 경로      .파일들을 복사할 경로 


xcopy /y/s	.\Engine\Public\*.*				.\Reference\Headers\
xcopy /y/s	.\Engine\bin\*.lib				.\Reference\Librarys\
xcopy /y/s	.\Engine\bin\*.lib				.\Reference\Librarys\

xcopy /y/s	.\Engine\bin\*.dll				.\CodeDungeon\Bin\
xcopy /y/s	.\Engine\bin\*.dll				.\Tool\Bin\

xcopy /y/s	.\Engine\bin\*.dll				.\CodeDungeon\Bin\
xcopy /y/s	.\Engine\bin\*.dll				.\Tool\Bin\

xcopy /y/s	.\Shader\Shaders\*.cso				.\CodeDungeon\Bin\
xcopy /y/s	.\Shader\Shaders\*.cso				.\Tool\Bin\

xcopy /y/s	.\Engine\ThirdPartyLib\*.Lib	.\Reference\Librarys\
