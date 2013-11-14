// angles.h
// Defines a simple set of routines that can be used to modify angle values
// <see>SpritePlayer.cpp</see>
// Author(s): Eagan Rackley
#ifndef ANGLES_H
#define ANGLES_H

#define PI 3.14159265358979323846F

///<summary>
/// Routine for adjusting a sprite rotation to reflect it's target rotation over a period of time ...
/// I'm not a math guy so I have no idea if this is the best way...	
///</summary>
void adjustAngleRotation(float &currentAngle, const float targetAngle, float rotationSpeed);

///<summary>
/// Routine that will modify an angle by a certain number of degrees and adjust for above or below 360..
///</summary>
float modifyAngleByDegrees(float angle, float modifier);

///<summary>
/// Routine that will return radians (0 to 255) from anle containing degrees (0 to 360)
///</summary>
float getRadiansForAngle(float angle);

///<summary>
/// Routine that gets cartesian coordinates for a Vector from location data, length, and radian angle..
///</summary>													
Vector modifyVectorByAngle(Vector location, float distanceLength, float angleInDegrees);

///<summary>
/// Rotates a vector around the z axis it's currently on...
///</summary>													
Vector rotateVectorByAngle(Vector location, float angleInDegrees);

///<summary>
/// Returns the angle in degrees between two vectors
///</summary>
float getAngleBetweenTwoVectors(Vector v1, Vector v2);

///<summary>
/// Returns the distance between two angles ...
///</summary>
float getDistanceBetweenTwoAngles(float a1, float a2);

///<summary>
/// Returns the angle in degrees between two points
///</summary>
float getAngleBetweenTwoPoints(float x1, float y1, float x2, float y2);

///<summary>
/// Adjusts the input angle by the distances of the camera angle from 180.0f degrees
/// input angle must be in degrees for this to work ...
///</summary>
float adjustInputAngleByCameraAngle(float inputAngle, float cameraAngle);


#endif
