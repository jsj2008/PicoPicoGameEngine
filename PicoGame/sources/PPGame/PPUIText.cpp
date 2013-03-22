/*-----------------------------------------------------------------------------------------------
	名前	PPUIText.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPUIText.h"
#include "QBGame.h"
#include "PPNormalFont.h"

PPUIText::PPUIText(PPWorld* world) : PPUIScrollView(world)
{
}

PPUIText::~PPUIText()
{
	clearLineStr();
}

void PPUIText::init()
{
	PPUIScrollView::init();
	column = 1;
	autoScroll = false;
	runScroll = false;
	message = "";
	messagePtr = 0;
	messageTimer = 0;
	messageSpeed = 0;
	autoCenter = true;
	hideCell = false;
	updateString = false;
	dragged = false;
	selectable = false;
	multiselectable = false;
	emptyselection = true;
	marginLeft = 0;
	marginRight = 0;
	marginTop = 0;
	marginBottom = 0;
	selectedCell = -1;
	selectedLines.clear();
	selectedColor = PPColor::red();
	clearLineStr();
}

void PPUIText::start()
{
	lineHeight = 16;
	maxLine = 0;
	column = 1;
	autoScroll = false;
	runScroll = false;
	message = "";
	messagePtr = 0;
	messageTimer = 0;
	messageSpeed = 0;
	autoCenter = true;
	hideCell = false;
	updateString = false;
	dragged = false;
	selectable = false;
	multiselectable = false;
	emptyselection = true;
	marginLeft = 0;
	marginRight = 0;
	marginTop = 0;
	marginBottom = 0;
	selectedCell = -1;
	selectedLines.clear();
	selectedColor = PPColor::red();
	clearLineStr();
	PPUIScrollView::start();
}

void PPUIText::stepIdle()
{
	if (!runScroll) {
		if (message!="") {
			if (messageSpeed==0) {
				str += message;
				message = "";
				if (autoScroll) runScroll = true;
				calcContentsRect();
			} else {
				messageTimer++;
				if (messageTimer >= messageSpeed) {
					messageTimer = 0;
					int p=messagePtr;
					if (message.c_str()[p]==0) {
						message = "";
						messageTimer = 0;
						messagePtr = 0;
						calcContentsRect();
					} else {
						p = PPGetCharBytesUTF8(&message.c_str()[p]);
						char* s=(char*)calloc(1,p+1);
						if (s) {
							memcpy(s,&message.c_str()[messagePtr],p);
							s[p] = 0;
							std::string tt=s;
							str += tt;
							messagePtr += p;
							if (!(s[0] >= 0 && s[0] < 0x20)) {
								updateString = true;
							}
							free(s);
							if (autoScroll) runScroll = true;
	//						calcContentsRect(false);
							calcContentsRect();
						}
					}
				}
			}
		}
	}
	if (autoScroll && runScroll) {
		if (_contentsRect.y+_contentsRect.height > pos.y+_frameSize.height) {
			manualScroll = false;
			float d = (_contentsRect.y+_contentsRect.height-(pos.y+_frameSize.height))/8;
			if (d < 4) d = 4;
			dpos.y -= d;
		} else {
			manualScroll = true;
			runScroll = false;
		}
	}
}

void PPUIText::setFrameSize(PPSize r)
{
	PPUIScrollView::setFrameSize(r);
	_contentsRect.width = r.width;
}

PPPoint PPUIText::nextLine(int line,PPPoint pos)
{
	colPos[line % column].y = pos.y+lineHeight*poly.scale.y+marginTop+marginBottom;
	return colPos[(line+1) % column];
}

void PPUIText::calcContentsRect(bool updateLineStr)
{
	if (updateLineStr) clearLineStr();
	resetColPos();
	std::vector<std::string*>* linearray=NULL;
	std::string linestr="";
	float maxy = 0;
	float lineh=lineHeight*poly.scale.y;
	maxLine=0;
	lineWidth = _frameSize.width;
	PPPoint tpos = colPos[0];
	if (maxy < tpos.y) maxy = tpos.y;
	char* sptr = (char*)str.c_str();
	int i;
	for (i=0;i<strlen(sptr);) {
		char s[8];
		int n = PPGetCharBytesUTF8(&sptr[i]);
		strncpy(s,&sptr[i],n);
		s[n] = 0;
		if (s[0] == '\r' || s[0] == '\n') {
			if (updateLineStr) {
				if (linearray==NULL) linearray = new std::vector<std::string*>;
				linearray->push_back(new std::string(linestr));
				appendLineStr(linearray);
				linearray = new std::vector<std::string*>;
				linestr = "";
			}
			
			tpos = nextLine(maxLine,tpos);
			if (maxy < tpos.y+lineh) maxy = tpos.y+lineh;

//			tpos.x = 0;
//			tpos.y += lineHeight*poly.scale.y;

			maxLine++;
		} else {
			if (tpos.x+length(s)*poly.scale.x > colPos[maxLine % column].x+lineWidth/column-(marginLeft+marginRight)) {
				if (updateLineStr) {
					if (linearray==NULL) linearray = new std::vector<std::string*>;
					linearray->push_back(new std::string(linestr));
					linestr = "";
				}

				tpos.x = colPos[maxLine % column].x;
				tpos.y += lineHeight*poly.scale.y;
				if (maxy < tpos.y+lineh) maxy = tpos.y+lineh;

			}
			tpos.x += length(s)*poly.scale.x;
			if (updateLineStr) {
				linestr+=s;
			}
		}
		i += n;
	}
	if (i>0) {
//		tpos = nextLine(maxLine,tpos);
//		if (maxy < tpos.y) maxy = tpos.y;
		maxLine++;
//		tpos = nextLine(maxLine,tpos);
//		if (maxy < tpos.y) maxy = tpos.y;
//		tpos.y += lineHeight*poly.scale.y;
		if (updateLineStr) {
			if (linearray==NULL) linearray = new std::vector<std::string*>;
			linearray->push_back(new std::string(linestr));
			appendLineStr(linearray);
			linestr = "";
		}
	}
	_contentsRect.width = _frameSize.width;
	_contentsRect.height = maxy;//tpos.y;
	if (!autoCenter) {
		if (_contentsRect.height < _frameSize.height) {
			_contentsRect.height = _frameSize.height;
		}
	}
}

PPRect PPUIText::getLineRect(int _line)
{
	int line=0;
	if (_line < 0 || _line >= maxLine) return PPRectZero;
	resetColPos();
	PPRect r(0,0,0,0);
	lineWidth = _frameSize.width;
	PPPoint tpos = colPos[0];
	r.x = tpos.x;
	r.y = tpos.y;
	if (_line >= 0) {
		char* sptr = (char*)str.c_str();
		for (int i=0;i<strlen(sptr);) {
			char s[8];
			int n = PPGetCharBytesUTF8(&sptr[i]);
			strncpy(s,&sptr[i],n);
			s[n] = 0;
			if (s[0] == '\r' || s[0] == '\n') {
				tpos = nextLine(line,tpos);
				line++;
				if (line == _line) {
					r = tpos;
				}
			} else {
				if (tpos.x+length(s)*poly.scale.x > colPos[line % column].x+lineWidth/column-(marginLeft+marginRight)) {

					tpos.x = colPos[line % column].x;
					tpos.y += lineHeight*poly.scale.y;

				}
				tpos.x += length(s)*poly.scale.x;
				if (line == _line) {
					if (r.width < tpos.x-r.x) r.width = tpos.x-r.x;
					r.height = tpos.y+lineHeight*poly.scale.y-r.y;
				}
			}
			if (line > _line) {
				break;
			}
			i += n;
		}
	}
	r.x += _contentsRect.x;
	r.y += _contentsRect.y;
	return r;
}

PPRect PPUIText::getCurRect(int _line)
{
	int line=0;
	if (_line < 0 || _line >= maxLine) return PPRectZero;
	resetColPos();
	PPRect r(0,0,0,0);
	lineWidth = _frameSize.width;
	PPPoint tpos = colPos[0];
	r.x = tpos.x;
	r.y = tpos.y;
	if (_line >= 0) {
		char* sptr = (char*)str.c_str();
		for (int i=0;i<strlen(sptr);) {
			char s[8];
			int n = PPGetCharBytesUTF8(&sptr[i]);
			strncpy(s,&sptr[i],n);
			s[n] = 0;
			if (s[0] == '\r' || s[0] == '\n') {
				tpos = nextLine(line,tpos);
				line++;
				if (line == _line) {
					r = tpos;
				}
			} else {
				if (tpos.x+length(s)*poly.scale.x > colPos[line % column].x+lineWidth/column-(marginLeft+marginRight)) {

					tpos.x = colPos[line % column].x;
					tpos.y += lineHeight*poly.scale.y;

				}
				tpos.x += length(s)*poly.scale.x;
				if (line == _line) {
//					if (r.width < tpos.x-r.x) r.width = tpos.x-r.x;
//					r.height = tpos.y+lineHeight*poly.scale.y-r.y;
					r = tpos;
					r.height = lineHeight;
				}
			}
			if (line > _line) {
				break;
			}
			i += n;
		}
	}
	r.x += _contentsRect.x;
	r.y += _contentsRect.y;
	return r;
}

void PPUIText::drawSelf(PPPoint _pos)
{
	resetColPos();
	lineWidth = _frameSize.width;
	QBGame* m = (QBGame*)world();

	float fontHeight=32;
	if (m->curFont) {
		fontHeight=m->curFont->height();
	} else {
		fontHeight=m->__normalFont->height();
	}

	PPSize r = frameSize();

	PPPoint tpos = colPos[0];
	PPPoint oldp = PPPoint(m->locatex,m->locatey);
	PPPoint olds = m->GetScale();
	PPPoint oldo = m->rotate_center;
	PPColor oldc = m->color();
	
	bool* selectflag = (bool*)calloc(1,maxLine*sizeof(bool));
	for (int i=0;i<selectedLines.size();i++) {
		int t=selectedLines[i];
		if (t >= 0 && t < maxLine) {
			selectflag[t] = true;
		}
	}
	
	m->scale(poly.scale);
	char* sptr = (char*)str.c_str();
	int line=0;
	for (int i=0;i<strlen(sptr);) {
		char s[8];
		int n = PPGetCharBytesUTF8(&sptr[i]);
		strncpy(s,&sptr[i],n);
		s[n] = 0;
		if (s[0] == '\r' || s[0] == '\n') {

			tpos = nextLine(line,tpos);

//			tpos.x = 0;
//			tpos.y += lineHeight*poly.scale.y;

			line++;
		} else {
			if (tpos.x+length(s)*poly.scale.x  > colPos[line % column].x+lineWidth/column-(marginLeft+marginRight)) {

				tpos.x = colPos[line % column].x;
				tpos.y += lineHeight*poly.scale.y;

			}
			PPPoint ppp = (_contentsRect.pos() + tpos)+_pos;
			if (ppp.y >= pos.y+r.height+(lineHeight+fontHeight)) break;
			if (ppp.y < pos.y+r.height+(lineHeight+fontHeight) && ppp.y > pos.y-(lineHeight+fontHeight)
			 && ppp.x < pos.x+r.width +lineWidth  && ppp.x > pos.x-lineWidth) {
				m->rotate_center = ppp;
				if (!hideCell) {
					if (selectflag[line]) {
						m->color(selectedColor);
					} else {
						m->color(PPColor(255,255,255,255));
					}
					m->locate(ppp);
					m->print(s);
				}
			}

			tpos.x += length(s)*poly.scale.x;
		}
		i += n;
	}
	m->locate(oldp);
	m->scale(olds);
	m->color(oldc);
	m->rotate_center = oldo;
	
	if (selectflag) free(selectflag);
}

void PPUIText::touchDown(PPPoint _pos)
{
	PPUIScrollView::touchDown(_pos);
	if (speed.length() > 0) {
		dragged = true;
	} else {
		dragged = false;
	}
}

void PPUIText::touchUp(PPPoint _pos)
{
	selectedCell = -1;
	PPUIScrollView::touchUp(_pos);
//printf("pos %f,%f\n",_pos.x,_pos.y);
	if (column > 0 && dragged==false) {
		PPPoint s = world()->projector->st.scale;
		PPPoint g = world()->projector->st.origin;
		PPPoint o = world()->projector->st.offset;
		
		for (int i=0;i<maxLine;i++) {
			PPRect r = getLineRect(i);
			r.x = (r.x-g.x)*s.x+g.x+o.x;
			r.y = (r.y-g.y)*s.y+g.y+o.y;
			r.width = lineWidth/column;
			r.width  *= s.x;
			r.height *= s.y;
//printf("r[%d] %f,%f,%f,%f\n",i,r.x,r.y,r.width,r.height);
			if (_pos.hitCheck(r)) {
				bool strexist=false;
				
				if (lineStr.size() > 0) {
					if (lineStr[i]->size() > 0) {
						for (int j=0;j<lineStr[i]->size();j++) {
							if ((*lineStr[i])[j]->c_str()[0]!=0) {
								strexist = true;
								break;
							}
						}
					}
				}
				
				if (strexist) {
					if (selectable) {
						if (!multiselectable) {
							if (!emptyselection) {
								selectedLines.clear();
							}
						}
						bool s=false;
						for (int j=0;j<selectedLines.size();j++) {
							if (selectedLines[j] == i) {
								selectedLines[j] = -1;
								if (!emptyselection && isEmpty()) {
									selectedLines[j] = i;
									selectedCell = i;
								}
								s=true;
							}
						}
						if (!s) {
							if (!multiselectable) {
								if (emptyselection) {
									selectedLines.clear();
								}
							}
							s=false;
							for (int j=0;j<selectedLines.size();j++) {
								if (selectedLines[j] < 0) {
									selectedLines[j] = i;
									selectedCell = i;
									s=true;
									break;
								}
							}
							if (!s) {
								selectedLines.push_back(i);
								selectedCell = i;
							}
						}
						if (!emptyselection && isEmpty()) {
							selectedLines.push_back(0);
						}
					}
				}
				
				break;
			}
		}
	}
}

void PPUIText::touchDrag(PPPoint _pos)
{
	PPUIScrollView::touchDrag(_pos);
	dragged = true;
}

#include "PPLuaScript.h"

static int funcIdle(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
//	PPUIText* m = (PPUIText*)s->userData(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	m->selectedCell=-1;
	m->idle();
	lua_pushboolean(L,m->updateString);
	m->updateString = false;
	return 1;
}

static int funcText(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->str = s->args(0);
		m->calcContentsRect();
		m->runScroll = true;
		m->updateString = true;
	}
	lua_pushstring(L,m->str.c_str());
	return 1;
}

static int funcClear(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	m->str = "";
	m->calcContentsRect();
	m->selectedCell = -1;
	m->selectedLines.clear();
	return 0;
}

static int funcLineHeight(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->lineHeight = s->number(0);
		m->calcContentsRect();
		return 0;
	}
	lua_pushnumber(L,m->lineHeight);
	return 1;
}

static int funcLineWidth(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->lineWidth = s->number(0);
		m->calcContentsRect();
		return 0;
	}
	lua_pushnumber(L,m->lineWidth);
	return 1;
}

static int funcLineRect(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	PPRect r = PPRect(0,0,0,0);
	if (s->argCount > 0) {
		int i = (int)s->integer(0);
		r = m->getLineRect(i-1);
	}
	return s->returnRect(L,r);
}

static int funcLineString(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	bool ok=false;
	if (s->argCount > 0) {
		int i = (int)s->integer(0)-1;
		if (i >= 0 && i < m->lineStr.size()) {
			if (m->lineStr[i]) {
				lua_createtable(L,(int)m->lineStr[i]->size(),0);
				for (int j=0;j<m->lineStr[i]->size();j++) {
					std::vector<std::string*>* s = m->lineStr[i];
					lua_pushstring(L,(*s)[j]->c_str());
					lua_rawseti(L,-2,j+1);
				}
				ok=true;
			}
		}
	}
	if (!ok) {
		lua_createtable(L,0,0);
//		lua_pushstring(L,"");
	}
	return 1;
}

static int funcAutoScroll(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->autoScroll = s->boolean(0);
		return 0;
	}
	lua_pushboolean(L,m->autoScroll);
	return 1;
}

static int funcAutoCenter(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->autoCenter = s->boolean(0);
		m->calcContentsRect();
		return 0;
	}
	lua_pushboolean(L,m->autoCenter);
	return 1;
}

static int funcHideCell(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->hideCell = s->boolean(0);
		return 0;
	}
	lua_pushboolean(L,m->hideCell);
	return 1;
}

static int funcSetSpeed(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->messageSpeed = (int)s->integer(0);
		return 0;
	}
	lua_pushinteger(L,m->messageSpeed);
	return 0;
}

static int funcAdd(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->messageSpeed==0) {
		m->str+=s->args(0);
		m->str+="\n";
		m->calcContentsRect();
		m->runScroll = true;
		m->updateString = true;
	} else {
		m->message+=s->args(0);
		m->message+="\n";
	}
	return 0;
}

static int funcSetMessage(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (m->messageSpeed==0) {
		m->str+=s->args(0);
		m->calcContentsRect();
		m->runScroll = true;
		m->updateString = true;
	} else {
		m->message+=s->args(0);
	}
	return 0;
}

static int funcDone(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	lua_pushboolean(L,m->message=="");
	return 1;
}

static int funcWaiting(lua_State* L)
{
	return 0;
}

static int funcCurPos(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	PPRect r = m->getCurRect(m->maxLine-1);
	if (r.height == 0) {
		return s->returnPoint(L,PPPoint(r.x+r.width,r.y+r.height));
	}
	return s->returnPoint(L,PPPoint(r.x+r.width,r.y+r.height-m->lineHeight));
}

static int funcLineCount(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	lua_pushinteger(L,m->maxLine);
	return 1;
}

static int funcSelectedLines(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->selectedLines.clear();
		if (s->isTable(L,0)) {
			int n = luaL_len(L,2);
			for (int i=0;i<n;i++) {
				int top = lua_gettop(L);
				lua_rawgeti(L,2,i+1);
				m->selectedLines.push_back((int)lua_tointeger(L,-1)-1);
				lua_settop(L,top);
			}
		} else {
			m->selectedLines.push_back((int)s->integer(0));
		}
		return 0;
	}
	lua_createtable(L,(int)m->selectedLines.size(),0);
	for (int i=0;i<m->selectedLines.size();i++) {
		lua_pushinteger(L,m->selectedLines[i]+1);
		lua_rawseti(L,-2,i+1);
	}
	return 1;
}

static int funcSelectable(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->selectable = s->boolean(0);
		return 0;
	}
	lua_pushboolean(L,m->selectable);
	return 1;
}

static int funcMultiSelectable(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->multiselectable = s->boolean(0);
		return 0;
	}
	lua_pushboolean(L,m->multiselectable);
	return 1;
}

static int funcEmptySelection(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->emptyselection = s->boolean(0);
		if (!m->emptyselection && m->isEmpty()) {
			m->selectedLines.push_back(0);
		}
		return 0;
	}
	lua_pushboolean(L,m->emptyselection);
	return 1;
}

static int funcSelectedColor(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->selectedColor.r = s->tableInteger(L,0,1,"r",m->selectedColor.r);
			m->selectedColor.g = s->tableInteger(L,0,2,"g",m->selectedColor.g);
			m->selectedColor.b = s->tableInteger(L,0,3,"b",m->selectedColor.b);
			m->selectedColor.a = s->tableInteger(L,0,4,"a",m->selectedColor.a);
		} else {
			m->selectedColor.r = s->integer(0);
			if (s->argCount > 1) m->selectedColor.g = s->integer(1);
			if (s->argCount > 2) m->selectedColor.b = s->integer(2);
			if (s->argCount > 3) m->selectedColor.a = s->integer(3);
		}
		return 0;
	}
	lua_createtable(L, 0, 4);
	lua_pushinteger(L,m->selectedColor.r);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L,m->selectedColor.g);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L,m->selectedColor.b);
	lua_setfield(L, -2, "b");
	lua_pushinteger(L,m->selectedColor.a);
	lua_setfield(L, -2, "a");
	return 1;
}

static int funcSetColumn(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		m->column = (int)s->integer(0);//atoi(s->args(0));
		if (m->column < 1) m->column=1;
		if (m->column >= PPUIText::MAX_COLUMN) m->column=PPUIText::MAX_COLUMN;
		m->calcContentsRect();
		return 0;
	}
	lua_pushinteger(L,m->column);
	return 1;
}

static int funcSelectedCell(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	lua_pushinteger(L,m->selectedCell+1);
	return 1;
}

static int funcMargin(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->marginTop = s->tableInteger(L,0,1,"top",m->marginTop);
			m->marginRight = s->tableInteger(L,0,2,"right",m->marginRight);
			m->marginBottom = s->tableInteger(L,0,3,"bottom",m->marginBottom);
			m->marginLeft = s->tableInteger(L,0,4,"left",m->marginLeft);
		} else {
			if (s->argCount == 1) {
				m->marginTop = m->marginRight = m->marginBottom = m->marginLeft = s->number(0);
			}
			if (s->argCount == 2) {
				m->marginTop = m->marginBottom = s->number(0);
				m->marginRight =  m->marginLeft = s->number(1);
			}
			if (s->argCount == 3) {
				m->marginTop = s->number(0);
				m->marginRight =  m->marginLeft = s->number(1);
				m->marginBottom = s->number(2);
			}
			if (s->argCount >= 4) {
				m->marginTop = s->number(0);
				m->marginRight = s->number(1);
				m->marginBottom = s->number(2);
				m->marginLeft = s->number(3);
			}
		}
		m->calcContentsRect();
		return 0;
	}
	lua_createtable(L, 0, 4);
	lua_pushinteger(L,m->marginTop);
	lua_setfield(L, -2, "top");
	lua_pushinteger(L,m->marginRight);
	lua_setfield(L, -2, "right");
	lua_pushinteger(L,m->marginBottom);
	lua_setfield(L, -2, "bottom");
	lua_pushinteger(L,m->marginLeft);
	lua_setfield(L, -2, "left");
	return 1;
}

static int funcScale(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			m->poly.scale = s->getPoint(L,0,m->poly.scale.x,m->poly.scale.y);
		} else
		if (s->argCount == 1) {
			m->poly.scale = PPPoint(s->number(0),s->number(0));
		} else {
			m->poly.scale = s->getPoint(L,0,m->poly.scale.x,m->poly.scale.y);
		}
		m->calcContentsRect();
		return 0;
	}
	return s->returnPoint(L,m->poly.scale);
}

static int funcHide(lua_State* L)
{
	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
//	PPLuaScript* s = PPLuaScript::sharedScript();
//	PPObject* m = (PPObject*)s->userData(L);
	if (m==NULL) {
		return luaL_argerror(L,1,"invalid argument.");
	}
	m->resetScroll();
//	m->dragged=false;
//	m->setTouch(false);
	m->hide();
	return 0;
}

static int funcDelete(lua_State *L)
{
	delete (PPUIText*)(PPLuaScript::DeleteObject(L));
	return 0;
}

static int funcNew(lua_State *L)
{
	PPWorld* world = PPLuaScript::World(L);
//	PPUIText* m = (PPUIText*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	if (s->argCount > 0) {
		lua_createtable(L,(int)s->integer(0),0);
		int table = lua_gettop(L);
		for (int i=0;i<s->integer(0);i++) {
			PPUIText* obj = new PPUIText(world);
			obj->init();
			obj->start();
			PPLuaScript::newObject(L,PPUIText::className.c_str(),obj,funcDelete);
			lua_rawseti(L,table,i+1);
		}
	} else {
		PPUIText* obj = new PPUIText(world);
		obj->init();
		obj->start();
		PPLuaScript::newObject(L,PPUIText::className.c_str(),obj,funcDelete);
	}
	return 1;
}

std::string PPUIText::className = "PPUIText";

PPObject* PPUIText::registClass(PPLuaScript* script,const char* name,const char* superclass)
{
	if (name) PPUIText::className = name;
	return PPUIText::registClass(script,PPUIText::className.c_str(),new PPUIText(script->world()),superclass);
}

PPObject* PPUIText::registClass(PPLuaScript* s,const char* name,PPObject* obj,const char* superclass)
{
	obj->init(s->world());
	PPUIScrollView::registClass(s,name,obj);
	s->openModule(name,obj,funcDelete,superclass);
		s->addCommand("new",funcNew);
		s->addCommand("idle",funcIdle);
		s->addCommand("clear",funcClear);
		s->addCommand("text",funcText);
		s->addCommand("lineHeight",funcLineHeight);
		s->addCommand("lineWidth",funcLineWidth);
		s->addCommand("cellRect",funcLineRect);
		s->addCommand("cellString",funcLineString);
		s->addCommand("scale",funcScale);
		s->addCommand("autoScroll",funcAutoScroll);
		s->addCommand("autoCenter",funcAutoCenter);
		s->addCommand("cellCount",funcLineCount);
		s->addCommand("hideCell",funcHideCell);
		s->addCommand("select",funcSelectedLines);
		s->addCommand("selectedColor",funcSelectedColor);
		s->addCommand("selectable",funcSelectable);
		s->addCommand("multiselectable",funcMultiSelectable);
		s->addCommand("emptyselection",funcEmptySelection);
		s->addCommand("column",funcSetColumn);
		s->addCommand("speed",funcSetSpeed);
		s->addCommand("add",funcAdd);
		s->addCommand("append",funcSetMessage);
		s->addCommand("done",funcDone);
		s->addCommand("cursorPosition",funcCurPos);
		s->addCommand("cursorPos",funcCurPos);
		s->addCommand("margin",funcMargin);
		s->addCommand("lastSelect",funcSelectedCell);
		s->addCommand("hide",funcHide);
	s->closeModule();
	return obj;
}

void PPUIText::openLibrary(PPLuaScript* script,const char* name,const char* superclass)
{
	registClass(script,name,this,superclass);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
