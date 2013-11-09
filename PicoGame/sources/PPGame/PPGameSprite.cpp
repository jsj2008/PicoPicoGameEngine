/*-----------------------------------------------------------------------------------------------
	名前	PPGameSprite.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PPGameSprite.h"
#include "PPGameUtil.h"
#include "PPFont.h"
#include "QBGame.h"

#define VN 2

//static PPGameSprite* __sprite = NULL;

#pragma mark -

//PPGameSprite* PPGameSprite::select(PPGameSprite* projector)
//{
//	PPGameSprite* p = __sprite;
//	__sprite = projector;
//	PPGameTextureManager::select(__sprite->textureManager);
//	return p;
//}

//PPGameSprite* PPGameSprite::instance()
//{
//	return __sprite;
//}

//void PPGameSprite::deleteInstance()
//{
//	if (__sprite) delete __sprite;
//	__sprite = NULL;
//}

PPGameSprite::PPGameSprite()
{
	st.init();
	initLoadTex = false;
	reloadTex = false;
	b_vert = NULL;
	b_uv = NULL;
	b_color = NULL;
	b_size = 0;
//	__sprite = this;
	oldstptr = 0;
	animationFrameInterval = 1;
	resetBlendType();
//printf("new PPGameSprite\n");
	textureManager = new PPGameTextureManager();
//	PPGameTextureManager::select(textureManager);
}

PPGameSprite::~PPGameSprite()
{
	if (b_vert) free(b_vert);
	if (b_uv) free(b_uv);
	if (b_color) free(b_color);
	
//	__sprite = NULL;
	
	delete textureManager;
}

int PPGameSprite::Init()
{
	return 0;
}

int PPGameSprite::Exit()
{
	return 0;
}

int PPGameSprite::InitOT()
{
	glEnable(GL_TEXTURE_2D);
	textureManager->resetTextureState();
	BlendOff();
	st.start_ptr = 0;
	st.start_stack_ptr = 0;
	return 0;
}

static PPPoint calcPoint(PPPoint pos,PPPoint scale,PPPoint origin,float rotate)
{
	pos = pos - origin;
	pos = pos * scale;
	PPPoint p;
	p.x = pos.x*cos(rotate)-pos.y*sin(rotate);
	p.y = pos.x*sin(rotate)+pos.y*cos(rotate);
	pos = p;
	pos = pos + origin;
	return pos;
}

static PPPoint calcPoint(PPPoint pos,PPPoint scale,PPPoint origin)
{
	pos = pos - origin;
	pos = pos * scale;
	pos = pos + origin;
	return pos;
}

void PPGameSprite::CalcPolyPoint(PPGamePoly* poly,PPRect* size,PPPoint* delta,PPPoint* p)
{
	if (st.flags & PPGameState::UseLocalScale) {
		delta->x *= st.localScale.x;
		delta->y *= st.localScale.y;
		p[0] = size->pos()*st.localScale;
		p[1] = PPPoint(size->width*st.localScale.x,size->y*st.localScale.y);
		p[2] = PPPoint(size->width*st.localScale.x,size->height*st.localScale.y);
		p[3] = PPPoint(size->x*st.localScale.x,size->height*st.localScale.y);
	} else {
		p[0] = size->pos();
		p[1] = PPPoint(size->width,size->y);
		p[2] = PPPoint(size->width,size->height);
		p[3] = PPPoint(size->x,size->height);
	}
	if (poly->rotate == 0) {
		for (int i=0;i<4;i++) {
			p[i] += poly->pos;
			p[i] = calcPoint(*delta+p[i],poly->scale,poly->origin);
		}
	} else {
		for (int i=0;i<4;i++) {
			p[i] += poly->pos;
			p[i] = calcPoint(*delta+p[i],poly->scale,poly->origin,poly->rotate);
		}
	}
}

void PPGameSprite::PreCalc(PPPoint *pos,PPPoint *outpos)
{
	PPPoint p;
	p = calcPoint(*pos,st.scale,st.origin);
	p = p+st.offset;
	p = p+st.offset2;
	p.x =  (p.x)/(st.screenWidth /2)-1.0f;
	p.y = -(p.y)/(st.screenHeight/2)+1.0f;
	*outpos = p;
}

int PPGameSprite::DrawCore(PPGamePoly* poly,PPPoint uvpos,PPSize size,PPPoint delta)
{
	int textureid = poly->_texture;
	PPGameTexture* tex=textureManager->getTexture(textureid);
	if (tex) {
		GLfloat u1,v1,u2,v2;
		
		GLfloat* vert = &b_vert[st.b_ptr*VN];
		GLfloat* uv = &b_uv[st.b_ptr*2];
		GLubyte* color = &b_color[st.b_ptr*4];
		
		PPPoint p[4];
		PPRect r(0,0,size.width,size.height);
		CalcPolyPoint(poly,&r,&delta,p);

		if (st.rotate==0) {
			for (int i=0;i<4;i++) {
				p[i] = calcPoint(p[i],st.scale,st.origin);
				p[i] = p[i]+st.offset;
				p[i] = p[i]+st.offset2;
				p[i].x =  (p[i].x)/(st.screenWidth /2)-1.0f;
				p[i].y = -(p[i].y)/(st.screenHeight/2)+1.0f;
			}
		} else {
			for (int i=0;i<4;i++) {
				p[i] = calcPoint(p[i],st.scale,st.origin,st.rotate);
				p[i] = p[i]+st.offset;
				p[i] = p[i]+st.offset2;
				p[i].x =  (p[i].x)/(st.screenWidth /2)-1.0f;
				p[i].y = -(p[i].y)/(st.screenHeight/2)+1.0f;
			}
		}
		
		{
			int nx=0;
			int ny=0;
			for (int i=0;i<4;i++) {
				if (p[i].x < -1) nx --;
				if (p[i].y < -1) ny --;
				if (p[i].x >  1) nx ++;
				if (p[i].y >  1) ny ++;
			}
			if (nx <= -4 || nx >= 4 || ny <= -4 || ny >= 4) {
				return -1;
			}
		}
		
		float tw = tex->width;
		float th = tex->height;
		float itw = tex->invwidth;
		float ith = tex->invheight;
		float tw100 = tex->width100;
		float th100 = tex->height100;
		if (tw > 0 && th > 0) {
			float dd = 0;

			u1 = (uvpos.x)*itw+tw100;//1.0/(tw*100);//+0.0001;
			v1 = (uvpos.y)*ith+th100;//1.0/(th*100);//+0.0001;
			u2 = (uvpos.x+size.width )*itw-tw100;//1.0/(tw*100);
			v2 = (uvpos.y+size.height)*ith-th100;//1.0/(th*100);

			{
				int pt=0;
				vert[pt++] = p[0].x;
				vert[pt++] = p[0].y;
				vert[pt++] = p[1].x;
				vert[pt++] = p[1].y;
				vert[pt++] = p[3].x;
				vert[pt++] = p[3].y;

				vert[pt++] = p[3].x;
				vert[pt++] = p[3].y;
				vert[pt++] = p[1].x;
				vert[pt++] = p[1].y;
				vert[pt++] = p[2].x;
				vert[pt++] = p[2].y;
			}
			
			{
				int p=0;

				if (poly->flags & PPFLIP_H) {
					float t = u1;
					u1 = u2;
					u2 = t;
				}
				if (poly->flags & PPFLIP_V) {
					float t = v1;
					v1 = v2;
					v2 = t;
				}
				switch (poly->flags & PPFLIP_RMASK) {
				case PPFLIP_R90:
					uv[p++] = u2-dd;
					uv[p++] = v1+dd;
					uv[p++] = u2-dd;
					uv[p++] = v2-dd;
					uv[p++] = u1+dd;
					uv[p++] = v1+dd;

					uv[p++] = u1+dd;
					uv[p++] = v1+dd;
					uv[p++] = u2-dd;
					uv[p++] = v2-dd;
					uv[p++] = u1+dd;
					uv[p++] = v2-dd;
					break;
				case PPFLIP_R180:
					uv[p++] = u2-dd;
					uv[p++] = v2-dd;
					uv[p++] = u1+dd;
					uv[p++] = v2-dd;
					uv[p++] = u2-dd;
					uv[p++] = v1+dd;

					uv[p++] = u2-dd;
					uv[p++] = v1+dd;
					uv[p++] = u1+dd;
					uv[p++] = v2-dd;
					uv[p++] = u1+dd;
					uv[p++] = v1+dd;
					break;
				case PPFLIP_R270:
					uv[p++] = u1+dd;
					uv[p++] = v2-dd;
					uv[p++] = u1+dd;
					uv[p++] = v1+dd;
					uv[p++] = u2-dd;
					uv[p++] = v2-dd;

					uv[p++] = u2-dd;
					uv[p++] = v2-dd;
					uv[p++] = u1+dd;
					uv[p++] = v1+dd;
					uv[p++] = u2-dd;
					uv[p++] = v1+dd;
					break;
				default:
					uv[p++] = u1+dd;
					uv[p++] = v1+dd;
					uv[p++] = u2-dd;
					uv[p++] = v1+dd;
					uv[p++] = u1+dd;
					uv[p++] = v2-dd;

					uv[p++] = u1+dd;
					uv[p++] = v2-dd;
					uv[p++] = u2-dd;
					uv[p++] = v1+dd;
					uv[p++] = u2-dd;
					uv[p++] = v2-dd;
					break;
				}
			}

			{
				for (int i=0;i<6*4;i+=4) {
					color[i+0] = poly->color.r;
					color[i+1] = poly->color.g;
					color[i+2] = poly->color.b;
					color[i+3] = poly->color.a;
				}
			}
			
		}
	}
	return 0;
}

int PPGameSprite::Draw(PPGamePoly* poly)
{
	int textureid = poly->_texture;
	PPGameTexture* tex=textureManager->getTexture(textureid);
	if (tex == NULL) return 0;
	PPGameImage* patternMaster = tex->group;
	if (patternMaster) {
		int ppp=poly->pat;
		short* pat;
		pat = patternMaster[poly->group].pat;
		int px = pat[ppp*9+7];
		int py = pat[ppp*9+8];
		int pw = pat[ppp*9+3];
		int ph = pat[ppp*9+4];
		int dx = pat[ppp*9+5];
		int dy = pat[ppp*9+6];
		return DrawCore(poly,PPPoint(px,py),PPSize(pw,ph),PPPoint(dx,dy));
	} else {
		int pat=poly->pat;
//		PPGameTexture* tex = PPGameTexture::texture[textureid];
		if (tex) {
			if (/*tex->sizew > 0 && */tex->width > 0) {
				int tw = tex->width;
#ifdef __TILE_SIZE__
				return DrawCore(poly,poly->tilePosition(pat,tw),poly->texTileSize,PPPointZero);
#else
//				int px = (pat % (tw/tex->sizew))*tex->sizew;
//				int py = (pat / (tw/tex->sizew))*tex->sizeh;
//				int pw = tex->sizew;
//				int ph = tex->sizeh;
//				int dx = 0;
//				int dy = 0;
//				return DrawCore(poly,PPPoint(px,py),PPSize(pw,ph),PPPoint(dx,dy));
#endif
			} else {
printf("error texture %d\n",tex->index);
			}
		}
	}
	return 0;
}

