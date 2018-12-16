@echo off
REM %1 <=> "$(ProjectDir)"
REM %~1 <=> $(ProjectDir)
REM %%a <=> UIName
REM %%b <=> UIRef

echo MuthM MakeFile
echo Gen UIProvider...
echo // Copyright (C) 2018 JackMyth. All Rights Reserved. >"%temp%/UIProvider.cpp"
echo // Copyright (C) 2018 JackMyth. All Rights Reserved. >"%temp%/UIProvider.h"
echo // Code is generated by PreBuildSteps, Don't modify it directly, Use UIRec.txt instead. >>"%temp%/UIProvider.cpp"
echo // Code is generated by PreBuildSteps, Don't modify it directly, Use UIRec.txt instead. >>"%temp%/UIProvider.h"
echo: >> "%temp%/UIProvider.cpp"
echo:  >> "%temp%/UIProvider.h"
echo #include "UIProvider.h" >> "%temp%/UIProvider.cpp"
for /f "eol=# tokens=1,2 delims=," %%a in (%~1\Source\MuthM\UIRec.txt) do (
echo #include "%%aBase.h" >> "%temp%/UIProvider.cpp"
)
echo #pragma once>>"%temp%/UIProvider.h"
echo #include "CoreMinimal.h" >> "%temp%/UIProvider.h"
echo #include "UObject/NoExportTypes.h" >> "%temp%/UIProvider.h"
echo #include "Object.h">>"%temp%/UIProvider.h"
echo #include "UIProvider.generated.h" >> "%temp%/UIProvider.h"
echo: >> "%temp%/UIProvider.cpp"
echo: >> "%temp%/UIProvider.h"
echo "%~1\Source\MuthM\UIRec.txt"
for /f "eol=# tokens=1,2 delims=," %%a in (%~1\Source\MuthM\UIRec.txt) do (
echo Process UI base class: %%a
echo DECLARE_DYNAMIC_DELEGATE_OneParam(F%%aDelegate, TSubclassOf^<class U%%aBase^>^&, _returnV^);>>"%temp%/UIProvider.h"
echo TSubclassOf^<U%%aBase^> UUIProvider::Get%%a(^) >> "%temp%/UIProvider.cpp"
echo { >> "%temp%/UIProvider.cpp"
echo 	if(%%a.IsBound(^)^) >> "%temp%/UIProvider.cpp"
echo 	{ >> "%temp%/UIProvider.cpp"
echo 		TSubclassOf^<U%%aBase^> _uclass; >> "%temp%/UIProvider.cpp"
echo 		%%a.Execute^(_uclass^);>>"%temp%/UIProvider.cpp"
echo 		return _uclass;>> "%temp%/UIProvider.cpp"
echo 	} >> "%temp%/UIProvider.cpp"
echo 	else>> "%temp%/UIProvider.cpp"
echo 	{>> "%temp%/UIProvider.cpp"
echo 		return LoadClass^<U%%aBase^>(NULL,TEXT(%%b^)^);>> "%temp%/UIProvider.cpp"
echo 	} >> "%temp%/UIProvider.cpp"
echo } >> "%temp%/UIProvider.cpp"
echo: >> "%temp%/UIProvider.cpp"
)
echo: >> "%temp%/UIProvider.h"
REM Process class
echo /**Provide UI for modding,should get all UI from this class.**/  >> "%temp%/UIProvider.h"
echo UCLASS(NotBlueprintable^)>> "%temp%/UIProvider.h"
echo class MUTHM_API UUIProvider : public UObject>> "%temp%/UIProvider.h"
echo {>> "%temp%/UIProvider.h"
echo 	GENERATED_BODY(^)>> "%temp%/UIProvider.h"
echo public:>> "%temp%/UIProvider.h"
echo 	UFUNCTION(BlueprintPure,DisplayName="Get UI Provider"^)>> "%temp%/UIProvider.h"
echo 	static UUIProvider* Get(^)>> "%temp%/UIProvider.h"
echo 	{>> "%temp%/UIProvider.h"
echo 		return GetMutableDefault^<UUIProvider^>(^);>> "%temp%/UIProvider.h"
echo 	}>> "%temp%/UIProvider.h"
echo: >> "%temp%/UIProvider.h"
for /f "eol=# tokens=1,2 delims=," %%a in (%~1\Source\MuthM\UIRec.txt) do (
echo 	UPROPERTY(BlueprintReadWrite^)>> "%temp%/UIProvider.h"
echo 		F%%aDelegate %%a;>> "%temp%/UIProvider.h"
echo 	UFUNCTION(BlueprintCallable^)>> "%temp%/UIProvider.h"
echo 		void Clear%%a(^) { %%a.Unbind(^); }>> "%temp%/UIProvider.h"
echo 	TSubclassOf^<class U%%aBase^> Get%%a(^); >> "%temp%/UIProvider.h"
echo: >> "%temp%/UIProvider.h"
)
echo }; >> "%temp%/UIProvider.h"
fc /b "%temp%\UIProvider.cpp" "%~1\Source\MuthM\Private\UIProvider.cpp" > nul
if errorlevel 1 (
copy /y "%temp%\UIProvider.cpp" "%~1\Source\MuthM\Private\UIProvider.cpp"
)
fc /b "%temp%\UIProvider.h" "%~1\Source\MuthM\Public\UIProvider.h" > nul
if errorlevel 1 (
copy /y "%temp%\UIProvider.h" "%~1\Source\MuthM\Public\UIProvider.h"
)
exit /b 0