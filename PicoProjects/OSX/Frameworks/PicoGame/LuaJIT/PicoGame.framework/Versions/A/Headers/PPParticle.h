/*-----------------------------------------------------------------------------------------------
	名前	PPParticle.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef __PPParticle_h__
#define __PPParticle_h__

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPObject.h"
#include "PPGameTexture.h"
#include <string>

enum kParticleTypes {
	kParticleTypeGravity,
	kParticleTypeRadial
};

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPLuaScript;
class PPParticleEmitter;

class PPParticle {
public:
	PPPoint position;
	PPPoint direction;
    PPPoint startPos;
	PPFColor color;
	PPFColor deltaColor;
    float rotation;
    float rotationDelta;
    float radialAcceleration;
    float tangentialAcceleration;
	float radius;
	float radiusDelta;
	float angle;
	float degreesPerSecond;
	float particleSize;
	float particleSizeDelta;
	float timeToLive;
	float animationTime;

	void init(PPParticleEmitter* emitter);
	bool updateWithDelta(PPParticleEmitter* emitter,float aDelta);
};

class PPParticleEmitter : public PPObject {
public:
	PPParticleEmitter(PPWorld* world);
	virtual ~PPParticleEmitter();
	
	virtual void init(PPWorld* world);
	virtual void start();
	
	virtual void stepIdle();

	virtual void drawSelf(PPPoint pos);

	virtual void openLibrary(PPLuaScript* script,const char* name,const char* superclass);

	static std::string className;
	static PPObject* registClass(PPLuaScript* script,const char* name=NULL,const char* superclass=NULL);
	static PPObject* registClass(PPLuaScript* script,const char* name,PPObject* obj,const char* superclass=NULL);

	bool config(const char* xmlpath);
	void updateWithDelta(float aDelta);
	bool addParticle();
	void stopParticleEmitter();
	
	int emitterType;
	PPPoint sourcePosition, sourcePositionVariance;			
	float angle, angleVariance;								
	float speed, speedVariance;	
  float radialAcceleration, tangentialAcceleration;
  float radialAccelVariance, tangentialAccelVariance;
	PPPoint gravity;	
	float particleLifeSpan, particleLifeSpanVariance;			
	PPFColor startColor, startColorVariance;						
	PPFColor finishColor, finishColorVariance;
	float startParticleSize, startParticleSizeVariance;
	float finishParticleSize, finishParticleSizeVariance;
	unsigned int maxParticles;
	int particleCount;
	float emissionRate;
	float emitCounter;	
	float elapsedTime;
	float duration;
  float rotationStart, rotationStartVariance;
  float rotationEnd, rotationEndVariance;
	
	std::string textureName;
	std::string particleName;

	int blendFuncSource, blendFuncDestination;

	float maxRadius;					// Max radius at which particles are drawn when rotating
	float maxRadiusVariance;			// Variance of the maxRadius
	float radiusSpeed;					// The speed at which a particle moves from maxRadius to minRadius
	float minRadius;					// Radius from source below which a particle dies
	float rotatePerSecond;				// Numeber of degress to rotate a particle around the source pos per second
	float rotatePerSecondVariance;		// Variance in degrees for rotatePerSecond

	bool active;
	PPParticle *particles;
	
	int* animationData;
	int animationDataLength;
	float animationTime;
	int animationLoopPoint;
	
	unsigned char* textureData;
	unsigned long textureDataSize;

	PPGameTextureOption option;
	
	bool loadPNGData(unsigned char* pngZipData,unsigned long length);
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
