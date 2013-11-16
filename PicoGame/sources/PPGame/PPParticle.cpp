/*-----------------------------------------------------------------------------------------------
	名前	PPParticle.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPParticle.h"
#include "PPGameUtil.h"
#include "PPGameSprite.h"
#include <string>
#include <zlib.h>
#include <float.h>

#ifdef _WIN32
#define __USE_PPSAX__
#endif

#ifdef __USE_PPSAX__
#include "PPXMLParser.h"
typedef char PPXmlChar;
#else
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
typedef xmlChar PPXmlChar;
#endif

#ifdef __COCOS2DX__
#include "CCFileUtils.h"
#endif

#define MAXIMUM_UPDATE_RATE 90.0f	// The maximum number of updates that occur per frame
#define RANDOM_MINUS_1_TO_1() ((rand() / (RAND_MAX/2.0))-1.0f)
#define RANDOM_0_TO_1() (rand() / ((float)RAND_MAX))
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)
#define CLAMP(X, A, B) ((X < A) ? A : ((X > B) ? B : X))
#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) ((a)>(b)?(a):(b))

static void PPParticle_startElement(void *ctx, const PPXmlChar *name, const PPXmlChar **atts);
static void PPParticle_endElement(void *ctx, const PPXmlChar *name);
static void PPParticle_textHandler(void *ctx, const PPXmlChar *name, int len);
static void PPParticle_error(void * ctx,const char * msg,...);
static void PPParticle_warning( void * ctx,const char * msg,...);

#define BUFFER_INC_FACTOR (2)

static int inflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint)
{
	/* ret value */
	int err = Z_OK;

	int bufferSize = outLenghtHint;
#if 1
	*out = (unsigned char*)malloc(bufferSize);
#else
	*out = new unsigned char[bufferSize];
#endif

	z_stream d_stream; /* decompression stream */	
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = in;
	d_stream.avail_in = inLength;
	d_stream.next_out = *out;
	d_stream.avail_out = bufferSize;

	/* window size to hold 256k */
	if( (err = inflateInit2(&d_stream, 15 + 32)) != Z_OK )
		return err;

	for (;;) 
	{
		err = inflate(&d_stream, Z_NO_FLUSH);

		if (err == Z_STREAM_END)
		{
			break;
		}

		switch (err) 
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != Z_STREAM_END) {
#if 1
//			free(*out);
			*out = (unsigned char*)realloc(*out,bufferSize * BUFFER_INC_FACTOR);
#else			
			delete [] *out;
			*out = new unsigned char[bufferSize * BUFFER_INC_FACTOR];
#endif
			/* not enough memory, ouch */
			if (! *out ) 
			{
//				CCLOG("cocos2d: ZipUtils: realloc failed");
				inflateEnd(&d_stream);
				return Z_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = bufferSize;
			bufferSize *= BUFFER_INC_FACTOR;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = inflateEnd(&d_stream);
	return err;
}

static unsigned char* base64decode(const char* string,unsigned int* length)
{
	long i;
	unsigned char* buffer = NULL;
	int outsize = 0;
	long size = 0;
	if (string == NULL) return NULL;
//	if (length == 0) length = strlen(string);
	while (1) {
	
		/* バッファサイズの計算 */
		for (i=0;i<*length;i++) {
			if ((string[i] >= 'A' && string[i] <= 'Z')
			 || (string[i] >= 'a' && string[i] <= 'z')
			 || (string[i] >= '0' && string[i] <= '9')
			 || (string[i] == '+' || string[i] == '/')) {
				size ++;
			} else
			if (string[i] == '=' || string[i] == 0) {
				break;
			}
		}
		
		if (buffer==NULL) {
			long tsize = (size / 4)*3;
			switch (size % 4) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				tsize += 1;
				break;
			case 3:
				tsize += 2;
				break;
			}
			if (tsize>0) {
				buffer = (unsigned char*)malloc(tsize+1);
			}
			if (buffer == NULL) break;
		}
		
		/* エンコード */
		{
			long j;
			int s=0;
			for (i=0,j=0;i<*length;i++) {
				int c = -1;
				char a = string[i];
			
				if (a >= 'A' && a <= 'Z') {
					c = 0+a-'A';
				} else
				if (a >= 'a' && a <= 'z') {
					c = 26+a-'a';
				} else
				if (a >= '0' && a <= '9') {
					c = 52+a-'0';
				} else
				if (a == '+') {
					c = 62;
				} else
				if (a == '/') {
					c = 63;
				} else
				if (a == '=' || a == 0) {
					break;
				}
				
				if (c >= 0) {
					switch (s%4) {
					case 0:
						buffer[j] = c << 2;
						break;
					case 1:
						buffer[j] |= c >> 4;
						j ++;
						buffer[j] = c << 4;
						break;
					case 2:
						buffer[j] |= c >> 2;
						j ++;
						buffer[j] = c << 6;
						break;
					case 3:
						buffer[j] |= c;
						j ++;
						break;
					}
					outsize = (int)j;
					s ++;
				}
			}
		}
		
		break;
	}
	*length = outsize;
	return buffer;
}

