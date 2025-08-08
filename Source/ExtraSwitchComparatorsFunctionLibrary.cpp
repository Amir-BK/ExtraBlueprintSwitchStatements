bool UExtraSwitchComparatorsFunctionLibrary::IsVectorWithToleranceNotNearlyEqual(const FVector& A, const FVector& B, float Tolerance)
{
    float Distance = FVector::Dist(A, B);
    bool Result = Distance > Tolerance;

    UE_LOG(LogExtraSwitch, Warning, TEXT("IsVectorWithToleranceNotNearlyEqual CALLED - A(%f,%f,%f), B(%f,%f,%f), Tolerance=%f, Distance=%f, Result=%s"),
        A.X, A.Y, A.Z, B.X, B.Y, B.Z, Tolerance, Distance, Result ? TEXT("true") : TEXT("false"));

    return Result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsIntNotWithinRange(int32 A, int32 RangeMin, int32 RangeMax, bool Inclusive)
{
    bool Result;
    if (Inclusive)
    {
        Result = (A < RangeMin || A > RangeMax);
    }
    else
    {
        Result = (A <= RangeMin || A >= RangeMax);
    }

    UE_LOG(LogExtraSwitch, Warning, TEXT("IsIntNotWithinRange CALLED - A=%d, RangeMin=%d, RangeMax=%d, Inclusive=%s, Result=%s"),
        A, RangeMin, RangeMax, Inclusive ? TEXT("true") : TEXT("false"), Result ? TEXT("true") : TEXT("false"));

    return Result;
}