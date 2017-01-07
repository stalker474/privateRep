// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyGameBlueprintLibrary.generated.h"

class UObject;

UCLASS()
class UStrategyGameBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** 
	 * Are the two actors on the same team.
	 *
	 * @param Actor1	First actor to compare.
	 * @param Actor2	Second actor to compare.
	 *
	 * @returns true if the actors are on the same team.
	 */
	 UFUNCTION(BlueprintPure, Category=Game)
	static bool AreFriends(AActor* Actor1, AActor* Actor2);

	/** 
	 * Are the two actors on different teams.
	 *
	 * @param Actor1	First actor to compare.
	 * @param Actor2	Second actor to compare.
	 *
	 * @returns true if the actors are on different teams.
	 */
	UFUNCTION(BlueprintPure, Category=Game)
	static bool AreEnemies(AActor* Actor1, AActor* Actor2);

	/** 
	 * Give a weapon to specified strategy character. 
	 *
	 * @param InChar	The Strategy character to give the weapon to.
	 * @param Armor		The weapon to give the to the character.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DeprecatedFunction, DeprecationMessage="Use GiveWeaponFromClass"))
	static void GiveWeapon(class AStrategyChar* InChar, UBlueprint* Weapon);

	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void GiveWeaponFromClass(AStrategyChar* InChar, TSubclassOf<class UStrategyAttachment> WeaponClass);

	/** 
	 * Give armor to specified strategy character. 
	 *
	 * @param InChar	The Strategy character to give the armor to.
	 * @param Armor		The armor to give the to the character.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn, meta=(DeprecatedFunction, DeprecationMessage="Use GiveArmorFromClass"))
	static void GiveArmor(class AStrategyChar* InChar, UBlueprint* Armor);
	
	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void GiveArmorFromClass(AStrategyChar* InChar, TSubclassOf<class UStrategyAttachment> ArmorClass);

	/** 
	 * Toggle visibility of specified minion. 
	 *
	 * @param InMinionChar	The minion to toggle the visibilty of.
	 */	
	UFUNCTION(BlueprintCallable, Category=Pawn)
	static void ToggleMinionVisibility(class AStrategyChar* InMinionChar);

	/** 
	 * Remove specified minion. 
	 * 
	 * @param InMinionChar	The minion to remove.
	 */
	UFUNCTION(BlueprintCallable, Category=Pawn)
	static bool RemoveMinion(class AStrategyChar* InMinionChar);

	/** Deprecated function. Use ShowWaveTitle. */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject", DeprecatedFunction, DeprecationMessage="Please use ShowWaveTitle instead"))
	static void ShowTitle(class UObject* WorldContextObject, FString LocKey);

	/** 
	 * Display the wave title. 
	 * 
	 * @param WaveNumber	The Wave number to show the title for.
	 */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static void ShowWaveTitle(class UObject* WorldContextObject, int32 WaveNumber);
};
