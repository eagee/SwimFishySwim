// SpriteEffect.cpp
// Defines implementation for a common interface for dealing with multiple sprite behaviors...
// Author(s): Eagan Rackley
#include "SFSHeaders.h"
#include "IParticleEffect.h"
#include "ParticleEffect.h"
#include "ISprite.h"

#define RIPPLE_LIFE_SPAN 	1.85f
#define REPEL_LIFE_SPAN  	0.38f
#define SPARK_LIFE_SPAN  	1.24f
#define SPLASH_LIFE_SPAN 	1.00f
#define WARP_LIFE_SPAN   	0.25f

///<summary>
/// Constructor
///</summary>
ParticleEffect::ParticleEffect(PARTICLE_EFFECT_TYPE effectType, int maxParticles)
{
	myTextureID = UNDEFINED_TEXTURE_HANDLE;
	myEffectType = effectType;
	// Allocate our pool of particles
	for(int idx = 0; idx < maxParticles; idx++)
	{
		Vector zeroVector = {0.0f, 0.0f, 0.0f};
		Particle newParticle;
		newParticle.location = zeroVector;
		newParticle.lifeSpan = 0.0f;
		newParticle.scale = 1.0f;
		newParticle.velocity = zeroVector;
		myParticlePool.push_back(newParticle);
	}
	myParticleDegrees = 0.0f;
	myParticleIncrement = 0.0f;
}

///<summary>
/// Destructor
///</summary>
ParticleEffect::~ParticleEffect()
{
	// Deallocate our pool of particles
	myParticlePool.clear();
}

///<summary>
/// Assigns an IModel instances to a sprite
///</summary>
void ParticleEffect::assignTexture(TextureHandle textureID)
{
	myTextureID = textureID;
}

///<summary>
/// Handles setup when adding a water ripplie
///</summary>
static void setupWaterRipple(Particle *particle, Vector location)
{
	particle->lifeSpan = RIPPLE_LIFE_SPAN;
	// Ripple particles are stationary for their life span
	Vector zeroVector = {0.0f, 0.0f, 0.0f};
	particle->velocity = zeroVector;
	particle->location = location;
	particle->location.z -= 0.14f;
	particle->scale = 0.55f;
}

///<summary>
/// Handles setup when adding a repel effect
///</summary>
static void setupRepelEffect(Particle *particle, Vector location, ISprite *sprite)
{
	particle->lifeSpan = REPEL_LIFE_SPAN;
	// Repel particles are stationary for their life span
	particle->velocity = sprite->getVelocity();
	particle->location = location;
	particle->location.z -= 0.65f;
	particle->scale = 1.0f;
}

///<summary>
/// Handles setup when adding a repel effect
///</summary>
static void setupWarpEffect(Particle *particle, Vector location, ISprite *sprite)
{
	particle->lifeSpan = WARP_LIFE_SPAN;
	// Repel particles are stationary for their life span
	particle->velocity = sprite->getVelocity();
	particle->location = location;
	particle->location.z -= 0.15f;
	particle->scale = 10.0f;
}

///<summary>
/// Handles setup when adding a spark effect
///</summary>
static void setupSparkEffect(Particle *particle, Vector location)
{
	particle->lifeSpan = SPARK_LIFE_SPAN;
	// Assign a random velocity to each spark (a spark can fly off in any direction)
	// The minus at the end of the mod rand is half of the max result (so that it can go
	// either direction)
	particle->velocity.x = ((fmod((float)rand(), 32.0f) * 0.01f) - 0.16f);
	particle->velocity.y = ((fmod((float)rand(), 32.0f) * 0.01f) - 0.16f);
	particle->velocity.z = ((fmod((float)rand(), 32.0f) * 0.01f) - 0.16f);
	particle->location.x += particle->velocity.x * 2.0f;
	particle->location.y += particle->velocity.y * 2.0f;
	particle->location.z += particle->velocity.z * 2.0f;
	particle->location = location;
	particle->location.z -= 1.00f;
	particle->scale = particle->lifeSpan;
}