int PPGameSprite::ViewPort(PPGamePoly* poly)
{
	GLfloat* vert = &b_vert[st.b_ptr*VN];
	vert[0] = poly->pos.x;
	vert[1] = poly->pos.y;
	vert[2] = poly->scale.x;
	vert[3] = poly->scale.y;
	return 0;
}

int PPGameSprite::Fill(PPGamePoly* poly)
{
	PPPoint p[4];
	p[0] = poly->pos;
	p[1] = PPPoint(poly->size.x,poly->pos.y);
	p[2] = poly->size;
	p[3] = PPPoint(poly->pos.x,poly->size.y);
	
	GLfloat* vert = &b_vert[st.b_ptr*VN];
	GLubyte* color = &b_color[st.b_ptr*4];

	for (int i=0;i<4;i++) {
		p[i] = calcPoint(p[i],poly->scale,poly->origin,poly->rotate);
		p[i] = calcPoint(p[i],st.scale,st.origin,st.rotate);
		p[i] = p[i] + st.offset+st.offset2;
		p[i].x =  (p[i].x)/(st.screenWidth /2)-1.0f;
		p[i].y = -(p[i].y)/(st.screenHeight/2)+1.0f;
	}

	{
		int nx=0;
		int ny=0;
		for (int i=0;i<4;i++) {
			if (p[i].x < -1) nx --;
			if (p[i].y < -1) ny --;
			if (p[i].x >  1) nx ++;
			if (p[i].y >  1) ny ++;
		}
		if (nx <= -4 || nx >= 4 || ny <= -4 || ny >= 4) return -1;
	}

	{
		int pt=0;
		vert[pt++] = p[0].x;
		vert[pt++] = p[0].y;
		vert[pt++] = p[1].x;
		vert[pt++] = p[1].y;
		vert[pt++] = p[3].x;
		vert[pt++] = p[3].y;

		vert[pt++] = p[3].x;
		vert[pt++] = p[3].y;
		vert[pt++] = p[1].x;
		vert[pt++] = p[1].y;
		vert[pt++] = p[2].x;
		vert[pt++] = p[2].y;
	}

	{
		for (int i=0;i<6*4;i+=4) {
			color[i+0] = poly->color.r;
			color[i+1] = poly->color.g;
			color[i+2] = poly->color.b;
			color[i+3] = poly->color.a;
		}
	}

	return 0;
}

