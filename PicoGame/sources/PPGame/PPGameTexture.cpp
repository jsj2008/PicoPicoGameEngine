/*-----------------------------------------------------------------------------------------------
	名前	PPGameTexture.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPGameTexture.h"
#include "PPFont.h"
#include "PPGameUtil.h"

static int resetTexture(int value,const char* str) {
//printf("resetTexture %s\n",str);
	return value;
}

PPGameTextureManager::PPGameTextureManager() : defaultTexture(-1),fontManager(NULL)
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		texture[i] = NULL;
	}
	fontManager = new PPFontManager();
}

PPGameTextureManager::~PPGameTextureManager()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) delete texture[i];
	}
	delete fontManager;
}

//static PPGameTextureManager* __m=NULL;

//PPGameTextureManager* PPGameTextureManager::select(PPGameTextureManager* manager)
//{
//	PPGameTextureManager* m = __m;
//	__m = manager;
//	return m;
//}

#pragma mark -

PPGameTextureOption::PPGameTextureOption()
{
	linear = false;
	wrap_s = GL_CLAMP_TO_EDGE;
	wrap_t = GL_CLAMP_TO_EDGE;
}

bool PPGameTextureOption::operator == (PPGameTextureOption& o) {
	if (linear == o.linear && wrap_s == o.wrap_s && wrap_t == o.wrap_t) return true;
	return false;
};

int PPGameTextureOption::wrap(const char* name)
{
	int wrap = GL_CLAMP_TO_EDGE;
	if (strcasecmp(name,"linear") == 0) {
		return GL_LINEAR;
	}
	if (strcasecmp(name,"repeat") == 0) {
		return GL_REPEAT;
	}
#if 0
	if (strcasecmp(name,"clamp") == 0) {
		return GL_CLAMP;
	}
#endif
	return wrap;
}

const char* PPGameTextureOption::wrap(int wrap)
{
	if (wrap == GL_LINEAR) return "linear";
	if (wrap == GL_REPEAT) return "repeat";
#if 0
	if (wrap == GL_CLAMP) return "clamp";
#endif
	return "clamp_to_edge";
}

PPGameTexture::PPGameTexture() : manager(NULL),notexturefile(false)
{
//	if (!boot) {
//		for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
//			texture[i] = NULL;
//		}
//		boot = true;
//	}
	pixel = NULL;
	//bind = false;
	nofree = false;
	group = NULL;
	name = "";
//	sizew = 32;
//	sizeh = 32;
//	sharedTexture = -1;
//	retainCount = 1;
	loaded = false;
//	linear = true;
	texture_name = 0;
}

PPGameTexture::~PPGameTexture()
{
}

#pragma mark -

//int PPGameTexture::RegistTexture(unsigned char* pixel,int width,int height,int rowbytes,bool nofree)
//{
//	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
//		if (texture[i]==NULL) {
//			texture[i] = new PPGameTexture();
//			if (texture[i]) {
//				texture[i]->index = i;
//				texture[i]->pixel = pixel;
//printf("pixel(%d) 0x%lX\n",texture[i]->index,(unsigned long)pixel);
//				texture[i]->width = width;
//				texture[i]->height = height;
//				texture[i]->rowbytes = rowbytes;
//				texture[i]->bind = false;
//				texture[i]->nofree = nofree;
////printf("RegistTexture %d,%d\n",texture[i]->width,texture[i]->height);
//				return i;
//			}
//			break;
//		}
//	}
//	return -1;
//}

bool PPGameTextureManager::setTexture(int index,unsigned char* pixel,unsigned long width,unsigned long height,unsigned long bytesPerRow,PPGameTextureOption option)
{
	if (index < 0) return false;
	int i=index;
	freeTexture(i);
	if (texture[i]!=NULL) {
//		if (pixel) {
			if (texture[i]->pixel != pixel) {
				if (texture[i]->pixel) {
					free(texture[i]->pixel);
				}
			}
			texture[i]->pixel = pixel;
			texture[i]->width = (int)width;
			texture[i]->height = (int)height;
			texture[i]->width100 = 1.0/(width*100.0);
			texture[i]->height100 = 1.0/(height*100.0);
			texture[i]->invwidth = 1.0/texture[i]->width;
			texture[i]->invheight = 1.0/texture[i]->height;
			texture[i]->rowbytes = (int)bytesPerRow;
			//texture[i]->bind = false;
			texture[i]->nofree = false;
			texture[i]->compressed = false;
			texture[i]->loaded = true;
			texture[i]->option = option;
//printf("sizew %d\n",texture[i]->sizew);
			return true;
//		} else {
////printf("can't set texture %s\n",texture[i]->name.c_str());
//		}
	} else {
//printf("can't set texture\n");
	}
	return false;
}

//bool PPGameTexture::SetTexture(int index,unsigned char* pixel,unsigned long width,unsigned long height,unsigned long bytesPerRow,PPGameTextureOption option)
//{
//	return __m->setTexture(index, pixel, width, height, bytesPerRow, option);
//}

int PPGameTextureManager::unbindAllTexture()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			if (texture[i]->texture_name == 0) {
			} else {
//				if (texture[i]->sharedTexture < 0) {
					glDeleteTextures(1,&texture[i]->texture_name);
//				}
			}
			texture[i]->texture_name = resetTexture(0,"1");
		}
	}
	return 0;
}

//int PPGameTexture::UnbindAllTexture()
//{
//	return __m->unbindAllTexture();
//}

int PPGameTextureManager::unbindAllTextureForDebug()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			if (texture[i]->texture_name == 0) {
			} else {
//				if (texture[i]->sharedTexture < 0) {
					glDeleteTextures(1,&texture[i]->texture_name);
//				}
			}
		}
	}
	return 0;
}

//int PPGameTexture::UnbindAllTextureForDebug()
//{
//	return __m->unbindAllTextureForDebug();
//}

int PPGameTextureManager::reloadAllTexture()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			if (texture[i]->texture_name == 0) {
			} else {
				glDeleteTextures(1,&texture[i]->texture_name);
			}
			texture[i]->texture_name = resetTexture(0,"2");
		}
	}
	fontManager->fontManagerIdle();
	return 0;
}

//int PPGameTexture::ReloadAllTexture()
//{
//	return __m->reloadAllTexture();
//}

static void texImage2DPVRTC(GLint level, GLsizei bpp, GLboolean hasAlpha, GLsizei width, GLsizei height, void *pvrtcData)
{
#if TARGET_OS_IPHONE
    GLenum format;
    GLsizei size = width * height * bpp / 8;
    if(hasAlpha) {
        format = (bpp == 4) ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    } else {
        format = (bpp == 4) ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    }
    if(size < 32) {
        size = 32;
    }
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, pvrtcData);
#endif
}

void PPGameTextureManager::resetTextureState()
{
	texId=-1;
	glDisable(GL_TEXTURE_2D);
}

void PPGameTextureManager::disableTexture()
{
	if (texId >= 0) {
		texId=-1;
	}
}

void PPGameTextureManager::enableTexture(int texNo)
{
	if (texId != texNo) {
		texId=texNo;
	}
}

//void PPGameTexture::EnableTexture(int texNo)
//{
//	if (__m->texId != texNo) {
//		__m->texId=texNo;
//	}
//}

int PPGameTexture::bindTexture()
{
//	if (sharedTexture >= 0) {
//		BindTexture(sharedTexture);
//	} else
	if (texture_name == 0) {
		//if (pixel) {
			glGenTextures(1,&texture_name);
			glBindTexture(GL_TEXTURE_2D,texture_name);
			if (option.linear) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//#if TARGET_OS_IPHONE
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, option.wrap_s);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, option.wrap_t);
//#else
//				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//#endif
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, option.wrap_s);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, option.wrap_t);
			}
			if (compressed) {
				texImage2DPVRTC(0,4,false,width,height,pixel);
			} else {
				if (width == rowbytes) {
					glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,width,height,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pixel);
				} else
				if (width == rowbytes/2) {
					glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE_ALPHA,width,height,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,pixel);
				} else {
					glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixel);
				}
			}
			//bind = true;
//printf("glTexImage2D %d,texid %d\n",index,texid);
		//}
	} else {
//printf("bindTexture %d\n",i);
		glBindTexture(GL_TEXTURE_2D,texture_name);
	}
	manager->texId = index;
	return 0;
}

int PPGameTexture::unbindTexture()
{
//	if (sharedTexture >= 0) {
//		UnbindTexture(sharedTexture);
//	} else
	if (texture_name != 0) {
		glDeleteTextures(1,&texture_name);
		texture_name = resetTexture(0,"3");
	}
	return 0;
}

int PPGameTextureManager::bindTexture(int i)
{
	if (i >= 0) {
		if (texture[i]) {
			texture[i]->index = i;
			return texture[i]->bindTexture();
		}
	}
	return 0;
}

//int PPGameTexture::BindTexture(int i)
//{
//	return __m->bindTexture(i);
//}

int PPGameTextureManager::unbindTexture(int i)
{
	if (i >= 0) {
		if (texture[i]) {
			texture[i]->index = i;
			return texture[i]->unbindTexture();
		}
	}
	return 0;
}

//int PPGameTexture::UnbindTexture(int i)
//{
//	return __m->unbindTexture(i);
//}

int PPGameTextureManager::countTexture()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]==NULL) {
			return i;
		}
	}
	return PPGAME_MAX_TEXTURE;
}

//int PPGameTexture::CountTexture()
//{
//	return __m->countTexture();
//}

int PPGameTextureManager::releaseAllTexture()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			delete texture[i];
			texture[i] = NULL;
		}
	}
	return 0;
}

//int PPGameTexture::ReleaseAllTexture()
//{
//	if (__m) return __m->releaseAllTexture();
//	return 0;
//}

void PPGameTextureManager::deleteTextureForDevelop(int textureid)
{
	if (textureid < 0) textureid = 0;
	for (int i=textureid;i<PPGAME_MAX_TEXTURE;i++) {
		deleteTexture(i);
	}
}

//void PPGameTexture::DeleteTextureForDevelop(int textureid)
//{
//	__m->deleteTextureForDevelop(textureid);
//}

int PPGameTextureManager::deleteTexture(int textureid)
{
	if (textureid < 0) return -1;
	if (textureid >= PPGAME_MAX_TEXTURE) return -1;
	int i=textureid;
	if (i >= 0) {
		if (texture[i]) {
			if (texture[i]->loaded) {
				texture[i]->loaded = false;
				if (texture[i]->texture_name) {
					glDeleteTextures(1,&texture[i]->texture_name);
					texture[i]->texture_name = resetTexture(0,"4");
				}
				delete texture[i];
				texture[i] = NULL;
			}
		}
	}
	return 0;
}

//int PPGameTexture::DeleteTexture(int textureid)
//{
//	return __m->deleteTexture(textureid);
//}

int PPGameTexture::loadTexture()
{
//	if (sharedTexture >= 0) {
//		int s = sharedTexture;
//		LoadTexture(s);
//		width = texture[s]->width;
//		height = texture[s]->height;
//		size = texture[s]->size;
//		loaded =  true;
//	} else
	if (texture_name == 0) {
		loaded =  true;
		if (strcmp(name.c_str(),"") == 0 || notexturefile) {
//			pixel = NULL;
//			width = 0;
//			height = 0;
//			size = PPSize(0,0);
//			rowbytes = 0;
//			//bind = false;
//			nofree = false;
//			compressed = false;
		} else if (PPGameDataPath(name.c_str())) {
			int l = (int)strlen(name.c_str());
			if (name.c_str()[0] == '.') {
				int n = (int)strlen(name.c_str());
				int j=1;
				int w=0;
				int h=0;
				for (;j<n;j++) {
					char a = name.c_str()[j];
					if (a >= '0' && a <= '9') {
						w *= 10;
						w += a - '0';
					} else {
						break;
					}
				}
				for (;j<n;j++) {
					char a = name.c_str()[j];
					if (a >= '0' && a <= '9') {
						h *= 10;
						h += a - '0';
					} else {
						break;
					}
				}
				pixel = NULL;
				width = w;
				height = h;
				width100 = 1.0/(width*100.0);
				height100 = 1.0/(height*100.0);
				invwidth = 1.0/width;
				invheight = 1.0/height;
				size = PPSize(w,h);
				rowbytes = w*4;
				//bind = false;
				nofree = false;
				compressed = false;
			} else if (name.c_str()[l-1] == 'c') {
				//PVRTC対応
				unsigned char* _pixel;
				FILE* fp = fopen(PPGameDataPath(name.c_str()),"r");
				if (fp) {
					long _size;
					fseek(fp,0,SEEK_END);
					_size = ftell(fp);
					rewind(fp);
					_pixel = (unsigned char*)calloc(1,_size);
					//int len = 
					fread(_pixel,1,_size,fp);
					unsigned long _width,_height,bytesPerRow;
					_width = 512;
					_height = 512;
					bytesPerRow = 4*512/8;
					if (_pixel) {
						pixel = _pixel;
						width = (int)_width;
						height = (int)_height;
						width100 = 1.0/(width*100.0);
						height100 = 1.0/(height*100.0);
						invwidth = 1.0/width;
						invheight = 1.0/height;
						size = PPSize(_width,_height);
						rowbytes = (int)bytesPerRow;
						//bind = false;
						nofree = false;
						compressed = true;
						bindTexture();
						free(pixel);
						pixel = NULL;
					}
					fclose(fp);
				}
			} else {
				//PNGファイル
				unsigned long w,h,bytesPerRow;
				unsigned long ow,oh,ob;
//printf("PPGameTexture:%s\n",PPGameDataPath(name.c_str()));
				unsigned char* p=NULL;
				p = PPGame_LoadPNG(PPGameDataPath(name.c_str()),&w,&h,&bytesPerRow);
				if (p == NULL) {
					PPReadErrorSet(name.c_str());
				}
				{
					ow = w;
					oh = h;
					ob = bytesPerRow;
					pixel = PPGameTextureManager::loadPNG(p,&w,&h,&bytesPerRow);
					if (pixel) {
						width = (int)w;
						height = (int)h;
						width100 = 1.0/(width*100.0);
						height100 = 1.0/(height*100.0);
						invwidth = 1.0/width;
						invheight = 1.0/height;
						size = PPSize(ow,oh);
						rowbytes = (int)bytesPerRow;
						//bind = false;
						nofree = false;
						compressed = false;
						bindTexture();
						free(pixel);
						pixel = NULL;
					} else {
						PPReadErrorSet(name.c_str());
					}
				}
			}
		}
	}
	return 0;
}

int PPGameTextureManager::loadTexture(const char* filename,PPGameTextureOption option)
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			if (texture[i]->name == filename && texture[i]->option == option) {
				return i;
			}
		}
	}
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (!texture[i]) {
			texture[i] = new PPGameTexture();
			if (texture[i]) {
				texture[i]->index = i;
				texture[i]->name = filename;
				texture[i]->option = option;
				texture[i]->group = 0;
				texture[i]->manager = this;
				texture[i]->loadTexture();
//printf("loadtexture[%d] %s,%d\n",i,texture[i]->name.c_str(),texture[i]->linear);
				return i;
			}
		}
	}
	return -1;
}

//int PPGameTexture::LoadTexture(const char* filename,PPGameTextureOption option)
//{
//	return __m->loadTexture(filename,option);
//}

int PPGameTextureManager::loadTexture(int i)
{
	if (i < 0) return -1;
	if (i >= PPGAME_MAX_TEXTURE) return -1;
	if (texture[i]!=NULL) {
//		texture[i]->index = i;
		return texture[i]->loadTexture();
	}
	return 0;
}

//int PPGameTexture::LoadTexture(int i)
//{
//	return __m->loadTexture(i);
//}

int PPGameTextureManager::freeTexture(int index)
{
	int i=index;
	if (i < 0) return -1;
	if (i >= PPGAME_MAX_TEXTURE) return -1;
	if (i >= 0) {
		if (texture[i]) {
//			if (texture[i]->sharedTexture < 0) {
				if (texture[i]->texture_name) {
					glDeleteTextures(1,&texture[i]->texture_name);
					texture[i]->texture_name = resetTexture(0,"5");
				}
//			}
		}
	}
	return 0;
}

//int PPGameTexture::FreeTexture(int index)
//{
//	return __m->freeTexture(index);
//}

void PPGameTextureManager::idle()
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]!=NULL) {
//			if (texture[i]->pixel) {
//				texture[i]->bindTexture();
//			}
			if (texture[i]->texture_name == 0) {
				texture[i]->loadTexture();
			}
		}
	}
	fontManager->fontManagerIdle();
}

//void PPGameTexture::idle()
//{
//	__m->idle();
//}

static int calcTextureSize(int s)
{
	if (s < 32) return 32;
	int t = 1;
	while (t < s) {
		t = t<<1;
	}
	return t;
}

unsigned char* PPGameTextureManager::loadPNG(unsigned char* pixel,unsigned long* width,unsigned long* height,unsigned long* bytesPerRow)
{
	if (pixel == NULL) {
		
		pixel = (unsigned char*)calloc(1,64*64*4);
		*width = 64;
		*height = 64;
		*bytesPerRow = 64*4;
		
	} else {

		int w,h;
		w = (int)(*width);
		h = (int)(*height);
		int tw = calcTextureSize(w);
		int th = calcTextureSize(h);
		if (tw == w && th == h) return pixel;
		
		int ps = (int)(*bytesPerRow/w);
		unsigned char* pt = (unsigned char*)malloc(tw*th*ps);
		for (int y=0;y<th;y++) {
			for (int x=0;x<tw;x++) {
				if (x < w && y < h) {
					for (int i=0;i<ps;i++) {
						pt[x*ps+i+y*tw*ps] = pixel[x*ps+i+y*(*bytesPerRow)];
					}
				} else {
					for (int i=0;i<ps;i++) {
						pt[x*ps+i+y*tw*ps] = 0;
					}
				}
			}
		}
		
		free(pixel);

		pixel = pt;
		*width = tw;
		*height = th;
		*bytesPerRow = tw*ps;
	}
	
 	return pixel;
}

//int PPGameTexture::SetTileSize(int index,int tileWidth,int tileHeight)
//{
//	int i=index;
//	if (i < 0) return -1;
//	if (texture[i]!=NULL) {
//		texture[i]->sizew = tileWidth;
//		texture[i]->sizeh = tileHeight;
//	}
//	return 0;
//}

GLuint PPGameTextureManager::getTextureName(int index)
{
	int i=index;
	if (i < 0) return 0;
	if (i >= PPGAME_MAX_TEXTURE) return -1;
	if (texture[i]!=NULL) {
//		if (texture[i]->sharedTexture >= 0) {
//			return texture[texture[i]->sharedTexture]->texture_name;
//		}
		return texture[i]->texture_name;
	}
	return 0;
}

//GLuint PPGameTexture::GetTextureName(int index)
//{
//	return __m->getTextureName(index);
//}

const char* PPGameTextureManager::getName(int index)
{
	int i=index;
	if (i < 0) return "";
	if (i >= PPGAME_MAX_TEXTURE) return "";
	if (texture[i]!=NULL) {
		return texture[i]->name.c_str();
	}
	return "";
}

//const char* PPGameTexture::GetName(int index)
//{
//	return __m->getName(index);
//}

//PPSize PPGameTexture::GetTileSize(int index)
//{
//	PPSize s(0,0);
//	int i=index;
//	if (i < 0) return s;
//	if (texture[i]!=NULL) {
//		s.width = texture[i]->sizew;
//		s.height = texture[i]->sizeh;
//	}
//	return s;
//}

PPSize PPGameTextureManager::getTextureSize(int index)
{
	PPSize s(0,0);
	int i=index;
	if (i < 0) return s;
	if (i >= PPGAME_MAX_TEXTURE) return s;
	if (texture[i]!=NULL) {
		s.width  = texture[i]->width;
		s.height = texture[i]->height;
	}
	return s;
}

//PPSize PPGameTexture::GetTextureSize(int index)
//{
//	return __m->getTextureSize(index);
//}

PPSize PPGameTextureManager::getImageSize(int index)
{
	PPSize s(0,0);
	int i=index;
	if (i < 0) return s;
	if (i >= PPGAME_MAX_TEXTURE) return s;
	if (texture[i]!=NULL) {
		s.width  = texture[i]->size.width;
		s.height = texture[i]->size.height;
	}
	return s;
}

//PPSize PPGameTexture::GetImageSize(int index)
//{
//	return __m->getImageSize(index);
//}

PPSize PPGameTextureManager::patternSize(int textureid,int group)
{
	if (textureid < 0) return PPSizeZero;
	if (textureid >= PPGAME_MAX_TEXTURE) return PPSizeZero;
	PPGameImage* p = texture[textureid]->group;
	return PPSize(p[group].width,p[group].height);
}

//PPSize PPGameTexture::PatternSize(int textureid,int group)
//{
//	return __m->patternSize(textureid,group);
//}

bool PPGameTextureManager::checkBind(int index)
{
	int i=index;
	if (i < 0) return false;
	if (i >= PPGAME_MAX_TEXTURE) return false;
	if (texture[i]!=NULL) {
//		if (texture[i]->sharedTexture >= 0) {
//			return (texture[texture[i]->sharedTexture]->texture_name != 0);
//		}
		return (texture[i]->texture_name != 0);
	}
	return false;
}

//bool PPGameTexture::checkBind(int index)
//{
//	return __m->checkBind(index);
//}

int PPGameTextureManager::setTextureWithNameCheck(PPGameTexture* _texture)
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			if (texture[i]->name == _texture->name && texture[i]->option == _texture->option) {
			
				deleteTexture(i);
			
				texture[i] = _texture;
				texture[i]->index = i;
				texture[i]->manager = this;
				texture[i]->notexturefile = _texture->notexturefile;
			
				return i;
			}
		}
	}
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (!texture[i]) {
			texture[i] = _texture;
			texture[i]->index = i;
			texture[i]->manager = this;
			return i;
		}
	}
	return -1;
}

int PPGameTextureManager::setTexture(PPGameTexture* _texture)
{
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (!texture[i]) {
			texture[i] = _texture;
			texture[i]->index = i;
			texture[i]->manager = this;
			return i;
		}
	}
	return -1;
}

//int PPGameTexture::SetTexture(PPGameTexture* _texture)
//{
//	return __m->setTexture(_texture);
//}

int PPGameTextureManager::setTextureList(PPGameTextureInfo* texList)
{
	for (int i=0;texList[i].name!=NULL;i++) {
		if (texture[i]==NULL) {
			texture[i] = new PPGameTexture();
			if (texture[i]) {
				texture[i]->index = i;
				texture[i]->name = texList[i].name;
				texture[i]->group = texList[i].group;
//				int chipsize = texList[i].chipsize;
//				if (chipsize == 0) chipsize = 32;
//				texture[i]->sizew = chipsize;
//				texture[i]->sizeh = chipsize;
				texture[i]->option = texList[i].option;
				texture[i]->manager = this;
			}
		} else {
			texture[i]->index = i;
			texture[i]->name = texList[i].name;
			texture[i]->group = texList[i].group;
//			int chipsize = texList[i].chipsize;
//			if (chipsize == 0) chipsize = 32;
//			texture[i]->sizew = chipsize;
//			texture[i]->sizeh = chipsize;
			texture[i]->option = texList[i].option;
		}
		if (texList[i].chipsize != 1) {
			loadTexture(i);
		} else {
			freeTexture(i);
		}
	}
	return 0;
}

//int PPGameTexture::SetTextureList(PPGameTextureInfo* texList)
//{
//	return __m->setTextureList(texList);
//}

int PPGameTextureManager::getTextureCount()
{
	int c = 0;
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			c++;
		}
	}
	return c;
}

//int PPGameTexture::GetTextureCount()
//{
//	return __m->getTextureCount();
//}

PPGameTexture* PPGameTextureManager::getTexture(int index)
{
	return texture[index];
}

//PPGameTexture* PPGameTexture::GetTexture(int index)
//{
//	return __m->getTexture(index);
//}

//void PPGameTexture::SetDefaultTexture(int texture)
//{
//	__m->defaultTexture = texture;
//}

//int PPGameTexture::DefaultTexture()
//{
//	return __m->defaultTexture;
//}

void PPGameTextureManager::dump()
{
	printf("------\n");
	for (int i=0;i<PPGAME_MAX_TEXTURE;i++) {
		if (texture[i]) {
			printf("texture[%d]->texture_name = %d\n",i,texture[i]->texture_name);
		}
	}
}

//void PPGameTexture::dump()
//{
//	__m->dump();
//}

void PPGameTextureManager::pushTextureInfo(lua_State* L,int tex)
{
	lua_createtable(L,0,3);
	lua_pushinteger(L,tex+1);
	lua_setfield(L,-2,"index");			//Texture

	lua_pushstring(L,getName(tex));
	lua_setfield(L,-2,"name");			//Texture

	{
		PPSize ts = getTextureSize(tex);
		lua_createtable(L, 0, 2);
		lua_pushinteger(L,ts.width);
		lua_setfield(L, -2, "width");
		lua_pushinteger(L,ts.height);
		lua_setfield(L, -2, "height");
	}
	lua_setfield(L,-2,"size");

	{
		PPSize ts = getImageSize(tex);
		lua_createtable(L, 0, 2);
		lua_pushinteger(L,ts.width);
		lua_setfield(L, -2, "width");
		lua_pushinteger(L,ts.height);
		lua_setfield(L, -2, "height");
	}
	lua_setfield(L,-2,"imageSize");
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
