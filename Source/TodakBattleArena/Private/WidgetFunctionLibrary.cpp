// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetFunctionLibrary.h"
#include "GameFramework/GameUserSettings.h"

void UWidgetFunctionLibrary::UpdateSetGraphicSettingsWidget(int32 GraphicsQualityIndex)
{
	UGameUserSettings* currGameUserSetting = UGameUserSettings::GetGameUserSettings();

	if (currGameUserSetting)
	{
		currGameUserSetting->SetTextureQuality(GraphicsQualityIndex);
		currGameUserSetting->SetAntiAliasingQuality(GraphicsQualityIndex);
		currGameUserSetting->SetFoliageQuality(GraphicsQualityIndex);
		currGameUserSetting->SetPostProcessingQuality(GraphicsQualityIndex);
		currGameUserSetting->SetShadingQuality(GraphicsQualityIndex);
		currGameUserSetting->SetShadowQuality(GraphicsQualityIndex);
		currGameUserSetting->SetViewDistanceQuality(GraphicsQualityIndex);
		currGameUserSetting->SetVisualEffectQuality(GraphicsQualityIndex);

		//Apply all the settings
		currGameUserSetting->SaveSettings();
	}
}