//static float boxd[][2] = {
//	{1,0},{0,0},{0,-1},{1,-1}
//};

int PPGameSprite::Box(PPGamePoly* poly)
{
	PPPoint p[4];
	p[0] = poly->pos;
	p[1] = PPPoint(poly->size.x,poly->pos.y);
	p[2] = poly->size;
	p[3] = PPPoint(poly->pos.x,poly->size.y);
	
	GLfloat* vert = &b_vert[st.b_ptr*VN];
	GLubyte* color = &b_color[st.b_ptr*4];

	for (int i=0;i<4;i++) {
		p[i] = calcPoint(p[i],poly->scale,poly->origin,poly->rotate);
		p[i] = calcPoint(p[i],st.scale,st.origin,st.rotate);
		p[i] = p[i] + st.offset+st.offset2;
//		p[i].x += boxd[i][0];
//		p[i].y += boxd[i][1];
		p[i].x =  (p[i].x)/(st.screenWidth /2)-1.0f;
		p[i].y = -(p[i].y)/(st.screenHeight/2)+1.0f;
	}

	{
		int nx=0;
		int ny=0;
		for (int i=0;i<4;i++) {
			if (p[i].x < -1) nx --;
			if (p[i].y < -1) ny --;
			if (p[i].x >  1) nx ++;
			if (p[i].y >  1) ny ++;
		}
		if (nx <= -4 || nx >= 4 || ny <= -4 || ny >= 4) return -1;
	}

	{
		int pt=0;
		vert[pt++] = p[0].x;
		vert[pt++] = p[0].y;
		vert[pt++] = p[1].x;
		vert[pt++] = p[1].y;
		vert[pt++] = p[2].x;
		vert[pt++] = p[2].y;
		vert[pt++] = p[3].x;
		vert[pt++] = p[3].y;
		vert[pt++] = p[0].x;
		vert[pt++] = p[0].y;
	}

	{
		for (int i=0;i<5*4;i+=4) {
			color[i+0] = poly->color.r;
			color[i+1] = poly->color.g;
			color[i+2] = poly->color.b;
			color[i+3] = poly->color.a;
		}
	}

	return 0;
}

