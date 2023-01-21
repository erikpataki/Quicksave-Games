// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogwaterStatics.h"

FRotator UFrogwaterStatics::AddRotator(const FRotator& A, const FRotator& B)
{
	return A + B;
}

bool UFrogwaterStatics::IsPointInBox(const FVector& Point, const FBox& Box)
{
	return FMath::PointBoxIntersection(Point, Box);
}