void PPParticle::init(PPParticleEmitter* emitter)
{
	// Init the position of the particle.  This is based on the source position of the particle emitter
	// plus a configured variance.  The RANDOM_MINUS_1_TO_1 macro allows the number to be both positive
	// and negative
	position.x = emitter->sourcePosition.x + emitter->sourcePositionVariance.x * RANDOM_MINUS_1_TO_1();
	position.y = emitter->sourcePosition.y + emitter->sourcePositionVariance.y * RANDOM_MINUS_1_TO_1();
    startPos.x = emitter->sourcePosition.x;
    startPos.y = emitter->sourcePosition.y;
	
	// Init the direction of the particle.  The newAngle is calculated using the angle passed in and the
	// angle variance.
	float newAngle = (float)DEGREES_TO_RADIANS(emitter->angle + emitter->angleVariance * RANDOM_MINUS_1_TO_1());
	
	// Create a new Vector2f using the newAngle
	PPPoint vector = PPPoint(cosf(newAngle), sinf(newAngle));
	
	// Calculate the vectorSpeed using the speed and speedVariance which has been passed in
	float vectorSpeed = emitter->speed + emitter->speedVariance * RANDOM_MINUS_1_TO_1();
	
	// The particles direction vector is calculated by taking the vector calculated above and
	// multiplying that by the speed
	direction = vector*vectorSpeed;
	
	// Set the default diameter of the particle from the source position
	radius = emitter->maxRadius + emitter->maxRadiusVariance * RANDOM_MINUS_1_TO_1();
	if (emitter->particleLifeSpan == 0) {
		radiusDelta = 0;
	} else {
		radiusDelta = (emitter->maxRadius / emitter->particleLifeSpan) * (1.0 / MAXIMUM_UPDATE_RATE);
	}
	angle = DEGREES_TO_RADIANS(emitter->angle + emitter->angleVariance * RANDOM_MINUS_1_TO_1());
	degreesPerSecond = DEGREES_TO_RADIANS(emitter->rotatePerSecond + emitter->rotatePerSecondVariance * RANDOM_MINUS_1_TO_1());
    
    radialAcceleration = emitter->radialAcceleration;
    tangentialAcceleration = emitter->tangentialAcceleration;
	
	// Calculate the particles life span using the life span and variance passed in
	float t = emitter->particleLifeSpan + emitter->particleLifeSpanVariance * RANDOM_MINUS_1_TO_1();
	timeToLive = MAX(0, t);
	
	if (timeToLive == 0) timeToLive = FLT_MIN;

	// Calculate the particle size using the start and finish particle sizes
	float particleStartSize = emitter->startParticleSize + emitter->startParticleSizeVariance * RANDOM_MINUS_1_TO_1();
	float particleFinishSize = emitter->finishParticleSize + emitter->finishParticleSizeVariance * RANDOM_MINUS_1_TO_1();
	particleSizeDelta = ((particleFinishSize - particleStartSize) / timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
	particleSize = MAX(0, particleStartSize);
	
	// Calculate the color the particle should have when it starts its life.  All the elements
	// of the start color passed in along with the variance are used to calculate the star color
	PPFColor start(0, 0, 0, 0);
	start.r = emitter->startColor.r + emitter->startColorVariance.r * RANDOM_MINUS_1_TO_1();
	start.g = emitter->startColor.g + emitter->startColorVariance.g * RANDOM_MINUS_1_TO_1();
	start.b = emitter->startColor.b + emitter->startColorVariance.b * RANDOM_MINUS_1_TO_1();
	start.a = emitter->startColor.a + emitter->startColorVariance.a * RANDOM_MINUS_1_TO_1();
	
	start.limit();
	
	// Calculate the color the particle should be when its life is over.  This is done the same
	// way as the start color above
	PPFColor end(0, 0, 0, 0);
	end.r = emitter->finishColor.r + emitter->finishColorVariance.r * RANDOM_MINUS_1_TO_1();
	end.g = emitter->finishColor.g + emitter->finishColorVariance.g * RANDOM_MINUS_1_TO_1();
	end.b = emitter->finishColor.b + emitter->finishColorVariance.b * RANDOM_MINUS_1_TO_1();
	end.a = emitter->finishColor.a + emitter->finishColorVariance.a * RANDOM_MINUS_1_TO_1();
	
	end.limit();

	// Calculate the delta which is to be applied to the particles color during each cycle of its
	// life.  The delta calculation uses the life span of the particle to make sure that the 
	// particles color will transition from the start to end color during its life time.  As the game
	// loop is using a fixed delta value we can calculate the delta color once saving cycles in the 
	// update method
	color = start;
	deltaColor.r = ((end.r - start.r) / timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
	deltaColor.g = ((end.g - start.g) / timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
	deltaColor.b = ((end.b - start.b) / timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
	deltaColor.a = ((end.a - start.a) / timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
    
    // Calculate the rotation
    float startA = emitter->rotationStart + emitter->rotationStartVariance * RANDOM_MINUS_1_TO_1();
    float endA = emitter->rotationEnd + emitter->rotationEndVariance * RANDOM_MINUS_1_TO_1();
    rotation = startA;
    rotationDelta = (endA - startA) / timeToLive;
	
	animationTime = 0;
}

bool PPParticle::updateWithDelta(PPParticleEmitter* emitter,float aDelta)
{
	// FIX 1
	// Reduce the life span of the particle
	timeToLive -= aDelta;
	
	// If the current particle is alive then update it
	if(timeToLive > 0) {
		
		// If maxRadius is greater than 0 then the particles are going to spin otherwise
		// they are effected by speed and gravity
		if (emitter->emitterType == kParticleTypeRadial) {
			
			// FIX 2
			// Update the angle of the particle from the sourcePosition and the radius.  This is only
			// done of the particles are rotating
			angle += degreesPerSecond * aDelta;
			radius -= radiusDelta;
			
			PPPoint tmp;
			tmp.x = emitter->sourcePosition.x - cosf(angle) * radius;
			tmp.y = emitter->sourcePosition.y - sinf(angle) * radius;
			position = tmp;
			
			if (radius < emitter->minRadius)
				timeToLive = 0;
		} else {
			PPPoint tmp, radial, tangential;
			
			radial = PPPointZero;
			PPPoint diff = startPos-PPPointZero;
			
			position = position-diff;
			
			if (position.x || position.y) {
				radial = position.normal();
			}
			
			tangential.x = radial.x;
			tangential.y = radial.y;
			radial = radial*radialAcceleration;
			
			float newy = tangential.x;
			tangential.x = -tangential.y;
			tangential.y = newy;
			tangential = tangential*tangentialAcceleration;
			
			tmp = radial+tangential+emitter->gravity;
			tmp = tmp*aDelta;
			direction = direction+tmp;
			tmp = direction*aDelta;
			position = position+tmp;
			position = position+diff;
		}
		
		// Update the particles color
		color.r += deltaColor.r*(aDelta*MAXIMUM_UPDATE_RATE);
		color.g += deltaColor.g*(aDelta*MAXIMUM_UPDATE_RATE);
		color.b += deltaColor.b*(aDelta*MAXIMUM_UPDATE_RATE);
		color.a += deltaColor.a*(aDelta*MAXIMUM_UPDATE_RATE);
		
		// Update the particle size
		particleSize += particleSizeDelta;
		
		// Update the rotation of the particle
		rotation += (rotationDelta * aDelta);
		
		return true;
	}
	return false;
}

#pragma mark -

PPParticleEmitter::PPParticleEmitter(PPWorld* world) : PPObject(world),particles(NULL),active(false),particleCount(0),textureData(NULL)
{
	poly._texture = -1;
	animationData = NULL;
	animationDataLength = 0;
	animationLoopPoint = 1;
#ifdef _OBJMEM_DEBUG_
  objname="PPParticleEmitter";
  printf("alloc %s\n",objname);
  fflush(stdout);
#endif
}

PPParticleEmitter::~PPParticleEmitter()
{
//	if (poly._texture >= 0) {
//		world()->projector->textureManager->deleteTexture(poly._texture);
//		poly._texture = -1;
//	}
	if (particles) delete []particles;
	if (textureData) free(textureData);
	if (animationData) free(animationData);
}

void PPParticleEmitter::init(PPWorld* world)
{
	PPObject::init(world);
//	if (poly._texture >= 0) {
//		world->projector->textureManager->deleteTexture(poly._texture);
//		poly._texture = -1;
//	}
	if (particles) delete []particles;
	if (textureData) free(textureData);
}

void PPParticleEmitter::start()
{
	emitCounter = 0;
	PPObject::start();


	emitterType=0;
	speed=50;
	speedVariance=11;
	particleLifeSpan=0.5;
	particleLifeSpanVariance=0;
	angle=0;
	angleVariance=360;
	radialAcceleration=0;
	tangentialAcceleration=0;
	radialAccelVariance=0;
	tangentialAccelVariance=0;
	maxParticles=100;
	startParticleSize=32;
	startParticleSizeVariance=0;
	finishParticleSize=32;
	finishParticleSizeVariance=0;
	duration=0.1;
	blendFuncSource=770;
	blendFuncDestination=771;
	maxRadius=10;
	maxRadiusVariance=0;
	radiusSpeed=0;
	minRadius=10;
	rotatePerSecond=0;
	rotatePerSecondVariance=0;
	rotationStart=0;
	rotationStartVariance=0;
	rotationEnd=0;
	rotationEndVariance=0;
	sourcePosition=PPPoint(160,160);
	sourcePositionVariance=PPPoint(7,7);
	gravity=PPPoint(0,0);
	startColor=PPFColor(1,1,1,1);
	startColorVariance=PPFColor(0,0,0,0);
	finishColor=PPFColor(1,1,1,0);
	finishColorVariance=PPFColor(0,0,0,0);
	animationTime=1/60.0;
//	animationData=ti.tile[3],
	animationLoopPoint=1;

	// Calculate the emission rate
	emissionRate = maxParticles / particleLifeSpan;

	if (particles) delete[] particles;
	particles = NULL;
	if (maxParticles > 0) {
		particles = new PPParticle[maxParticles+1];
	}

	active = false;
	particleCount = 0;
	elapsedTime = 0;

	NEXT(PPParticleEmitter::stepIdle);
}

bool PPParticleEmitter::config(const char* xmlpath)
{
//printf("---------config in\n");
	if (textureData) free(textureData);
	textureData = NULL;
	
	particleName = xmlpath;
	
	if (animationData) free(animationData);
	animationData = NULL;
	animationDataLength = 0;
	animationLoopPoint = 1;

	particleCount = 0;
	
    emitterType = 0;
	sourcePosition = PPPointZero;
	sourcePositionVariance = PPPointZero;
	speed = 0;
	speedVariance = 0;
	particleLifeSpan = 1;
	particleLifeSpanVariance = 0;
	angle = 0;
	angleVariance = 0;
	gravity = PPPointZero;
    radialAcceleration = 0;
    tangentialAcceleration = 0;
	radialAccelVariance = 0;
	tangentialAccelVariance = 0;
	startColor = PPFColor(0,0,0,0);
	startColorVariance = PPFColor(0,0,0,0);
	finishColor = PPFColor(0,0,0,0);
	finishColorVariance = PPFColor(0,0,0,0);
	maxParticles = 10;
	startParticleSize = 0;
	startParticleSizeVariance = 0;
	finishParticleSize = 0;
	finishParticleSizeVariance = 0;
	duration = 0;
	blendFuncSource = 0;
    blendFuncDestination = 0;
	
	// These paramters are used when you want to have the particles spinning around the source location
	maxRadius = 0;
	maxRadiusVariance = 0;
	radiusSpeed = 0;
	minRadius = 0;
	rotatePerSecond = 0;
	rotatePerSecondVariance = 0;
    
    rotationStart = 0;
    rotationStartVariance = 0;
    rotationEnd = 0;
    rotationEndVariance = 0;
	
	emitCounter = 0;
	
	char* str = NULL;
	unsigned long size=0;

#ifdef __COCOS2DX__
	str = (char*)cocos2d::CCFileUtils::getFileData(PPGameResourcePath(xmlpath),"r",&size);
	if (str == NULL) {
		str = (char*)cocos2d::CCFileUtils::getFileData(xmlpath,"r",&size);
	}
#else
	FILE* fp = fopen(PPGameDataPath(xmlpath),"r");
//printf("PPGameDataPath:%s\n",PPGameDataPath(xmlpath));
	if (fp == NULL) {
		fp = fopen(PPGameResourcePath(xmlpath),"r");
//printf("PPGameResourcePath:%s\n",PPGameResourcePath(xmlpath));
	}
	if (fp) {
		fseek(fp,0,SEEK_END);
		size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		str = (char*)malloc(size+1);
		fread(str,size,1,fp);
		str[size] = 0;
		fclose(fp);
	}
#endif

	if (str==NULL) {
		PPReadErrorSet(xmlpath);
		return false;
	}

#ifdef __USE_PPSAX__
	PPSAXHandler saxHandler;
	memset( &saxHandler, 0, sizeof(PPSAXHandler) );
	saxHandler.startElement = &PPParticle_startElement;
	saxHandler.endElement = &PPParticle_endElement;
	saxHandler.characters = &PPParticle_textHandler;
	saxHandler.userdata = this;
	saxHandler.xmltext = str;
	PPSAXParser(&saxHandler);
#else
//	xmlParserCtxtPtr ctxt = xmlCreateDocParserCtxt((const xmlChar*)str);
	xmlSAXHandler saxHandler;
	memset( &saxHandler, 0, sizeof(saxHandler) );
	saxHandler.initialized = XML_SAX2_MAGIC;
	saxHandler.startElement = &PPParticle_startElement;
	saxHandler.endElement = &PPParticle_endElement;
	saxHandler.characters = &PPParticle_textHandler;
	saxHandler.warning = &PPParticle_warning;
	saxHandler.error = &PPParticle_error;
	int result = xmlSAXUserParseMemory(&saxHandler,this,str,(int)size);
	if ( result != 0 )
	{
		return false;
	}
//	xmlCleanupThreads();
//	xmlFreeParserCtxt(ctxt);
//	xmlCleanupMemory();
//	xmlCleanupParser();
	xmlCleanupGlobals();
//	xmlMemoryDump();
#endif

#ifdef __COCOS2DX__
	delete str;
#else
	free(str);
#endif		

	// Calculate the emission rate
	emissionRate = maxParticles / particleLifeSpan;

	if (particles) delete[] particles;
	particles = NULL;
	if (maxParticles > 0) {
		particles = new PPParticle[maxParticles+1];
	}

	active = false;
	particleCount = 0;
	elapsedTime = 0;

	return true;
}

void PPParticleEmitter::stepIdle()
{
	if (particles) {
		updateWithDelta(1.0/60.0);
	}
}

void PPParticleEmitter::updateWithDelta(float aDelta)
{
	if (particles == NULL) return;

	// If the emitter is active and the emission rate is greater than zero then emit
	// particles
	if(active && emissionRate) {
		float rate = 1.0f/emissionRate;
		emitCounter += aDelta;
		while(particleCount < maxParticles && emitCounter > rate) {
			addParticle();
			emitCounter -= rate;
		}

		elapsedTime += aDelta;
		if(duration != -1 && duration < elapsedTime) {
			stopParticleEmitter();
		}
	}
	
    // Loop through all the particles updating their location and color
	for (int i=0;i<particleCount;i++) {
		if (particles[i].updateWithDelta(this,aDelta)) {
			
		} else {
			if(i != particleCount - 1)
				particles[i] = particles[particleCount - 1];
			particleCount--;
		}
	}
}

void PPParticleEmitter::drawSelf(PPPoint _pos)
{
	if (particles == NULL) return;
	
	GLuint texture_name = world()->projector->textureManager->getTextureName(poly._texture);
	if (texture_name == 0) {
		loadPNGData(textureData,textureDataSize);
	}
	
	float animeDelta = 0;
	if (animationDataLength > 0 && animationData != NULL) {
		animeDelta = (animationDataLength/60.0)/animationTime;
		if (animationLoopPoint <= 0) animationLoopPoint = 1;
		if (animationLoopPoint > animationDataLength) animationLoopPoint = animationDataLength;
	}

	PPSize tiles = tileSize()/2;
	if (tiles.width == 0 || tiles.height == 0) return;
	PPPoint center = PPPoint(tiles.width,tiles.height);
	PPPoint half = PPPoint(tileSize().width,tileSize().height)/2;
	PPPoint p = autoLayout(pos)+_pos-half;
	for (int i=0;i<particleCount;i++) {
		poly.rotate = particles[i].rotation*M_PI/180.0;
		poly.color = particles[i].color.byteColor();
		PPPoint _p = particles[i].position+p;
		poly.origin = center+_p;
		if (animationDataLength > 0 && animationData != NULL) {
			int ptr = (int)(particles[i].animationTime);
			if (ptr < 0) ptr = 0;
			if (ptr >= animationDataLength) {
				ptr = animationLoopPoint-1;
				particles[i].animationTime = animationLoopPoint-1;
			} else {
				particles[i].animationTime += animeDelta;
			}
			if (ptr < 0) ptr = 0;
			if (ptr >= animationDataLength) ptr = animationDataLength-1;
			poly.sprite(_p.x,_p.y,animation.key()+animationData[ptr],0);
		} else {
			poly.sprite(_p.x,_p.y,animation.key(),0);
		}
		poly.scale.x = (particles[i].particleSize/tiles.width )/2;
		poly.scale.y = (particles[i].particleSize/tiles.height)/2;
		poly.blend.blend = true;
		poly.blend.blendSrc = blendFuncSource;
		poly.blend.blendDst = blendFuncDestination;
		world()->projector->DrawPoly(&poly);
	}
}

bool PPParticleEmitter::addParticle()
{
	if (particles == NULL) return false;

	// If we have already reached the maximum number of particles then do nothing
	if(particleCount == maxParticles)
		return false;
	
	// Take the next particle out of the particle pool we have created and initialize it
	PPParticle *particle = &particles[particleCount];
	particle->init(this);
	
	// Increment the particle count
	particleCount++;
	
	// Return YES to show that a particle has been created
	return true;
}

void PPParticleEmitter::stopParticleEmitter()
{
	active = false;
	elapsedTime = 0;
	emitCounter = 0;
}

bool PPParticleEmitter::loadPNGData(unsigned char* pngZipData,unsigned long length)
{
	bool r = false;
	if (textureData != pngZipData) {
		if (textureData) free(textureData);
	}
	textureData = pngZipData;
	textureDataSize = length;
	if (textureData) {
		unsigned char* pngdata;
		unsigned int pnglength;
		unsigned int loadlen = (unsigned int)length;
		int err=inflateMemoryWithHint(textureData,loadlen,&pngdata,&pnglength,32);
		if (err == 0) {
			unsigned long width,height,bytesPerRow;
			unsigned char* pixel = PPGame_DecodePNG(pngdata,pnglength,&width,&height,&bytesPerRow);

			if (pixel!=NULL) {
//				if (poly._texture >= 0) {
//					world()->projector->textureManager->deleteTexture(poly._texture);
//					poly._texture = -1;
//				}

				PPGameTexture* tex = new PPGameTexture();
				tex->name = particleName;
				tex->option = option;
				tex->notexturefile = true;

				poly._texture = world()->projector->textureManager->setTextureWithNameCheck(tex);
#if 1
				world()->projector->textureManager->setTexture(poly._texture,pixel,width,height,bytesPerRow,option);
				tex->bindTexture();

//				free(pixel);
//				tex->pixel = NULL;

				r = true;
				poly.texTileSize.width = tex->width;
				poly.texTileSize.height = tex->height;
#else
				if (!world()->projector->textureManager->setTexture(poly._texture,pixel,width,height,bytesPerRow)) {
					PPGameTexture* tex = new PPGameTexture();
					poly._texture = world()->projector->textureManager->setTexture(tex);
					world()->projector->textureManager->setTexture(poly._texture,pixel,width,height,bytesPerRow);
//					world()->projector->textureManager->bindTexture(poly._texture);
					r = true;
				}
#endif

			}
		}
	}
	return r;
}

static const char* valueForKey(const char* key,const PPXmlChar **atts)
{
	if (atts) {
		if (atts[0]) {
			for(int i=0;atts[i];i+=2) {
				if (strcmp((char*)atts[i],(char*)key) == 0) {
					return (char*)atts[i+1];
				}
			}
		}
	}
	return "";
}

static void PPParticle_startElement(void *ctx, const PPXmlChar *name, const PPXmlChar **atts)
{
	PPParticleEmitter* p = (PPParticleEmitter*)ctx;
	std::string elementName = (char*)name;

	if(strcasecmp(elementName.c_str(),"emitterType") == 0) {
		p->emitterType = atoi(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"sourcePosition") == 0) {
		p->sourcePosition.x = atof(valueForKey("x",atts));
		p->sourcePosition.y = atof(valueForKey("y",atts));
	}else
	if(strcasecmp(elementName.c_str(),"sourcePositionVariance") == 0) {
		p->sourcePositionVariance.x = atof(valueForKey("x",atts));
		p->sourcePositionVariance.y = atof(valueForKey("y",atts));
	}else
	if(strcasecmp(elementName.c_str(),"speed") == 0) {
		p->speed = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"speedVariance") == 0) {
		p->speedVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"particleLifeSpan") == 0) {
		p->particleLifeSpan = atof(valueForKey("value",atts));
		if (p->particleLifeSpan <= 0) p->particleLifeSpan = FLT_MIN;
	}else
	if(strcasecmp(elementName.c_str(),"particleLifespanVariance") == 0) {
		p->particleLifeSpanVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"angle") == 0) {
		p->angle = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"angleVariance") == 0) {
		p->angleVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"gravity") == 0) {
		p->gravity.x = atof(valueForKey("x",atts));
		p->gravity.y = atof(valueForKey("y",atts));
	}else
	if(strcasecmp(elementName.c_str(),"radialAcceleration") == 0) {
		p->radialAcceleration = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"tangentialAcceleration") == 0) {
		p->tangentialAcceleration = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"radialAccelVariance") == 0) {
		p->radialAccelVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"tangentialAccelVariance") == 0) {
		p->tangentialAccelVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"startColor") == 0) {
		p->startColor.r = atof(valueForKey("red",atts));
		p->startColor.g = atof(valueForKey("green",atts));
		p->startColor.b = atof(valueForKey("blue",atts));
		p->startColor.a = atof(valueForKey("alpha",atts));
	}else
	if(strcasecmp(elementName.c_str(),"startColorVariance") == 0) {
		p->startColorVariance.r = atof(valueForKey("red",atts));
		p->startColorVariance.g = atof(valueForKey("green",atts));
		p->startColorVariance.b = atof(valueForKey("blue",atts));
		p->startColorVariance.a = atof(valueForKey("alpha",atts));
	}else
	if(strcasecmp(elementName.c_str(),"finishColor") == 0) {
		p->finishColor.r = atof(valueForKey("red",atts));
		p->finishColor.g = atof(valueForKey("green",atts));
		p->finishColor.b = atof(valueForKey("blue",atts));
		p->finishColor.a = atof(valueForKey("alpha",atts));
	}else
	if(strcasecmp(elementName.c_str(),"finishColorVariance") == 0) {
		p->finishColorVariance.r = atof(valueForKey("red",atts));
		p->finishColorVariance.g = atof(valueForKey("green",atts));
		p->finishColorVariance.b = atof(valueForKey("blue",atts));
		p->finishColorVariance.a = atof(valueForKey("alpha",atts));
	}else
	if(strcasecmp(elementName.c_str(),"maxParticles") == 0) {
		p->maxParticles = atoi(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"startParticleSize") == 0) {
		p->startParticleSize = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"startParticleSizeVariance") == 0) {
		p->startParticleSizeVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"finishParticleSize") == 0) {
		p->finishParticleSize = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"finishParticleSizeVariance") == 0) {
		p->finishParticleSizeVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"duration") == 0) {
		p->duration = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"blendFuncSource") == 0) {
		p->blendFuncSource = atoi(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"blendFuncDestination") == 0) {
		p->blendFuncDestination = atoi(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"maxRadius") == 0) {
		p->maxRadius = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"maxRadiusVariance") == 0) {
		p->maxRadiusVariance = atof(valueForKey("value",atts));
//	}else
//	if(elementName == "radiusSpeed") {
//		p->radiusSpeed = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"minRadius") == 0) {
		p->minRadius = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"rotatePerSecond") == 0) {
		p->rotatePerSecond = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"rotatePerSecondVariance") == 0) {
		p->rotatePerSecondVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"rotationStart") == 0) {
		p->rotationStart = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"rotationStartVariance") == 0) {
		p->rotationStartVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"rotationEnd") == 0) {
		p->rotationEnd = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"rotationEndVariance") == 0) {
		p->rotationEndVariance = atof(valueForKey("value",atts));
	}else
	if(strcasecmp(elementName.c_str(),"texture") == 0) {
		p->textureName = valueForKey("name",atts);
		std::string* dataStr = new std::string(valueForKey("data",atts));
		unsigned int length = (unsigned int)dataStr->length();

		unsigned char* pixel = base64decode(dataStr->c_str(),&length);

		if (!p->loadPNGData(pixel,length)) {
		}
		if (length==0) {
			PPGameTextureOption option;
			PPGameTextureManager* manager = p->world()->projector->textureManager;
			int texture = manager->loadTexture(p->textureName.c_str(),option);
			if (texture>=0) {
				p->poly.initTexture(texture);
				p->poly.texTileSize.width = manager->texture[texture]->width;
				p->poly.texTileSize.height = manager->texture[texture]->height;
			}
			PPReadErrorReset();
		}
		delete dataStr;
	}
}