///<summary>
/// Handles setup when adding a splash effect
///</summary>
static void setupSplashEffect(Particle *particle, Vector location,
		ISprite *sprite, float &particleDegrees, float &particleIncrement)
{
	AnimationType animation = sprite->getCurrentAnimation();

	if ((animation != ATTACK_A) && (animation != REVERSE_ATTACK) && (animation != SALUTE) && (animation != STAND))
	{
		particle->lifeSpan = sprite->getSpeed() * 0.5f;//SPLASH_LIFE_SPAN;
		particle->scale = sprite->getSpeed() * 0.14f;
	}
	else
	{
		particle->lifeSpan = 0.4f;//SPLASH_LIFE_SPAN;
		particle->scale = 0.1f;
	}

	float zSpeed = sprite->getSpeed();
	if (zSpeed <= 0.5f)
	{
		zSpeed = 0.5f;
	}
	else if (zSpeed >= 0.5f)
	{
		zSpeed = 0.5f;
	}

	// Particles need to either move left or right of current velocity
	// because we want our splash to look like a wake when things are moving
	// in the water :)
	// If we're creating a salute splash (big one!)
	if ((animation != ATTACK_A) && (animation != REVERSE_ATTACK) && (animation != SALUTE) && (animation != STAND))
	{
		// Assign a random velocity to each spark (a spark can fly off in any direction)
		// The minus at the end of the mod rand is half of the max result (so that it can go
		// either direction)
		particle->velocity = sprite->getVelocity();
		particle->velocity.x = -(particle->velocity.x * gTimeScale) / 7;
		particle->velocity.y = -(particle->velocity.y * gTimeScale) / 7;

		if ((rand() % 2) == 0)
		{
			// get our new angle 45 degrees away from the direction the sprite is heading
			float newAngle = sprite->getAngle();
			newAngle = modifyAngleByDegrees(newAngle, 122.5f);

			Vector newLocation = modifyVectorByAngle(location, 1.5f, newAngle);

			particle->velocity.x += (newLocation.x - location.x) * (gTimeScale * 2);
			particle->velocity.y += (newLocation.y - location.y) * (gTimeScale * 2);
			particle->location = newLocation;

		}
		else
		{
			// get our new angle 45 degrees away from the direction the sprite is heading
			float newAngle = sprite->getAngle();
			newAngle = modifyAngleByDegrees(newAngle, -122.5f);

			Vector newLocation = modifyVectorByAngle(location, 1.5f, newAngle);

			particle->velocity.x += (newLocation.x - location.x) * (gTimeScale * 2);
			particle->velocity.y += (newLocation.y - location.y) * (gTimeScale * 2);
			particle->location = newLocation;
		}
		particle->velocity.z = -zSpeed * (gTimeScale * 12);
	}
	// If we're creating an ATTACK or SALUTE splash :)
	else
	{
		Vector newLocation;
		// Assign a random velocity to each spark (a spark can fly off in any direction)
		// The minus at the end of the mod rand is half of the max result (so that it can go
		// either direction)
		particle->velocity = sprite->getVelocity();
		particle->velocity.x = -(particle->velocity.x * gTimeScale) / 7;
		particle->velocity.y = -(particle->velocity.y * gTimeScale) / 7;

		// Have our angle increment so that a circle is made out of the
		// available particles that seems to map with the attack mode...
		if (animation == SALUTE)
		{
			particleDegrees = modifyAngleByDegrees(particleDegrees, particleIncrement);
			newLocation = modifyVectorByAngle(location, fmod((float)rand(), 2.6f), particleDegrees);
			particle->location = newLocation;
		}
		else if (animation == STAND)
		{
			particleDegrees = modifyAngleByDegrees(particleDegrees, particleIncrement);
			newLocation = modifyVectorByAngle(location, fmod((float)rand(), 4.0f), particleDegrees);
			particle->location = newLocation;
		}
		else if (animation == ATTACK_A)
		{
			particleDegrees = modifyAngleByDegrees(particleDegrees, particleIncrement);
			newLocation = modifyVectorByAngle(location, 2.60f, particleDegrees);
			particle->location = newLocation;
		}
		else
		{
			particleDegrees = modifyAngleByDegrees(particleDegrees, -particleIncrement);
			newLocation = modifyVectorByAngle(location, 2.60f, particleDegrees);
			particle->location = newLocation;
		}

		if(animation == SALUTE)
		{
			particle->velocity.z = -zSpeed * (0.016f * 65.0f);

		}
		if(animation == STAND)
		{
				particle->velocity.z = -zSpeed * (0.016f * 20.0f);
		}
		else
		{
			particle->velocity.z = -zSpeed * (0.016f * 30.0f);
		}
		particle->velocity.x += (newLocation.x - location.x) * (gTimeScale * 2);
		particle->velocity.y += (newLocation.y - location.y) * (gTimeScale * 2);
	}

	particle->velocity.x += (fmod((float)rand(), 32.0f) * 0.001f) - 0.016f;
	particle->velocity.y += (fmod((float)rand(), 32.0f) * 0.001f) - 0.016f;
	particle->velocity.z += (fmod((float)rand(), 8.0f) * 0.01f) - 0.04f;
}

