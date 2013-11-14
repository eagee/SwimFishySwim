// fastVectors.h
// Defines preprocessor macros that can be used to perform accelerated vector operations
// <see>SpritePlayer.cpp</see>
// Author(s): Eagan Rackley
#ifndef FAST_VECTORS_H
#define FAST_VECTORS_H

///<summary>
/// Calls an accelerated function (if available on platform) that will multiply src by scale and store the output in dst
///</summary>
void fastVectorScale(Vector *src, Vector *dst, f32 scale);

///<summary>
/// Calls an accelerated function (if available on platform) that will add Vectors a and b and place the sum in ab
///</summary>
void fastVectorAdd(Vector *a, Vector *b, Vector *ab);

///<summary>
/// Calls an accelerated function (if available on platform) that will subtract Vectors a and b and place the difference in ab
///</summary>
void fastVectorSubtract(Vector *a, Vector *b, Vector *ab);

///<summary>
/// Calls an accelerated function (if available on platform) that will obtain the dot product for a vector
///</summary>
float fastVectorDotProduct(Vector *a, Vector *b);

///<summary>
/// Calls an accelerated function (if available on platform) that will obtain the cross product for a pair of vectors
///</summary>
void fastVectorCrossProduct(Vector *a, Vector *b, Vector *axb);

///<summary>
/// Calls an accelerated function (if available on platform) that will normalize the vector
///</summary>
void fastVectorNormalize(Vector *a);

///<summary>
/// Returns the length of a vector
///</summary>
float fastVectorLength(Vector *a);

///<summary>
/// Gets a linear interpolation between valueOne and valueTwo based on the interpolationValue
/// Doesn't really go here, when there is time/energy put in it's own lib?
///</summary>
float fastLinearInterpolation(float valueOne, float valueTwo, float interpolationValue);

///<summary>
/// Returns initialized Vector for use with functions below
///</summary>
Vector fastVectorInit(float x, float y, float z);


#endif
