// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSProjectile::AFPSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		
		// Set the sphere's collision profile name to "Projectile". // 물체에 닿게 해주는 부분 : Collision setting
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		// Event called when component hits something : 델리게이트
		CollisionComponent->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);
		
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 1900.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.2f;
		ProjectileMovementComponent->ProjectileGravityScale = 2.5f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/Assets/Sphere.Sphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/Assets/SphereMaterial.SphereMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	// 2초 후 사라짐
	InitialLifeSpan = 2.0f;

	static ConstructorHelpers::FObjectFinder<USoundBase> SB(TEXT("SoundWave'/Game/MilitaryWeapSilver/Sound/GrenadeLauncher/Wavs/GrenadeLauncher_Explosion01.GrenadeLauncher_Explosion01'"));
	if (SB.Succeeded())
	{
		ExplosionSound = SB.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SA(TEXT("SoundAttenuation'/Game/Assets/GrenadeAttenuation.GrenadeAttenuation'"));
	if (SA.Succeeded())
	{
		ExplosionAttenuation = SA.Object;
	}

	/*static ConstructorHelpers::FClassFinder<AActor> Explosion(TEXT("Blueprint'/Game/Assets/Explosion.Explosion_C'"));
	if (Explosion.Succeeded())
	{
		ExplosionActor = Cast<AActor>(Explosion.Class);
	}*/
}

// Called when the game starts or when spawned
void AFPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExplosionTimer, this, &AFPSProjectile::Detonate, 1.95f, false);
}

// Called every frame
void AFPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// You only needed to supply a launch direction because the projectile's speed is defined by 'ProjectileMovementComponent'.
void AFPSProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
//OnHit
/*if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}*/


void AFPSProjectile::Detonate()
{
	UWorld* World = GetWorld();

	/*FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();*/

	if (World)
	{		
		TArray<AActor*> ignoredActors{};
		ignoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			GrenadeBaseDamage,
			GetActorLocation(),
			GrenadeRadius,
			UDamageType::StaticClass(),
			ignoredActors,
			this,
			GetInstigatorController(),
			true // 거리 감쇄 여부
		);

		AGrenadeExplosion* GrenadeExplosion = World->SpawnActor<AGrenadeExplosion>(AGrenadeExplosion::StaticClass(), GetActorTransform());

		UGameplayStatics::PlaySoundAtLocation( // 폭발음
			GetWorld(),
			ExplosionSound,
			GetActorLocation(),
			1.f,
			1.f,
			0.f,
			ExplosionAttenuation
		);
		
		Destroy();
	}
}

// 발사체가 적중했을 때 이벤트
void AFPSProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("OnHit Projectile"));
	

	/*if (ExplosionClass)
	{
		UE_LOG(LogTemp, Log, TEXT("BO"));
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParamsGrenade;
			SpawnParamsGrenade.Owner = this;
			SpawnParamsGrenade.Instigator = GetInstigator();

			World->SpawnActor<AGrenadeExplosion>(ExplosionClass, GetActorTransform(), SpawnParamsGrenade);
			UE_LOG(LogTemp, Log, TEXT("BOOM"));
		}
	}
	else
	{

	}*/

	//Destroy();
}