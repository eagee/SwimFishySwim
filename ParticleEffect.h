// IParticleEffects.h
// Defines a common interface for implementation classes that deal with particle effects...
// Author(s): Eagan Rackley
#ifndef PARTICLE_EFFECT_H
#define PARTICLE_EFFECT_H

#include "SFSHeaders.h"
#include "IParticleEffect.h"

///<summary>
/// Provides an interface for sprite behavior, whether that is a player sprite, or an NPC of some sort ...
///</summary>
class ParticleEffect: public IParticleEffect
{ 
	public:
		ParticleEffect(PARTICLE_EFFECT_TYPE effectType, int maxParticles);
		
		virtual ~ParticleEffect();

		///<summary>
		/// Assigns an IModel instances to a sprite
		///</summary>	
		virtual void assignTexture(TextureHandle textureID);
		
		///<summary>
		/// Adds a particle (if there are particles to spare) to the list of active particles
		/// otherwise, particle is not activated
		///</summary>
		virtual void addParticles(ISprite *sprite, Vector offset, int number);

		///<summary>
		/// Updates all active particles
		///</summary>							
		virtual void updateParticles();
		
		///<summary>
		/// Renders all active particles using IGraphics implementation, and offset by the specified vector (e.g. for tilemap position)
		///</summary>											
		virtual void renderParticles(Vector offset, IGraphics *graphics);

	private:
		TextureHandle myTextureID;
		PARTICLE_EFFECT_TYPE myEffectType;
		std::vector<Particle> myParticlePool;
		float myParticleDegrees;
		float myParticleIncrement;
};

#endif
