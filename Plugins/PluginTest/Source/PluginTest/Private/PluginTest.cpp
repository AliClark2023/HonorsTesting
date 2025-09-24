// Copyright Epic Games, Inc. All Rights Reserved.

#include "PluginTest.h"

#define LOCTEXT_NAMESPACE "FPluginTestModule"

void FPluginTestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	AActor* testPlane = AddActor(UStaticMesh::StaticClass());
}

void FPluginTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

AActor* FPluginTestModule::AddActor(TSubclassOf<AActor> ActorClass)
{
	ULevel* level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	if (level) {
		return GEditor->AddActor(level, ActorClass, FTransform());
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPluginTestModule, PluginTest)