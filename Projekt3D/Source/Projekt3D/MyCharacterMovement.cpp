// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterMovement.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AMyCharacterMovement::AMyCharacterMovement()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	// Camera create
	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"),true);

	spring_arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Springarm2"));
	spring_arm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	spring_arm->SetRelativeLocation(FVector(0,0,40));
	spring_arm->TargetArmLength = 300.f;
	spring_arm->SetRelativeRotation(FRotator(-30.f,0,0));

	//
	// Camera lag
	// Movement
	
	spring_arm->bEnableCameraLag = true;
	spring_arm->CameraLagSpeed = 2; // How fast camera approach its positon
	spring_arm->CameraLagMaxDistance = 1.5; // Maxium distance camera can lag behind current rotation
	// Rotation
	spring_arm->bEnableCameraRotationLag = true;
	spring_arm->CameraLagSpeed = 4;
	spring_arm->CameraLagMaxTimeStep = 1;
	
	cam->AttachToComponent(spring_arm, FAttachmentTransformRules::KeepRelativeTransform ,USpringArmComponent::SocketName);
		
	
	speed = 0.5f;
	sprinting = false;
	crouching = false;
	crouchingTogg = false;
	toCrouch = false;
	toStand = false;
	canStand = false;
	
	firstPerson = false;

	zaladowano = false;
	
	GetCapsuleComponent()->GetUnscaledCapsuleSize(promien,wysokosc);
	sunsword = CreateDefaultSubobject<ASunSword>(TEXT("SUNSWORDDS"));
	kolizja = CreateDefaultSubobject<UKolizjaGowy>(TEXT("KolizjaGowy"));

}

void AMyCharacterMovement::Laduj()
{
	if(!zaladowano)
	{
		FActorSpawnParameters SpawnInfo;
		sunsword = GetWorld()->SpawnActor<ASunSword>(FVector::ZeroVector, FRotator::ZeroRotator,SpawnInfo);
		sunsword->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,FName("index_01_rSocket"));
		zaladowano = !zaladowano;
	}

}


// Called when the game starts or when spawned
void AMyCharacterMovement::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacterMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Laduj();
	if(jumping)
	{
		Jump();
	}

	if(wantsToStand)
	{
		canStand = !this->kolizja->sprawdz();
	}


	if(toCrouch)
		{
			float p,w;
			GetCapsuleComponent()->GetUnscaledCapsuleSize(p,w);
			if(w>wysokosc/2)
			{
				GetCapsuleComponent()->SetCapsuleSize(promien,w-4.f);
			}else{
				crouching = true;
				UE_LOG(LogTemp, Warning, TEXT("KUCAM"))
				toCrouch = false;
			}
	} else if(canStand && toStand)
	{
		float p,w;
		GetCapsuleComponent()->GetUnscaledCapsuleSize(p,w);
		if(w<wysokosc)
		{
			GetCapsuleComponent()->SetCapsuleSize(promien,w+4.f);
		}else
		{
			toStand = false;
			crouching = false;
			UE_LOG(LogTemp, Warning, TEXT("WSTALEM"))
			speed = 0.5f;
		}
	}
	
	
}

// Called to bind functionality to input
void AMyCharacterMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("HorizontalMovement",this,&AMyCharacterMovement::HorizontalMove);
	InputComponent->BindAxis("VerticalMovement",this,&AMyCharacterMovement::VerticalMove);
	InputComponent->BindAxis("HorizontalRotation",this,&AMyCharacterMovement::HorizontalRotation);
	InputComponent->BindAxis("VerticalRotation",this,&AMyCharacterMovement::VerticalRotation);
	
	InputComponent->BindAxis("Scroll",this,&AMyCharacterMovement::CameraZoom);

	InputComponent->BindAction("Jump",IE_Pressed,this,&AMyCharacterMovement::IsJumping);
	InputComponent->BindAction("Jump",IE_Released,this,&AMyCharacterMovement::IsJumping);

	InputComponent->BindAction("Sprint",IE_Pressed,this,&AMyCharacterMovement::StartSprint);
	InputComponent->BindAction("Sprint",IE_Released,this,&AMyCharacterMovement::StopSprint);

	InputComponent->BindAction("Crouch",IE_Pressed,this,&AMyCharacterMovement::ToggleCrouch);
	InputComponent->BindAction("Crouch",IE_Released,this,&AMyCharacterMovement::ToggleCrouch);


}



void AMyCharacterMovement::HorizontalMove(float value)
{
	if(value)
	{
		AddMovementInput(GetActorRightVector(),value*speed);
	}
}

void AMyCharacterMovement::VerticalMove(float value)
{
	if(value)
	{
		AddMovementInput(GetActorForwardVector(),value*speed);
	}
}

void AMyCharacterMovement::HorizontalRotation(float value)
{
	if(value)
	{
		AddActorLocalRotation(FRotator(0,value,0));
	}
}

void AMyCharacterMovement::VerticalRotation(float value)
{
	if(value)
	{
		if(firstPerson)
		{
			float temp = cam->GetRelativeRotation().Pitch + value;
			if(temp > -80 && temp < 80){
				cam->AddLocalRotation(FRotator(value,0,0));
			}
		}else
		{
			float temp = spring_arm->GetRelativeRotation().Pitch + value;
			if(temp > -65 && temp < 25){
				spring_arm->AddLocalRotation(FRotator(value,0,0));
			}
		}
	}
}

	
void AMyCharacterMovement::IsJumping()
{
	if(jumping)
	{
		jumping = false;
	} else
	{
		jumping = true;
	}
}

void AMyCharacterMovement::StartSprint()
{
	if(!crouching)
	{
		sprinting = true;
		speed = 1.0f;
	}
}

void AMyCharacterMovement::StopSprint()
{
	sprinting = false;;
	speed = 0.5f;
	
}
void AMyCharacterMovement::ToggleCrouch()
{
	crouchingTogg = !crouchingTogg;
	if(crouchingTogg)
	{
		speed = 0.1f;
		toStand = false;
		toCrouch = true;
		wantsToStand = false;
	}else
	{
		toCrouch = false;
		toStand = true;
		wantsToStand = true;
	}
}

void AMyCharacterMovement::CameraZoom(float value)
{
	if(value)
	{
		float temp = spring_arm->TargetArmLength + (value*-25);
		if(!firstPerson)
		{
			if(temp < 600 && temp > 100)
			{
				spring_arm->TargetArmLength = temp;
			}else if(temp <= 100)
			{
				spring_arm->TargetArmLength = 100;
				CameraSwap();
			}
		}else
		{
			if(temp > 100) CameraSwap();
		}
	}
}

void AMyCharacterMovement::CameraSwap()
{
	firstPerson = !firstPerson;
	if(firstPerson)
	{
		cam->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		cam->SetRelativeLocation(FVector(20,0,75));
	}else
	{
		spring_arm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		cam->AttachToComponent(spring_arm, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
		cam->SetRelativeLocation(FVector(00,0,0));
	}
}

void AMyCharacterMovement::InsertItemInHand()
{
	
}