int PPGameSprite::Line(PPGamePoly* poly)
{
	PPPoint p[2];
	p[0] = poly->pos;
	p[1] = poly->size;
	
	GLfloat* vert = &b_vert[st.b_ptr*VN];
	GLubyte* color = &b_color[st.b_ptr*4];
	
	for (int i=0;i<2;i++) {
		p[i] = calcPoint(p[i],poly->scale,poly->origin,poly->rotate);
		p[i] = calcPoint(p[i],st.scale,st.origin,st.rotate);
		p[i] = p[i] + st.offset+st.offset2;
		p[i].x =  (p[i].x)/(st.screenWidth /2)-1.0f;
		p[i].y = -(p[i].y)/(st.screenHeight/2)+1.0f;
	}

	{
		int nx=0;
		int ny=0;
		for (int i=0;i<2;i++) {
			if (p[i].x < -1) nx --;
			if (p[i].y < -1) ny --;
			if (p[i].x >  1) nx ++;
			if (p[i].y >  1) ny ++;
		}
		if (nx <= -2 || nx >= 2 || ny <= -2 || ny >= 2) return -1;
	}

	{
		int pt=0;
		vert[pt++] = p[0].x;
		vert[pt++] = p[0].y;
		vert[pt++] = p[1].x;
		vert[pt++] = p[1].y;
	}

	{
		for (int i=0;i<2*4;i+=4) {
			color[i+0] = poly->color.r;
			color[i+1] = poly->color.g;
			color[i+2] = poly->color.b;
			color[i+3] = poly->color.a;
		}
	}

	return 0;
}

