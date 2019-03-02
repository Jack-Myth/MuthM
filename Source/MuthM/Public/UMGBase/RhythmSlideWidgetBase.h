// Copyright (C) 2019 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMGBase/RhythmWidgetBase.h"
#include "RhythmSlide.h"
#include "RhythmSlideWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API URhythmSlideWidgetBase : public URhythmWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnSubnoteInfo(const TArray<FRhythmSlideSubNoteInfo>& SubnotesInfo);
	
};
