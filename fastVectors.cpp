// fastVectors.cpp
// Provides concrete routines to modify Vector values using hardware acceleration (if available)
// Author(s): Eagan Rackley
#include "SFSHeaders.h"

// This would change for different platforms which can be supported in this file...
#define FAST_WII_VECTORS 1

#ifdef FAST_WII_VECTORS
///<summary>
/// Calls an accelerated function (if available on platform) that will multiply src by scale and store the output in dst
///</summary>
void fastVectorScale(Vector *src, Vector *dst, f32 scale)
{
#ifdef PLATFORM_IS_WII
	guVecScale(src, dst, scale);	
#endif
#ifdef PLATFORM_IS_WIN
	dst->x = src->x * scale;
	dst->y = src->y * scale;
	dst->z = src->z * scale;
#endif
}

///<summary>
/// Calls an accelerated function (if available on platform) that will add Vectors a and b and place the sum in ab
///</summary>
void fastVectorAdd(Vector *a, Vector *b, Vector *ab)
{
#ifdef PLATFORM_IS_WII
	guVecAdd(a, b, ab);	
#endif
#ifdef PLATFORM_IS_WIN
	ab->x = a->x + b->x;
	ab->y = a->y + b->y;
	ab->z = a->z + b->z;
#endif
}

///<summary>
/// Calls an accelerated function (if available on platform) that will subtract Vectors a and b and place the difference in ab
///</summary>
void fastVectorSubtract(Vector *a, Vector *b, Vector *ab)
{	
#ifdef PLATFORM_IS_WII
	guVecSub(a, b, ab);	
#endif
#ifdef PLATFORM_IS_WIN
	ab->x = a->x - b->x;
	ab->y = a->y - b->y;
	ab->z = a->z - b->z;
#endif	
}

///<summary>
/// Calls an accelerated function (if available on platform) that will obtain the dot product for a vector
///</summary>
float fastVectorDotProduct(Vector *a, Vector *b)
{	
#ifdef PLATFORM_IS_WII
	return guVecDotProduct(a, b);
#endif
#ifdef PLATFORM_IS_WIN
    float dotProduct = 0.0f;
	dotProduct += a->x * b->x;
	dotProduct += a->y * b->y;
	dotProduct += a->z * b->z;
    return dotProduct;
#endif	
}

///<summary>
/// Calls an accelerated function (if available on platform) that will obtain the cross product for a pair of vectors
///</summary>
void fastVectorCrossProduct(Vector *a, Vector *b, Vector *axb)
{	
#ifdef PLATFORM_IS_WII
	guVecCross(a, b, axb);
#endif
#ifdef PLATFORM_IS_WIN
   axb->x = a->y * b->z - b->y * a->z;
   axb->y = a->z * b->x - b->z * a->x;
   axb->z = a->x * b->y - b->x * a->y;
#endif		
}

///<summary>
/// Returns the length of a vector
///</summary>
float fastVectorLength(Vector *a)
{
	return sqrt( (a->x * a->x) + (a->y * a->y) + (a->z * a->z) );
}

///<summary>
/// Calls an accelerated function (if available on platform) that will normalize the vector
///</summary>
void fastVectorNormalize(Vector *a)
{	
#ifdef PLATFORM_IS_WII
	guVecNormalize(a);
#endif
#ifdef PLATFORM_IS_WIN
		float length = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
        a->x = (a->x / length);
		a->y = (a->y / length);
		a->z = (a->z / length);
#endif			
}

///<summary>
/// Gets a linear interpolation between valueOne and valueTwo based on the interpolationValue
/// Doesn't really go here, when there is time/energy put in it's own lib?
///</summary>
float fastLinearInterpolation(float valueOne, float valueTwo, float interpolationValue)
{
	float returnValue = valueTwo;

	// To perform linear interpolation we find the lowest and higest value between one and two
	// subtract the lowest from the highest, multiply that value by the interpolation value, and
	// then add the product to the low value to get our interpolated value ;)
	// (e.g. low = 5 high = 10 interpolation = 0.5 returnValue = (((10 - 5) * 0.5) + 5) = 7.5)
	
	if(valueOne > valueTwo)
	{
		returnValue = (((valueOne - valueTwo) * interpolationValue) + valueTwo);
	}
	else if (valueTwo > valueOne)
	{
		returnValue = (((valueOne - valueTwo) * interpolationValue) + valueOne);
	}
	return returnValue;
}

///<summary>
/// Returns initialized Vector for use with functions below
///</summary>
Vector fastVectorInit(float x, float y, float z)
{
	Vector newVector = {x, y, z};
	return newVector;
}

#endif