///<summary>
/// Adds a particle (if there are particles to spare) to the list of active particles
/// otherwise, particle is not activated
///</summary>
void ParticleEffect::addParticles(ISprite *sprite, Vector offset, int number)
{
	// iterate through our particle list and assign as many particles of the specified
	// type as possible (depending on whether or not there is a slot with a dead particle open...

	int numberAssigned = 0;
	float delayValue = 0;

	std::vector<Particle>::iterator particleIterator;
	Vector location = sprite->getLocation();

	location.x -= offset.x;
	location.y -= offset.y;
	location.z = sprite->getWaterLevel();

	myParticleDegrees = sprite->getAngle();
	myParticleIncrement = (360.0f / (float)number);

	particleIterator = myParticlePool.begin();
	while ((numberAssigned < number) && (particleIterator != myParticlePool.end()))
	{
		if (particleIterator->lifeSpan <= 0)
		{
			if (myEffectType == PARTICLE_EFFECT_WATER_RIPPLE)
			{
				particleIterator->delay = 0;
				setupWaterRipple(&*particleIterator, location);
			}
			else if (myEffectType == PARTICLE_EFFECT_REPEL_ENEMIES)
			{
				particleIterator->delay = 0;
				setupRepelEffect(&*particleIterator, location, sprite);
			}
			else if (myEffectType == PARTICLE_EFFECT_WARP)
			{
				particleIterator->delay = 0;
				setupWarpEffect(&*particleIterator, location, sprite);
			}
			else if (myEffectType == PARTICLE_EFFECT_SPARK)
			{
				particleIterator->delay = 0;
				setupSparkEffect(&*particleIterator, location);
			}
			else if (myEffectType == PARTICLE_EFFECT_SPLASH)
			{
				delayValue += 0.5f;
				particleIterator->delay = (int)delayValue;
				setupSplashEffect(&*particleIterator, location,
						sprite, myParticleDegrees, myParticleIncrement);
			} // end setup splash...
			numberAssigned++;
		}
		particleIterator++;
	}
}


///<summary>
/// Updates all active particles
///</summary>							
void ParticleEffect::updateParticles()
{
	// Iterate through our particles and update their life span and locations
	std::vector<Particle>::iterator particleIterator;

	particleIterator = myParticlePool.begin();
	while(particleIterator != myParticlePool.end())
	{
		if(particleIterator->delay > 0)
		{
			particleIterator->delay -= 3;
		}
		else if(particleIterator->lifeSpan > 0)
		{
			// Water ripple doesn't change it's location...
			if(myEffectType == PARTICLE_EFFECT_WATER_RIPPLE)
			{
				particleIterator->lifeSpan -= (1.0f * gTimeScale);
				particleIterator->scale += (10.0f * gTimeScale);
				particleIterator->location.z += 0.0001f;

			}
			else if(myEffectType == PARTICLE_EFFECT_REPEL_ENEMIES)
			{
				particleIterator->lifeSpan -= (0.8f * gTimeScale);
				if(particleIterator->lifeSpan > (REPEL_LIFE_SPAN/2))
				{
					particleIterator->scale += (30.0f * gTimeScale);
				}
				else
				{
					particleIterator->scale -= (40.0f * gTimeScale);
				}
				particleIterator->location.z += 0.0001f;
				particleIterator->location.x += -(particleIterator->velocity.x * gTimeScale);
				particleIterator->location.y += -(particleIterator->velocity.y * gTimeScale);
			}
			else if (myEffectType == PARTICLE_EFFECT_WARP)
			{
				particleIterator->lifeSpan -= (1.6f * gTimeScale);
				if (particleIterator->lifeSpan > (WARP_LIFE_SPAN / 2))
				{
					particleIterator->scale += (60.0f * gTimeScale);
				}
				else
				{
					particleIterator->scale -= (80.0f * gTimeScale);
				}
				particleIterator->location.z -= 0.01f;
				particleIterator->location.x += -(particleIterator->velocity.x * gTimeScale);
				particleIterator->location.y += -(particleIterator->velocity.y * gTimeScale);
			}
			else if (myEffectType == PARTICLE_EFFECT_SPARK)
			{
				particleIterator->lifeSpan -= (1.0f * gTimeScale);
				particleIterator->scale = particleIterator->lifeSpan;
				particleIterator->location.x += particleIterator->velocity.x;
				particleIterator->location.y += particleIterator->velocity.y;
				particleIterator->location.z += particleIterator->velocity.z;
				//particleIterator->velocity.z += 0.005f;
			}
			else if (myEffectType == PARTICLE_EFFECT_SPLASH)
			{
				particleIterator->lifeSpan -= (1.0f * gTimeScale);
				particleIterator->location.x += particleIterator->velocity.x;
				particleIterator->location.y += particleIterator->velocity.y;
				particleIterator->location.z += particleIterator->velocity.z;

				if(particleIterator->location.z <= SPRITE_WATER_LEVEL)
				{
					particleIterator->velocity.z += 0.0235f;
					particleIterator->scale += 1.0f * gTimeScale;
				}
				else
				{
					particleIterator->velocity.z -= 0.0470f;
					particleIterator->scale -= 4.0f * gTimeScale;
				}
			}

		}

		particleIterator++;
	}

}