int PPGameSprite::DrawOT()
{
	textureManager->idle();
	if (st.start_stack_ptr == st.b_stack_ptr) return 0;
	int p = st.start_ptr;
	if (st.b_stack_ptr > 0) b_stack[st.b_stack_ptr-1].ptr = st.b_ptr;
	for (int i=st.start_stack_ptr;i<st.b_stack_ptr;i++) {
		//if (i >= start_ptr) 
		{
			switch (b_stack[i].type) {
			case QBFILL_TAG:
	//		case QBPOLY_TAG:
				glDisable(GL_TEXTURE_2D);
				glVertexPointer(VN,GL_FLOAT,0,&b_vert[p*VN]);
				glEnableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glColorPointer(4,GL_UNSIGNED_BYTE,0,&b_color[p*4]);
				glEnableClientState(GL_COLOR_ARRAY);
				BlendOn(b_stack[i].alphaValue,b_stack[i].blendType);
				glPushMatrix();
				glDrawArrays(GL_TRIANGLES,0,(b_stack[i].ptr-p));
				glPopMatrix();
				break;
			case QBBOX_TAG:
				glDisable(GL_TEXTURE_2D);
				glVertexPointer(VN,GL_FLOAT,0,&b_vert[p*VN]);
				glEnableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glColorPointer(4,GL_UNSIGNED_BYTE,0,&b_color[p*4]);
				glEnableClientState(GL_COLOR_ARRAY);
				BlendOn(b_stack[i].alphaValue,b_stack[i].blendType);
				glPushMatrix();
				glLineWidth(1);
				glDrawArrays(GL_LINE_STRIP,0,(b_stack[i].ptr-p));
				glPopMatrix();
				break;
			case QBLINE_TAG:
				glDisable(GL_TEXTURE_2D);
				glVertexPointer(VN,GL_FLOAT,0,&b_vert[p*VN]);
				glEnableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glColorPointer(4,GL_UNSIGNED_BYTE,0,&b_color[p*4]);
				glEnableClientState(GL_COLOR_ARRAY);
				BlendOn(b_stack[i].alphaValue,b_stack[i].blendType);
				glPushMatrix();
				glLineWidth(1);
				glDrawArrays(GL_LINES,0,(b_stack[i].ptr-p));
				glPopMatrix();
				break;
			case QBVIEW_TAG:
				{
					int t=p*2;//b_vert[p*VN];
					glViewport(b_vert[t+0],b_vert[t+1],b_vert[t+2],b_vert[t+3]);
				}
				break;
			default:
				if (b_stack[i].type >= 0) 
				{
					glEnable(GL_TEXTURE_2D);
					textureManager->bindTexture(b_stack[i].type);
					glVertexPointer(VN,GL_FLOAT,0,&b_vert[p*VN]);
					glEnableClientState(GL_VERTEX_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,&b_uv[p*2]);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glColorPointer(4,GL_UNSIGNED_BYTE,0,&b_color[p*4]);
					glEnableClientState(GL_COLOR_ARRAY);
					BlendOn(b_stack[i].alphaValue,b_stack[i].blendType);
					glPushMatrix();
					glDrawArrays(GL_TRIANGLES,0,(b_stack[i].ptr-p));
					glPopMatrix();
				}
				break;
			}
		}
		p = b_stack[i].ptr;
	}

	return 1;
}

