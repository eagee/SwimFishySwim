// IParticleEffect.h
// Defines a common interface for implementation classes that deal with particle effects...
// Author(s): Eagan Rackley
#ifndef IPARTICLE_EFFECTS_H
#define IPARTICLE_EFFECTS_H

#include "SFSHeaders.h"
#include "ISprite.h"

///<summary>
/// Defines the various types of particle that can be created
///</summary>
typedef enum
{
	PARTICLE_EFFECT_WATER_RIPPLE,
	PARTICLE_EFFECT_REPEL_ENEMIES,
	PARTICLE_EFFECT_SPARK,
	PARTICLE_EFFECT_SPLASH,
	PARTICLE_EFFECT_WARP
} PARTICLE_EFFECT_TYPE;

///<summary>
/// Defines the contents of an individual particle
///</summary>
typedef struct tagParticle
{
	Vector location;
	Vector velocity;
	float lifeSpan;
	float scale;
	int delay;
} Particle;

///<summary>
/// Provides an interface for sprite behavior, whether that is a player sprite, or an NPC of some sort ...
///</summary>
class IParticleEffect
{ 
	public:
		IParticleEffect(){}
		
		virtual ~IParticleEffect(){}

		///<summary>
		/// Assigns an IModel instances to a sprite
		///</summary>	
		virtual void assignTexture(TextureHandle textureID) = 0;
		
		///<summary>
		/// Adds a particle (if there are particles to spare) to the list of active particles
		/// otherwise, particle is not activated
		///</summary>
		virtual void addParticles(ISprite *sprite, Vector offset, int number) = 0;

		///<summary>
		/// Updates all active particles
		///</summary>							
		virtual void updateParticles() = 0;
		
		///<summary>
		/// Renders all active particles using IGraphics implementation, and offset by the specified vector (e.g. for tilemap position)
		///</summary>											
		virtual void renderParticles(Vector offset, IGraphics *graphics) = 0;
};

#endif
