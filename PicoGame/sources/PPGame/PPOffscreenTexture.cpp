/*-----------------------------------------------------------------------------------------------
	名前	PPOffscreenTexture.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPOffscreenTexture.h"
#include "QBGame.h"

PPOffscreenTexture::PPOffscreenTexture(PPWorld* world) : PPObject(world),imageSize(PPSizeZero)
{
	poly._texture = -1;
	colorbuffer_name = 0;
	framebuffer_name = 0;
	_pixel = NULL;
//printf("new PPOffscreenTexture\n");
}

PPOffscreenTexture::~PPOffscreenTexture()
{
//printf("delete PPOffscreenTexture\n");
	if (poly._texture >= 0) {
		world()->projector->textureManager->deleteTexture(poly._texture);
		poly._texture = -1;
	}
	closeFrameBuffer();
	if (_pixel) free(_pixel);
}

void PPOffscreenTexture::init()
{
	if (poly._texture >= 0) {
		world()->projector->textureManager->deleteTexture(poly._texture);
		poly._texture = -1;
	}
	closeFrameBuffer();
	if (_pixel) free(_pixel);
}

void PPOffscreenTexture::start()
{
	PPObject::start();
}

static unsigned long nextPowerOfTwo(unsigned long x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}

void PPOffscreenTexture::create(PPSize size,PPGameTextureOption _option)
{
	option = _option;
	if (size.width > 0 && size.height > 0) {
		//if (imageSize != size) 
		{
			imageSize = size;
			textureSize = PPSize(nextPowerOfTwo(size.width),nextPowerOfTwo(size.height));

			if (poly._texture >= 0) {
				world()->projector->textureManager->deleteTexture(poly._texture);
				poly._texture = -1;
			}

			PPGameTexture* tex = new PPGameTexture();
			tex->option = option;
			poly.initTexture(world()->projector->textureManager->setTexture(tex));

			poly.texTileSize = imageSize;
			poly.texTileStride = PPSizeZero;
			poly.texOffset = PPPoint(0,0);

			world()->projector->textureManager->unbindTexture(poly._texture);
			
			closeFrameBuffer();
			
			firstFrame = true;
			update = false;

		}
	} else {
		imageSize = size;
		if (poly._texture >= 0) {
			world()->projector->textureManager->deleteTexture(poly._texture);
			poly._texture = -1;
		}
		closeFrameBuffer();
	}
	if (_pixel) free(_pixel);
	_pixel = NULL;
}

void PPOffscreenTexture::openFrameBuffer()
{
	GLuint texture_name = world()->projector->textureManager->getTextureName(poly._texture);
	if (texture_name != 0) {
		world()->projector->textureManager->freeTexture(poly._texture);
		texture_name = 0;
	}
	if (texture_name == 0) {
		world()->projector->textureManager->setTexture(poly._texture,pixel(),textureSize.width,textureSize.height,textureSize.width*4,option);
		world()->projector->textureManager->bindTexture(poly._texture);
		texture_name = world()->projector->textureManager->getTextureName(poly._texture);
		closeFrameBuffer();
	}

	if (colorbuffer_name == 0 && framebuffer_name == 0) {
		GLint oldframebuffer;
		glGetIntegerv(PP_FRAMEBUFFER_BINDING,&oldframebuffer);

		ppGenFramebuffers(1,&framebuffer_name);
		ppBindFramebuffer(PP_FRAMEBUFFER,framebuffer_name);

		ppGenRenderbuffers(1,&colorbuffer_name);
		ppFramebufferRenderbuffer(PP_FRAMEBUFFER,PP_COLOR_ATTACHMENT0,PP_RENDERBUFFER,colorbuffer_name);
		ppBindRenderbuffer(PP_RENDERBUFFER,colorbuffer_name);
		ppFramebufferTexture2D(PP_FRAMEBUFFER,PP_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture_name,0);

//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//		glClear(GL_COLOR_BUFFER_BIT);

		ppBindFramebuffer(PP_FRAMEBUFFER,oldframebuffer);
	}
}

void PPOffscreenTexture::closeFrameBuffer()
{
	if (colorbuffer_name != 0) ppDeleteRenderbuffers(1,&colorbuffer_name);
	if (framebuffer_name != 0) ppDeleteFramebuffers(1,&framebuffer_name);
	colorbuffer_name = 0;
	framebuffer_name = 0;
}

unsigned char* PPOffscreenTexture::pixel(PPPoint pos)
{
	unsigned char* p = pixel();
	int x = pos.x;
	int y = pos.y;
	if (x >= 0 && y >= 0 && x < imageSize.width && y < imageSize.height) {
		return &p[(int)(x*4+(imageSize.height-y-1)*textureSize.width*4)];
	}
	return NULL;
}

unsigned char* PPOffscreenTexture::pixel()
{
	if (_pixel==NULL) {
		_pixel = (unsigned char*)calloc(1,textureSize.width*textureSize.height*4);
//		for (int y=0;y<100;y++) {
//			for (int x=0;x<100;x++) {
//				unsigned char* p = &_pixel[(int)(x*4+y*textureSize.width*4)];
//				p[0] = x;
//				p[1] = y;
//				p[2] = x+y;
//				p[3] = 0xff;
//			}
//		}
	}
	return _pixel;
}

void PPOffscreenTexture::drawSelf(PPPoint pos)
{
	GLuint texture_name = world()->projector->textureManager->getTextureName(poly._texture);
	if (texture_name == 0) {
		if (pixel()) {
			update = true;
//			lock(true);
//			unlock(false);
		}
	}
	
	if (update) {
		lock(true);
		unlock(false);
		update = false;
	}

	PPObject::drawSelf(pos);
	
	firstFrame = true;
}

void PPOffscreenTexture::lock(bool sync)
{
 	PPGameSprite* sp = world()->projector;
	if (sp->oldstptr >= PPGameSprite::MAX_ST_STACK) {
		sp->oldstptr++;
		return;
	}
	sp->oldst[sp->oldstptr] = sp->st;
	glGetIntegerv(PP_FRAMEBUFFER_BINDING,&sp->oldst[sp->oldstptr].gloldframebuffer);
	
	openFrameBuffer();

	{
		glGetIntegerv(GL_VIEWPORT,sp->oldst[sp->oldstptr].glviewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
	}
	sp->oldstptr++;

	ppBindFramebuffer(PP_FRAMEBUFFER,framebuffer_name);
	sp->RestartOT();
	sp->st.screenWidth = imageSize.width;
	sp->st.screenHeight = imageSize.height;
	sp->st.offset2 = PPPointZero;
	sp->st.viewPort.x = 0;
	sp->st.viewPort.y = 0;
	sp->st.viewPort.width = imageSize.width;
	sp->st.viewPort.height = imageSize.height;

	glViewport(0,0,imageSize.width,imageSize.height);

	if (sync) {
		{
//			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//			glClear(GL_COLOR_BUFFER_BIT);
//			glRasterPos2i(-1,-1);
//			glDrawPixels(textureSize.width,textureSize.height,GL_RGBA,GL_UNSIGNED_BYTE,pixel());
		}
	}
}

void PPOffscreenTexture::unlock(bool sync)
{
	PPGameSprite* sp = world()->projector;
	if (sp->oldstptr > PPGameSprite::MAX_ST_STACK) return;
	if (sp->DrawOT()) {
		if (sync) {
			glFlush();
		}
	} else {
		sync = false;
	}
	sp->oldstptr--;
	sp->st = sp->oldst[sp->oldstptr];

	if (sync) {
		glReadPixels(0,0,textureSize.width,textureSize.height,GL_RGBA,GL_UNSIGNED_BYTE,pixel());
	}
	
	ppBindFramebuffer(PP_FRAMEBUFFER,sp->oldst[sp->oldstptr].gloldframebuffer);

    glViewport(sp->st.glviewport[0],sp->st.glviewport[1],sp->st.glviewport[2],sp->st.glviewport[3]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

//	glDisable(GL_ALPHA_TEST);
//glEnable(GL_BLEND);
	
//	closeFrameBuffer();
}

void PPOffscreenTexture::stepIdle()
{
}

PPSize PPOffscreenTexture::size()
{
	return imageSize*scale();
}

void PPOffscreenTexture::pset(PPPoint pos,PPColor col)
{
	unsigned char* p = pixel(pos);
	if (p) {
		p[0] = col.r;
		p[1] = col.g;
		p[2] = col.b;
		p[3] = col.a;
	}
}

PPColor PPOffscreenTexture::pget(PPPoint pos,PPColor col)
{
	unsigned char* p = pixel(pos);
	if (p) {
		col.r = p[0];
		col.g = p[1];
		col.b = p[2];
		col.a = p[3];
	}
	return col;
}

void PPOffscreenTexture::line(PPPoint pos1,PPPoint pos2,PPColor col)
{
	int x1,y1,x2,y2;
	int _width = imageSize.width;
	int _height = imageSize.height;
	x1 = pos1.x;
	y1 = pos1.y;
	x2 = pos2.x;
	y2 = pos2.y;

	int dx = 1;
	int dy = 1;
	
	if (x1 >= 0 && x1 < _width && y1 >= 0 && y1 < _height) {
	} else
	if (x2 >= 0 && x2 < _width && y2 >= 0 && y2 < _height) {
		int t;
		t = x2;
		x2 = x1;
		x1 = t;
		t = y2;
		y2 = y1;
		y1 = t;
	}
	
	int width = x2-x1;
	if (width < 0) {
		width = -width;
		dx = -1;
	}
	int height = y2-y1;
	if (height < 0) {
		height = -height;
		dy = -1;
	}
	
	if (width > height) {
		int px,py;
		int sum=0;

		if (x2 < x1) {
			int t;
			t = x2;
			x2 = x1;
			x1 = t;
			t = y2;
			y1 = t;
			dy = -dy;
		}
	
		py = y1;
		for (px=x1;px<=x2;px++) {
			sum += height;
			if (sum > width) {
				sum -= width;
				py += dy;
			}
			if (px >= 0 && px < _width && py >= 0 && py < _height) {
				pset(PPPoint(px,py),col);
			}
		}
	} else {
		int px,py;
		int sum=0;

		if (y2 < y1) {
			int t;
			t = x2;
			x1 = t;
			t = y2;
			y2 = y1;
			y1 = t;
			dx = -dx;
		}

		px = x1;
		for (py=y1;py<=y2;py++) {
			sum += width;
			if (sum > height) {
				sum -= height;
				px += dx;
			}
			if (px >= 0 && px < _width && py >= 0 && py < _height) {
				pset(PPPoint(px,py),col);
			}
		}
	}
	
	update = true;
}

void PPOffscreenTexture::box(PPRect rect,PPColor col)
{
	for (int x=rect.x;x<rect.x+rect.width;x++) {
		pset(PPPoint(x,rect.y),col);
		pset(PPPoint(x,rect.y+rect.height-1),col);
	}
	for (int y=rect.y+1;y<rect.y+rect.height-1;y++) {
		pset(PPPoint(rect.x,y),col);
		pset(PPPoint(rect.x+rect.width-1,y),col);
	}
	update = true;
}

void PPOffscreenTexture::fill(PPRect rect,PPColor col)
{
	for (int x=rect.x;x<rect.x+rect.width;x++) {
		for (int y=rect.y;y<rect.y+rect.height;y++) {
			pset(PPPoint(x,y),col);
		}
	}
	update = true;
}

void PPOffscreenTexture::paint(PPPoint pos,PPColor col)
{
	if (pos.x >= 0 && pos.x < imageSize.width &&  pos.y >= 0 && pos.y < imageSize.height) {
		PPColor spat = pget(pos);
		if (spat == col) return;
		{
			int px;
			int sx;
			int ex;

			for (sx=pos.x;;--sx) {
				if (!(sx >= 0 && sx < imageSize.width)) break;
				if (pget(PPPoint(sx,pos.y)) != spat) {
					sx ++;
					break;
				}
			}
			if (sx<0) sx=0;
			if (sx>=imageSize.width) sx=imageSize.width-1;

			for (ex=pos.x;;++ex) {
				if (!(ex >= 0 && ex < imageSize.width)) break;
				if (pget(PPPoint(ex,pos.y)) != spat) {
					ex --;
					break;
				}
			}
			if (ex<0) ex=0;
			if (ex>=imageSize.width) ex=imageSize.width-1;

			for (px=sx;px<=ex;px++) {
				pset(PPPoint(px,pos.y),col);
			}
			
			for (px=sx;px<=ex;px++) {
				if (pos.y-1>=0) {
					if (pget(PPPoint(px,pos.y-1)) == spat) {
						paint(PPPoint(px,pos.y-1),col);
					}
				}
				if (pos.y+1<imageSize.height) {
					if (pget(PPPoint(px,pos.y+1)) == spat) {
						paint(PPPoint(px,pos.y+1),col);
					}
				}
			}
		}
	}
	update = true;
}

void PPOffscreenTexture::paint(PPPoint pos,PPColor col,PPColor borderColor)
{
	if (pos.x >= 0 && pos.x < imageSize.width &&  pos.y >= 0 && pos.y < imageSize.height) {
		PPColor spat = pget(pos);
		if (col == spat) return;
		{
			int px;
			int sx;
			int ex;

			for (sx=pos.x;;--sx) {
				if (!(sx >= 0 && sx < imageSize.width)) break;
				if (pget(PPPoint(sx,pos.y)) == borderColor) {
					sx ++;
					break;
				}
			}
			if (sx<0) sx=0;
			if (sx>=imageSize.width) sx=imageSize.width-1;

			for (ex=pos.x;;++ex) {
				if (!(ex >= 0 && ex < imageSize.width)) break;
				if (pget(PPPoint(ex,pos.y)) == borderColor) {
					ex --;
					break;
				}
			}
			if (ex<0) ex=0;
			if (ex>=imageSize.width) ex=imageSize.width-1;

			for (px=sx;px<=ex;px++) {
				pset(PPPoint(px,pos.y),col);
			}
			
			for (px=sx;px<=ex;px++) {
				if (pos.y-1>=0) {
					if (pget(PPPoint(px,pos.y-1)) != borderColor) {
						paint(PPPoint(px,pos.y-1),col,borderColor);
					}
				}
				if (pos.y+1<imageSize.height) {
					if (pget(PPPoint(px,pos.y+1)) != borderColor) {
						paint(PPPoint(px,pos.y+1),col,borderColor);
					}
				}
			}
		}
	}
	update = true;
}

void PPOffscreenTexture::circle(PPPoint pos,float r,PPColor col,float start,float end)
{
	if (r <= 0) return;
	while (start < 0) {
		start += 360;
		end += 360;
	}
	while (start >= 360) {
		start -= 360;
		end -= 360;
	}
	int count = 0;
	{
		int x = r;
		int y = 0;
		int F = -2 * r + 3;
		while (x >= y) {
			if ( F >= 0 ) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
			count ++;
		}
	}
	int sq = start*count*8/360;
	int eq = end  *count*8/360;
	int c[16];
	int d[8] = { 1,-1,-1, 1,-1, 1, 1,-1};
	int t[8] = { 0, 4, 8, 4, 2, 2, 6, 6};
	for (int i=0;i<16;i++) {
		c[i] = t[i%8]*count+8*count*(i/8);
	}
	{
		int x = r;
		int y = 0;
		int F = -2 * r + 3;
		while (x >= y) {
			if ((sq <= c[0] && c[0] <= eq) || (sq <= c[ 8] && c[ 8] <= eq)) pset(PPPoint(pos.x+x,pos.y+y),col);
			if ((sq <= c[1] && c[1] <= eq) || (sq <= c[ 9] && c[ 9] <= eq)) pset(PPPoint(pos.x-x,pos.y+y),col);
			if ((sq <= c[2] && c[2] <= eq) || (sq <= c[10] && c[10] <= eq)) pset(PPPoint(pos.x+x,pos.y-y),col);
			if ((sq <= c[3] && c[3] <= eq) || (sq <= c[11] && c[11] <= eq)) pset(PPPoint(pos.x-x,pos.y-y),col);
			if ((sq <= c[4] && c[4] <= eq) || (sq <= c[12] && c[12] <= eq)) pset(PPPoint(pos.x+y,pos.y+x),col);
			if ((sq <= c[5] && c[5] <= eq) || (sq <= c[13] && c[13] <= eq)) pset(PPPoint(pos.x-y,pos.y+x),col);
			if ((sq <= c[6] && c[6] <= eq) || (sq <= c[14] && c[14] <= eq)) pset(PPPoint(pos.x+y,pos.y-x),col);
			if ((sq <= c[7] && c[7] <= eq) || (sq <= c[15] && c[15] <= eq)) pset(PPPoint(pos.x-y,pos.y-x),col);
			if ( F >= 0 ) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
			for (int i=0;i<16;i++) {
				c[i] += d[i%8];
			}
		}
	}
	update = true;
}

void PPOffscreenTexture::hflip()
{
	if (imageSize.width > 0 && imageSize.height > 0 && pixel()) {
		unsigned long size=imageSize.width*4;
		unsigned char* line = (unsigned char*)calloc(1,size);
		for (int i=0;i<imageSize.height/2;i++) {
			memcpy(line,pixel(PPPoint(0,imageSize.height-i-1)),size);
			memcpy(pixel(PPPoint(0,imageSize.height-i-1)),pixel(PPPoint(0,i)),size);
			memcpy(pixel(PPPoint(0,i)),line,size);
		}
		free(line);
		update = true;
	}
}

void PPOffscreenTexture::vflip()
{
	if (imageSize.width > 0 && imageSize.height > 0 && pixel()) {
		unsigned long size=imageSize.width*4-4;
		for (int y=0;y<imageSize.height;y++) {
			unsigned char* line = pixel(PPPoint(0,y));
			unsigned char pixel[4];
			for (int x=0;x<imageSize.width/2;x++) {
				memcpy(pixel,&line[x*4],4);
				memcpy(&line[x*4],&line[size-x*4],4);
				memcpy(&line[size-x*4],pixel,4);
			}
		}
		update = true;
	}
}

#pragma mark -

#include "PPLuaScript.h"

static int funcDelete(lua_State *L)
{
	delete (PPOffscreenTexture*)(PPLuaScript::DeleteObject(L));
	return 0;
}

static int funcNew(lua_State *L)
{
	PPWorld* world = PPLuaScript::World(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPObject* o = (PPObject*)s->userdata;
	if (s->argCount > 0) {
		lua_createtable(L,(int)s->integer(0),0);
		int table = lua_gettop(L);
		for (int i=0;i<s->integer(0);i++) {
			PPOffscreenTexture* obj = new PPOffscreenTexture(world);
			obj->init();
			obj->start();
			PPLuaScript::newObject(L,PPOffscreenTexture::className.c_str(),obj,funcDelete);
			lua_rawseti(L,table,i+1);
		}
	} else {
		PPOffscreenTexture* obj = new PPOffscreenTexture(world);
		obj->init();
		obj->start();
		PPLuaScript::newObject(L,PPOffscreenTexture::className.c_str(),obj,funcDelete);
	}
	return 1;
}

std::string PPOffscreenTexture::className = "PPOffscreenTexture";

PPObject* PPOffscreenTexture::registClass(PPLuaScript* s,const char* name,const char* superclass)
{
	if (name) PPOffscreenTexture::className = name;
	PPOffscreenTexture* obj = new PPOffscreenTexture(s->world());
	obj->start();
	return PPOffscreenTexture::registClass(s,PPOffscreenTexture::className.c_str(),obj,superclass);
}

static int funcCreate(lua_State *L)
{
	PPGameTextureOption option;
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0 && s->isTable(L,0)) {
		if (s->argCount > 1 && s->isTable(L,1)) {
			option = s->getTextureOption(L,1,option);//s->boolean(1);
		}
		m->create(s->getSize(L,0,256,256),option);
	} else
	if (s->argCount > 1) {
		if (s->argCount > 2 && s->isTable(L,2)) {
			option = s->getTextureOption(L,2,option);//s->boolean(1);
		}
		m->create(PPSize(s->number(0),s->number(1)),option);
	} else {
		m->create(PPSize(256,256),option);
	}
	m->poly.flags |= PPFLIP_V;
	return 0;
}

static int funcLock(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	bool hold = true;
	if (s->argCount > 0 && s->isBoolean(L,0)) {
		hold = s->boolean(0);
	}
	m->lock(hold);
	return 0;
}

static int funcUnlock(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	bool hold = true;
	if (s->argCount > 0 && s->isBoolean(L,0)) {
		hold = s->boolean(0);
	}
	m->unlock(hold);
	return 0;
}

static int funcPixel(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		PPPoint pos = PPPointZero;
		if (s->argCount > 0 && s->isTable(L,0)) {
			pos = s->getPoint(L,0);
			if (s->argCount > 1) {
				m->pset(pos,s->getColor(L,1));
			}
		} else
		if (s->argCount > 1) {
			pos = PPPoint(s->number(0),s->number(1));
			if (s->argCount > 3) {
				m->pset(pos,s->getColor(L,2));
			}
		}
		return s->returnColor(L,m->pget(pos,PPColor::white()));
	}
	return s->returnColor(L,PPColor::white());
}

static int funcBind(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		m->lock(true);
		m->unlock(false);
	}
	return 0;
}

static int funcLine(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		int n=0;
		PPPoint pos1 = PPPointZero;
		PPPoint pos2 = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos1 = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos1 = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		if (s->argCount > n && s->isTable(L,n)) {
			pos2 = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos2 = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		PPColor col = PPColor::white();
		if (n < s->argCount) {
			col = s->getColor(L,n,col);
		}
		m->line(pos1,pos2,col);
	}
	return 0;
}

static int funcBox(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		int n=0;
		PPRect rect = PPRect(0,0,0,0);
		if (n < s->argCount && s->isTable(L,n)) {
			rect = s->getRect(L,n);
		} else
		if (n+3 < s->argCount) {
			rect = s->getRect(L,n);
			n+=3;
		}
		n ++;
		PPColor col = PPColor::white();
		if (n < s->argCount) {
			col = s->getColor(L,n,col);
		}
		m->box(rect,col);
	}
	return 0;
}

static int funcFill(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		int n=0;
		PPRect rect = PPRect(0,0,0,0);
		if (n < s->argCount && s->isTable(L,n)) {
			rect = s->getRect(L,n);
		} else
		if (n+3 < s->argCount) {
			rect = s->getRect(L,n);
			n+=3;
		}
		n ++;
		PPColor col = PPColor::white();
		if (n < s->argCount) {
			col = s->getColor(L,n,col);
		}
		m->fill(rect,col);
	}
	return 0;
}

static int funcClear(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		int n=0;
		PPColor col = PPColor(0,0,0,0);
		if (n < s->argCount) {
			col = s->getColor(L,n,col);
		}
		m->fill(PPRect(0,0,m->imageSize.width,m->imageSize.height),col);
	}
	return 0;
}

static int funcPaint(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		int n=0;
		PPPoint pos = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		PPColor col = PPColor::white();
		if (n < s->argCount) {
			col = s->getColor(L,n);
			if (!s->isTable(L,n)) {
				n +=3;
			}
			n ++;
			if (n < s->argCount) {
				m->paint(pos,col,s->getColor(L,n));
				return 0;
			}
		}
		m->paint(pos,col);
	}
	return 0;
}

static int funcCircle(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m->pixel()) {
		int n=0;
		float r,start=0,end=360;
		PPPoint pos = PPPointZero;
		if (s->argCount > n && s->isTable(L,n)) {
			pos = s->getPoint(L,n);
		} else
		if (s->argCount > n+1) {
			pos = s->getPoint(L,n);
			n += 1;
		}
		n ++;
		PPColor col = PPColor::white();
		if (n < s->argCount) {
			r = s->number(n);
			n++;
			if (n < s->argCount) {
				col = s->getColor(L,n);
				if (!s->isTable(L,n)) {
					n +=3;
				}
				n++;
				if (n < s->argCount) {
					start = s->number(n);
					n++;
					if (n < s->argCount) {
						end = s->number(n);
						n++;
					}
				}
			}
			m->circle(pos,r,col,start,end);
		}
	}
	return 0;
}

//static int funcRender(lua_State *L)
//{
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPOffscreenTexture* m = (PPOffscreenTexture*)s->userdata;
//	m->render();
//	return 0;
//}

static int funcTexture(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	m->pushTextureInfo(L,m->poly._texture);
//	lua_pushinteger(L,m->poly._texture+1);		//Texture
	return 1;
}

static int funcHFlip(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
	m->hflip();
	return 1;
}

static int funcVFlip(lua_State *L)
{
	PPOffscreenTexture* m = (PPOffscreenTexture*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
	m->vflip();
	return 1;
}

PPObject* PPOffscreenTexture::registClass(PPLuaScript* s,const char* name,PPObject* obj,const char* superclass)
{
//	PPObject::registClass(s,name,obj);
	s->openModule(name,obj,funcDelete,superclass);
		s->addCommand("new",funcNew);
		s->addCommand("create",funcCreate);
		s->addCommand("lock",funcLock);
		s->addCommand("unlock",funcUnlock);
		s->addCommand("bind",funcBind);
		s->addCommand("pixel",funcPixel);
		s->addCommand("line",funcLine);
		s->addCommand("box",funcBox);
		s->addCommand("fill",funcFill);
		s->addCommand("clear",funcClear);
		s->addCommand("paint",funcPaint);
		s->addCommand("circle",funcCircle);
		s->addCommand("texture",funcTexture);
		s->addCommand("hflip",funcHFlip);
		s->addCommand("vflip",funcVFlip);
	s->closeModule();
	return obj;
}

void PPOffscreenTexture::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
	registClass(s,name,this,superclass);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
