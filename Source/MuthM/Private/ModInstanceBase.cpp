// Copyright (C) 2019 JackMyth. All Rights Reserved.


#include "ModInstanceBase.h"

bool UModInstanceBase::OnModUnload_Implementation()
{
	return false;
}

class UWorld* UModInstanceBase::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UObject* Outer=GetOuter();
		while (Outer && !Outer->GetWorld())
			Outer = Outer->GetOuter();
		return Outer ? Outer->GetWorld() : nullptr;
	}
	return nullptr;
}