void PPGameSprite::RestartOT()
{
	st.start_stack_ptr = st.b_stack_ptr;
	st.start_ptr = st.b_ptr;
	st.s_type = 1000;
	st.s_alphaValue = 1.0;
	st.s_blendType = 1;
}

void PPGameSprite::ResetOT()
{
	st.s_type = 1000;
	st.s_texId = 1000;
	st.s_alphaValue = 1.0;
	st.s_blendType = 1;
	st.b_ptr = 0;
	st.b_stack_ptr = 0;
	st.b_start_ptr = 0;
	resetBlendType();
}

void PPGameSprite::Clear()
{
	ResetOT();
}

int PPGameSprite::BlendOn(float alpha,int type)
{
	PPGameBlend* b = &blendTable[type];
	if (b->blend) {
		glEnable(GL_BLEND);
		glBlendFunc(b->blendSrc,b->blendDst);
	} else {
		glDisable(GL_BLEND);
	}
	if (b->fog) {
		glEnable(GL_FOG);
#if TARGET_OS_IPHONE
		glFogx(GL_FOG_MODE,GL_LINEAR);
#else
#ifdef _ANDROID
		glFogx(GL_FOG_MODE,GL_LINEAR);
#else
#ifdef _LINUX
		glFogx(GL_FOG_MODE,GL_LINEAR);
#else
		glFogi(GL_FOG_MODE,GL_LINEAR);
#endif
#endif
#endif
		glFogf(GL_FOG_START, -1.0f);
		glFogf(GL_FOG_END,0.0f);
		{
			GLfloat fogc[]={b->fogColor.r,b->fogColor.g,b->fogColor.b,b->fogColor.a};
			glFogfv(GL_FOG_COLOR,fogc);
		}
	} else {
		glDisable(GL_FOG);
	}
	return 0;
}

int PPGameSprite::BlendOff()
{
	st._alpha = 1;
	glDisable(GL_FOG);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	return 0;
}

int PPGameSprite::PushStack(int type,float alphaValue,int blendType)
{
	if (type != st.s_type
	 || type == QBVIEW_TAG
	 || type == QBBOX_TAG
	 || alphaValue != st.s_alphaValue
	 || blendType != st.s_blendType) {
		st.s_type = type;
		st.s_alphaValue = alphaValue;
		st.s_blendType = blendType;
		if (st.b_stack_ptr > 0) {
			st.b_start_ptr = st.b_ptr;
			b_stack[st.b_stack_ptr-1].ptr = st.b_ptr;
		}
		b_stack[st.b_stack_ptr].type = type;
		b_stack[st.b_stack_ptr].ptr = st.b_ptr;
		b_stack[st.b_stack_ptr].alphaValue = alphaValue;
		b_stack[st.b_stack_ptr].blendType = blendType;
		st.b_stack_ptr ++;
	}
	if (type == QBLINE_TAG) {
		st.b_ptr += 2;
	} else
	if (type == QBVIEW_TAG) {
		st.b_ptr += 2;
	} else
	if (type == QBBOX_TAG) {
		st.b_ptr += 5;
	} else {
		st.b_ptr += 6;
	}
	return 0;
}

void PPGameSprite::resetBlendType()
{
	st.blendCount = 1;
	blendTable[0].set(PPGameBlend_None);
}

int PPGameSprite::getBlendType(PPGamePoly* poly)
{
	int i=0;
	for (i=0;i<st.blendCount;i++) {
		if (blendTable[i] == poly->blend) return i;
	}
	blendTable[st.blendCount] = poly->blend;
	st.blendCount ++;
	return i;
}

