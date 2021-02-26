// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterMovement.h"

// Sets default values
AMyCharacterMovement::AMyCharacterMovement()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	cam->AttachTo(RootComponent);
	
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

	if(jumping)
	{
		Jump();
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

	InputComponent->BindAction("Jump",IE_Pressed,this,&AMyCharacterMovement::IsJumping);
	InputComponent->BindAction("Jump",IE_Released,this,&AMyCharacterMovement::IsJumping);
}

void AMyCharacterMovement::HorizontalMove(float value)
{
	if(value)
	{
		AddMovementInput(GetActorRightVector(),value);
	}
}

void AMyCharacterMovement::VerticalMove(float value)
{
	if(value)
	{
		AddMovementInput(GetActorForwardVector(),value);
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
		float temp = cam->GetRelativeRotation().Pitch + value;
		if(temp > -80 && temp < 80){
			cam->AddLocalRotation(FRotator(value,0,0));
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