static void PPParticle_endElement(void *ctx, const PPXmlChar *name)
{
}

static void PPParticle_textHandler(void *ctx, const PPXmlChar *name, int len)
{
}

static void PPParticle_error(void * ctx,const char * msg,...)
{
  //ParseFSM &fsm = *( static_cast<ParseFSM *>( ctx ) );
  va_list args;
  va_start(args, msg);
  vprintf( msg, args );
  va_end(args);
}

static void PPParticle_warning( void * ctx,const char * msg,...)
{
  //ParseFSM &fsm = *( static_cast<ParseFSM *>( ctx ) );
  va_list args;
  va_start(args, msg);
  vprintf( msg, args );
  va_end(args);
}

#pragma mark -

#include "PPLuaScript.h"

static int funcLoad(lua_State* L)
{
	PPParticleEmitter* m = (PPParticleEmitter*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPParticleEmitter* m = (PPParticleEmitter*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 1) {
		if (s->isTable(L,1)) {
			m->option = s->getTextureOption(L,1,m->option);
		}
	}
	lua_pushboolean(L,m->config(s->args(0)));
	return 1;
}

#define PT_NUM(name,member) if (s->tableMember(L,0,name)) {m->member = s->tableNumber(L,0,name);}
#define PT_INT(name,member) if (s->tableMember(L,0,name)) {m->member = (int)s->tableInteger(L,0,name);}

static int funcSet(lua_State* L)
{
	PPParticleEmitter* m = (PPParticleEmitter*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPParticleEmitter* m = (PPParticleEmitter*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}

	if (s->isTable(L,0)) {
		PT_INT("emitterType",emitterType);
		PT_NUM("speed",speed);
		PT_NUM("speedVariance",speedVariance);
		PT_NUM("particleLifeSpan",particleLifeSpan);
		PT_NUM("particleLifeSpanVariance",particleLifeSpanVariance);
		PT_NUM("particleLifespan",particleLifeSpan);
		PT_NUM("particleLifespanVariance",particleLifeSpanVariance);
		PT_NUM("angle",angle);
		PT_NUM("angleVariance",angleVariance);
		PT_NUM("radialAcceleration",radialAcceleration);
		PT_NUM("tangentialAcceleration",tangentialAcceleration);
		PT_NUM("radialAccelVariance",radialAccelVariance);
		PT_NUM("tangentialAccelVariance",tangentialAccelVariance);
		
		int maxParticles = m->maxParticles;
		PT_INT("maxParticles",maxParticles);
		if (maxParticles != m->maxParticles) {
			PPParticle* particles = new PPParticle[m->maxParticles];
			if (m->particles) {
				for (int i=0;i<m->maxParticles&&i<maxParticles;i++) {
					particles[i] = m->particles[i];
				}
				delete[] m->particles;
			}
			m->particles = particles;
			if (m->particleCount > m->maxParticles) m->particleCount = m->maxParticles;
		}

		PT_NUM("startParticleSize",startParticleSize);
		PT_NUM("startParticleSizeVariance",startParticleSizeVariance);
		PT_NUM("finishParticleSize",finishParticleSize);
		PT_NUM("finishParticleSizeVariance",finishParticleSizeVariance);
		PT_NUM("duration",duration);
		PT_INT("blendFuncSource",blendFuncSource);
		PT_INT("blendFuncDestination",blendFuncDestination);
		PT_NUM("maxRadius",maxRadius);
		PT_NUM("maxRadiusVariance",maxRadiusVariance);
//		PT_NUM("radiusSpeed",radiusSpeed);
		PT_NUM("minRadius",minRadius);
		PT_NUM("rotatePerSecond",rotatePerSecond);
		PT_NUM("rotatePerSecondVariance",rotatePerSecondVariance);
		PT_NUM("rotationStart",rotationStart);
		PT_NUM("rotationStartVariance",rotationStartVariance);
		PT_NUM("rotationEnd",rotationEnd);
		PT_NUM("rotationEndVariance",rotationEndVariance);
		m->sourcePosition.x = s->tableFieldNumber(L,0,"sourcePosition","x",1,m->sourcePosition.x);
		m->sourcePosition.y = s->tableFieldNumber(L,0,"sourcePosition","y",2,m->sourcePosition.y);
		m->sourcePositionVariance.x = s->tableFieldNumber(L,0,"sourcePositionVariance","x",1,m->sourcePositionVariance.x);
		m->sourcePositionVariance.y = s->tableFieldNumber(L,0,"sourcePositionVariance","y",2,m->sourcePositionVariance.y);
		m->gravity.x = s->tableFieldNumber(L,0,"gravity","x",1,m->gravity.x);
		m->gravity.y = s->tableFieldNumber(L,0,"gravity","y",2,m->gravity.y);
		m->startColor.r = s->tableFieldNumber(L,0,"startColor","r",1,m->startColor.r*255)/255.0;
		m->startColor.g = s->tableFieldNumber(L,0,"startColor","g",2,m->startColor.g*255)/255.0;
		m->startColor.b = s->tableFieldNumber(L,0,"startColor","b",3,m->startColor.b*255)/255.0;
		m->startColor.a = s->tableFieldNumber(L,0,"startColor","a",4,m->startColor.a*255)/255.0;
		m->startColorVariance.r = s->tableFieldNumber(L,0,"startColorVariance","r",1,m->startColorVariance.r*255)/255.0;
		m->startColorVariance.g = s->tableFieldNumber(L,0,"startColorVariance","g",2,m->startColorVariance.g*255)/255.0;
		m->startColorVariance.b = s->tableFieldNumber(L,0,"startColorVariance","b",3,m->startColorVariance.b*255)/255.0;
		m->startColorVariance.a = s->tableFieldNumber(L,0,"startColorVariance","a",4,m->startColorVariance.a*255)/255.0;
		m->finishColor.r = s->tableFieldNumber(L,0,"finishColor","r",1,m->finishColor.r*255)/255.0;
		m->finishColor.g = s->tableFieldNumber(L,0,"finishColor","g",2,m->finishColor.g*255)/255.0;
		m->finishColor.b = s->tableFieldNumber(L,0,"finishColor","b",3,m->finishColor.b*255)/255.0;
		m->finishColor.a = s->tableFieldNumber(L,0,"finishColor","a",4,m->finishColor.a*255)/255.0;
		m->finishColorVariance.r = s->tableFieldNumber(L,0,"finishColorVariance","r",1,m->finishColorVariance.r*255)/255.0;
		m->finishColorVariance.g = s->tableFieldNumber(L,0,"finishColorVariance","g",2,m->finishColorVariance.g*255)/255.0;
		m->finishColorVariance.b = s->tableFieldNumber(L,0,"finishColorVariance","b",3,m->finishColorVariance.b*255)/255.0;
		m->finishColorVariance.a = s->tableFieldNumber(L,0,"finishColorVariance","a",4,m->finishColorVariance.a*255)/255.0;

		if (m->particleLifeSpan <= 0) m->particleLifeSpan = FLT_MIN;

		m->emissionRate = m->maxParticles / m->particleLifeSpan;

		{
			lua_getfield(L,2,"animationData");
			if (lua_istable(L,-1)) {
#ifdef __LUAJIT__
				int len= (int)lua_objlen(L,-1);
#else
				lua_len(L,-1);
				int len = (int)lua_tointeger(L,-1);
				lua_pop(L,1);
#endif
				if (m->animationData) free(m->animationData);
				m->animationData = (int*)calloc(1,sizeof(int)*len);
				m->animationDataLength = len;
				for (int i=0;i<len;i++) {
					lua_rawgeti(L,-1,i+1);
					m->animationData[i] = (int)lua_tointeger(L,-1)-1;
					lua_pop(L,1);
				}
			}
			lua_pop(L,1);
		}
		
		PT_NUM("animationTime",animationTime);
		if (m->animationTime <= 0) m->animationTime = 1/60.0;
		PT_INT("animationLoopPoint",animationLoopPoint);
		if (m->animationLoopPoint <= 0) m->animationLoopPoint = 1;
		if (m->animationLoopPoint > m->animationDataLength) m->animationLoopPoint = m->animationDataLength;
	} else {
		lua_createtable(L,0,35);
		lua_pushinteger(L,m->emitterType);
		lua_setfield(L,-2,"emitterType");
		lua_pushnumber(L,m->speed);
		lua_setfield(L,-2,"speed");
		lua_pushnumber(L,m->speedVariance);
		lua_setfield(L,-2,"speedVariance");
		lua_pushnumber(L,m->particleLifeSpan);
		lua_setfield(L,-2,"particleLifespan");
		lua_pushnumber(L,m->particleLifeSpanVariance);
		lua_setfield(L,-2,"particleLifespanVariance");
		lua_pushnumber(L,m->angle);
		lua_setfield(L,-2,"angle");
		lua_pushnumber(L,m->angleVariance);
		lua_setfield(L,-2,"angleVariance");
		lua_pushnumber(L,m->radialAcceleration);
		lua_setfield(L,-2,"radialAcceleration");
		lua_pushnumber(L,m->tangentialAcceleration);
		lua_setfield(L,-2,"tangentialAcceleration");
		lua_pushnumber(L,m->radialAccelVariance);
		lua_setfield(L,-2,"radialAccelVariance");
		lua_pushnumber(L,m->tangentialAccelVariance);
		lua_setfield(L,-2,"tangentialAccelVariance");

		lua_pushinteger(L,m->maxParticles);
		lua_setfield(L,-2,"maxParticles");

		lua_pushnumber(L,m->startParticleSize);
		lua_setfield(L,-2,"startParticleSize");
		lua_pushnumber(L,m->startParticleSizeVariance);
		lua_setfield(L,-2,"startParticleSizeVariance");
		lua_pushnumber(L,m->finishParticleSize);
		lua_setfield(L,-2,"finishParticleSize");
		lua_pushnumber(L,m->finishParticleSizeVariance);
		lua_setfield(L,-2,"finishParticleSizeVariance");
		lua_pushnumber(L,m->duration);
		lua_setfield(L,-2,"duration");
		lua_pushinteger(L,m->blendFuncSource);
		lua_setfield(L,-2,"blendFuncSource");
		lua_pushinteger(L,m->blendFuncDestination);
		lua_setfield(L,-2,"blendFuncDestination");
		lua_pushnumber(L,m->maxRadius);
		lua_setfield(L,-2,"maxRadius");
		lua_pushnumber(L,m->maxRadiusVariance);
		lua_setfield(L,-2,"maxRadiusVariance");
		lua_pushnumber(L,m->minRadius);
		lua_setfield(L,-2,"minRadius");
		lua_pushnumber(L,m->rotatePerSecond);
		lua_setfield(L,-2,"rotatePerSecond");
		lua_pushnumber(L,m->rotatePerSecondVariance);
		lua_setfield(L,-2,"rotatePerSecondVariance");
		lua_pushnumber(L,m->rotationStart);
		lua_setfield(L,-2,"rotationStart");
		lua_pushnumber(L,m->rotationStartVariance);
		lua_setfield(L,-2,"rotationStartVariance");
		lua_pushnumber(L,m->rotationEnd);
		lua_setfield(L,-2,"rotationEnd");
		lua_pushnumber(L,m->rotationEndVariance);
		lua_setfield(L,-2,"rotationEndVariance");
		
		s->pushPoint(L,m->sourcePosition);
		lua_setfield(L,-2,"sourcePosition");
		s->pushPoint(L,m->sourcePositionVariance);
		lua_setfield(L,-2,"sourcePositionVariance");
		s->pushPoint(L,m->gravity);
		lua_setfield(L,-2,"gravity");
		
		PPColor startColor(m->startColor);
		s->pushColor(L,startColor);
		lua_setfield(L,-2,"startColor");
		PPColor startColorVariance(m->startColorVariance);
		s->pushColor(L,startColorVariance);
		lua_setfield(L,-2,"startColorVariance");
		PPColor finishColor(m->finishColor);
		s->pushColor(L,finishColor);
		lua_setfield(L,-2,"finishColor");
		PPColor finishColorVariance(m->finishColorVariance);
		s->pushColor(L,finishColorVariance);
		lua_setfield(L,-2,"finishColorVariance");
		
		if (m->animationDataLength>0) {
			lua_createtable(L,m->animationDataLength,0);
			for (int i=0;i<m->animationDataLength;i++) {
				lua_pushinteger(L,m->animationData[i]);
				lua_rawseti(L,-2,i+1);
			}
			lua_setfield(L,-2,"animationData");
		}

		lua_pushnumber(L,m->animationTime);
		lua_setfield(L,-2,"animationTime");
		lua_pushinteger(L,m->animationLoopPoint);
		lua_setfield(L,-2,"animationLoopPoint");
		return 1;
	}
	return 0;
}

//static int funcReset(lua_State* L)
//{
//	return 0;
//}

static int funcElapsedTime(lua_State* L)
{
	PPParticleEmitter* m = (PPParticleEmitter*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPParticleEmitter* m = (PPParticleEmitter*)s->userdata;
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->elapsedTime = s->number(0,m->elapsedTime);
		return 0;
	}
	lua_pushnumber(L,m->elapsedTime);
	return 1;
}

static int funcFire(lua_State* L)
{
	PPParticleEmitter* m = (PPParticleEmitter*)PPLuaScript::UserData(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	m->active = true;
	m->elapsedTime = 0;
	return 0;
}

static int funcParticleCount(lua_State* L)
{
	PPParticleEmitter* m = (PPParticleEmitter*)PPLuaScript::UserData(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	lua_pushinteger(L,m->particleCount);
	return 1;
}

static int funcStop(lua_State* L)
{
	PPParticleEmitter* m = (PPParticleEmitter*)PPLuaScript::UserData(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	m->particleCount = 0;
	m->stopParticleEmitter();
	return 0;
}

static int funcDelete(lua_State *L)
{
	delete (PPParticleEmitter*)(PPLuaScript::DeleteObject(L));
	return 0;
}

static int funcNew(lua_State *L)
{
	PPWorld* world = PPLuaScript::World(L);
//	PPParticleEmitter* o = (PPParticleEmitter*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPObject* o = (PPObject*)s->userdata;
	if (s->argCount > 0) {
		lua_createtable(L,(int)s->integer(0),0);
		int table = lua_gettop(L);
		for (int i=0;i<s->integer(0);i++) {
			PPParticleEmitter* obj = new PPParticleEmitter(world);
			obj->start();
			obj->poly.initTexture(world->projector->textureManager->defaultTexture);
			PPLuaScript::newObject(L,PPParticleEmitter::className.c_str(),obj,funcDelete);
			lua_rawseti(L,table,i+1);
		}
	} else {
		PPParticleEmitter* obj = new PPParticleEmitter(world);
		obj->start();
		obj->poly.initTexture(world->projector->textureManager->defaultTexture);
		PPLuaScript::newObject(L,PPParticleEmitter::className.c_str(),obj,funcDelete);
	}
	return 1;
}

std::string PPParticleEmitter::className = "PPParticleEmitter";

PPObject* PPParticleEmitter::registClass(PPLuaScript* script,const char* name,const char* superclass)
{
	if (name) PPParticleEmitter::className = name;
	return PPParticleEmitter::registClass(script,PPParticleEmitter::className.c_str(),new PPParticleEmitter(script->world()),superclass);
}

PPObject* PPParticleEmitter::registClass(PPLuaScript* s,const char* name,PPObject* obj,const char* superclass)
{
//	PPObject::registClass(s,name,obj);
	s->openModule(name,obj,0,superclass);
		s->addCommand("new",funcNew);
		s->addCommand("load",funcLoad);
//		s->addCommand("reset",funcReset);
		s->addCommand("property",funcSet);
		s->addCommand("fire",funcFire);
		s->addCommand("elapsedTime",funcElapsedTime);
		s->addCommand("count",funcParticleCount);
		s->addCommand("stop",funcStop);
	s->closeModule();
	return obj;
}

void PPParticleEmitter::openLibrary(PPLuaScript* script,const char* name,const char* superclass)
{
	registClass(script,name,this,superclass);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