int PPGameSprite::DrawPoly(PPGamePoly* poly)
{
	if (st.b_ptr >= b_size-10 || st.b_stack_ptr >= PPGAME_VBO_NUM-1) {
//printf("DrawPoly\n");
		DrawOT();
		st.b_ptr = st.start_ptr;
		st.b_stack_ptr = st.start_stack_ptr;
		st.s_type = 1000;
		st.s_alphaValue = 1.0;
		st.s_blendType = 1;
	}

	switch (poly->_texture) {
	case QBFILL_TAG:
		textureManager->disableTexture();
		if (Fill(poly) == 0) PushStack(poly->_texture);
		break;
	case QBBOX_TAG:
		textureManager->disableTexture();
		if (Box(poly) == 0) PushStack(poly->_texture);
		break;
	case QBLINE_TAG:
		textureManager->disableTexture();
		if (Line(poly) == 0) PushStack(poly->_texture);
		break;
	case QBVIEW_TAG:
		ViewPort(poly);
		PushStack(poly->_texture);
		st.screenWidth  = poly->scale.x;
		st.screenHeight = poly->scale.y;
		break;
	default:
		if (poly->_texture >= 0 && poly->_texture<PPGAME_MAX_TEXTURE) {
			PPGameTexture* tex = textureManager->getTexture(poly->_texture);
			if (tex) {
				if (tex->loaded) {
					textureManager->enableTexture(poly->_texture);
					if (Draw(poly) == 0) PushStack(poly->_texture,poly->color.a,getBlendType(poly));
				}
			}
		}
		break;
	}
	return 0;
}

int PPGameSprite::CaptureScreen(int x,int y,int w,int h,int rowbyte,unsigned char* pixel)
{
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	for (int i=0;i<h;i++) {
		glReadPixels(x,y+i,w,1,GL_RGBA,GL_UNSIGNED_BYTE,&pixel[((h-1)-i)*rowbyte]);
	}
	GLenum err = glGetError();
	if(err) {
		return -1;
	}
	return 0;
}

int PPGameSprite::ClearScreen2D(GLfloat r,GLfloat g,GLfloat b)
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	return 0;
}

void PPGameSprite::SetViewPort()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#if TARGET_OS_IPHONE
	glOrthof(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);
#else
#ifdef _ANDROID
	glOrthof(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);
#else
#ifdef _LINUX
	glOrthof(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);
#else
	glOrtho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);
#endif
#endif
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glShadeModel(GL_FLAT);
}

void PPGameSprite::viewPort(PPRect rect)
{
	PPGamePoly poly;
	st.viewPort = rect;
	poly._texture = QBVIEW_TAG;
	poly.pos.x = rect.x;
	poly.pos.y = st.windowSize.height-rect.y-rect.height;
	poly.scale.x = rect.width;
	poly.scale.y = rect.height;
	DrawPoly(&poly);
	st.offset2.x = -rect.x;
	st.offset2.y = -rect.y;
}

PPRect PPGameSprite::viewPort()
{
	return st.viewPort;
}

#pragma mark -

#include "PPLuaScript.h"

static int funcOffset(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->st.offset.x = s->tableNumber(L,0,1,"x",m->st.offset.x);
			m->st.offset.y = s->tableNumber(L,0,2,"y",m->st.offset.y);
		} else {
			if (s->argCount > 1) {
				m->st.offset.x = s->number(0);
				m->st.offset.y = s->number(1);
			} else {
				m->st.offset.x = s->number(0);
				m->st.offset.y = s->number(0);
			}
		}
	}
	return s->returnPoint(L,m->st.offset);
}

static int funcOrigin(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->st.origin.x = s->tableNumber(L,0,1,"x",m->st.origin.x);
			m->st.origin.y = s->tableNumber(L,0,2,"y",m->st.origin.y);
		} else {
			if (s->argCount > 1) {
				m->st.origin.x = s->number(0);
				m->st.origin.y = s->number(1);
			} else {
				m->st.origin.x = s->number(0);
				m->st.origin.y = s->number(0);
			}
		}
	}
	return s->returnPoint(L,m->st.origin);
}