static Quad setupQuad(PARTICLE_EFFECT_TYPE effectType, Particle particle)
{
	Quad quad;

	// Set up the verticies for quad so that we're drawing a health bar on top of the sprite
	// and in the center...

	if((effectType == PARTICLE_EFFECT_WATER_RIPPLE) || (effectType == PARTICLE_EFFECT_REPEL_ENEMIES) || (effectType == PARTICLE_EFFECT_WARP))
	{
		quad.verticies[0].x = -(particle.scale / 2);
		quad.verticies[0].y = -(particle.scale / 2);
		quad.verticies[0].z = 0.0f;

		quad.verticies[1].x = (particle.scale / 2);
		quad.verticies[1].y = -(particle.scale / 2);
		quad.verticies[1].z = 0.0f;

		quad.verticies[2].x = (particle.scale / 2);
		quad.verticies[2].y = (particle.scale / 2);
		quad.verticies[2].z = 0.0f;

		quad.verticies[3].x = -(particle.scale / 2);
		quad.verticies[3].y = (particle.scale / 2);
		quad.verticies[3].z = 0.0f;
	}
	else if((effectType == PARTICLE_EFFECT_SPARK) || (effectType == PARTICLE_EFFECT_SPLASH))
	{
		quad.verticies[0].x = -(particle.scale / 2);
		quad.verticies[0].y = 0.0f;
		quad.verticies[0].z = -(particle.scale / 2);

		quad.verticies[1].x = (particle.scale / 2);
		quad.verticies[1].y = 0.0f;
		quad.verticies[1].z = -(particle.scale / 2);

		quad.verticies[2].x = (particle.scale / 2);
		quad.verticies[2].y = 0.0f;
		quad.verticies[2].z = (particle.scale / 2);

		quad.verticies[3].x = -(particle.scale / 2);
		quad.verticies[3].y = 0.0f;
		quad.verticies[3].z = (particle.scale / 2);
	}

	// TopLeft, TopRight, BottomRight, BottomLeft tex coords particle
	quad.texture_S[0] = 0.0f;
	quad.texture_T[0] = 0.0f;

	quad.texture_S[1] = 1.0f;
	quad.texture_T[1] = 0.0f;

	quad.texture_S[2] = 1.0f;
	quad.texture_T[2] = 1.0f;

	quad.texture_S[3] = 0.0f;
	quad.texture_T[3] = 1.0f;

	// That should be everything, the quad should be all set up...
	return quad;
}

///<summary>
/// Renders all active particles using IGraphics implementation, and offset by the specified vector (e.g. for tilemap position)
///</summary>													
void ParticleEffect::renderParticles(Vector offset, IGraphics *graphics)
{

	ASSERT(myTextureID != UNDEFINED_TEXTURE_HANDLE);

	// Iterate through our particles and render them!
	std::vector<Particle>::iterator particleIterator;

	particleIterator = myParticlePool.begin();
	while (particleIterator != myParticlePool.end())
	{
		if (particleIterator->lifeSpan > 0)
		{
			std::map<int, Quad> quads;
			Quad renderingQuad = setupQuad(myEffectType, *particleIterator);
			quads[0] = renderingQuad;
			Vector renderingLocation;
			renderingLocation.x = (particleIterator->location.x + offset.x);
			renderingLocation.y = (particleIterator->location.y + offset.y);
			renderingLocation.z = (particleIterator->location.z + offset.z);

			// If it's one of the below types, then rotate the particle toward the camera
			if((myEffectType == PARTICLE_EFFECT_SPARK) || (myEffectType == PARTICLE_EFFECT_SPLASH))
			{
				
				Rotate rotation = {{0.0f, 0.0f, 1.0f}, graphics->getCameraAngle() };
				graphics->rotateNextObjectInScene(&rotation);
			}

			graphics->addQuadsToScene(renderingLocation, quads, quads.size(), myTextureID);
			quads.clear();
		}
		particleIterator++;
	}
}
