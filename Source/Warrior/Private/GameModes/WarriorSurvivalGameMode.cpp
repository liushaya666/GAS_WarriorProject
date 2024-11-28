// Learning Project


#include "GameModes/WarriorSurvivalGameMode.h"

#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

void AWarriorSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	EWarriorGameDifficulty SavedGameDifficulty;
	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		CurrentGameDifficulty = SavedGameDifficulty;
	}
	
}

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();
	checkf(EnemyWaveSpawnerDataTable, TEXT("Forget to assign a valid data table in survival game mode blueprint"));
	SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
	TotalWavesToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();
	PreLoadNextWaveEnemies();
}

void AWarriorSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::SpawningNeWWave);
		}
	}
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::SpawningNeWWave)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
			TimePassedSinceStart = 0.f;
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::InProgress);
		}
	}
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;
			CurrentWaveCount++;
			if (HasFinishedAllWaves())
			{
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::AllWavesDone);
			}
			else
			{
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AWarriorSurvivalGameMode::SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState)
{
	CurrentSurvivalGameModeState = InState;
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWavesToSpawn;
}

void AWarriorSurvivalGameMode::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves())
	{
		return;
	}
	PreLoadedEnemyClassMap.Empty();
	for (const FWarriorEnemyWaveSpawnInfo& SpawnInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([SpawnInfo, this]()
			{
				if (UClass* LoadedEnemyClass = SpawnInfo.SoftEnemyClassToSpawn.Get())
				{
					PreLoadedEnemyClassMap.Emplace(SpawnInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
				}
			})
		);	
	}
}

FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));
	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, FString());
	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table"), *RowName.ToString());
	return FoundRow;
}

int32 AWarriorSurvivalGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}
	checkf(!TargetPointsArray.IsEmpty(), TEXT("No valid target point found in level: %s for spawning enemies"), *GetWorld()->GetName());
	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (const FWarriorEnemyWaveSpawnInfo& SpawnInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		const int32 NumToSpawn = FMath::RandRange(SpawnInfo.MinPerSpawnCount, SpawnInfo.MaxPerSpawnCount);
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnInfo.SoftEnemyClassToSpawn);
		for (int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);

			RandomLocation += FVector(0.f, 0.f, 150.f);

			AWarriorEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParameters);
			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &AWarriorSurvivalGameMode::OnEnemyDestroyed);
				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	return EnemiesSpawnedThisTime;
}

bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow() -> TotalEnemyToSpawnThisWave;
}

void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;
	Debug::Print(FString::Printf(TEXT("CurrentSpawnedEnemiesCounter: %i, TotalSpawnedEnemiesThisWaveCounter:%i"),CurrentSpawnedEnemiesCounter,TotalSpawnedEnemiesThisWaveCounter ));
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;
		SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaveCompleted);
	}
}

void AWarriorSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister)
{
	for (AWarriorEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &AWarriorSurvivalGameMode::OnEnemyDestroyed);
		}
	}
}
