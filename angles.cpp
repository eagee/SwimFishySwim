// angles.c
// Provides concrete routines to modify float angle values
// Author(s): Eagan Rackley
#include "SFSHeaders.h"

///<summary>
/// Routine for adjusting a sprite rotation to reflect it's target rotation over a period of time ...
/// I'm not a math guy so I have no idea if this is the best way...	
///</summary>
void adjustAngleRotation(float &currentAngle, const float targetAngle, float rotationSpeed)
{	
	float difference = currentAngle - targetAngle;
	float velocity = (rotationSpeed * gTimeScale);
	
	// If currentAngle minus targetAngle > 0 then velocity is 1 else velocity is -1
	if(difference < 0)
	{
		velocity = -velocity;
	}
	
	// If the player wants to rotate more than the speed at which we rotate...
	if(abs(difference) > abs(velocity))
	{
		// If absolute of currentAngle minus targetAngle is < 180 then we reverse our velocity	
		if(abs(difference) < 180) 
		{
			velocity = -velocity;
		}
	
		// Add velocity to current angle
		currentAngle += velocity;
	
		// Adjust to see if we need to roll from 360->0 or 0->360
		if(currentAngle < 0)
		{
			currentAngle = 360;
		}
		else if(currentAngle > 360)
		{
			currentAngle -= 360;
		}
	}
}

///<summary>
/// Routine that will modify an angle by a certain number of degrees and adjust for above or below 360..
///</summary>
float modifyAngleByDegrees(float angle, float modifier)
{
	angle += modifier;
	// Adjust to see if we need to roll from 360->0 or 0->360
	if(angle < 0)
	{
		angle = 360;
	}
	else if(angle > 360)
	{
		angle -= 360;
	}
	
	return angle;
}

///<summary>
/// Routine that will return radians (0 to 255) from anle containing degrees (0 to 360)
///</summary>
float getRadiansForAngle(float angle)
{
	return (angle * (PI / 180));
}

///<summary>
/// Routine that will return degrees (0 to 360) from angle containing radians (0 to 255)
///</summary>
//static float getDegreesForAngle(float angle)
//{
//	return (angle * (180 / PI));
//}

///<summary>
/// Routine that gets cartesian coordinates for a Vector from location data, length, and radian angle..
///</summary>													
Vector modifyVectorByAngle(Vector location, float distanceLength, float angleInDegrees)
{
	Vector returnValue;
	float angleInRadians= getRadiansForAngle(angleInDegrees);
	returnValue.x = location.x + (distanceLength * cos(angleInRadians));
	returnValue.y = location.y + (distanceLength * sin(angleInRadians));
	returnValue.z = location.z;
	
	return returnValue;
}

///<summary>
/// Rotates a vector around the z axis it's currently on...
///</summary>													
Vector rotateVectorByAngle(Vector location, float angleInDegrees)
{
	Vector returnValue;
	float angleInRadians= getRadiansForAngle(angleInDegrees);
	
	returnValue.x = cos(angleInRadians) * location.x - sin(angleInRadians) * location.y;
	returnValue.y = sin(angleInRadians) * location.x + cos(angleInRadians) * location.y;
	returnValue.z = location.z;
	
	return returnValue;
}

///<summary>
/// Returns the angle in degrees between two vectors
///</summary>
float getAngleBetweenTwoVectors(Vector v1, Vector v2)
{
	float returnValue = 0.0f;
	float dot = 0.0f;
    // Do we need to normalize the vectors?
    //fastVectorNormalize(&v1);
	//fastVectorNormalize(&v2);

	dot = fastVectorDotProduct(&v1, &v2);

    // Force the dot product of the two input vectors to 
    // fall within the domain for inverse cosine, which 
    // is -1 <= x <= 1. This will prevent runtime 
    // "domain error" math exceptions.
	if(dot < -1.0f)
	{
		dot = -1.0f;
	}
	else if(dot > 1.0f)
	{
		dot = 1.0f;
	}
	float pie = (PI * 2.0f);
	float angle = acos(dot);

	returnValue = (pie - angle);
	
	return returnValue;
}

///<summary>
/// Returns the angle in degrees between two points
///</summary>
float getAngleBetweenTwoPoints(float x1, float y1, float x2, float y2)
{	
	float returnValue = 0.0f;
	if((x1 != x2) && (y1 != y2))
	{		
		// Get the angle in radians and convert to degrees, then correct for discontinuity (since arctan can return a negative)
		returnValue = (float)atan2((y2 - y1), (x2 - x1)) * 180.0f / PI;		
		returnValue = (float)(returnValue > 0.0 ? returnValue : (360.0 + returnValue)); 
	}
	
	return returnValue;	
}


///<summary>
/// Adjusts the input angle by the distances of the camera angle from 180.0f degrees
/// input angle must be in degrees for this to work ...
///</summary>
float adjustInputAngleByCameraAngle(float inputAngle, float cameraAngle)
{
	float difference = fmod(fabs(180.0f - cameraAngle), 360.0f);
	if (difference > 180)
	{
		difference = 360 - difference;
	}

	if(cameraAngle < 180.0f)
	{
		
		inputAngle -= difference;
	
	}
	else
	{
		inputAngle += difference;
	}
	
	// Adjust to see if we need to roll from 360->0 or 0->360
	if(inputAngle < 0)
	{
		inputAngle += 360;
	}
	else if(inputAngle > 360)
	{
		inputAngle -= 360;
	}

    return inputAngle;
}


///<summary>
/// Returns the distance between two angles ...
///</summary>
float getDistanceBetweenTwoAngles(float a1, float a2)
{
	float difference = fmod(fabs(a1 - a2), 360.0f);
	if (difference > 180)
	{
		difference = 360 - difference;
	}
	return difference;
}

	// If our camera angle ranges from > 270.0f to < 90.0f we get the difference from 0
	// If our camera angle ranges from >= 90.0f to <= 270.0f we get the difference from 180
	/*if((cameraAngle > 270.0f) && (cameraAngle <= 360.0f))
	{
		difference = fmod(fabs(0.0f - cameraAngle), 360.0f);
		if (difference > 180)
		{
			difference = 360 - difference;
		}
		inputAngle -= difference;
	}
	else if((cameraAngle >= 0.0f) && (cameraAngle < 90.0f))
	{
		difference = fmod(fabs(0.0f - cameraAngle), 360.0f);
		if (difference > 180)
		{
			difference = 360 - difference;
		}
		inputAngle += difference;		
	}
	else if((cameraAngle >= 180.0f) && (cameraAngle <= 270.0f))
	{
		difference = fmod(fabs(180.0f - cameraAngle), 360.0f);		
		inputAngle += difference;
	}
	else if((cameraAngle >= 90.0f) && (cameraAngle < 180.0f))
	{
		difference = fmod(fabs(180.0f - cameraAngle), 360.0f);				
		inputAngle -= difference;
	}*/