static int funcScale(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->st.scale.x = s->tableNumber(L,0,1,"x",m->st.scale.x);
			m->st.scale.y = s->tableNumber(L,0,2,"y",m->st.scale.y);
		} else {
			if (s->argCount > 1) {
				m->st.scale.x = s->number(0);
				m->st.scale.y = s->number(1);
			} else {
				m->st.scale.x = s->number(0);
				m->st.scale.y = s->number(0);
			}
		}
	}
	return s->returnPoint(L,m->st.scale);
}

static int funcRotate(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		m->st.rotate = s->number(0);//atof(s->args(0))*2*M_PI/360;
	}
	lua_pushnumber(L,m->st.rotate);
	return 1;
}

//static int funcWinSize(lua_State* L)
//{
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
//	PPGameSprite* m = (PPGameSprite*)s->userdata;
//	lua_createtable(L, 0, 2);
//	lua_pushnumber(L,m->st.screenWidth);
//	lua_setfield(L, -2, "width");
//	lua_pushnumber(L,m->st.screenHeight);
//	lua_setfield(L, -2, "height");
//	return 1;
//}

static int funcWinRect(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	return s->returnRect(L,PPRect(0,0,m->st.windowSize.width,m->st.windowSize.height));
}

static int funcViewport(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			PPRect r = s->getRect(L,0);
			m->viewPort(r);
		} else {
			PPRect r;
			if (s->argCount > 3) {
				r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
			} else
			if (s->argCount > 2) {
				r = PPRect(s->number(0),s->number(1),s->number(2),s->number(2));
			} else {
				r = PPRect(s->number(0),s->number(1),1,1);
			}
//			PPRect r = PPRect(s->number(0),s->number(1),s->number(2),s->number(3));
			m->viewPort(r);
		}
	}
//printf("2:%f,%f\n",m->viewPort().width,m->viewPort().height);
	return s->returnRect(L,m->viewPort());
}

static int funcFrameInterval(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPGameSprite* m = (PPGameSprite*)s->userdata;
	if (s->argCount > 0) {
		int v = (int)s->integer(0);
		if (v < 1) v = 1;
		if (v > 4) v = 4;
		m->animationFrameInterval = v;
	}
	lua_pushinteger(L,m->animationFrameInterval);
	return 1;
}

static int funcLayout(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPGameSprite* m = (PPGameSprite*)s->userdata;
	QBGame* g = (QBGame*)m->world();//s->userdata;
	if (lua_istable(L,2)) {
		PPRect base=s->getRect(L,0);
		PPRect win=PPRect(0,0,m->st.windowSize.width,m->st.windowSize.height);
		float ss=win.width/base.width;
		float tt=win.height/base.height;
		float scale=tt;
		if (ss<tt) scale = ss;
		PPPoint offset = g->layout(PPSize(base.width*scale,base.height*scale),base.pos(),PP_CENTER_X | PP_CENTER_Y,g->viewPort());
		m->st.scale.x = scale;
		m->st.scale.y = scale;
		m->st.offset = offset;
		return s->returnRect(L,PPRect(offset.x,offset.y,base.width*scale,base.height*scale));
	}
	return s->returnRect(L,PPRectZero);
}

static int funcGetDensity(lua_State* L)
{
	PPGameSprite* m = (PPGameSprite*)PPLuaScript::UserData(L);
	QBGame* g = (QBGame*)m->world();//s->userdata;
	lua_pushnumber(L,g->scale_factor);
	return 1;
}

void PPGameSprite::openLibrary(PPLuaScript* script,const char* name,const char* superclass)
{
	script->openModule(name,this,0,superclass);
		script->addCommand("size",funcWinRect);
//		script->addCommand("rect",funcWinRect);
		script->addCommand("offset",funcOffset);
		script->addCommand("rotate",funcRotate);
//		script->addCommand("origin",funcOrigin);
		script->addCommand("pivot",funcOrigin);
		script->addCommand("scale",funcScale);
		script->addCommand("viewport",funcViewport);
		script->addCommand("interval",funcFrameInterval);
		script->addCommand("density",funcGetDensity);
		script->addCommand("layout",funcLayout);
	script->closeModule();
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
