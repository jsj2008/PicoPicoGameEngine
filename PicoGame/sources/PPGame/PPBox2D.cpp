/*-----------------------------------------------------------------------------------------------
	名前	PPBox2D.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include "PPBox2D.h"
#include "PPLuaScript.h"
#include <Box2D/Box2D.h>
#include "QBGame.h"

/*-----------------------------------------------------------------------------------------------
	定数の定義
-----------------------------------------------------------------------------------------------*/

#define BODY_LIST "__bodyList"
#define JOINT_LIST "__jointList"
#define FIX_LIST "__fixtureList"
#define CONTACT_LIST "__contactList"
#define WORLD_LIST "__worldList"
#define FIX_SHAPE "__shape"

#define B2CONTACT "ppb2Contact"
#define B2FIXTURE "ppb2Fixture"
#define B2JOINT "ppb2Joint"
#define B2BODY "ppb2Body"
#define B2WORLD "ppb2World"

#define B2REVOLUTEJOINT "ppb2RevoluteJoint"
#define B2PRISMATICJOINT "ppb2PrismaticJoint"
#define B2DISTANCEJOINT "ppb2DistanceJoint"
#define B2PULLEYJOINT "ppb2PulleyJoint"
#define B2MOUSEJOINT "ppb2MouseJoint"
#define B2GEARJOINT "ppb2GearJoint"
#define B2WHEELJOINT "ppb2WheelJoint"
#define B2WELDJOINT "ppb2WeldJoint"
#define B2FRICTIONJOINT "ppb2FrictionJoint"
#define B2ROPEJOINT "ppb2RopeJoint"
#define B2MOTORJOINT "ppb2MotorJoint"

/*-----------------------------------------------------------------------------------------------
	外部に公開しないプログラム
-----------------------------------------------------------------------------------------------*/

static int funcDestroyJoint(lua_State *L);
static int funcDestroyFixture(lua_State *L);
static int funcDestroyBody(lua_State *L);
static int funcDeleteContact(lua_State *L);

static float getNumber(lua_State* L,int n,float def,const char* field1,const char* field2=NULL)
{
	int top = lua_gettop(L);
	if (lua_istable(L,n)) {
		lua_getfield(L,n,field1);
		if (!lua_isnil(L,-1)) {
			if (field2) {
				lua_getfield(L,-1,field2);
				if (!lua_isnil(L,-1)) {
					def = lua_tonumber(L,-1);
				}
			} else {
				def = lua_tonumber(L,-1);
			}
		}
	}
	lua_settop(L,top);
	return def;
}

static bool getBool(lua_State* L,int n,bool def,const char* field1,const char* field2=NULL)
{
	int top = lua_gettop(L);
	if (lua_istable(L,n)) {
		lua_getfield(L,n,field1);
		if (!lua_isnil(L,-1)) {
			if (field2) {
				lua_getfield(L,-1,field2);
				if (!lua_isnil(L,-1)) {
					def = lua_toboolean(L,-1);
				}
			} else {
				def = lua_toboolean(L,-1);
			}
		}
	}
	lua_settop(L,top);
	return def;
}

static b2Vec2 getPoint(lua_State* L,int n,b2Vec2 def)
{
  b2Vec2 p=def;
	int top = lua_gettop(L);
	if (lua_type(L,n)==LUA_TTABLE) {
    lua_getfield(L,n,"x");
    if (lua_isnil(L,-1)) {
      lua_pop(L,1);
      lua_rawgeti(L,n,1);
    }
    if (!lua_isnil(L,-1)) {
      p.x=lua_tonumber(L,-1);
    }
    lua_pop(L,1);
    lua_getfield(L,n,"y");
    if (lua_isnil(L,-1)) {
      lua_pop(L,1);
      lua_rawgeti(L,n,2);
    }
    if (!lua_isnil(L,-1)) {
      p.y=lua_tonumber(L,-1);
    }
  }
	lua_settop(L,top);
  return p;
}

static b2Vec2 getPoint(lua_State* L,int n,b2Vec2 def,const char* field1,const char* field2=NULL)
{
  b2Vec2 p=def;
	int top = lua_gettop(L);
	if (lua_istable(L,n)) {
		lua_getfield(L,n,field1);
		if (lua_type(L,-1)==LUA_TTABLE) {
			if (field2) {
				lua_getfield(L,-1,field2);
      }
      p=getPoint(L,lua_gettop(L),def);
    }
  }
	lua_settop(L,top);
  return p;
}

static void pushPoint(lua_State* L,b2Vec2 p)
{
  lua_createtable(L,0,2);
  lua_pushnumber(L,p.x);
  lua_setfield(L,-2,"x");
  lua_pushnumber(L,p.y);
  lua_setfield(L,-2,"y");
  lua_getglobal(L,"pppoint_mt");
  lua_setmetatable(L,-2);
}

static lua_Integer getBodyType(lua_State* L,int argn,const char* field,lua_Integer def)
{
	lua_Integer r=def;
	if (lua_istable(L,argn)) {
    int top=lua_gettop(L);
		lua_getfield(L, argn, field);
    if (lua_type(L,-1)==LUA_TSTRING) {
      std::string type = lua_tostring(L,-1);
      if (type == "static") {
        r = b2_staticBody;
      } else
      if (type == "dynamic") {
        r = b2_dynamicBody;
      } else
      if (type == "kinematic") {
        r = b2_kinematicBody;
      } else {
        r = luaL_optint(L, -1, def);
      }
    } else {
      r = luaL_optint(L, -1, def);
    }
		lua_settop(L,top);
	}
	return r;
}

/*-----------------------------------------------------------------------------------------------
	プログラム
-----------------------------------------------------------------------------------------------*/

#pragma mark -

std::string PPBox2D::className;

struct b2AABB;

class PPBox2DDebugDraw : public b2Draw
{
public:
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	void DrawTransform(const b2Transform& xf);

    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    void DrawString(int x, int y, const char* string, ...); 

    void DrawString(const b2Vec2& p, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);
  
  QBGame* game;
  b2Vec2 scale;
};

void PPBox2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
  unsigned char r,g,b,a;
  game->GetRGBColor(game->colorIndex, &r, &g, &b, &a);
  game->SetRGBColor(game->colorIndex, color.r*255, color.g*255, color.b*255, a);
  if (vertexCount > 2) {
    int32 i;
    for (i = 0; i < vertexCount-1; ++i)
    {
      game->Line(vertices[i].x*scale.x,vertices[i].y*scale.y,vertices[i+1].x*scale.x,vertices[i+1].y*scale.y,game->colorIndex);
    }
    game->Line(vertices[i].x*scale.x,vertices[i].y*scale.y,vertices[0].x*scale.x,vertices[0].y*scale.y,game->colorIndex);
  }
  game->SetRGBColor(game->colorIndex, r, g, b, a);
}

void PPBox2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
  unsigned char r,g,b,a;
  game->GetRGBColor(game->colorIndex, &r, &g, &b, &a);
  game->SetRGBColor(game->colorIndex, color.r*255, color.g*255, color.b*255, a);
  if (vertexCount > 2) {
    int32 i;
    for (i = 0; i < vertexCount-1; ++i)
    {
      game->Line(vertices[i].x*scale.x,vertices[i].y*scale.y,vertices[i+1].x*scale.x,vertices[i+1].y*scale.y,game->colorIndex);
    }
    game->Line(vertices[i].x*scale.x,vertices[i].y*scale.y,vertices[0].x*scale.x,vertices[0].y*scale.y,game->colorIndex);
	}
  game->SetRGBColor(game->colorIndex, r, g, b, a);
}

void PPBox2DDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
  unsigned char r,g,b,a;
  game->GetRGBColor(game->colorIndex, &r, &g, &b, &a);
  game->SetRGBColor(game->colorIndex, color.r*255, color.g*255, color.b*255, a);
  PPPoint p0,p1,p2;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
    p2.x=v.x*scale.x;
    p2.y=v.y*scale.y;
    if (i>0) {
      game->Line(p1.x,p1.y,p2.x,p2.y,game->colorIndex);
    } else {
      p0=p2;
    }
    p1=p2;
		theta += k_increment;
	}
  game->Line(p1.x,p1.y,p0.x,p0.y,game->colorIndex);
  game->SetRGBColor(game->colorIndex, r, g, b, a);
}

void PPBox2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
  unsigned char r,g,b,a;
  game->GetRGBColor(game->colorIndex, &r, &g, &b, &a);
  game->SetRGBColor(game->colorIndex, color.r*255, color.g*255, color.b*255, a);
  PPPoint p0,p1,p2;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
    p2.x=v.x*scale.x;
    p2.y=v.y*scale.x;
    if (i>0) {
      game->Line(p1.x,p1.y,p2.x,p2.y,game->colorIndex);
    } else {
      p0=p2;
    }
    p1=p2;
		theta += k_increment;
	}
  game->Line(p1.x,p1.y,p0.x,p0.y,game->colorIndex);
  game->SetRGBColor(game->colorIndex, r, g, b, a);
}

void PPBox2DDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
  unsigned char r,g,b,a;
  game->GetRGBColor(game->colorIndex, &r, &g, &b, &a);
  game->SetRGBColor(game->colorIndex, color.r*255, color.g*255, color.b*255, a);
  game->Line(p1.x*scale.x,p1.y*scale.y,p2.x*scale.x,p2.y*scale.y,game->colorIndex);
  game->SetRGBColor(game->colorIndex, r, g, b, a);
}

void PPBox2DDebugDraw::DrawTransform(const b2Transform& xf)
{
}

void PPBox2DDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
}

void PPBox2DDebugDraw::DrawString(int x, int y, const char *string, ...)
{
}

void PPBox2DDebugDraw::DrawString(const b2Vec2& p, const char *string, ...)
{
}

void PPBox2DDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
  unsigned char r,g,b,a;
  game->GetRGBColor(game->colorIndex, &r, &g, &b, &a);
  game->SetRGBColor(game->colorIndex, c.r*255, c.g*255, c.b*255, a);
  game->Box(aabb->upperBound.x*scale.x,aabb->upperBound.y*scale.y,
            aabb->lowerBound.x*scale.x,aabb->lowerBound.y*scale.y,game->colorIndex);
  game->SetRGBColor(game->colorIndex, r, g, b, a);
}

class ppDestructionListener : public b2DestructionListener {
public:
  ppDestructionListener(PPBox2DWorld*world): world(world) {
  }
	virtual void SayGoodbye(b2Joint* joint);
  virtual void SayGoodbye(b2Fixture* fixture);

  PPBox2DWorld* world;
  unsigned int worldId;
};

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
	float32 normalImpulse;
	float32 tangentImpulse;
	float32 separation;
};

const int32 k_maxContactPoints = 2048;
class ppContactListener : public b2ContactListener {
public:
  ppContactListener(PPBox2DWorld* world): world(world) {
  }

	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
  
  PPBox2DWorld* world;
  unsigned int worldId;
};

class ppContactFilter : public b2ContactFilter {
public:
  ppContactFilter(PPBox2DWorld* world): world(world) {
  }
//	virtual bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);

  PPBox2DWorld* world;
  unsigned int worldId;
};

class PPBox2DContact;

class PPBox2DWorld : public b2World
{
public:
  static unsigned int worldCount;

	PPBox2DWorld(const b2Vec2& gravity) : b2World(gravity),drawScale(10,10),jointEdited(false),bodyEdited(false) {
    destructionListener=NULL;
    contactListener=NULL;
    contactFilter=NULL;
    callbackL=NULL;
    callbackIdx=0;
    box2d = NULL;
    worldId = PPBox2DWorld::worldCount;
    PPBox2DWorld::worldCount++;
	}
	virtual ~PPBox2DWorld();
//  void retain() {
//    if (retainCount == 0) {
//      setListener();
//    }
//    retainCount ++;
//  }
//  bool release() {
//    retainCount --;
//    if (retainCount) return true;
//    return false;
//  }
  
  virtual void drawShape(lua_State* L,int idx=1);
  virtual void drawJoint(lua_State* L,int idx=1);
  
  static bool findFixture(lua_State* L,b2Fixture* fixture,int idx=1);
  static bool findJoint(lua_State* L,b2Joint* joint,int idx=1);
  static bool findBody(lua_State* L,b2Body* body,int idx=1);
  
  static bool findWorld(lua_State* L,unsigned int worldId);
  static void setWorld(lua_State* L,unsigned int worldId,int idx=1);
  static void deleteWorld(lua_State* L,unsigned int worldId);

  void setListener() {
    destructionListener = new ppDestructionListener(this);
    destructionListener->worldId = worldId;
    contactListener = new ppContactListener(this);
    contactListener->worldId = worldId;
    contactFilter = new ppContactFilter(this);
    contactFilter->worldId = worldId;
    //SetDestructionListener(destructionListener);    //不要
    SetContactListener(contactListener);
    SetContactFilter(contactFilter);
  }
  
  ppDestructionListener* destructionListener;
  ppContactListener* contactListener;
  ppContactFilter* contactFilter;
  
  //int retainCount;
  b2Vec2 drawScale;
  
  unsigned int worldId;

  PPBox2D* box2d;
  lua_State* callbackL;
  int callbackIdx;
  
  bool jointEdited;
  bool bodyEdited;
  
//  PPBox2DContact* contactTop;
  
  static std::string className;
};

std::string PPBox2DWorld::className;

unsigned int PPBox2DWorld::worldCount=0;

class PPBox2DBody;
class PPBox2DBodyUserData {
public:
  PPBox2DBodyUserData() {
    dataIndex = count++;
printf("- instance body userdata %d\n",dataIndex);
  }
  virtual ~PPBox2DBodyUserData() {
printf("- destruct body userdata %d\n",dataIndex);
  }
  //std::vector<PPBox2DBody*> list;
  PPBox2DBody* body;
  size_t index;
  unsigned int dataIndex;
  
  static unsigned int count;
};

unsigned int PPBox2DBodyUserData::count=0;

class PPBox2DBody {
public:
  PPBox2DBody(b2Body* body,PPBox2DWorld* world): body(body),world(world)
  {
    worldId = world->worldId;
    PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)body->GetUserData();
    if (userData == NULL) {
      userData = new PPBox2DBodyUserData();
      body->SetUserData(userData);
    }
    fixtureEdited=false;
    userData->index = 0;
    userData->body = this;
    bodyIndex = bodyCount++;
    //userData->list.push_back(this);
  }
  virtual ~PPBox2DBody() {
    if (body) {
      PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)body->GetUserData();
      if (userData) {
//        std::vector<PPBox2DBody*>::iterator it;
//        for( it = userData->list.begin(); it != userData->list.end(); it++ ) {
//          if ((*it) == this) {
//            userData->list.erase(it);
////            delete userData;
//            break;
//          }
//        }
        delete userData;
        body->SetUserData(NULL);
      }
    }
  }
  void destroy(b2World* world) {
    if (body) {
      PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)body->GetUserData();
      if (userData) {
//        std::vector<PPBox2DBody*>::iterator it;
//        for( it = userData->list.begin(); it != userData->list.end(); it++ ) {
//          (*it)->body = NULL;
//        }
        delete userData;
        body->SetUserData(NULL);
      }
      world->DestroyBody(body);
      body=NULL;
    }
  }
  b2Body* body;
  PPBox2DWorld* world;
  unsigned int worldId;
  static std::string className;
  bool fixtureEdited;
  unsigned int bodyIndex;
  
  static unsigned int bodyCount;
};

unsigned int PPBox2DBody::bodyCount=0;

std::string PPBox2DBody::className;

class PPBox2DFixture;
class PPBox2DFixtureUserData {
public:
  PPBox2DFixtureUserData() {
  }
  virtual ~PPBox2DFixtureUserData() {
  }
  PPBox2DFixture* fixture;
  unsigned int worldId;
  size_t index;
};

class PPBox2DFixture {
public:
  PPBox2DFixture(b2Fixture* _fixture): fixture(_fixture)
  {
    PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)fixture->GetUserData();
    if (userData == NULL) {
      userData = new PPBox2DFixtureUserData();
      fixture->SetUserData(userData);
    }
    userData->index = 0;
    userData->fixture=this;
//    userData->list.push_back(this);
  }
  virtual ~PPBox2DFixture() {
    if (fixture) {
      PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)fixture->GetUserData();
      if (userData) {
//        std::vector<PPBox2DFixture*>::iterator it;
//        for( it = userData->list.begin(); it != userData->list.end(); it++ ) {
//          if ((*it) == this) {
//            userData->list.erase(it);
////            delete userData;
//            break;
//          }
//        }
        delete userData;
        fixture->SetUserData(NULL);
      }
    }
  }
  void destroy(b2Body* body) {
    if (fixture) {
      PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)fixture->GetUserData();
      if (userData) {
//        std::vector<PPBox2DFixture*>::iterator it;
//        for( it = userData->list.begin(); it != userData->list.end(); it++ ) {
//          (*it)->fixture = NULL;
//        }
        delete userData;
        fixture->SetUserData(NULL);
      }
      body->DestroyFixture(fixture);
      fixture=NULL;
    }
  }
  b2Fixture* fixture;
  b2Vec2 pivot;
  unsigned int worldId;
  
  static std::string className;
};

std::string PPBox2DFixture::className;

class PPBox2DJoint;
class PPBox2DJointUserData {
public:
  virtual ~PPBox2DJointUserData() {
  }
  //std::vector<PPBox2DJoint*> list;
  PPBox2DJoint* joint;
  unsigned int worldId;
  size_t index;
};

class PPBox2DJoint {
public:
  PPBox2DJoint(b2Joint* joint,PPBox2DWorld* world): joint(joint),world(world)
  {
    PPBox2DJointUserData* userData = (PPBox2DJointUserData*)joint->GetUserData();
    if (userData == NULL) {
      userData = new PPBox2DJointUserData();
      joint->SetUserData(userData);
    }
    userData->index = 0;
    userData->joint = this;
//    userData->list.push_back(this);
  }
  virtual ~PPBox2DJoint() {
    if (joint) {
      PPBox2DJointUserData* userData = (PPBox2DJointUserData*)joint->GetUserData();
      if (userData) {
//        std::vector<PPBox2DJoint*>::iterator it;
//        for( it = userData->list.begin(); it != userData->list.end(); it++ ) {
//          if ((*it) == this) {
//            userData->list.erase(it);
////            delete userData;
//            break;
//          }
//        }
        delete userData;
        joint->SetUserData(NULL);
      }
    }
  }
  void destroy(b2World* world) {
    if (joint) {
      PPBox2DJointUserData* userData = (PPBox2DJointUserData*)joint->GetUserData();
      if (userData) {
//        std::vector<PPBox2DJoint*>::iterator it;
//        for( it = userData->list.begin(); it != userData->list.end(); it++ ) {
//          (*it)->joint = NULL;
//        }
        delete userData;
        joint->SetUserData(NULL);
      }
      world->DestroyJoint(joint);
      joint=NULL;
    }
  }
  b2Joint* joint;
  PPBox2DWorld* world;
  unsigned int worldId;
  
  static std::string className;
  static std::string motorJointClassName;
  static std::string distanceJointClassName;
  static std::string frictionJointClassName;
  static std::string gearJointClassName;
  static std::string prismaticJointClassName;
  static std::string pulleyJointClassName;
  static std::string revoluteJointClassName;
  static std::string ropeJointClassName;
  static std::string weldJointClassName;
  static std::string wheelJointClassName;
  static std::string mouseJointClassName;
};

std::string PPBox2DJoint::className;
std::string PPBox2DJoint::motorJointClassName;
std::string PPBox2DJoint::distanceJointClassName;
std::string PPBox2DJoint::frictionJointClassName;
std::string PPBox2DJoint::gearJointClassName;
std::string PPBox2DJoint::prismaticJointClassName;
std::string PPBox2DJoint::pulleyJointClassName;
std::string PPBox2DJoint::revoluteJointClassName;
std::string PPBox2DJoint::ropeJointClassName;
std::string PPBox2DJoint::weldJointClassName;
std::string PPBox2DJoint::wheelJointClassName;
std::string PPBox2DJoint::mouseJointClassName;

static b2Joint* getJoint(lua_State* L,int n,b2Joint* def,const char* field1)
{
	int top = lua_gettop(L);
	if (lua_istable(L,n)) {
		lua_getfield(L,n,field1);
		if (!lua_isnil(L,-1)) {
			lua_getmetatable(L,-1);
			lua_getfield(L,-1,PPGAMEINSTNACE);
      PPBox2DJoint* joint = (PPBox2DJoint*)lua_touserdata(L,-1);
      if (joint) {
        def=joint->joint;
      }
		}
	}
	lua_settop(L,top);
	return def;
}

class PPBox2DContact {
public:
  PPBox2DContact(b2Contact* contact): contact(contact),prev(NULL),next(NULL)
  {
  }
  
  PPBox2DContact* prev;
  PPBox2DContact* next;
  b2Contact* contact;
  unsigned int worldId;
  
  static std::string className;
};

std::string PPBox2DContact::className;

void ppDestructionListener::SayGoodbye(b2Joint* joint)
{
  lua_State* L = world->callbackL;
  int n=world->callbackIdx;
  if (L) {
    if (lua_gettop(L)>=n) {
      if (lua_istable(L,n)) {
        lua_getfield(L,n,"sayGoodbyJoint");
        if (lua_isfunction(L,-1)) {
          if (!PPBox2DWorld::findJoint(L,joint)) {
            
          }
          if(lua_pcall(L, 1, 0, 0) != 0) {
          }
        }
      }
    }
  }
}

void ppDestructionListener::SayGoodbye(b2Fixture* fixture)
{
  lua_State* L = world->callbackL;
  int n=world->callbackIdx;
  if (L) {
    if (lua_gettop(L)>=n) {
      if (lua_istable(L,n)) {
        lua_getfield(L,n,"sayGoodbyFixture");
        if (lua_isfunction(L,-1)) {
          world->findFixture(L,fixture,1);
          if(lua_pcall(L, 1, 0, 0) != 0) {
          }
        }
      }
    }
  }
}

void ppContactListener::BeginContact(b2Contact* contact)
{
  lua_State* L = world->callbackL;
  int n=world->callbackIdx;
  if (L) {
    if (lua_gettop(L)>=n) {
      if (lua_istable(L,n)) {
        lua_getfield(L,n,"beginContact");
        if (lua_isfunction(L,-1)) {

          PPBox2DContact* ppcontact = new PPBox2DContact(contact);
          ppcontact->worldId = worldId;
          PPLuaScript::newObject(L,B2CONTACT,ppcontact,funcDeleteContact);
          lua_pushvalue(L,1);
          lua_setfield(L,-2,"world");

          if(lua_pcall(L, 1, 0, 0) != 0) {
          }
          
          PPBox2DContact* c=ppcontact;
          while (c) {
            PPBox2DContact* n=c->next;
            c->contact = NULL;
            c=n;
          }

        }
      }
    }
  }
}

void ppContactListener::EndContact(b2Contact* contact)
{
  lua_State* L = world->callbackL;
  int n=world->callbackIdx;
  if (L) {
    if (lua_gettop(L)>=n) {
      if (lua_istable(L,n)) {
        lua_getfield(L,n,"endContact");
        if (lua_isfunction(L,-1)) {
          PPBox2DContact* ppcontact = new PPBox2DContact(contact);
          ppcontact->worldId = worldId;
          PPLuaScript::newObject(L,B2CONTACT,ppcontact,funcDeleteContact);
          lua_pushvalue(L,1);
          lua_setfield(L,-2,"world");

          if(lua_pcall(L, 1, 0, 0) != 0) {
          }
          
          PPBox2DContact* c=ppcontact;
          while (c) {
            PPBox2DContact* n=c->next;
            c->contact = NULL;
            c=n;
          }
        }
      }
    }
  }
}

void ppContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
  lua_State* L = world->callbackL;
  int n=world->callbackIdx;
  if (L) {
    if (lua_gettop(L)>=n) {
      if (lua_istable(L,n)) {
        lua_getfield(L,n,"preSolve");
        if (lua_isfunction(L,-1)) {

          const b2Manifold* manifold = contact->GetManifold();
          b2WorldManifold worldManifold;
          contact->GetWorldManifold(&worldManifold);
          b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
          b2GetPointStates(state1, state2, oldManifold, manifold);

          PPBox2DContact* ppcontact = new PPBox2DContact(contact);
          ppcontact->worldId = worldId;
          PPLuaScript::newObject(L,B2CONTACT,ppcontact,funcDeleteContact);
          lua_pushvalue(L,1);
          lua_setfield(L,-2,"world");

          lua_createtable(L,manifold->pointCount,0);
          for (int32 i=0;i<manifold->pointCount;++i) {
            lua_createtable(L,0,6);
            lua_createtable(L,0,2);
            lua_pushnumber(L,worldManifold.points[i].x);
            lua_setfield(L,-2,"x");
            lua_pushnumber(L,worldManifold.points[i].y);
            lua_setfield(L,-2,"y");
            lua_getglobal(L,"pppoint_mt");
            lua_setmetatable(L,-2);
            lua_setfield(L,-2,"position");

            lua_createtable(L,0,2);
            lua_pushnumber(L,worldManifold.normal.x);
            lua_setfield(L,-2,"x");
            lua_pushnumber(L,worldManifold.normal.y);
            lua_setfield(L,-2,"y");
            lua_getglobal(L,"pppoint_mt");
            lua_setmetatable(L,-2);
            lua_setfield(L,-2,"normal");

            lua_pushinteger(L,state2[i]);
            lua_setfield(L,-2,"state");
            lua_pushnumber(L,manifold->points[i].normalImpulse);
            lua_setfield(L,-2,"normalImpulse");
            lua_pushnumber(L,manifold->points[i].tangentImpulse);
            lua_setfield(L,-2,"tangentImpulse");
            lua_pushnumber(L,worldManifold.separations[i]);
            lua_setfield(L,-2,"separations");
            lua_rawseti(L, -2, i+1);
          }

          if(lua_pcall(L, 2, 0, 0) != 0) {
          }
          
          PPBox2DContact* c=ppcontact;
          while (c) {
            PPBox2DContact* n=c->next;
            c->contact = NULL;
            c=n;
          }

        }
      }
    }
  }
}

void ppContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
  lua_State* L = world->callbackL;
  int n=world->callbackIdx;
  if (L) {
    if (lua_gettop(L)>=n) {
      if (lua_istable(L,n)) {
        lua_getfield(L,n,"postSolve");
        if (lua_isfunction(L,-1)) {

          const b2Manifold* manifold = contact->GetManifold();
          PPBox2DContact* ppcontact = new PPBox2DContact(contact);
          ppcontact->worldId = worldId;
          PPLuaScript::newObject(L,B2CONTACT,ppcontact,funcDeleteContact);
          lua_pushvalue(L,1);
          lua_setfield(L,-2,"world");

          lua_createtable(L,manifold->pointCount,0);

          for (int32 i = 0; i < manifold->pointCount; ++i) {
            lua_createtable(L,0,2);
            lua_pushnumber(L,impulse->normalImpulses[i]);
            lua_setfield(L,-2,"normalImpulse");
            lua_pushnumber(L,impulse->tangentImpulses[i]);
            lua_setfield(L,-2,"tangentImpulse");
            lua_rawseti(L, -2, i+1);
          }

          if(lua_pcall(L, 2, 0, 0) != 0) {
          }
          
          PPBox2DContact* c=ppcontact;
          while (c) {
            PPBox2DContact* n=c->next;
            c->contact = NULL;
            c=n;
          }

        }
      }
    }
  }
}

//bool ppContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) {
//  lua_State* L = world->L;
//  if (lua_gettop(L)>=5) {
//    if (lua_istable(L,5)) {
//      lua_getfield(L,5,"shouldCollide");
//      if (lua_isfunction(L,-1)) {
//        world->findFixture(L,fixtureA,1);
//        world->findFixture(L,fixtureB,1);
//        if(lua_pcall(L, 2, 1, 0) != 0) {
//        }
//      }
//    }
//  }
//  return true;
//}

bool PPBox2DWorld::findFixture(lua_State* L,b2Fixture* fixture,int idx)
{
  if (fixture==NULL) {
    lua_pushstring(L,"not found fixture.");
    return false;
  }
  if (!findBody(L,fixture->GetBody(),idx)) {
    
  }
  int list=lua_gettop(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,list,PPBox2DBody::className);
  if (m==NULL) return false;

  lua_getfield(L,-1,FIX_LIST);
  if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
    size_t len = lua_objlen(L,-1);
#else
    size_t len = lua_rawlen(L,-1);
#endif

    if (m->fixtureEdited) {
      b2Fixture* f = m->body->GetFixtureList();
      while (f) {
        PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)f->GetUserData();
        if (userData) {
          userData->index=0;
        }
        f=f->GetNext();
      }
      m->fixtureEdited=false;
    }

    PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)fixture->GetUserData();
    if (userData) {
      userData->index=0;
    
      if (userData->index == 0) {
        for (int i=0;i<len;i++) {
          int top=lua_gettop(L);
          lua_rawgeti(L,top,i+1);
          int item=lua_gettop(L);
          lua_getmetatable(L,-1);
          lua_getfield(L,-1,PPGAMEINSTNACE);
          PPBox2DFixture* fixture2 = (PPBox2DFixture*)lua_touserdata(L,-1);
          if (fixture2) {
            if (fixture2->fixture == fixture) {
              userData->index=i+1;
              lua_settop(L,item);
              lua_replace(L,list);
              lua_settop(L,list);
              return true;
            }
          }
          lua_settop(L,top);
        }
      }
      
      lua_rawgeti(L,-1,(int)userData->index);
      lua_replace(L,list);
      lua_settop(L,list);
      return true;
    
    } else {
      lua_pushstring(L,"not found fixture.");
      return false;
    }
  }
  lua_settop(L,list);
  lua_pushnil(L);
  return true;
}

bool PPBox2DWorld::findJoint(lua_State* L,b2Joint* joint,int worldIdx)
{
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,worldIdx,PPBox2DWorld::className);
  if (m==NULL) return false;
  if (joint==NULL) {
    lua_pushstring(L,"not found joint.");
    return false;
  }

  lua_getfield(L,worldIdx,JOINT_LIST);
  int list=lua_gettop(L);
  if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
    size_t len = lua_objlen(L,-1);
#else
    size_t len = lua_rawlen(L,-1);
#endif

    if (m->jointEdited) {
      b2Joint* j = m->GetJointList();
      while (j) {
        PPBox2DJointUserData* userData = (PPBox2DJointUserData*)j->GetUserData();
        if (userData) {
          userData->index=0;
        }
        j=j->GetNext();
      }
      m->jointEdited=false;
    }

    PPBox2DJointUserData* userData = (PPBox2DJointUserData*)joint->GetUserData();
    if (userData) {
      userData->index=0;
    
      if (userData->index == 0) {
        for (int i=0;i<len;i++) {
          int top=lua_gettop(L);
          lua_rawgeti(L,top,i+1);
          int item=lua_gettop(L);
          lua_getmetatable(L,-1);
          lua_getfield(L,-1,PPGAMEINSTNACE);
          PPBox2DJoint* joint2 = (PPBox2DJoint*)lua_touserdata(L,-1);
          if (joint2) {
            if (joint2->joint == joint) {
              userData->index=i+1;
              lua_settop(L,item);
              lua_replace(L,list);
              lua_settop(L,list);
              return true;
            }
          }
          lua_settop(L,top);
        }
      }
      
      lua_rawgeti(L,-1,(int)userData->index);
      lua_replace(L,list);
      lua_settop(L,list);
      return true;
    
    } else {
      lua_pushstring(L,"not found joint.");
      return false;
    }

  }
  lua_settop(L,list);
  lua_pushnil(L);
  return true;
}

bool PPBox2DWorld::findBody(lua_State* L,b2Body* body,int worldIdx)
{
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,worldIdx,PPBox2DWorld::className);
  if (m==NULL) return false;
  if (body==NULL) {
    lua_pushstring(L,"not found body.");
    return false;
  }

  lua_getfield(L,worldIdx,BODY_LIST);
  int list=lua_gettop(L);
  if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
    size_t len = lua_objlen(L,-1);
#else
    size_t len = lua_rawlen(L,-1);
#endif

    if (m->bodyEdited) {
      b2Body* b = m->GetBodyList();
      while (b) {
        PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)b->GetUserData();
        if (userData) {
          userData->index=0;
        }
        b=b->GetNext();
      }
      m->bodyEdited=false;
    }

    PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)body->GetUserData();
    if (userData) {
      userData->index=0;
    
      if (userData->index == 0) {
        for (int i=0;i<len;i++) {
          int top=lua_gettop(L);
          lua_rawgeti(L,top,i+1);
          int item=lua_gettop(L);
          lua_getmetatable(L,-1);
          lua_getfield(L,-1,PPGAMEINSTNACE);
          PPBox2DBody* body2 = (PPBox2DBody*)lua_touserdata(L,-1);
          if (body2) {
            if (body2->body == body) {
              userData->index=i+1;
              lua_settop(L,item);
              lua_replace(L,list);
              lua_settop(L,list);
              return true;
            }
          }
          lua_settop(L,top);
        }
      }
      
      lua_rawgeti(L,-1,(int)userData->index);
      lua_replace(L,list);
      lua_settop(L,list);
      return true;
    
    } else {
      lua_pushstring(L,"not found joint.");
      return false;
    }
  }
  lua_settop(L,list);
  lua_pushnil(L);
  return true;
}

bool PPBox2DWorld::findWorld(lua_State* L,unsigned int worldId)
{
  int top=lua_gettop(L);
  lua_getglobal(L,WORLD_LIST);
  if (lua_isnil(L,-1)) {
    lua_settop(L,top);
    return false;
  }
  char str[256];
  snprintf(str,sizeof(str),"%d",worldId);
  lua_getfield(L,-1,str);
  if (lua_isnil(L,-1)) {
    lua_settop(L,top);
    return false;
  }
  return true;
}

void PPBox2DWorld::setWorld(lua_State* L,unsigned int worldId,int idx)
{
  char str[256];
  snprintf(str,sizeof(str),"%d",worldId);
  int top=lua_gettop(L);
  lua_getglobal(L,WORLD_LIST);
  if (lua_isnil(L,-1)) {
    lua_createtable(L,0,0);
    lua_setglobal(L,WORLD_LIST);
  }
  lua_getglobal(L,WORLD_LIST);
  lua_pushvalue(L,idx);
  lua_setfield(L,-2,str);
  lua_settop(L,top);
}

void PPBox2DWorld::deleteWorld(lua_State* L,unsigned int worldId)
{
  int top=lua_gettop(L);
  lua_getglobal(L,WORLD_LIST);
  if (lua_isnil(L,-1)) {
    lua_settop(L,top);
    return;
  }
  char str[256];
  snprintf(str,sizeof(str),"%d",worldId);
  lua_pushnil(L);
  lua_setfield(L,-2,str);
  lua_settop(L,top);
}

void PPBox2DWorld::drawShape(lua_State* L,int idx)
{
  lua_getfield(L,idx,BODY_LIST);
  if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
    size_t len = lua_objlen(L,-1);
#else
    size_t len = lua_rawlen(L,-1);
#endif
    int top=lua_gettop(L);
    for (int i=0;i<len;i++) {
      lua_rawgeti(L,-1,i+1);
      int bodyidx=lua_gettop(L);
      lua_getmetatable(L,-1);
      lua_getfield(L,-1,PPGAMEINSTNACE);
      if (!lua_isnil(L,-1)) {
        PPBox2DBody* body = (PPBox2DBody*)lua_touserdata(L,-1);
        if (body) {
          if (body->body) {
            b2Body* b=body->body;
            lua_settop(L,bodyidx);
            lua_getfield(L,-1,FIX_LIST);
#ifdef __LUAJIT__
            size_t len = lua_objlen(L,-1);
#else
            size_t len = lua_rawlen(L,-1);
#endif
            int top=lua_gettop(L);
            for (int i=0;i<len;i++) {
              lua_rawgeti(L,-1,i+1);
              int fixidx=lua_gettop(L);
              lua_getmetatable(L,-1);
              lua_getfield(L,-1,PPGAMEINSTNACE);
              if (!lua_isnil(L,-1)) {
                PPBox2DFixture* fix = (PPBox2DFixture*)lua_touserdata(L,-1);
                if (fix) {
                  b2Fixture* f=fix->fixture;
                  if (f) {
                    
                    const b2Transform& xf = b->GetTransform();
                    
                    switch (f->GetType()) {
                    case b2Shape::e_circle:
                      lua_getfield(L,fixidx,"draw");
                      if (!lua_isnil(L,-1)) {
                        lua_pushvalue(L,fixidx);
                        lua_pushvalue(L,bodyidx);

                        b2CircleShape* circle = (b2CircleShape*)f->GetShape();

                        b2Vec2 pivot = b2Mul(xf, circle->m_p+fix->pivot);
                        b2Vec2 center = b2Mul(xf, circle->m_p);
                        float32 radius = circle->m_radius;
                        b2Vec2 axis = b2Mul(xf.q, b2Vec2(1.0f, 0.0f));
                        
                        lua_createtable(L, 5, 0);

                        lua_createtable(L,2,0);
                        lua_pushnumber(L,center.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,center.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2,"center");

                        lua_createtable(L,2,0);
                        lua_pushnumber(L,axis.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,axis.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2,"axis");

                        lua_createtable(L,2,0);
                        lua_pushnumber(L,pivot.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,pivot.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2,"pivot");

                        lua_pushnumber(L,radius*drawScale.x);
                        lua_setfield(L,-2,"radius");

                        lua_createtable(L, 0, 2);
                        lua_pushnumber(L,drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2, "drawScale");

                        lua_pushnumber(L,b->GetAngle());
                        lua_setfield(L,-2, "angle");

                        if (lua_pcall(L, 3, 0, 0)!=0) {
                          
                        }

                        //m_debugDraw->DrawSolidCircle(center, radius, axis, color);
                      }
                      break;
                    case b2Shape::e_edge:
                      lua_getfield(L,fixidx,"draw");
                      if (!lua_isnil(L,-1)) {
                        lua_pushvalue(L,fixidx);
                        lua_pushvalue(L,bodyidx);

                        b2EdgeShape* edge = (b2EdgeShape*)f->GetShape();
                        b2Vec2 v1 = b2Mul(xf, edge->m_vertex1);
                        b2Vec2 v2 = b2Mul(xf, edge->m_vertex2);

                        lua_createtable(L, 2, 0);

                        lua_createtable(L,2,0);
                        lua_pushnumber(L,v1.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,v1.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2,"v1");

                        lua_createtable(L,2,0);
                        lua_pushnumber(L,v2.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,v2.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2,"v2");

                        lua_createtable(L, 0, 2);
                        lua_pushnumber(L,drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2, "drawScale");

                        lua_pushnumber(L,b->GetAngle());
                        lua_setfield(L,-2, "angle");
                        
                        if (lua_pcall(L, 3, 0, 0)!=0) {
                          
                        }
                      }
                      break;
                    case b2Shape::e_chain:
                      lua_getfield(L,fixidx,"draw");
                      if (!lua_isnil(L,-1)) {

                        b2ChainShape* chain = (b2ChainShape*)f->GetShape();
                        int32 count = chain->m_count;
                        const b2Vec2* vertices = chain->m_vertices;

                        lua_createtable(L, 0, count);

                        b2Vec2 v1 = b2Mul(xf, vertices[0]);
                        for (int32 i = 1; i < count; ++i)
                        {
                          b2Vec2 v2 = b2Mul(xf, vertices[i]);

                          lua_pushvalue(L,fixidx);
                          lua_pushvalue(L,bodyidx);

                          lua_createtable(L,2,0);

                          lua_createtable(L,2,0);
                          lua_pushnumber(L,v1.x*drawScale.x);
                          lua_setfield(L,-2, "x");
                          lua_pushnumber(L,v1.y*drawScale.y);
                          lua_setfield(L,-2, "y");
                          lua_getglobal(L,"pppoint_mt");
                          lua_setmetatable(L,-2);
                          lua_setfield(L,-2,"v1");

                          lua_createtable(L,2,0);
                          lua_pushnumber(L,v2.x*drawScale.x);
                          lua_setfield(L,-2, "x");
                          lua_pushnumber(L,v2.y*drawScale.y);
                          lua_setfield(L,-2, "y");
                          lua_getglobal(L,"pppoint_mt");
                          lua_setmetatable(L,-2);
                          lua_setfield(L,-2,"v2");

                          lua_createtable(L, 0, 2);
                          lua_pushnumber(L,drawScale.x);
                          lua_setfield(L,-2, "x");
                          lua_pushnumber(L,drawScale.y);
                          lua_setfield(L,-2, "y");
                          lua_getglobal(L,"pppoint_mt");
                          lua_setmetatable(L,-2);
                          lua_setfield(L,-2, "drawScale");

                          lua_pushnumber(L,b->GetAngle());
                          lua_setfield(L,-2, "angle");

                          if (lua_pcall(L, 3, 0, 0)!=0) {
                            
                          }

                          v1 = v2;
                        }

                      }
                      break;
                    case b2Shape::e_polygon:
                      lua_getfield(L,fixidx,"draw");
                      if (!lua_isnil(L,-1)) {
                        lua_pushvalue(L,fixidx);
                        lua_pushvalue(L,bodyidx);

                        b2PolygonShape* poly = (b2PolygonShape*)f->GetShape();
                        int32 vertexCount = poly->m_count;
                        b2Assert(vertexCount <= b2_maxPolygonVertices);
                        b2Vec2 vertices[b2_maxPolygonVertices];

                        lua_createtable(L,2,0);

                        lua_createtable(L,0,vertexCount);
                        for (int32 i = 0; i < vertexCount; ++i)
                        {
                          vertices[i] = b2Mul(xf, poly->m_vertices[i]);

                          lua_createtable(L,2,0);
                          lua_pushnumber(L,vertices[i].x*drawScale.x);
                          lua_setfield(L,-2, "x");
                          lua_pushnumber(L,vertices[i].y*drawScale.y);
                          lua_setfield(L,-2, "y");
                          lua_getglobal(L,"pppoint_mt");
                          lua_setmetatable(L,-2);
                          lua_rawseti(L,-2,i+1);

                        }

                        lua_setfield(L,-2, "vertices");

                        b2Vec2 center = b2Mul(xf, b2Vec2(0,0));
                        lua_createtable(L,2,0);
                        lua_pushnumber(L,center.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,center.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2, "origin");

                        b2Vec2 pivot = b2Mul(xf, poly->m_vertices[0]+fix->pivot);
                        lua_createtable(L,2,0);
                        lua_pushnumber(L,pivot.x*drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,pivot.y*drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2, "pivot");
                        
                        lua_createtable(L, 0, 2);
                        lua_pushnumber(L,drawScale.x);
                        lua_setfield(L,-2, "x");
                        lua_pushnumber(L,drawScale.y);
                        lua_setfield(L,-2, "y");
                        lua_getglobal(L,"pppoint_mt");
                        lua_setmetatable(L,-2);
                        lua_setfield(L,-2, "drawScale");

                        lua_pushnumber(L,b->GetAngle());
                        lua_setfield(L,-2, "angle");

                        if (lua_pcall(L, 3, 0, 0)!=0) {
                          
                        }
                        //m_debugDraw->DrawSolidPolygon(vertices, vertexCount, color);
                      }
                      break;
                    case b2Shape::e_typeCount:
                      break;
                    }
                    
                  }
                }
              }
              lua_settop(L,top);
            }
          }
        }
      }
      lua_settop(L,top);
    }
    
  }
}

void PPBox2DWorld::drawJoint(lua_State* L,int idx)
{
}

PPBox2DWorld::~PPBox2DWorld() {
printf("--- destruct world %u\n",worldId);
  b2Body* body = GetBodyList();
  while (body) {
    b2Body* next = body->GetNext();
    PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)body->GetUserData();
    if (userData) {
      userData->body->body=NULL;
      delete userData;
      body->SetUserData(NULL);
    }
    b2Fixture* fixture=body->GetFixtureList();
    while (fixture) {
      b2Fixture* next=fixture->GetNext();
      PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)fixture->GetUserData();
      if (userData) {
        userData->fixture->fixture=NULL;
        delete userData;
        fixture->SetUserData(NULL);
      }
      fixture=next;
    }
    body=next;
  }
  b2Joint* joint = GetJointList();
  while (joint) {
    b2Joint* next = joint->GetNext();
    PPBox2DJointUserData* userData = (PPBox2DJointUserData*)joint->GetUserData();
    if (userData) {
      userData->joint->joint=NULL;
      delete userData;
      joint->SetUserData(NULL);
    }
    joint=next;
  }
  if (box2d) {
    box2d->removeWorld(this);
  }
  if (destructionListener) delete destructionListener;
  if (contactListener) delete contactListener;
  if (contactFilter) delete contactFilter;
}

#pragma mark -

void PPBox2D::addWorld(PPBox2DWorld* world)
{
  worlds.push_back(world);
}

void PPBox2D::removeWorld(PPBox2DWorld* world)
{
  std::vector<PPBox2DWorld*>::iterator it;
  for( it = worlds.begin(); it != worlds.end(); it++ ) {
    if (*it == world) {
      worlds.erase(it);
      break;
    }
  }
}

#pragma mark -

static void funcJointRevolute(lua_State* L,int n,b2RevoluteJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
	def->referenceAngle = getNumber(L,n,def->referenceAngle,"referenceAngle");
	def->enableLimit = getBool(L,n,def->enableLimit,"enableLimit");
	def->lowerAngle = getNumber(L,n,def->lowerAngle,"lowerAngle");
	def->upperAngle = getNumber(L,n,def->upperAngle,"upperAngle");
	def->enableMotor = getBool(L,n,def->enableMotor,"enableMotor");
	def->motorSpeed = getNumber(L,n,def->motorSpeed,"motorSpeed");
	def->maxMotorTorque = getNumber(L,n,def->maxMotorTorque,"maxMotorTorque");
}
static void funcJointPrismatic(lua_State* L,int n,b2PrismaticJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
  def->localAxisA = getPoint(L,n,def->localAxisA,"localAxisA");
	def->referenceAngle = getNumber(L,n,def->referenceAngle,"referenceAngle");
	def->enableLimit = getBool(L,n,def->enableLimit,"enableLimit");
	def->lowerTranslation = getNumber(L,n,def->lowerTranslation,"lowerLimit");
	def->upperTranslation = getNumber(L,n,def->upperTranslation,"upperLimit");
	def->lowerTranslation = getNumber(L,n,def->lowerTranslation,"lowerTranslation");
	def->upperTranslation = getNumber(L,n,def->upperTranslation,"upperTranslation");
	def->enableMotor = getBool(L,n,def->enableMotor,"enableMotor");
	def->maxMotorForce = getNumber(L,n,def->maxMotorForce,"maxMotorForce");
	def->motorSpeed = getNumber(L,n,def->motorSpeed,"motorSpeed");
}
static void funcJointDistance(lua_State* L,int n,b2DistanceJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
	def->length = getNumber(L,n,def->length,"length");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequency");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}
static void funcJointGear(lua_State* L,int n,b2GearJointDef* def)
{
	def->joint1 = getJoint(L,n,def->joint1,"joint1");
	def->joint2 = getJoint(L,n,def->joint1,"joint2");
	def->ratio = getNumber(L,n,def->ratio,"ratio");
}
static void funcJointPulley(lua_State* L,int n,b2PulleyJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
	def->lengthA = getNumber(L,n,def->lengthA,"lengthA");
	def->lengthB = getNumber(L,n,def->lengthB,"lengthB");
	def->ratio = getNumber(L,n,def->ratio,"ratio");
}
static void funcJointWheel(lua_State* L,int n,b2WheelJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
  def->localAxisA = getPoint(L,n,def->localAxisA,"localAxisA");
	def->enableMotor = getBool(L,n,def->enableMotor,"enableMotor");
	def->maxMotorTorque = getNumber(L,n,def->maxMotorTorque,"maxMotorTorque");
	def->motorSpeed = getNumber(L,n,def->motorSpeed,"motorSpeed");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequency");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}
static void funcJointWeld(lua_State* L,int n,b2WeldJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
	def->referenceAngle = getNumber(L,n,def->referenceAngle,"referenceAngle");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequency");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}
static void funcJointFriction(lua_State* L,int n,b2FrictionJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
	def->maxForce = getNumber(L,n,def->maxForce,"maxForce");
	def->maxTorque = getNumber(L,n,def->maxTorque,"maxTorque");
}
static void funcJointRope(lua_State* L,int n,b2RopeJointDef* def)
{
  def->localAnchorA = getPoint(L,n,def->localAnchorA,"localAnchorA");
  def->localAnchorB = getPoint(L,n,def->localAnchorB,"localAnchorB");
	def->maxLength = getNumber(L,n,def->maxLength,"maxLength");
}
static void funcJointMouse(lua_State* L,int n,b2MouseJointDef* def)
{
  def->target = getPoint(L,n,def->target,"target");
	def->maxForce = getNumber(L,n,def->maxForce,"maxForce");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequency");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}

#pragma mark -

static int funcDeleteWorld(lua_State *L)
{
	PPBox2DWorld* w = (PPBox2DWorld*)(PPLuaScript::DeleteObject(L));
  if (w) {
    PPBox2DWorld::deleteWorld(L,w->worldId);
printf("--- delete world %d\n",w->worldId);
    delete w;
//    if (!w->release()) {
//      PPBox2DWorld::deleteWorld(L,w->worldId);
//      if (w->contactTop) {
//        PPBox2DContact* c=w->contactTop;
//        while (c) {
//          PPBox2DContact* n=c->next;
//          c->contact = NULL;
//          c=n;
//        }
//        w->contactTop = NULL;
//      }
//      delete w;
//    }
  }
	return 0;
}

static int funcCreateWorld(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2D* m = (PPBox2D*)PPLuaArg::UserData(L,PPBox2D::className);
  PPUserDataAssert(m!=NULL);
  PPPoint g=PPPointZero;
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			g = PPPoint(s->tableNumber(L,0,1,"x",0),s->tableNumber(L,0,2,"y",0));
		} else {
			g = PPPoint(s->number(0),s->number(1));
		}
  }
	PPBox2DWorld* world = new PPBox2DWorld(b2Vec2(g.x,g.y));
  world->setListener();
  world->box2d = m;
  m->addWorld(world);
	PPLuaScript::newObject(L,B2WORLD,world,funcDeleteWorld);
  int worldIdx=lua_gettop(L);
  lua_pushstring(L,BODY_LIST);
  lua_createtable(L,0,0);
  lua_rawset(L,worldIdx);
  lua_pushstring(L,JOINT_LIST);
  lua_createtable(L,0,0);
  lua_rawset(L,worldIdx);
  
  PPBox2DWorld::setWorld(L,world->worldId,lua_gettop(L));

printf("--- create world %d\n",world->worldId);
  
	return 1;
}

#pragma mark -

static int funcb2WorldAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className,false);
  do {
  if (m==NULL) break;
  std::string name = lua_tostring(L,2);
//  //drawScale
//  if (name == "drawScale") {
//    if (setter) {
//      m->drawScale = lua_tonumber(L,3);
//    } else {
//      lua_pushnumber(L,m->drawScale);
//    }
//    ret = true;
//  } else
  //allowSleep
  if (name == "allowSleep") {
    if (setter) {
      m->SetAllowSleeping(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->GetAllowSleeping());
    }
    ret = true;
  } else
  //wormStarting
  if (name == "wormStarting") {
    if (setter) {
      m->SetWarmStarting(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->GetWarmStarting());
    }
    ret = true;
  } else
  //continuousPhysics
  if (name == "continuousPhysics") {
    if (setter) {
      m->SetContinuousPhysics(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->GetContinuousPhysics());
    }
    ret = true;
  } else
  //subStepping
  if (name == "subStepping") {
    if (setter) {
      m->SetSubStepping(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->GetSubStepping());
    }
    ret = true;
  } else
  //proxyCount   ROnly
  if (name == "proxyCount") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->GetProxyCount());
    }
    ret = true;
  } else
  //bodyCount    ROnly
  if (name == "bodyCount") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->GetBodyCount());
    }
    ret = true;
  } else
  //jointCount   ROnly
  if (name == "jointCount") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->GetJointCount());
    }
    ret = true;
  } else
  //contactCount ROnly
  if (name == "contactCount") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->GetContactCount());
    }
    ret = true;
  } else
  //treeHeight   ROnly
  if (name == "treeHeight") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->GetTreeHeight());
    }
    ret = true;
  } else
  //treeBalance  ROnly
  if (name == "treeBalance") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->GetTreeBalance());
    }
    ret = true;
  } else
  //treeQuality  ROnly
  if (name == "treeQuality") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushnumber(L,m->GetTreeQuality());
    }
    ret = true;
  } else
//  //gravity
//  if (name == "gravity") {
//    b2Vec2 g;
//    if (setter) {
//      m->SetGravity(getPoint(L,3));
//    } else {
//      g = m->GetGravity();
//      pushPoint(L,g);
//    }
//    ret = true;
//  } else
  //isLocked     ROnly
  if (name == "locked") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushboolean(L,m->IsLocked());
    }
    ret = true;
  } else
  //autoClearForces
  if (name == "autoClearForces") {
    if (setter) {
      m->SetAutoClearForces(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->GetAutoClearForces());
    }
    ret = true;
  } else
  //BODY_LIST    ROnly
  if (name == BODY_LIST) {
    if (setter) {
      readonly=true;
    }
  } else
  //JOINT_LIST   ROnLy
  if (name == JOINT_LIST) {
    if (setter) {
      readonly=true;
    }
  } else
  //CONTACT_LIST ROnLy
  if (name == CONTACT_LIST) {
    if (setter) {
      readonly=true;
    }
  }
  } while (false);
  if (setter) {
    if (readonly) {
      return luaL_error(L,"%s is read only.",lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2WorldSetter(lua_State* L)
{
  return funcb2WorldAccessor(L,true);
}

static int funcb2WorldGetter(lua_State* L)
{
  return funcb2WorldAccessor(L);
}

static int funcDestroyBody(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    delete m;
printf("--- destroy body %u\n",m->bodyIndex);
//printf("delete body\n");
  }
  return 0;
}

static int functionb2WorldDrawShape(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    m->drawShape(L);
  }
  return 0;
}

static int functionb2WorldDrawJoint(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    m->drawJoint(L);
  }
  return 0;
}

static int functionb2WorldDebugDrawShape(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    QBGame* game = (QBGame*)PPLuaArg::World(L);
    PPBox2DDebugDraw m_debugDraw;
    m_debugDraw.SetFlags(b2Draw::e_shapeBit);
    m_debugDraw.game = game;
    m_debugDraw.scale = m->drawScale;
    m->SetDebugDraw(&m_debugDraw);
    m->DrawDebugData();
  }
  return 0;
}

static int functionb2WorldDebugDrawJoint(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    QBGame* game = (QBGame*)PPLuaArg::World(L);
    PPBox2DDebugDraw m_debugDraw;
    m_debugDraw.SetFlags(b2Draw::e_jointBit);
    m_debugDraw.game = game;
    m_debugDraw.scale = m->drawScale;
    m->SetDebugDraw(&m_debugDraw);
    m->DrawDebugData();
  }
  return 0;
}

static int functionb2WorldDebugDrawAABB(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    QBGame* game = (QBGame*)PPLuaArg::World(L);
    PPBox2DDebugDraw m_debugDraw;
    m_debugDraw.SetFlags(b2Draw::e_aabbBit);
    m_debugDraw.game = game;
    m_debugDraw.scale = m->drawScale;
    m->SetDebugDraw(&m_debugDraw);
    m->DrawDebugData();
  }
  return 0;
}

static int functionb2WorldDebugDrawCOMs(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    QBGame* game = (QBGame*)PPLuaArg::World(L);
    PPBox2DDebugDraw m_debugDraw;
    m_debugDraw.SetFlags(b2Draw::e_centerOfMassBit);
    m_debugDraw.game = game;
    m_debugDraw.scale = m->drawScale;
    m->SetDebugDraw(&m_debugDraw);
    m->DrawDebugData();
  }
  return 0;
}

static int functionb2WorldDrawScale(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    if (lua_gettop(L) > 1) {
      PPPoint p(m->drawScale.x,m->drawScale.y);

      if (s->isTable(L,0)) {
        p = s->getPoint(L,0,p.x,p.y);
      } else
      if (s->argCount == 1) {
        p = PPPoint(s->number(0),s->number(0));
      } else {
        p = s->getPoint(L,0,p.x,p.y);
      }

      m->drawScale.x=p.x;
      m->drawScale.y=p.y;
      return 0;
    }
  }
  pushPoint(L,m->drawScale);
  return 1;
}

static void pushStr(lua_State* L,const char* str,const char* field)
{
  lua_pushstring(L,str);
  lua_setfield(L,-2,field);
}

static void pushNum(lua_State* L,float32 val,const char* field)
{
  lua_pushnumber(L,val);
  lua_setfield(L,-2,field);
}

static void pushInt(lua_State* L,int val,const char* field)
{
  lua_pushinteger(L,val);
  lua_setfield(L,-2,field);
}

static void pushBool(lua_State* L,bool val,const char* field)
{
  lua_pushboolean(L,val);
  lua_setfield(L,-2,field);
}

static void pushVec2(lua_State* L,b2Vec2 vec,const char* field)
{
  lua_createtable(L,0,2);
  lua_pushnumber(L,vec.x);
  lua_setfield(L,-2,"x");
  lua_pushnumber(L,vec.y);
  lua_setfield(L,-2,"y");
  lua_setfield(L,-2,field);
}

static void pushMassData(lua_State* L,b2MassData &data)
{
	lua_createtable(L,0,0);
	lua_pushnumber(L,data.mass);
	lua_setfield(L,-2,"mass");
  lua_createtable(L,0,2);
	lua_pushnumber(L,data.center.x);
	lua_setfield(L,-2,"x");
	lua_pushnumber(L,data.center.y);
	lua_setfield(L,-2,"y");
  lua_getglobal(L,"pppoint_mt");
  lua_setmetatable(L,-2);
  lua_setfield(L,-2,"center");
	lua_pushnumber(L,data.I);
	lua_setfield(L,-2,"inertia");
	lua_pushnumber(L,data.I);
	lua_setfield(L,-2,"I");
}

static void pushBodyInfo(lua_State* L,b2BodyDef* def)
{
  lua_createtable(L, 0, 1);

  switch (def->type) {
  case b2_staticBody:
    pushStr(L,"static","typename");
    break;
  case b2_dynamicBody:
    pushStr(L,"dynamic","typename");
    break;
  case b2_kinematicBody:
    pushStr(L,"kinematic","typename");
    break;
  }
  pushInt(L,def->type,"type");
  pushVec2(L,def->position,"position");
  pushNum(L,def->angle,"angle");
  pushVec2(L,def->linearVelocity,"linearVelocity");
  pushNum(L,def->angularVelocity,"angularVelocity");
  pushNum(L,def->linearDamping,"linearDamping");
  pushNum(L,def->angularDamping,"angularDamping");
  pushBool(L,def->allowSleep,"allowSleep");
  pushBool(L,def->awake,"awake");
  pushBool(L,def->fixedRotation,"fixedRotation");
  pushBool(L,def->bullet,"bullet");
  pushBool(L,def->active,"active");
  pushNum(L,def->gravityScale,"gravityScale");
}

static void getbodydef(lua_State* L,PPLuaArg* s,int n,b2BodyDef &def)
{
  lua_getfield(L,n+2,"type");
  if (lua_type(L,-1)==LUA_TSTRING) {
    std::string type = lua_tostring(L,-1);
    if (type == "static") {
      def.type = b2_staticBody;
    } else
    if (type == "dynamic") {
      def.type = b2_dynamicBody;
    } else
    if (type == "kinematic") {
      def.type = b2_kinematicBody;
    }
    lua_pop(L,1);
  } else {
    lua_pop(L,1);
    def.type = (b2BodyType)getBodyType(L,n+2,"type",def.type);
  }
  def.position.x=s->tableNumber(L,n,"x",def.position.x);
  def.position.y=s->tableNumber(L,n,"y",def.position.y);
  def.angle = s->tableNumber(L,n,"angle",def.angle);
  def.position=getPoint(L,n+2,def.position,"position");
  def.linearVelocity=getPoint(L,n+2,def.linearVelocity,"linearVelocity");
  def.angularVelocity = s->tableNumber(L,n,"angularVelocity",def.angularVelocity);
  def.linearDamping = s->tableNumber(L,n,"linearDamping",def.linearDamping);
  def.angularDamping = s->tableNumber(L,n,"angularDamping",def.angularDamping);
  def.allowSleep = s->tableBoolean(L,n,"allowSleep",def.allowSleep);
  def.awake = s->tableBoolean(L,n,"awake",def.awake);
  def.fixedRotation = s->tableBoolean(L,n,"fixedRotation",def.fixedRotation);
  def.bullet = s->tableBoolean(L,n,"bullet",def.bullet);
  def.active = s->tableBoolean(L,n,"active",def.active);
  def.gravityScale = s->tableNumber(L,n,"gravityScale",def.gravityScale);
}

static int funcb2WorldCreateBody(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);

  bool pushDefInfo=false;
  b2BodyDef def;
  int n=0;
  if (lua_type(L,2)==LUA_TSTRING) {
    std::string type = lua_tostring(L,2);
    if (type == "static") {
      def.type = b2_staticBody;
      n++;
    } else
    if (type == "dynamic") {
      def.type = b2_dynamicBody;
      n++;
    } else
    if (type == "kinematic") {
      def.type = b2_kinematicBody;
      n++;
    }
  }

  if (s->argCount > n) {
    PPPoint p=s->getPoint(L,n);
    def.position.x = p.x;
    def.position.y = p.y;
    if (lua_istable(L,n+2)) {
      getbodydef(L,s,n,def);
      pushDefInfo = getBool(L,n+2,false,"defInfo");
      n++;
    } else {
      n+=2;
    }
    if (s->argCount > n) {
      if (lua_istable(L,n+2)) {
        getbodydef(L,s,n,def);
        pushDefInfo = getBool(L,n+2,false,"defInfo");
      }
    }
  }
  
  {
    lua_getfield(L,1,BODY_LIST);
    if (lua_isnil(L,-1)) {
      lua_createtable(L,0,0);
    }
#ifdef __LUAJIT__
    size_t len = lua_objlen(L,-1);
#else
    size_t len = lua_rawlen(L,-1);
#endif

    b2Body* body = m->CreateBody(&def);
    PPBox2DBody* ppbody = new PPBox2DBody(body,m);
    m->bodyEdited=true;
    PPBox2DBodyUserData* userData = (PPBox2DBodyUserData*)body->GetUserData();
    if (userData) {
      userData->index=len+1;
    }
    PPLuaScript::newObject(L,B2BODY,ppbody,funcDestroyBody);
    
    if (pushDefInfo) {
      pushBodyInfo(L,&def);
      lua_setfield(L,-2,"def");
    }

    lua_rawseti(L,-2,(int)len+1);
    lua_pushstring(L,BODY_LIST);
    lua_pushvalue(L,-2);
    lua_rawset(L,1);

    lua_getfield(L,1,BODY_LIST);
    lua_rawgeti(L,-1,(int)len+1);

//    PPBox2DWorld::findBody(L,body);
printf("--- create body %u\n",ppbody->bodyIndex);
  }

	return 1;
}

static int funcb2WorldDestroyBody(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	if (m == NULL) return 0;

	if (lua_gettop(L) > 1) {
    lua_getmetatable(L,2);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		PPBox2DBody* body = (PPBox2DBody*)lua_touserdata(L,-1);
		if (body) {
      b2Body* b2body = body->body;
      if (b2body) {

        b2Fixture* fixture = b2body->GetFixtureList();
        while (fixture) {
          b2Fixture* next=fixture->GetNext();
          PPBox2DFixtureUserData* userData = (PPBox2DFixtureUserData*)fixture->GetUserData();
          if (userData) {
            if (userData->fixture) {
              userData->fixture->destroy(b2body);
            }
          }
          fixture=next;
        }
        
        lua_getfield(L,1,BODY_LIST);
        int jointList = lua_gettop(L);
        if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
          size_t len = lua_objlen(L,-1);
#else
          size_t len = lua_rawlen(L,-1);
#endif
          lua_createtable(L, (int)len, 0);
          int newBodyList=lua_gettop(L);
          size_t j=1;
          for (int i=0;i<len;i++) {
            lua_rawgeti(L,jointList,i+1);
            int item=lua_gettop(L);
            lua_getmetatable(L,-1);
            lua_getfield(L,-1,PPGAMEINSTNACE);
            PPBox2DBody* body2 = (PPBox2DBody*)lua_touserdata(L,-1);
            if (body2) {
              if (body2->body == b2body) {
              } else {
                lua_settop(L,item);
                lua_rawseti(L,newBodyList,(unsigned int)j);
                j++;
              }
            }
            lua_settop(L,newBodyList);
          }
          lua_settop(L,newBodyList);
          lua_pushstring(L,BODY_LIST);
          lua_pushvalue(L,-2);
          lua_rawset(L,1);
        }

        m->bodyEdited=true;
        body->destroy(m);
        return 0;
      }
    }
  }
//printf("insntance is null\n");

  return 0;
}

static int funcDestroyJoint(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    delete m;
  }
//printf("delete Joint\n");
  return 0;
}

static void pushJointInfo(lua_State* L,b2JointDef* def)
{
  lua_createtable(L, 0, 1);
  
  pushBool(L,def->collideConnected,"collideConnected");

  switch (def->type) {
	case e_revoluteJoint:
    {
      b2RevoluteJointDef* revolute=(b2RevoluteJointDef*)def;
      pushInt(L,e_revoluteJoint,"type");
      pushStr(L,"revolute","typename");
      pushVec2(L,revolute->localAnchorA,"localAnchorA");
      pushVec2(L,revolute->localAnchorB,"localAnchorB");
      pushNum(L,revolute->referenceAngle,"referenceAngle");
      pushBool(L,revolute->enableLimit,"enableLimit");
      pushNum(L,revolute->referenceAngle,"referenceAngle");
      pushNum(L,revolute->lowerAngle,"lowerAngle");
      pushNum(L,revolute->upperAngle,"upperAngle");
      pushBool(L,revolute->enableMotor,"enableMotor");
      pushNum(L,revolute->motorSpeed,"motorSpeed");
      pushNum(L,revolute->maxMotorTorque,"maxMotorTorque");
    }
    break;
	case e_prismaticJoint:
    {
      b2PrismaticJointDef* prismatic=(b2PrismaticJointDef*)def;
      pushInt(L,e_prismaticJoint,"type");
      pushStr(L,"prismatic","typename");
      pushVec2(L,prismatic->localAnchorA,"localAnchorA");
      pushVec2(L,prismatic->localAnchorB,"localAnchorB");
      pushVec2(L,prismatic->localAxisA,"localAxisA");
      pushNum(L,prismatic->referenceAngle,"referenceAngle");
      pushBool(L,prismatic->enableLimit,"enableLimit");
      pushNum(L,prismatic->lowerTranslation,"lowerLimit");
      pushNum(L,prismatic->lowerTranslation,"upperLimit");
      pushNum(L,prismatic->lowerTranslation,"lowerTranslation");
      pushNum(L,prismatic->upperTranslation,"upperTranslation");
      pushBool(L,prismatic->enableMotor,"enableMotor");
      pushNum(L,prismatic->maxMotorForce,"maxMotorForce");
      pushNum(L,prismatic->motorSpeed,"motorSpeed");
    }
    break;
	case e_distanceJoint:
    {
      b2DistanceJointDef* distance=(b2DistanceJointDef*)def;
      pushInt(L,e_distanceJoint,"type");
      pushStr(L,"distance","typename");
      pushVec2(L,distance->localAnchorA,"localAnchorA");
      pushVec2(L,distance->localAnchorB,"localAnchorB");
      pushNum(L,distance->length,"length");
      pushNum(L,distance->frequencyHz,"frequencyHz");
      pushNum(L,distance->dampingRatio,"dampingRatio");
    }
    break;
	case e_pulleyJoint:
    {
      b2PulleyJointDef* pulley=(b2PulleyJointDef*)def;
      pushInt(L,e_pulleyJoint,"type");
      pushStr(L,"pulley","typename");
      pushVec2(L,pulley->groundAnchorA,"groundAnchorA");
      pushVec2(L,pulley->groundAnchorB,"groundAnchorB");
      pushVec2(L,pulley->localAnchorA,"localAnchorA");
      pushVec2(L,pulley->localAnchorB,"localAnchorB");
      pushNum(L,pulley->lengthA,"lengthA");
      pushNum(L,pulley->lengthB,"lengthB");
      pushNum(L,pulley->ratio,"ratio");
    }
    break;
	case e_mouseJoint:
    {
      b2MouseJointDef* mouse=(b2MouseJointDef*)def;
      pushInt(L,e_mouseJoint,"type");
      pushStr(L,"mouse","typename");
      pushVec2(L,mouse->target,"target");
      pushNum(L,mouse->maxForce,"maxForce");
      pushNum(L,mouse->frequencyHz,"frequencyHz");
      pushNum(L,mouse->dampingRatio,"dampingRatio");
    }
    break;
	case e_gearJoint:
    {
      b2GearJointDef* gear=(b2GearJointDef*)def;
      pushInt(L,e_gearJoint,"type");
      pushStr(L,"gear","typename");
      pushNum(L,gear->ratio,"ratio");
    }
    break;
	case e_wheelJoint:
    {
      b2WheelJointDef* wheel=(b2WheelJointDef*)def;
      pushInt(L,e_wheelJoint,"type");
      pushStr(L,"wheel","typename");
      pushVec2(L,wheel->localAnchorA,"localAnchorA");
      pushVec2(L,wheel->localAnchorB,"localAnchorB");
      pushVec2(L,wheel->localAxisA,"localAxisA");
      pushBool(L,wheel->enableMotor,"enableMotor");
      pushNum(L,wheel->maxMotorTorque,"maxMotorTorque");
      pushNum(L,wheel->motorSpeed,"motorSpeed");
      pushNum(L,wheel->frequencyHz,"frequencyHz");
      pushNum(L,wheel->dampingRatio,"dampingRatio");
    }
    break;
  case e_weldJoint:
    {
      b2WeldJointDef* weld=(b2WeldJointDef*)def;
      pushInt(L,e_weldJoint,"type");
      pushStr(L,"weld","typename");
      pushVec2(L,weld->localAnchorA,"localAnchorA");
      pushVec2(L,weld->localAnchorB,"localAnchorB");
      pushNum(L,weld->referenceAngle,"referenceAngle");
      pushNum(L,weld->frequencyHz,"frequencyHz");
      pushNum(L,weld->dampingRatio,"dampingRatio");
    }
    break;
	case e_frictionJoint:
    {
      b2FrictionJointDef* friction=(b2FrictionJointDef*)def;
      pushInt(L,e_frictionJoint,"type");
      pushStr(L,"friction","typename");
      pushVec2(L,friction->localAnchorA,"localAnchorA");
      pushVec2(L,friction->localAnchorB,"localAnchorB");
      pushNum(L,friction->maxForce,"maxForce");
      pushNum(L,friction->maxTorque,"maxTorque");
    }
    break;
	case e_ropeJoint:
    {
      b2RopeJointDef* rope=(b2RopeJointDef*)def;
      pushInt(L,e_ropeJoint,"type");
      pushStr(L,"rope","typename");
      pushVec2(L,rope->localAnchorA,"localAnchorA");
      pushVec2(L,rope->localAnchorB,"localAnchorB");
      pushNum(L,rope->maxLength,"maxLength");
    }
    break;
	case e_motorJoint:
    {
      b2MotorJointDef* motor=(b2MotorJointDef*)def;
      pushInt(L,e_motorJoint,"type");
      pushStr(L,"motor","typename");
      pushVec2(L,motor->linearOffset,"linearOffset");
      pushNum(L,motor->angularOffset,"angularOffset");
      pushNum(L,motor->maxForce,"maxForce");
      pushNum(L,motor->maxTorque,"maxTorque");
      pushNum(L,motor->correctionFactor,"correctionFactor");
    }
    break;
  default:
    break;
  }
}

static int funcb2WorldCreateJoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	if (m && s->argCount > 0) {

		b2JointDef *def=NULL;
		b2DistanceJointDef distance;
		b2FrictionJointDef friction;
		b2GearJointDef gear;
		b2PrismaticJointDef prismatic;
		b2PulleyJointDef pulley;
		b2RevoluteJointDef revolute;
		b2RopeJointDef rope;
		b2WeldJointDef weld;
		b2WheelJointDef wheel;
		b2MouseJointDef mouse;
		b2Vec2 anchor(0,0),axis(0,0);
		b2Vec2 anchorA(0,0),anchorB(0,0);
		b2Vec2 groundAnchorA(0,0),groundAnchorB(0,0);
		b2Body* bodyA=NULL;
		b2Body* bodyB=NULL;
		float ratio;
    std::string typestring;
    b2JointType jointType=e_unknownJoint;
    const char* jointclass=B2JOINT;
    bool pushDefInfo=false;
    
    int top=lua_gettop(L);
		int n=1;
    if (lua_isstring(L,2)) {
      typestring = lua_tostring(L,2);
      if (typestring == "revolute") {
        jointType=e_revoluteJoint;
      } else
      if (typestring == "prismatic") {
        jointType=e_prismaticJoint;
      } else
      if (typestring == "distance") {
        jointType=e_distanceJoint;
      } else
      if (typestring == "pulley") {
        jointType=e_pulleyJoint;
      } else
      if (typestring == "mouse") {
        jointType=e_mouseJoint;
      } else
      if (typestring == "gear") {
        jointType=e_gearJoint;
      } else
      if (typestring == "wheel") {
        jointType=e_wheelJoint;
      } else
      if (typestring == "weld") {
        jointType=e_weldJoint;
      } else
      if (typestring == "friction") {
        jointType=e_frictionJoint;
      } else
      if (typestring == "rope") {
        jointType=e_ropeJoint;
      } else
      if (typestring == "motor") {
        jointType=e_motorJoint;
      }
      
      if (lua_istable(L,n+2)) {
        lua_getmetatable(L,n+2);
        lua_getfield(L,-1,PPGAMEINSTNACE);
        if (!lua_isnil(L,-1)) {
          PPBox2DBody* ppbody = (PPBox2DBody*)lua_touserdata(L,-1);
          bodyA = ppbody->body;
        }
        lua_pop(L,2);

        lua_getmetatable(L,n+3);
        lua_getfield(L,-1,PPGAMEINSTNACE);
        if (!lua_isnil(L,-1)) {
          PPBox2DBody* ppbody = (PPBox2DBody*)lua_touserdata(L,-1);
          bodyB = ppbody->body;
        }
        lua_pop(L,2);
      
        n+=2;
      }
      n+=2;

    } else
    if (lua_istable(L,2)) {
      lua_getfield(L,2,"bodyA");
      if (!lua_isnil(L,-1)) {
        lua_getmetatable(L,-1);
        lua_getfield(L,-1,PPGAMEINSTNACE);
        if (!lua_isnil(L,-1)) {
          PPBox2DBody* ppbody = (PPBox2DBody*)lua_touserdata(L,-1);
          bodyA = ppbody->body;
        }
      }
      lua_getfield(L,2,"bodyB");
      if (!lua_isnil(L,-1)) {
        lua_getmetatable(L,-1);
        lua_getfield(L,-1,PPGAMEINSTNACE);
        if (!lua_isnil(L,-1)) {
          PPBox2DBody* ppbody = (PPBox2DBody*)lua_touserdata(L,-1);
          bodyB = ppbody->body;
        }
      }
      lua_getfield(L,2,"type");
      if (!lua_isnil(L,-1)) {
        jointType = (b2JointType)lua_tointeger(L,-1);
      }
      lua_settop(L,top);
      n+=1;
    }

    switch (jointType) {
    case e_revoluteJoint:
       jointclass = B2REVOLUTEJOINT;
       break;
    case e_prismaticJoint:
       jointclass = B2PRISMATICJOINT;
       break;
    case e_distanceJoint:
       jointclass = B2DISTANCEJOINT;
       break;
    case e_pulleyJoint:
       jointclass = B2PULLEYJOINT;
       break;
    case e_mouseJoint:
       jointclass = B2MOUSEJOINT;
       break;
    case e_gearJoint:
       jointclass = B2GEARJOINT;
       break;
    case e_wheelJoint:
      jointclass = B2WHEELJOINT;
       break;
    case e_weldJoint:
       jointclass = B2WELDJOINT;
       break;
    case e_frictionJoint:
       jointclass = B2FRICTIONJOINT;
       break;
    case e_ropeJoint:
       jointclass = B2ROPEJOINT;
       break;
    case e_motorJoint:
       jointclass = B2MOTORJOINT;
       break;
    case e_unknownJoint:
      break;
    }
    
    pushDefInfo = getBool(L,n,false,"defInfo");
    
		if (bodyA != NULL && bodyB != NULL) {
			if (jointType == e_revoluteJoint) {
				if (lua_istable(L,n)) {
					revolute.bodyA = bodyA;
					revolute.bodyB = bodyB;
					def = &revolute;
          anchor=getPoint(L,n,anchor,"anchor");
					revolute.Initialize(bodyA,bodyB,anchor);
					funcJointRevolute(L,n,&revolute);
				}
			} else
			if (jointType == e_pulleyJoint) {
				if (lua_istable(L,n)) {
					pulley.bodyA = bodyA;
					pulley.bodyB = bodyB;
					def = &pulley;
          groundAnchorA=getPoint(L,n,groundAnchorA,"groundAnchorA");
          groundAnchorB=getPoint(L,n,groundAnchorB,"groundAnchorB");
          anchorA=getPoint(L,n,anchorA,"anchorA");
          anchorB=getPoint(L,n,anchorB,"anchorB");
					ratio = getNumber(L,n,0,"ratio");
					pulley.Initialize(bodyA,bodyB,groundAnchorA,groundAnchorB,anchorA,anchorB,ratio);
					funcJointPulley(L,n,&pulley);
				}
			} else
			if (jointType == e_prismaticJoint) {
				if (lua_istable(L,n)) {
					prismatic.bodyA = bodyA;
					prismatic.bodyB = bodyB;
					def = &prismatic;
          anchor=getPoint(L,n,anchor,"anchor");
          axis=getPoint(L,n,axis,"axis");
					prismatic.Initialize(bodyA,bodyB,anchor,axis);
					funcJointPrismatic(L,n,&prismatic);
				}
			} else
			if (jointType == e_distanceJoint) {
				if (lua_istable(L,n)) {
					distance.bodyA = bodyA;
					distance.bodyB = bodyB;
					def = &distance;
          anchorA=getPoint(L,n,anchorA,"anchorA");
          anchorB=getPoint(L,n,anchorB,"anchorB");
					distance.Initialize(bodyA,bodyB,anchorA,anchorB);
					funcJointDistance(L,n,&distance);
				}
			} else
			if (jointType == e_gearJoint) {
				if (lua_istable(L,n)) {
					gear.bodyA = bodyA;
					gear.bodyB = bodyB;
					funcJointGear(L,n,&gear);
					if (gear.joint1 != NULL && gear.joint2 != NULL) {
						def = &gear;
					}
				}
			} else
			if (jointType == e_wheelJoint) {
				if (lua_istable(L,n)) {
          wheel.bodyA = bodyA;
          wheel.bodyB = bodyB;
					def = &wheel;
          anchor=getPoint(L,n,anchor,"anchor");
          axis=getPoint(L,n,axis,"axis");
					wheel.Initialize(bodyA,bodyB,anchor,axis);
					funcJointWheel(L,n,&wheel);
				}
			} else
			if (jointType == e_weldJoint) {
				if (lua_istable(L,n)) {
          weld.bodyA = bodyA;
          weld.bodyB = bodyB;
					def = &weld;
          anchor=getPoint(L,n,anchor,"anchor");
					weld.Initialize(bodyA,bodyB,anchor);
					funcJointWeld(L,n,&weld);
				}
			} else
			if (jointType == e_frictionJoint) {
				if (lua_istable(L,n)) {
          friction.bodyA = bodyA;
          friction.bodyB = bodyB;
					def = &friction;
          anchor=getPoint(L,n,anchor,"anchor");
					friction.Initialize(bodyA,bodyB,anchor);
					funcJointFriction(L,n,&friction);
				}
			} else
			if (jointType == e_ropeJoint) {
				if (lua_istable(L,n)) {
          rope.bodyA = bodyA;
          rope.bodyB = bodyB;
					def = &rope;
					funcJointRope(L,n,&rope);
				}
			} else
			if (jointType == e_mouseJoint) {
				if (lua_istable(L,n)) {
          mouse.bodyA = bodyA;
          mouse.bodyB = bodyB;
					def = &mouse;
					funcJointMouse(L,n,&mouse);
				}
			}

			if (def && def->bodyA!=NULL && def->bodyB!=NULL) {
				if (lua_istable(L,n)) {
          def->collideConnected = getBool(L,n,def->collideConnected,"collideConnected");
        }

        lua_getfield(L,1,JOINT_LIST);
        if (lua_isnil(L,-1)) {
          lua_createtable(L,0,0);
        }
#ifdef __LUAJIT__
        size_t len = lua_objlen(L,-1);
#else
        size_t len = lua_rawlen(L,-1);
#endif

				b2Joint* joint = m->CreateJoint(def);
        PPBox2DJoint* ppjoint = new PPBox2DJoint(joint,m);
        m->jointEdited=true;
        PPBox2DJointUserData* userData = (PPBox2DJointUserData*)joint->GetUserData();
        if (userData) {
          userData->index=len+1;
        }
        ppjoint->worldId = m->worldId;
				PPLuaScript::newObject(L,jointclass,ppjoint,funcDestroyJoint);
        
        if (pushDefInfo) {
          pushJointInfo(L,def);
          pushVec2(L,anchor,"anchor");
          pushVec2(L,anchorA,"anchorA");
          pushVec2(L,anchorB,"anchorB");
          pushVec2(L,axis,"axis");
          lua_setfield(L,-2,"def");
        }

        lua_rawseti(L,-2,(int)len+1);
        lua_pushstring(L,JOINT_LIST);
        lua_pushvalue(L,-2);
        lua_rawset(L,1);

        lua_getfield(L,1,JOINT_LIST);
        lua_rawgeti(L,-1,(int)len+1);

//    PPBox2DWorld::findJoint(L,joint);
			} else {
        lua_pushnil(L);
        return 1;
      }
		}
		
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int funcb2WorldDestroyJoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	if (m == NULL) return 0;

	if (lua_gettop(L) > 1) {
    lua_getmetatable(L,2);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		PPBox2DJoint* joint = (PPBox2DJoint*)lua_touserdata(L,-1);
		if (joint) {
      b2Joint* b2joint=joint->joint;
      if (b2joint) {

        lua_getfield(L,1,JOINT_LIST);
        int jointList = lua_gettop(L);
        if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
          size_t len = lua_objlen(L,-1);
#else
          size_t len = lua_rawlen(L,-1);
#endif
          lua_createtable(L, (int)len, 0);
          int newJointList=lua_gettop(L);
          size_t j=1;
          for (int i=0;i<len;i++) {
            lua_rawgeti(L,jointList,i+1);
            int item=lua_gettop(L);
            lua_getmetatable(L,-1);
            lua_getfield(L,-1,PPGAMEINSTNACE);
            PPBox2DJoint* joint2 = (PPBox2DJoint*)lua_touserdata(L,-1);
            if (joint2) {
              if (joint2->joint == b2joint) {
              } else {
                lua_settop(L,item);
                lua_rawseti(L,newJointList,(unsigned int)j);
                j++;
              }
            }
            lua_settop(L,newJointList);
          }
          lua_settop(L,newJointList);
          lua_pushstring(L,JOINT_LIST);
          lua_pushvalue(L,-2);
          lua_rawset(L,1);
        }

        m->jointEdited=true;
        joint->destroy(m);
        return 0;
      }
    }
  }
//printf("insntance is null\n");

  return 0;
}

static int funcb2WorldStep(lua_State *L)
{
  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
//  if (m->contactTop) {
//    PPBox2DContact* c=m->contactTop;
//    while (c) {
//      PPBox2DContact* n=c->next;
//      c->contact = NULL;
//      c=n;
//    }
//    m->contactTop = NULL;
//    lua_pushnil(L);
//    lua_setfield(L,1,CONTACT_LIST);
//  }
  m->callbackL = L;
  m->callbackIdx = 5;

	float32 timeStep=1.0f/60.0f;
	int32 velocityIterations=8;
	int32 positionIterations=3;
  
  int top=lua_gettop(L);
  if (top > 1) {
    if (lua_istable(L,2)) {
      m->callbackIdx = 2;
    } else {
      timeStep=lua_tonumber(L,2);
    }
  }
  if (top > 2) {
    if (lua_istable(L,3)) {
      m->callbackIdx = 3;
    } else {
      velocityIterations=(int32)lua_tointeger(L,3);
    }
  }
  if (top > 3) {
    if (lua_istable(L,4)) {
      m->callbackIdx = 4;
    } else {
      positionIterations=(int32)lua_tointeger(L,4);
    }
  }

	m->Step(timeStep,velocityIterations,positionIterations);
  
  m->callbackL = NULL;
	return 0;
}

static int funcb2WorldBodyList(lua_State *L) {
  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  PPBox2DWorld::findBody(L,m->GetBodyList());
	return 1;
}

static int funcb2WorldJointList(lua_State *L) {
  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  PPBox2DWorld::findJoint(L,m->GetJointList());
	return 1;
}

static int funcb2WorldBodyTable(lua_State *L) {
  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  lua_getfield(L,1,BODY_LIST);
	return 1;
}

static int funcb2WorldJointTable(lua_State *L) {
  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  lua_getfield(L,1,JOINT_LIST);
	return 1;
}

//static int funcb2WorldContactList(lua_State *L) {
//  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
//  PPUserDataAssert(m!=NULL);
//  if (m->GetContactList()) {
//    lua_getfield(L,1,CONTACT_LIST);
//    if (lua_isnil(L,-1)) {
//      PPBox2DContact* ppcontact = new PPBox2DContact(m->GetContactList());
//      ppcontact->worldId = m->worldId;
//      PPLuaScript::newObject(L,B2CONTACT,ppcontact,funcDeleteContact);
//      lua_setfield(L,1,CONTACT_LIST);
//      lua_getfield(L,1,CONTACT_LIST);
//      m->contactTop = ppcontact;
//    }
//  } else {
//    lua_pushnil(L);
//  }
//	return 1;
//}

//static int funcb2WorldAllowSleeping(lua_State *L) {
//  PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L);
//	if (m == NULL) return 0;
//
//	if (lua_gettop(L) > 1) {
//		m->SetAllowSleeping(lua_toboolean(L,2));
//    return 0;
//	}
//
//	lua_pushboolean(L,m->GetAllowSleeping());
//	return 1;
//}

//static int funcb2WorldWarmStarting(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	if (s->argCount > 0) {
//		m->SetWarmStarting(s->boolean(0));
//    return 0;
//	}
//	lua_pushboolean(L,m->GetWarmStarting());
//	return 1;
//}

//static int funcb2WorldContinuousPhysics(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//
//	if (s->argCount > 0) {
//		m->SetContinuousPhysics(s->boolean(0));
//    return 0;
//	}
//	lua_pushboolean(L,m->GetContinuousPhysics());
//	return 1;
//}

//static int funcb2WorldSubStepping(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//
//	if (s->argCount > 0) {
//		m->SetSubStepping(s->boolean(0));
//    return 0;
//	}
//	lua_pushboolean(L,m->GetSubStepping());
//	return 1;
//}

//static int funcb2WorldProxyCount(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushinteger(L,m->GetProxyCount());
//	return 1;
//}

//static int funcb2WorldBodyCount(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushinteger(L,m->GetBodyCount());
//	return 1;
//}

//static int funcb2WorldJointCount(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushinteger(L,m->GetJointCount());
//	return 1;
//}

//static int funcb2WorldContactCount(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushinteger(L,m->GetContactCount());
//	return 1;
//}

//static int funcb2WorldTreeHeight(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushinteger(L,m->GetTreeHeight());
//	return 1;
//}

//static int funcb2WorldTreeBalance(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushinteger(L,m->GetTreeBalance());
//	return 1;
//}

//static int funcb2WorldTreeQuality(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushnumber(L,m->GetTreeQuality());
//	return 1;
//}

static int funcb2WorldGravity(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	b2Vec2 g;
	if (s->argCount > 0) {
    g = m->GetGravity();
		PPPoint p = s->getPoint(L,0,g.x,g.y);
		g.x = p.x;
		g.y = p.y;
		m->SetGravity(g);
    return 0;
	}
	g = m->GetGravity();
	return s->returnPoint(L,PPPoint(g.x,g.y));
}

//static int funcb2WorldIsLocked(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	lua_pushboolean(L,m->IsLocked());
//	return 1;
//}

//static int funcb2WorldAutoClearForces(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	if (s->argCount > 0) {
//		m->SetAutoClearForces(s->boolean(0));
//    return 0;
//	}
//	lua_pushboolean(L,m->GetAutoClearForces());
//	return 1;
//}

//static int funcb2WorldGetContactManager(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
//	if (m == NULL) return 0;
//	return 0;
//}

static int funcb2WorldGetProfile(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	b2Profile p = m->GetProfile();
	lua_createtable(L,0,0);
	int t = lua_gettop(L);
	lua_pushnumber(L,p.step);
	lua_setfield(L,t,"step");
	lua_pushnumber(L,p.collide);
	lua_setfield(L,t,"collide");
	lua_pushnumber(L,p.solve);
	lua_setfield(L,t,"solve");
	lua_pushnumber(L,p.solveInit);
	lua_setfield(L,t,"solveInit");
	lua_pushnumber(L,p.solveVelocity);
	lua_setfield(L,t,"solveVelocity");
	lua_pushnumber(L,p.solvePosition);
	lua_setfield(L,t,"solvePosition");
	lua_pushnumber(L,p.broadphase);
	lua_setfield(L,t,"broadphase");
	lua_pushnumber(L,p.solveTOI);
	lua_setfield(L,t,"solveTOI");
	return 1;
}

class AABBQueryCallback : public b2QueryCallback
{
public:
	AABBQueryCallback(lua_State* L,int worldIdx,int callbackIdx) :L(L),worldIdx(worldIdx),callbackIdx(callbackIdx) {
	}
 
	bool ReportFixture(b2Fixture* fixture) {
    bool r = false;
    int top=lua_gettop(L);
    do {
      if (lua_isfunction(L,callbackIdx)) {
        lua_pushvalue(L,callbackIdx);
        if (fixture == NULL) {
          lua_pushnil(L);
        } else {
          PPBox2DWorld::findFixture(L,fixture,worldIdx);
        }
        if(lua_pcall(L, 1, 1, 0) != 0) {
          break;
        }
        r=lua_toboolean(L,-1);
      }
    } while (false);
    lua_settop(L,top);
    return r;
	}

	lua_State* L;
  int callbackIdx;
  int worldIdx;
};

static int funcb2WorldQueryAABB(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	if (s->argCount > 0) {
		b2AABB aabb;

    int callbackidx=0;
    int n=0;
    float v[10]={0};
    
    int top=lua_gettop(L);
    for (int i=2;i<=top&&i<7;i++) {
      if (lua_type(L,i)==LUA_TFUNCTION) {
        callbackidx = i;
      } else
      if (lua_type(L,i)==LUA_TTABLE) {
        int top=lua_gettop(L);
        float px=0;
        float py=0;
        bool hash=false;
        lua_getfield(L,i,"x");
        if (!lua_isnil(L, -1)) {
          hash=true;
          px=v[n]=lua_tonumber(L,-1);
          n++;
        }
        lua_getfield(L,i,"y");
        if (!lua_isnil(L, -1)) {
          hash=true;
          py=v[n]=lua_tonumber(L,-1);
          n++;
        }
        lua_getfield(L,i,"width");
        if (!lua_isnil(L, -1)) {
          hash=true;
          v[n]=px+lua_tonumber(L,-1);
          n++;
        }
        lua_getfield(L,i,"height");
        if (!lua_isnil(L, -1)) {
          hash=true;
          v[n]=py+lua_tonumber(L,-1);
          n++;
        }
        if (!hash) {
#ifdef __LUAJIT__
          int l=(int)lua_objlen(L,i);
#else
          int l=(int)lua_rawlen(L,i);
#endif
          for (int j=0;j<l;j++) {
            lua_rawgeti(L,i,j+1);
            v[n]=lua_tonumber(L,-1);
            n++;
          }
        }
        lua_settop(L,top);
      } else {
        v[n]=lua_tonumber(L,i);
        n++;
      }
    }

    if (callbackidx > 1) {
      if (v[0]>v[2]) std::swap<float>(v[0],v[2]);
      if (v[1]>v[3]) std::swap<float>(v[1],v[3]);
      aabb.lowerBound.x = v[0];
      aabb.lowerBound.y = v[1];
      aabb.upperBound.x = v[2];
      aabb.upperBound.y = v[3];
      AABBQueryCallback callback(L,1,callbackidx);
      m->QueryAABB(&callback,aabb);
    }
	}
	return 1;
}

class RayCastCallback : public b2RayCastCallback
{
public:
  RayCastCallback(lua_State* L,int worldIdx,int callbackIdx): L(L),worldIdx(worldIdx),callbackIdx(callbackIdx) {
  }
  
  float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
    int top=lua_gettop(L);
    do {
      if (lua_isfunction(L,callbackIdx)) {
        lua_pushvalue(L,callbackIdx);
        PPBox2DWorld::findFixture(L,fixture,worldIdx);

        lua_createtable(L,0,2);
        lua_pushnumber(L,point.x);
        lua_setfield(L,-2,"x");
        lua_pushnumber(L,point.y);
        lua_setfield(L,-2,"y");
        lua_getglobal(L,"pppoint_mt");
        lua_setmetatable(L,-2);
        
        lua_createtable(L,0,2);
        lua_pushnumber(L,normal.x);
        lua_setfield(L,-2,"x");
        lua_pushnumber(L,normal.y);
        lua_setfield(L,-2,"y");
        lua_getglobal(L,"pppoint_mt");
        lua_setmetatable(L,-2);
        
        lua_pushnumber(L,fraction);

        if(lua_pcall(L, 4, 1, 0) != 0) {
          break;
        }
        fraction=lua_tonumber(L, -1);
      }
    } while (false);
    lua_settop(L,top);
    return fraction;
  }

	lua_State* L;
  int callbackIdx;
  int worldIdx;
};

static int funcb2WorldRayCast(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	if (s->argCount > 0) {
    b2Vec2 point1,point2;

    int callbackidx=0;
    int n=0;
    float v[10]={0};
    
    int top=lua_gettop(L);
    for (int i=2;i<=top&&i<7;i++) {
      if (lua_type(L,i)==LUA_TFUNCTION) {
        callbackidx = i;
      } else
      if (lua_type(L,i)==LUA_TTABLE) {
        int top=lua_gettop(L);
        float px=0;
        float py=0;
        bool hash=false;
        lua_getfield(L,i,"x");
        if (!lua_isnil(L, -1)) {
          hash=true;
          px=v[n]=lua_tonumber(L,-1);
          n++;
        }
        lua_getfield(L,i,"y");
        if (!lua_isnil(L, -1)) {
          hash=true;
          py=v[n]=lua_tonumber(L,-1);
          n++;
        }
        lua_getfield(L,i,"width");
        if (!lua_isnil(L, -1)) {
          hash=true;
          v[n]=px+lua_tonumber(L,-1);
          n++;
        }
        lua_getfield(L,i,"height");
        if (!lua_isnil(L, -1)) {
          hash=true;
          v[n]=py+lua_tonumber(L,-1);
          n++;
        }
        if (!hash) {
#ifdef __LUAJIT__
          int l=(int)lua_objlen(L,i);
#else
          int l=(int)lua_rawlen(L,i);
#endif
          for (int j=0;j<l;j++) {
            lua_rawgeti(L,i,j+1);
            v[n]=lua_tonumber(L,-1);
            n++;
          }
        }
        lua_settop(L,top);
      } else {
        v[n]=lua_tonumber(L,i);
        n++;
      }
    }

#if 1
    if (callbackidx > 1) {
      point1.x=v[0];
      point1.y=v[1];
      point2.x=v[2];
      point2.y=v[3];
      RayCastCallback callback(L,1,callbackidx);
      m->RayCast(&callback,point1,point2);
    }
#else
    lua_getfield(L,2,"x");
    if (!lua_isnil(L, -1)) {
      point1.x=lua_tonumber(L,-1);
    }
    lua_getfield(L,2,"y");
    if (!lua_isnil(L, -1)) {
      point1.y=lua_tonumber(L,-1);
    }
    lua_getfield(L,3,"x");
    if (!lua_isnil(L, -1)) {
      point2.x=lua_tonumber(L,-1);
    }
    lua_getfield(L,3,"y");
    if (!lua_isnil(L, -1)) {
      point2.y=lua_tonumber(L,-1);
    }
    if (s->argCount > 2) {
      if (lua_isfunction(L,4)) {
        RayCastCallback callback(L,1,4);
        m->RayCast(&callback,point1,point2);
      }
    }
#endif
  }
	return 1;
}

static int funcb2WorldClearForces(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  m->ClearForces();
  return 0;
}

static int funcb2WorldShiftOrigin(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
  if (lua_gettop(L)>1) {
    b2Vec2 g;
		PPPoint p = s->getPoint(L,0);
		g.x = p.x;
		g.y = p.y;
    m->ShiftOrigin(g);
  }
  return 0;
}

static int funcb2WorldDump(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaArg::UserData(L,PPBox2DWorld::className);
  PPUserDataAssert(m!=NULL);
	m->Dump();
	return 0;
}

#pragma mark -

static int funcb2BodyAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className,false);
  do {
	if (m == NULL) break;
  if (m->body == NULL) break;
  std::string name = lua_tostring(L,2);
  //type ROnly
  if (name == "type") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushinteger(L,m->body->GetType());
    }
    ret = true;
  } else
  //typename ROnly
  if (name == "typename") {
    if (setter) {
      readonly=true;
    } else {
      switch (m->body->GetType()) {
      case b2_staticBody:
        lua_pushstring(L,"static");
        break;
      case b2_kinematicBody:
        lua_pushstring(L,"kinematic");
        break;
      case b2_dynamicBody:
        lua_pushstring(L,"dynamic");
        break;
      }
    }
    ret = true;
  } else
  //internalindex
  if (name == "internalindex") {
    if (setter) {
      readonly=true;
    } else {
      PPBox2DBodyUserData* d = (PPBox2DBodyUserData*)m->body->GetUserData();
      lua_pushinteger(L,d->index);
    }
    ret = true;
  } else
  //x
  if (name == "x") {
    b2Vec2 p = m->body->GetPosition();
    if (setter) {
      p.x=lua_tonumber(L,3);
      float angle=m->body->GetAngle();
      m->body->SetTransform(p,angle);
    } else {
      lua_pushnumber(L,p.x);
    }
    ret = true;
  } else
  //y
  if (name == "y") {
    b2Vec2 p = m->body->GetPosition();
    if (setter) {
      p.y=lua_tonumber(L,3);
      float angle=m->body->GetAngle();
      m->body->SetTransform(p,angle);
    } else {
      lua_pushnumber(L,p.y);
    }
    ret = true;
  } else
  //angle
  if (name == "angle") {
    float angle=m->body->GetAngle();
    if (setter) {
      angle=lua_tonumber(L,3);
      b2Vec2 p = m->body->GetPosition();
      m->body->SetTransform(p,angle);
    } else {
      lua_pushnumber(L,angle);
    }
    ret = true;
  } else
  //next ROnly
  if (name == "next") {
    if (setter) {
      readonly=true;
    } else {
      b2Body* body = m->body->GetNext();
      if (body == NULL) {
        lua_pushnil(L);
      } else {
        PPBox2DWorld::findWorld(L,m->worldId);
        if (!PPBox2DWorld::findBody(L,body,lua_gettop(L))) {
          
        }
      }
    }
    ret = true;
  } else
//  //linearVelocity
//  if (name == "linearVelocity") {
//    if (setter) {
//      m->body->SetLinearVelocity(getPoint(L,3));
//    } else {
//      pushPoint(L,m->body->GetLinearVelocity());
//    }
//    ret = true;
//  } else
  //angularVelocity
  if (name == "angularVelocity") {
    if (setter) {
      m->body->SetAngularVelocity(lua_tonumber(L,3));
    } else {
      lua_pushnumber(L,m->body->GetAngularVelocity());
    }
    ret = true;
  } else
  //mass ROnly
  if (name == "mass") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushnumber(L,m->body->GetMass());
    }
    ret = true;
  } else
  //inertia ROnly
  if (name == "inertia") {
    if (setter) {
      readonly=true;
    } else {
      lua_pushnumber(L,m->body->GetInertia());
    }
    ret = true;
  } else
  //linearDamping
  if (name == "linearDamping") {
    if (setter) {
      m->body->SetLinearDamping(lua_tonumber(L,3));
    } else {
      lua_pushnumber(L,m->body->GetLinearDamping());
    }
    ret = true;
  } else
  //angularDamping
  if (name == "angularDamping") {
    if (setter) {
      m->body->SetAngularDamping(lua_tonumber(L,3));
    } else {
      lua_pushnumber(L,m->body->GetAngularDamping());
    }
    ret = true;
  } else
  //gravityScale
  if (name == "gravityScale") {
    if (setter) {
      m->body->SetGravityScale(lua_tonumber(L,3));
    } else {
      lua_pushnumber(L,m->body->GetGravityScale());
    }
    ret = true;
  } else
  //bullet
  if (name == "bullet") {
    if (setter) {
      m->body->SetBullet(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->body->IsBullet());
    }
    ret = true;
  } else
  //allowSleep
  if (name == "allowSleep") {
    if (setter) {
      m->body->SetSleepingAllowed(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->body->IsSleepingAllowed());
    }
    ret = true;
  } else
  //awake
  if (name == "awake") {
    if (setter) {
      m->body->SetAwake(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->body->IsAwake());
    }
    ret = true;
  } else
  //active
  if (name == "active") {
    if (setter) {
      m->body->SetActive(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->body->IsActive());
    }
    ret = true;
  } else
  //fixedRotation
  if (name == "fixedRotation") {
    if (setter) {
      m->body->SetFixedRotation(lua_toboolean(L,3));
    } else {
      lua_pushboolean(L,m->body->IsFixedRotation());
    }
    ret = true;
  } else
  //world ROnly
  if (name == "world") {
    if (setter) {
      readonly=true;
    } else {
      PPBox2DWorld::findWorld(L,m->worldId);
    }
    ret = true;
//  } else
//  if (name == FIX_LIST) {
//    if (setter) {
//      readonly=true;
//    }
//    ret = true;
  }
  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2BodySetter(lua_State* L)
{
  return funcb2BodyAccessor(L,true);
}

static int funcb2BodyGetter(lua_State* L)
{
  return funcb2BodyAccessor(L);
}

static int funcDestroyFixture(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
  if (m) {
    delete m;
  }
  return 0;
}

static int funcb2BodyCreateFixture(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
  if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		b2CircleShape circleShape;
		b2PolygonShape polygonShape;
		b2EdgeShape edgeShape;
		b2ChainShape chainShape;
		b2FixtureDef def;
    b2Vec2 pivot(0,0);
    int top=lua_gettop(L);
    lua_getfield(L,2,"type");
    const char* type = lua_tostring(L,-1);
    if (type == NULL) return 0;
    lua_getfield(L,2,"pivot");
    if (!lua_isnil(L,-1)) {
      lua_getfield(L,-1,"x");
      pivot.x = lua_tonumber(L,-1);
      lua_pop(L,1);
      lua_getfield(L,-1,"y");
      pivot.y = lua_tonumber(L,-1);
    }
    lua_settop(L,top);
		if (strcmp(type,"circle") == 0) {
      lua_getfield(L,2,"radius");
//      lua_setfield(L,1,"radius");
//      lua_getfield(L,2,"radius");
      circleShape.m_radius = lua_tonumber(L,-1);
      lua_getfield(L,2,"position");
      lua_getfield(L,-1,"x");
      if (!lua_isnil(L,-1)) {
        circleShape.m_p.x=lua_tonumber(L,-1);
      }
      lua_getfield(L,2,"position");
      lua_getfield(L,-1,"y");
      if (!lua_isnil(L,-1)) {
        circleShape.m_p.y=lua_tonumber(L,-1);
      }
//printf("r %f,x %f,y %f\n",circleShape.m_radius,circleShape.m_p.x,circleShape.m_p.y);
      def.shape = &circleShape;
      lua_settop(L,top);
		} else
		if (strcmp(type,"edge") == 0) {
			PPPoint p[2];
      lua_getfield(L,2,"vertices");
      int vert = lua_gettop(L);
			if (lua_istable(L,vert)) {
#ifdef __LUAJIT__
				int len = (int)lua_objlen(L,vert);
#else
				int len = (int)lua_rawlen(L,vert);
#endif
        if (len/2 > 0) {
          int top = lua_gettop(L);
          for (int i=0,m=0;i<len&m<2;i+=2,m++) {
            lua_rawgeti(L,vert,i+1);
            p[m].x = luaL_optnumber(L,-1,0);
            lua_rawgeti(L,vert,i+2);
            p[m].y = luaL_optnumber(L,-1,0);
            lua_settop(L,top);
          }
          edgeShape.Set(b2Vec2(p[0].x,p[0].y),b2Vec2(p[1].x,p[1].y));
          def.shape = &edgeShape;
        }
			}
      lua_getfield(L,2,"hasVertex0");
      if (!lua_isnil(L,-1)) {
        edgeShape.m_hasVertex0 = lua_toboolean(L,-1);
      }
      lua_getfield(L,2,"hasVertex3");
      if (!lua_isnil(L,-1)) {
        edgeShape.m_hasVertex3 = lua_toboolean(L,-1);
      }
      lua_getfield(L,2,"vertex0");
      if (!lua_isnil(L,-1)) {
        int top=lua_gettop(L);
        lua_getfield(L,-1,"x");
        if (!lua_isnil(L,-1)) {
          edgeShape.m_vertex0.x = lua_tonumber(L,-1);
        }
        lua_settop(L,top);
        lua_getfield(L,-1,"y");
        if (!lua_isnil(L,-1)) {
          edgeShape.m_vertex0.y = lua_tonumber(L,-1);
        }
        lua_settop(L,top);
      }
      lua_getfield(L,2,"vertex3");
      if (!lua_isnil(L,-1)) {
        int top=lua_gettop(L);
        lua_getfield(L,-1,"x");
        if (!lua_isnil(L,-1)) {
          edgeShape.m_vertex3.x = lua_tonumber(L,-1);
        }
        lua_settop(L,top);
        lua_getfield(L,-1,"y");
        if (!lua_isnil(L,-1)) {
          edgeShape.m_vertex3.y = lua_tonumber(L,-1);
        }
        lua_settop(L,top);
      }
      lua_settop(L,top);
//		} else
//		if (strcmp(type,"box") == 0) {
//			PPPoint p;
//      lua_getfield(L,2,"vertices");
//      int vert = lua_gettop(L);
//			if (lua_istable(L,vert)) {
//#ifdef __LUAJIT__
//				int len = (int)lua_objlen(L,vert);
//#else
//				int len = (int)lua_rawlen(L,vert);
//#endif
//				if (len >= 2) {
//					lua_rawgeti(L,vert,1);
//					p.x = luaL_optnumber(L,-1,0);
//					lua_rawgeti(L,vert,2);
//					p.y = luaL_optnumber(L,-1,0);
//					polygonShape.SetAsBox(p.x,p.y);
//					def.shape = &polygonShape;
//				}
//			}
//      lua_settop(L,top);
		} else
		if (strcmp(type,"polygon") == 0) {
      {
        bool makeBox=false;
        int argCount=0;
        float32 hx,hy;
        b2Vec2 center;
        float32 angle;
        lua_settop(L,top);
        lua_getfield(L,2,"hx");
        if (!lua_isnil(L,-1)) {
          hx = lua_tonumber(L,-1);
          lua_pop(L,1);
          lua_getfield(L,2,"hy");
          if (!lua_isnil(L,-1)) {
            hy = lua_tonumber(L,-1);
            makeBox=true;
          }
        }
        lua_pop(L,1);
        lua_getfield(L,2,"center");
        if (!lua_isnil(L,-1)) {
          lua_getfield(L,-1,"x");
          if (!lua_isnil(L,-1)) {
            center.x = lua_tonumber(L,-1);
            lua_pop(L,1);
            lua_getfield(L,-1,"y");
            if (!lua_isnil(L,-1)) {
              center.y = lua_tonumber(L,-1);
              argCount++;
            }
          }
          lua_pop(L,1);
        }
        lua_settop(L,top);
        lua_getfield(L,2,"angle");
        if (!lua_isnil(L,-1)) {
          angle=lua_tonumber(L,-1);
          argCount++;
        }
        
        if (makeBox) {
          if (argCount>=2) {
            polygonShape.SetAsBox(hx,hy,center,angle);
          } else {
            polygonShape.SetAsBox(hx,hy);
          }
          def.shape = &polygonShape;
        } else {
          lua_getfield(L,2,"vertices");
          int vert = lua_gettop(L);
          if (lua_istable(L,vert)) {
#ifdef __LUAJIT__
            int len = (int)lua_objlen(L,vert);
#else
            int len = (int)lua_rawlen(L,vert);
#endif
            if (len/2 > 2) {
              int top = lua_gettop(L);
              b2Vec2* p = (b2Vec2*)malloc((len/2)*sizeof(b2Vec2));
              if (p) {
                for (int i=0,m=0;i<len&m<(len/2);i+=2,m++) {
                  lua_rawgeti(L,vert,i+1);
                  p[m].x = luaL_optnumber(L,-1,0);
                  lua_rawgeti(L,vert,i+2);
                  p[m].y = luaL_optnumber(L,-1,0);
                  lua_settop(L,top);
                }
                if (len/2>=3) {
                  if (len/2>8) len=8*2;
                  polygonShape.Set(p,len/2);
                }
                def.shape = &polygonShape;
                free(p);
              }
            } else {
              return 0;
            }
          }
        }
      }
      lua_settop(L,top);
		} else
		if (strcmp(type,"chain") == 0) {
      lua_getfield(L,2,"vertices");
      int vert = lua_gettop(L);
			if (lua_istable(L,vert)) {
#ifdef __LUAJIT__
				int len = (int)lua_objlen(L,vert);
#else
				int len = (int)lua_rawlen(L,vert);
#endif
        if (len/2 > 0) {
          int top = lua_gettop(L);
          b2Vec2* p = (b2Vec2*)malloc((len/2)*sizeof(b2Vec2));
          if (p) {
            for (int i=0,m=0;i<len&m<(len/2);i+=2,m++) {
              lua_rawgeti(L,vert,i+1);
              p[m].x = luaL_optnumber(L,-1,0);
              lua_rawgeti(L,vert,i+2);
              p[m].y = luaL_optnumber(L,-1,0);
              lua_settop(L,top);
            }
            chainShape.CreateChain(p,len/2);
            def.shape = &chainShape;
            free(p);
          }
        }
			}
      lua_settop(L,top);
		}
		if (def.shape) {
			if (s->argCount > 0) {
        int n=0;
				if (lua_istable(L,n+2)) {
					def.friction = s->tableNumber(L,n,"friction",def.friction);
					def.restitution = s->tableNumber(L,n,"restitution",def.restitution);
					def.density = s->tableNumber(L,n,"density",def.density);
					def.isSensor = s->tableBoolean(L,n,"isSensor",def.isSensor);
					def.filter.categoryBits = s->tableInteger(L,n,"categoryBits",def.filter.categoryBits);
					def.filter.maskBits = s->tableInteger(L,n,"maskBits",def.filter.maskBits);
					def.filter.groupIndex = s->tableInteger(L,n,"groupIndex",def.filter.groupIndex);
				}
			}
			if (s->argCount > 1) {
        int n=1;
				if (lua_istable(L,n+2)) {
					def.friction = s->tableNumber(L,n,"friction",def.friction);
					def.restitution = s->tableNumber(L,n,"restitution",def.restitution);
					def.density = s->tableNumber(L,n,"density",def.density);
					def.isSensor = s->tableBoolean(L,n,"isSensor",def.isSensor);
					def.filter.categoryBits = s->tableInteger(L,n,"categoryBits",def.filter.categoryBits);
					def.filter.maskBits = s->tableInteger(L,n,"maskBits",def.filter.maskBits);
					def.filter.groupIndex = s->tableInteger(L,n,"groupIndex",def.filter.groupIndex);
				}
			}

			lua_getfield(L,1,FIX_LIST);
			if (lua_isnil(L,-1)) {
				lua_createtable(L,0,0);
			}
#ifdef __LUAJIT__
			size_t len = lua_objlen(L,-1);
#else
			size_t len = lua_rawlen(L,-1);
#endif

			b2Fixture* fixture = m->body->CreateFixture(&def);
      m->fixtureEdited=true;
      PPBox2DFixture* ppfix = new PPBox2DFixture(fixture);
      ppfix->pivot = pivot;
      ppfix->worldId = m->worldId;
			PPLuaScript::newObject(L,B2FIXTURE,ppfix,funcDestroyFixture);
      
      lua_pushvalue(L,2);
      lua_createtable(L, 0, 2);
      lua_pushnumber(L,m->world->drawScale.x);
      lua_setfield(L,-2,"x");
      lua_pushnumber(L,m->world->drawScale.y);
      lua_setfield(L,-2,"y");
      lua_getglobal(L,"pppoint_mt");
      lua_setmetatable(L,-2);
      lua_setfield(L,-2,"drawScale");
      lua_getfield(L,-1,"draw");
      lua_setfield(L,-3,"draw");
      lua_setfield(L,-2,FIX_SHAPE);

			lua_rawseti(L,-2,(int)len+1);
      lua_setfield(L,1,FIX_LIST);

			lua_getfield(L,1,FIX_LIST);
      lua_rawgeti(L,-1,(int)len+1);

//      PPBox2DWorld::findWorld(L,m->worldId);
//      if (!PPBox2DWorld::findFixture(L,fixture,lua_gettop(L))) {
//        
//      }

		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int funcb2BodyDestroyFixture(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
  if (m->body == NULL) return 0;

	if (lua_gettop(L) > 1) {
    lua_getmetatable(L,2);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		PPBox2DFixture* fixture = (PPBox2DFixture*)lua_touserdata(L,-1);
		if (fixture) {
      if (fixture->fixture) {
        if (m->body != fixture->fixture->GetBody()) return 0;

        lua_getfield(L,1,FIX_LIST);
        int fixtureList = lua_gettop(L);
        if (!lua_isnil(L,-1)) {
#ifdef __LUAJIT__
          size_t len = lua_objlen(L,-1);
#else
          size_t len = lua_rawlen(L,-1);
#endif
          lua_createtable(L, (int)len, 0);
          int newFixtureList=lua_gettop(L);
          int j=1;
          for (int i=0;i<len;i++) {
            int top=lua_gettop(L);
            lua_rawgeti(L,fixtureList,i+1);
            int item=lua_gettop(L);
            lua_getmetatable(L,-1);
            lua_getfield(L,-1,PPGAMEINSTNACE);
            PPBox2DFixture* fixture2 = (PPBox2DFixture*)lua_touserdata(L,-1);
            if (fixture2) {
              if (fixture2->fixture == fixture->fixture) {
              } else {
                lua_settop(L,item);
                lua_rawseti(L,newFixtureList,j);
                j++;
              }
            }
            lua_settop(L,top);
          }
          lua_pushnil(L);
          lua_rawseti(L,fixtureList,1);
          lua_settop(L,newFixtureList);
          lua_setfield(L,1,FIX_LIST);
          lua_getfield(L,1,FIX_LIST);
        }
  
        m->fixtureEdited=true;
        fixture->destroy(m->body);
        return 1;
      }
    }
  }

	return 0;
}

//static int funcb2BodyType(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DBody* m = (PPBox2DBody*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->body == NULL) return 0;
//	if (s->argCount > 0) {
//    lua_Integer type =lua_tointeger(L,2);
//    switch (type) {
//    case b2_staticBody:
//    case b2_dynamicBody:
//    case b2_kinematicBody:
//      m->body->SetType((b2BodyType)type);
//      break;
//    }
//    return 0;
//	}
//	lua_pushinteger(L,m->body->GetType());
//	return 1;
//}

static int funcb2BodyTransform(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v(0,0);
		float angle=0;
		if (s->argCount == 1) {
			v.x = getNumber(L,2,v.x,"x");
			v.y = getNumber(L,2,v.y,"y");
			angle = getNumber(L,2,angle,"angle");
		} else {
			PPPoint p = s->getPoint(L,0);
			v.x = p.x;
			v.y = p.y;
      if (s->argCount > 1) {
        if (s->isTable(L,0)) {
          angle = s->number(1);
        } else {
          angle = s->number(2);
        }
      }
		}
		m->body->SetTransform(v,angle);
    return 0;
	}
	b2Transform t = m->body->GetTransform();
	lua_createtable(L,0,0);
	int b = lua_gettop(L);
	lua_pushnumber(L,t.p.x);
	lua_setfield(L,b,"x");
	lua_pushnumber(L,t.p.y);
	lua_setfield(L,b,"y");
	lua_pushnumber(L,t.q.s);
	lua_setfield(L,b,"sin");
	lua_pushnumber(L,t.q.c);
	lua_setfield(L,b,"cos");
	lua_pushnumber(L,t.q.GetAngle());
	lua_setfield(L,b,"angle");
	return 1;
}

static int funcb2BodyPosition(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v(0,0);
		float angle=m->body->GetAngle();
    PPPoint p=s->getPoint(L,0);
    v.x = p.x;
    v.y = p.y;
		m->body->SetTransform(v,angle);
    return 0;
	}
	b2Vec2 position = m->body->GetPosition();
	return s->returnPoint(L,PPPoint(position.x,position.y));
}

static int funcb2BodyMove(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v=m->body->GetPosition();
		float angle=m->body->GetAngle();
    PPPoint p=s->getPoint(L,0);
    v.x+=p.x;
    v.y+=p.y;
		m->body->SetTransform(v,angle);
	}
  return 0;
}

static int funcb2BodyWorldCenter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 position = m->body->GetWorldCenter();
	return s->returnPoint(L,PPPoint(position.x,position.y));
}

static int funcb2BodyLocalCenter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 position = m->body->GetLocalCenter();
	return s->returnPoint(L,PPPoint(position.x,position.y));
}

static int funcb2BodyApplyForce(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
  int top=lua_gettop(L);
	if (top >= 3) {
		PPPoint p = s->getPoint(L,0);
		b2Vec2 v1(p.x,p.y);
    int n=2;
		if (lua_istable(L,2)) {
			p = s->getPoint(L,1);
      n++;
		} else {
			p = s->getPoint(L,2);
      n+=2;
		}
		if (lua_istable(L,n)) {
      n++;
    } else {
      n+=2;
    }
    bool wake=true;
    if (top >= n) {
      wake = lua_toboolean(L,n);
    }
		b2Vec2 v2(p.x,p.y);
		m->body->ApplyForce(v1,v2,wake);
	}
	return 0;
}

static int funcb2BodyApplyForceToCenter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;

  int top=lua_gettop(L);
	if (top >= 2) {
		PPPoint p = s->getPoint(L,0);
		b2Vec2 v(p.x,p.y);
    int n=2;
		if (lua_istable(L,2)) {
      n++;
    } else {
      n+=2;
    }
    bool wake=true;
    if (top >= n) {
      wake = lua_toboolean(L,n);
    }
		m->body->ApplyForceToCenter(v,wake);
	}
	return 0;
}

static int funcb2BodyApplyTorque(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
    bool wake=true;
    if (s->argCount > 1) {
      wake = s->boolean(2);
    }
		m->body->ApplyTorque(s->number(0),wake);
	}
	return 0;
}

static int funcb2BodyApplyLinearImpulse(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
  int top=lua_gettop(L);
	if (top >= 3) {
		PPPoint p = s->getPoint(L,0);
		b2Vec2 v1(p.x,p.y);
    int n=2;
		if (lua_istable(L,2)) {
			p = s->getPoint(L,1);
      n++;
		} else {
			p = s->getPoint(L,2);
      n+=2;
		}
		if (lua_istable(L,n)) {
      n++;
    } else {
      n+=2;
    }
    bool wake=true;
    if (top >= n) {
      wake = lua_toboolean(L,n);
    }
		b2Vec2 v2(p.x,p.y);
		m->body->ApplyLinearImpulse(v1,v2,wake);
	}
	return 0;
}

static int funcb2BodyApplyAngularImpusle(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
    bool wake=true;
    if (s->argCount > 1) {
      wake = s->boolean(2);
    }
		m->body->ApplyAngularImpulse(s->number(0),wake);
	}
	return 0;
}

static int funcb2BodyMassData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
  int top=lua_gettop(L);
	b2MassData data;
	if (s->argCount > 0) {
    data.mass=0;
    data.center.x=0;
    data.center.y=0;
    data.I=0;
		if (s->isTable(L,0)) {
			data.mass = s->tableNumber(L,0,"mass",data.mass);
      data.center = getPoint(L,2,data.center,"center");
			data.I = s->tableNumber(L,0,"I",data.I);
			data.I = s->tableNumber(L,0,"inertia",data.I);
		} else {
			data.mass = lua_tonumber(L,2);
      int n=3;
      if (top > 2) {
        if (lua_istable(L,3)) {
          data.center = getPoint(L,3,data.center);
        } else {
          data.center.x = lua_tonumber(L,3);
          if (top > 3) {
            data.center.y = lua_tonumber(L,4);
            n++;
          }
        }
      }
      if (top > n) {
        data.I = lua_tonumber(L,n+1);
      }
		}
    m->body->SetMassData(&data);
    return 0;
	}
	m->body->GetMassData(&data);
  pushMassData(L,data);
	return 1;
}

static int funcb2BodyResetMassData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	m->body->ResetMassData();
	return 0;
}

static int funcb2BodyWorldPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->body->GetWorldPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyWorldVector(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->body->GetWorldVector(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyLocalPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->body->GetLocalPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyLocalVector(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->body->GetLocalVector(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyLinearVelocity(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
    m->body->SetLinearVelocity(v);
    return 0;
	}
	v = m->body->GetLinearVelocity();
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyLinearVelocityFromWorldPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->body->GetLinearVelocityFromWorldPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyLinearVelocityFromLocalPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->body->GetLinearVelocityFromLocalPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2BodyBullet(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		m->body->SetBullet(s->boolean(0));
    return 0;
	}
	lua_pushboolean(L,m->body->IsBullet());
	return 1;
}

static int funcb2BodySleepingAllowed(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		m->body->SetSleepingAllowed(s->boolean(0));
    return 0;
	}
	lua_pushboolean(L,m->body->IsSleepingAllowed());
	return 1;
}

static int funcb2BodyAwake(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		m->body->SetAwake(s->boolean(0));
    return 0;
	}
	lua_pushboolean(L,m->body->IsAwake());
	return 1;
}

static int funcb2BodyActive(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		m->body->SetActive(s->boolean(0));
    return 0;
	}
	lua_pushboolean(L,m->body->IsActive());
	return 1;
}

static int funcb2BodyFixedRotation(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		m->body->SetFixedRotation(s->boolean(0));
    return 0;
	}
	lua_pushboolean(L,m->body->IsFixedRotation());
	return 1;
}

static int funcb2BodyFixtureList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
  lua_getfield(L,1,FIX_LIST);
	return 1;
}

#if 0
static int funcb2BodyJointList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)s->userdata;
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2JointEdge* joint = m->body->GetJointList();
  PPLuaScript::newObject(L,"b2JointEdge",joint,funcDeleteJointEdge);
	return 1;
}

static int funcb2BodyContactList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)s->userdata;
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	b2ContactEdge* contact = m->body->GetContactList();
  PPLuaScript::newObject(L,"b2ContactEdge",contact,funcDeleteContactEdge);
	return 1;
}
#endif

static int funcb2BodyTestPoint(lua_State *L) {
	bool hit=false;
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v;
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
		b2Fixture* fix = m->body->GetFixtureList();
		while (fix) {
			if (fix->TestPoint(v)) {
				hit = true;
				break;
			}
			fix = fix->GetNext();
		}
		lua_pushboolean(L,hit);
		return 1;
	}
	return 0;
}

static int funcb2BodyDump(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DBody* m = (PPBox2DBody*)PPLuaArg::UserData(L,PPBox2DBody::className);
  PPUserDataAssert(m!=NULL);
	if (m->body == NULL) return 0;
	m->body->Dump();
	return 0;
}

#pragma mark -

static int funcb2JointReactionForce(lua_State *L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  float v=0;
  if (lua_gettop(L)>1) {
    v=lua_tonumber(L,2);
  }
  pushPoint(L,m->joint->GetReactionForce(v));
	return 1;
}

static int funcb2JointReactionTorque(lua_State *L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  float v=0;
  if (lua_gettop(L)>1) {
    v=lua_tonumber(L,2);
  }
  lua_pushnumber(L,m->joint->GetReactionTorque(v));
	return 1;
}

static int funcb2JointActive(lua_State *L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
	lua_pushboolean(L,m->joint->IsActive());
	return 1;
}

static int funcb2JointDump(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
	m->joint->Dump();
	return 0;
}

static int funcb2PrismaticJointEnableLimit(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::prismaticJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2PrismaticJoint* joint = (b2PrismaticJoint*)m->joint;
	lua_pushboolean(L,joint->IsLimitEnabled());
	return 1;
}

static int funcb2RevoluteJointEnableLimit(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::revoluteJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2RevoluteJoint* joint = (b2RevoluteJoint*)m->joint;
	lua_pushboolean(L,joint->IsLimitEnabled());
	return 1;
}

static int funcb2PrismaticJointIsMotorEnabled(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::prismaticJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2PrismaticJoint* joint = (b2PrismaticJoint*)m->joint;
	lua_pushboolean(L,joint->IsMotorEnabled());
  return 1;
}

static int funcb2RevoluteJointIsMotorEnabled(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::revoluteJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2RevoluteJoint* joint = (b2RevoluteJoint*)m->joint;
	lua_pushboolean(L,joint->IsMotorEnabled());
	return 1;
}

static int funcb2WheelJointIsMotorEnabled(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::wheelJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2WheelJoint* joint = (b2WheelJoint*)m->joint;
	lua_pushboolean(L,joint->IsMotorEnabled());
  return 1;
}

static int funcb2PrismaticJointLimits(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::prismaticJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2PrismaticJoint* joint = (b2PrismaticJoint*)m->joint;
	if (lua_gettop(L)>2) {
		joint->SetLimits(lua_tonumber(L,2),lua_tonumber(L,3));
  } else {
    //error
  }
	return 0;
}

static int funcb2RevoluteJointLimits(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::revoluteJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2RevoluteJoint* joint = (b2RevoluteJoint*)m->joint;
	if (lua_gettop(L)>2) {
		joint->SetLimits(lua_tonumber(L,2),lua_tonumber(L,3));
  } else {
    //error
  }
	return 0;
}

static int funcb2RevoluteJointMotorTorque(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::revoluteJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2RevoluteJoint* joint = (b2RevoluteJoint*)m->joint;
  if (lua_gettop(L)>1) {
   lua_pushnumber(L,joint->GetMotorTorque(lua_tonumber(L,2)));
  } else {
    //error
  }
	return 1;
}

static int funcb2WheelJointMotorTorque(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::wheelJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2WheelJoint* joint = (b2WheelJoint*)m->joint;
  if (lua_gettop(L)>1) {
   lua_pushnumber(L,joint->GetMotorTorque(lua_tonumber(L,2)));
  } else {
    //error
  }
	return 1;
}

static int funcb2PrismaticJointMotorForce(lua_State* L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::prismaticJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2PrismaticJoint* joint = (b2PrismaticJoint*)m->joint;
	if (lua_gettop(L)>2) {
		lua_pushnumber(L,joint->GetMotorForce(lua_tonumber(L,2)));
  } else {
    //error
  }
	return 1;
}

static int funcb2JointShiftOrigin(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  if (lua_gettop(L) > 1) {
    b2Vec2 g;
		PPPoint p = s->getPoint(L,0);
		g.x = p.x;
		g.y = p.y;
    m->joint->ShiftOrigin(g);
  } else {
    //error
  }
	return 0;
}

static int funcb2MotorJointLinearOffset(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::motorJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2MotorJoint* joint = (b2MotorJoint*)m->joint;
  b2Vec2 v;
	if (s->argCount > 0) {
    PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
    joint->SetLinearOffset(v);
    return 0;
  }
	v = joint->GetLinearOffset();
	return s->returnPoint(L,PPPoint(v.x,v.y));
}

static int funcb2MouseJointTarget(lua_State *L) {
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::mouseJointClassName);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  b2MouseJoint* joint = (b2MouseJoint*)m->joint;
  if (lua_gettop(L) > 1) {
    joint->SetTarget(getPoint(L,2,joint->GetTarget()));
    return 0;
  } else {
    pushPoint(L,joint->GetTarget());
  }
	return 1;
}

static int funcb2MouseJointShiftOrigin(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  if (lua_gettop(L) > 1) {
    b2Vec2 g;
		PPPoint p = s->getPoint(L,0);
		g.x = p.x;
		g.y = p.y;
    m->joint->ShiftOrigin(g);
  } else {
    //error
  }
	return 0;
}

static int funcb2PulleyJointShiftOrigin(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className);
  PPUserDataAssert(m!=NULL);
  if (m->joint == NULL) return 0;
  if (lua_gettop(L) > 1) {
    b2Vec2 g;
		PPPoint p = s->getPoint(L,0);
		g.x = p.x;
		g.y = p.y;
    m->joint->ShiftOrigin(g);
  } else {
    //error
  }
	return 0;
}

static int funcb2JointAccessor(lua_State* L,bool setter=false)
{
  bool ret=false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::className,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);

    //type ROnly
    if (name == "type") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushinteger(L,m->joint->GetType());
      }
      ret = true;
    } else
    //typename ROnly
    if (name == "typename") {
      if (setter) {
        readonly=true;
      } else {
        switch (m->joint->GetType()) {
        case e_unknownJoint:
          lua_pushstring(L,"unknown");
          break;
        case e_revoluteJoint:
          lua_pushstring(L,"revolute");
          break;
        case e_prismaticJoint:
          lua_pushstring(L,"prismatic");
          break;
        case e_distanceJoint:
          lua_pushstring(L,"distance");
          break;
        case e_pulleyJoint:
          lua_pushstring(L,"pulley");
          break;
        case e_mouseJoint:
          lua_pushstring(L,"mouse");
          break;
        case e_gearJoint:
          lua_pushstring(L,"gear");
          break;
        case e_wheelJoint:
          lua_pushstring(L,"wheel");
          break;
        case e_weldJoint:
          lua_pushstring(L,"weld");
          break;
        case e_frictionJoint:
          lua_pushstring(L,"friction");
          break;
        case e_ropeJoint:
          lua_pushstring(L,"rope");
          break;
        case e_motorJoint:
          lua_pushstring(L,"motor");
          break;
        }
      }
      ret = true;
    } else
    //internalindex
    if (name == "internalindex") {
      if (setter) {
        readonly=true;
      } else {
        PPBox2DJointUserData* d = (PPBox2DJointUserData*)m->joint->GetUserData();
        lua_pushinteger(L,d->index);
      }
      ret = true;
    } else
    //bodyA ROnly
    if (name == "bodyA") {
      if (setter) {
        readonly=true;
      } else {
        b2Body* body = m->joint->GetBodyA();
        PPBox2DWorld::findWorld(L,m->worldId);
        if (!PPBox2DWorld::findBody(L,body,lua_gettop(L))) {
          
        }
      }
      ret = true;
    } else
    //bodyB ROnly
    if (name == "bodyB") {
      if (setter) {
        readonly=true;
      } else {
        b2Body* body = m->joint->GetBodyB();
        PPBox2DWorld::findWorld(L,m->worldId);
        if (!PPBox2DWorld::findBody(L,body,lua_gettop(L))) {
          
        }
      }
      ret = true;
    } else
    //anchorA ROnly
    if (name == "anchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,m->joint->GetAnchorA());
      }
      ret = true;
    } else
    //anchorB ROnly
    if (name == "anchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,m->joint->GetAnchorB());
      }
      ret = true;
    } else
    //next ROnly
    if (name == "next") {
      if (setter) {
        readonly=true;
      } else {
        b2Joint* joint = m->joint->GetNext();
        if (joint == NULL) {
          lua_pushnil(L);
        } else {
          PPBox2DWorld::findWorld(L,m->worldId);
          if (!PPBox2DWorld::findJoint(L,joint,lua_gettop(L))) {
            
          }
        }
      }
      ret = true;
    } else
    //active ROnly
    if (name == "active") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushboolean(L,m->joint->IsActive());
      }
      ret = true;
    } else
    //collideConnected ROnly
    if (name == "collideConnected") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushboolean(L,m->joint->GetCollideConnected());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2JointSetter(lua_State* L)
{
  return funcb2JointAccessor(L,true);
}

static int funcb2JointGetter(lua_State* L)
{
  return funcb2JointAccessor(L);
}

static int funcb2MotorJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::motorJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2MotorJoint* joint = (b2MotorJoint*)m->joint;

//    //linearOffset
//    if (name == "linearOffset") {
//      if (setter) {
//        joint->SetLinearOffset(getPoint(L,3));
//      } else {
//        pushPoint(L,joint->GetLinearOffset());
//      }
//      ret = true;
//    } else
    //angularOffset
    if (name == "angularOffset") {
      if (setter) {
        joint->SetAngularOffset(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetAngularOffset());
      }
      ret = true;
    } else
    //maxForce
    if (name == "maxForce") {
      if (setter) {
        joint->SetMaxForce(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxForce());
      }
      ret = true;
    } else
    //maxTorque
    if (name == "maxTorque") {
      if (setter) {
        joint->SetMaxTorque(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxTorque());
      }
      ret = true;
    } else
    //correctionFactor
    if (name == "correctionFactor") {
      if (setter) {
        joint->SetCorrectionFactor(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetCorrectionFactor());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2MotorJointSetter(lua_State* L)
{
  return funcb2MotorJointAccessor(L,true);
}

static int funcb2MotorJointGetter(lua_State* L)
{
  return funcb2MotorJointAccessor(L);
}

static int funcb2DistanceJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::distanceJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2DistanceJoint* joint = (b2DistanceJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //length
    if (name == "length") {
      if (setter) {
        joint->SetLength(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetLength());
      }
      ret = true;
    } else
    //frequencyHz
    if (name == "frequencyHz") {
      if (setter) {
        joint->SetFrequency(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetFrequency());
      }
      ret = true;
    } else
    //frequency
    if (name == "frequency") {
      if (setter) {
        joint->SetFrequency(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetFrequency());
      }
      ret = true;
    } else
    //dampingRatio
    if (name == "dampingRatio") {
      if (setter) {
        joint->SetDampingRatio(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetDampingRatio());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2DistanceJointSetter(lua_State* L)
{
  return funcb2DistanceJointAccessor(L,true);
}

static int funcb2DistanceJointGetter(lua_State* L)
{
  return funcb2DistanceJointAccessor(L);
}

static int funcb2FrictionJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::frictionJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2FrictionJoint* joint = (b2FrictionJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //maxForce
    if (name == "maxForce") {
      if (setter) {
        joint->SetMaxForce(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxForce());
      }
      ret = true;
    } else
    //maxTorque
    if (name == "maxTorque") {
      if (setter) {
        joint->SetMaxTorque(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxTorque());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2FrictionJointSetter(lua_State* L)
{
  return funcb2FrictionJointAccessor(L,true);
}

static int funcb2FrictionJointGetter(lua_State* L)
{
  return funcb2FrictionJointAccessor(L);
}

static int funcb2GearJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::gearJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2GearJoint* joint = (b2GearJoint*)m->joint;

    //joint1 ROnly
    if (name == "joint1") {
      if (setter) {
        readonly=true;
      } else {
        b2Joint* joint = ((b2GearJoint*)m->joint)->GetJoint1();
        if (joint == NULL) {
          lua_pushnil(L);
        } else {
          PPBox2DWorld::findWorld(L,m->worldId);
          if (!PPBox2DWorld::findJoint(L,joint,lua_gettop(L))) {
            
          }
        }
      }
      ret = true;
    } else
    //joint2 ROnly
    if (name == "joint2") {
      if (setter) {
        readonly=true;
      } else {
        b2Joint* joint = ((b2GearJoint*)m->joint)->GetJoint2();
        if (joint == NULL) {
          lua_pushnil(L);
        } else {
          PPBox2DWorld::findWorld(L,m->worldId);
          if (!PPBox2DWorld::findJoint(L,joint,lua_gettop(L))) {
            
          }
        }
      }
      ret = true;
    } else
    //ratio
    if (name == "ratio") {
      if (setter) {
        joint->SetRatio(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetRatio());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2GearJointSetter(lua_State* L)
{
  return funcb2GearJointAccessor(L,true);
}

static int funcb2GearJointGetter(lua_State* L)
{
  return funcb2GearJointAccessor(L);
}

static int funcb2MouseJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::mouseJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2MouseJoint* joint = (b2MouseJoint*)m->joint;

//    //target
//    if (name == "target") {
//      if (setter) {
//        joint->SetTarget(getPoint(L,3));
//      } else {
//        pushPoint(L,joint->GetTarget());
//      }
//      ret = true;
//    } else
    //maxForce
    if (name == "maxForce") {
      if (setter) {
        joint->SetMaxForce(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxForce());
      }
      ret = true;
    } else
    //frequencyHz
    if (name == "frequencyHz") {
      if (setter) {
        joint->SetFrequency(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetFrequency());
      }
      ret = true;
    } else
    //frequency
    if (name == "frequency") {
      if (setter) {
        joint->SetFrequency(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetFrequency());
      }
      ret = true;
    } else
    //dampingRatio
    if (name == "dampingRatio") {
      if (setter) {
        joint->SetDampingRatio(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetDampingRatio());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2MouseJointSetter(lua_State* L)
{
  return funcb2MouseJointAccessor(L,true);
}

static int funcb2MouseJointGetter(lua_State* L)
{
  return funcb2MouseJointAccessor(L);
}

static int funcb2PrismaticJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::prismaticJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2PrismaticJoint* joint = (b2PrismaticJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //localAxisA ROnly
    if (name == "localAxisA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAxisA());
      }
      ret = true;
    } else
    //referenceAngle ROnly
    if (name == "referenceAngle") {
      if (setter) {
         readonly=true;
      } else {
        lua_pushnumber(L,joint->GetReferenceAngle());
      }
      ret = true;
    } else
    //jointTranslation ROnly
    if (name == "jointTranslation") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetJointTranslation());
      }
      ret = true;
    } else
    //jointSpeed ROnly
    if (name == "jointSpeed") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetJointSpeed());
      }
      ret = true;
    } else
    //enableLimit
    if (name == "enableLimit") {
      if (setter) {
        joint->EnableLimit(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,joint->IsLimitEnabled());
      }
      ret = true;
    } else
    //lowerLimit ROnly
    if (name == "lowerLimit") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetLowerLimit());
      }
      ret = true;
    } else
    //upperLimit ROnly
    if (name == "upperLimit") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetUpperLimit());
      }
      ret = true;
    } else
    //enableMotor
    if (name == "enableMotor") {
      if (setter) {
        joint->EnableMotor(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,joint->IsMotorEnabled());
      }
      ret = true;
    } else
    //motorSpeed
    if (name == "motorSpeed") {
      if (setter) {
        joint->SetMotorSpeed(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMotorSpeed());
      }
      ret = true;
    } else
    //maxMotorForce
    if (name == "maxMotorForce") {
      if (setter) {
        joint->SetMaxMotorForce(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxMotorForce());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2PrismaticJointSetter(lua_State* L)
{
  return funcb2PrismaticJointAccessor(L,true);
}

static int funcb2PrismaticJointGetter(lua_State* L)
{
  return funcb2PrismaticJointAccessor(L);
}

static int funcb2PulleyJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::pulleyJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2PulleyJoint* joint = (b2PulleyJoint*)m->joint;

    //groundAnchorA ROnly
    if (name == "groundAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetGroundAnchorA());
      }
      ret = true;
    } else
    //groundAnchorB ROnly
    if (name == "groundAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetGroundAnchorB());
      }
      ret = true;
    } else
    //lengthA ROnly
    if (name == "lengthA") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetLengthA());
      }
      ret = true;
    } else
    //lengthB ROnly
    if (name == "lengthB") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetLengthB());
      }
      ret = true;
    } else
    //ratio ROnly
    if (name == "ratio") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetRatio());
      }
      ret = true;
    } else
    //currentLengthA ROnly
    if (name == "currentLengthA") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetCurrentLengthA());
      }
      ret = true;
    } else
    //currentLengthB ROnly
    if (name == "currentLengthB") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetCurrentLengthB());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2PulleyJointSetter(lua_State* L)
{
  return funcb2PulleyJointAccessor(L,true);
}

static int funcb2PulleyJointGetter(lua_State* L)
{
  return funcb2PulleyJointAccessor(L);
}

static int funcb2RevoluteJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::revoluteJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2RevoluteJoint* joint = (b2RevoluteJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //referenceAngle ROnly
    if (name == "referenceAngle") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetReferenceAngle());
      }
      ret = true;
    } else
    //jointAngle ROnly
    if (name == "jointAngle") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetJointAngle());
      }
      ret = true;
    } else
    //jointSpeed ROnly
    if (name == "jointSpeed") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetJointSpeed());
      }
      ret = true;
    } else
    //enableLimit
    if (name == "enableLimit") {
      if (setter) {
        joint->EnableLimit(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,joint->IsLimitEnabled());
      }
      ret = true;
    } else
    //lowerLimit
    if (name == "lowerLimit") {
      if (setter) {
      } else {
        lua_pushnumber(L,joint->GetLowerLimit());
      }
      ret = true;
    } else
    //upperLimit
    if (name == "upperLimit") {
      if (setter) {
      } else {
        lua_pushnumber(L,joint->GetUpperLimit());
      }
      ret = true;
    } else
    //enableMotor
    if (name == "enableMotor") {
      if (setter) {
        joint->EnableMotor(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,joint->IsMotorEnabled());
      }
      ret = true;
    } else
    //motorSpeed
    if (name == "motorSpeed") {
      if (setter) {
        joint->SetMotorSpeed(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMotorSpeed());
      }
      ret = true;
    } else
    //maxMotorTorque
    if (name == "maxMotorTorque") {
      if (setter) {
        joint->SetMaxMotorTorque(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxMotorTorque());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2RevoluteJointSetter(lua_State* L)
{
  return funcb2RevoluteJointAccessor(L,true);
}

static int funcb2RevoluteJointGetter(lua_State* L)
{
  return funcb2RevoluteJointAccessor(L);
}

static int funcb2RopeJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::ropeJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2RopeJoint* joint = (b2RopeJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //maxLength
    if (name == "maxLength") {
      if (setter) {
        joint->SetMaxLength(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxLength());
      }
      ret = true;
    } else
    //limitState ROnly
    if (name == "limitState") {
      if (setter) {
        readonly=true;
     } else {
        lua_pushinteger(L,joint->GetLimitState());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2RopeJointSetter(lua_State* L)
{
  return funcb2RopeJointAccessor(L,true);
}

static int funcb2RopeJointGetter(lua_State* L)
{
  return funcb2RopeJointAccessor(L);
}

static int funcb2WeldJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::weldJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2WeldJoint* joint = (b2WeldJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
         readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //referenceAngle ROnly
    if (name == "referenceAngle") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetReferenceAngle());
      }
      ret = true;
    } else
    //frequencyHz
    if (name == "frequencyHz") {
      if (setter) {
        joint->SetFrequency(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetFrequency());
      }
      ret = true;
    } else
    //frequency
    if (name == "frequency") {
      if (setter) {
        joint->SetFrequency(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetFrequency());
      }
      ret = true;
    } else
    //dampingRatio
    if (name == "dampingRatio") {
      if (setter) {
        joint->SetDampingRatio(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetDampingRatio());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2WeldJointSetter(lua_State* L)
{
  return funcb2WeldJointAccessor(L,true);
}

static int funcb2WeldJointGetter(lua_State* L)
{
  return funcb2WeldJointAccessor(L);
}

static int funcb2WheelJointAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DJoint* m = (PPBox2DJoint*)PPLuaArg::UserData(L,PPBox2DJoint::wheelJointClassName,false);
  do {
    if (m == NULL) break;
    if (m->joint == NULL) break;
    std::string name = lua_tostring(L,2);
    b2WheelJoint* joint = (b2WheelJoint*)m->joint;

    //localAnchorA ROnly
    if (name == "localAnchorA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorA());
      }
      ret = true;
    } else
    //localAnchorB ROnly
    if (name == "localAnchorB") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAnchorB());
      }
      ret = true;
    } else
    //localAxisA ROnly
    if (name == "localAxisA") {
      if (setter) {
        readonly=true;
      } else {
        pushPoint(L,joint->GetLocalAxisA());
      }
      ret = true;
    } else
    //jointTranslation ROnly
    if (name == "jointTranslation") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushnumber(L,joint->GetJointTranslation());
      }
      ret = true;
    } else
    //jointSpeed ROnly
    if (name == "jointSpeed") {
      if (setter) {
         readonly=true;
      } else {
        lua_pushnumber(L,joint->GetJointSpeed());
      }
      ret = true;
    } else
    //enableMotor
    if (name == "enableMotor") {
      if (setter) {
        joint->EnableMotor(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,joint->IsMotorEnabled());
      }
      ret = true;
    } else
    //motorSpeed
    if (name == "motorSpeed") {
      if (setter) {
        joint->SetMotorSpeed(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMotorSpeed());
      }
      ret = true;
    } else
    //maxMotorTorque
    if (name == "maxMotorTorque") {
      if (setter) {
        joint->SetMaxMotorTorque(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetMaxMotorTorque());
      }
      ret = true;
    } else
    //springFrequencyHz
    if (name == "springFrequencyHz") {
      if (setter) {
        joint->SetSpringFrequencyHz(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetSpringFrequencyHz());
      }
      ret = true;
    } else
    //springDampingRatio
    if (name == "springDampingRatio") {
      if (setter) {
        joint->SetSpringDampingRatio(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,joint->GetSpringDampingRatio());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2WheelJointSetter(lua_State* L)
{
  return funcb2WheelJointAccessor(L,true);
}

static int funcb2WheelJointGetter(lua_State* L)
{
  return funcb2WheelJointAccessor(L);
}

#pragma mark -

static int funcb2FixtureAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className,false);
  do {
    if (m == NULL) break;
    if (m->fixture == NULL) break;
    std::string name = lua_tostring(L,2);
    b2Fixture* fixture = (b2Fixture*)m->fixture;

    //type ROnly
    if (name == "type") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushinteger(L,fixture->GetType());
      }
      ret = true;
    } else
    //typename ROnly
    if (name == "typename") {
      if (setter) {
        readonly=true;
      } else {
        switch (fixture->GetType()) {
        case b2Shape::e_circle:
          lua_pushstring(L,"circle");
          break;
        case b2Shape::e_edge:
          lua_pushstring(L,"edge");
          break;
        case b2Shape::e_polygon:
          lua_pushstring(L,"polygon");
          break;
        case b2Shape::e_chain:
          lua_pushstring(L,"chain");
          break;
        default:
          break;
        }
      }
      ret = true;
    } else
    //shape ROnly
    if (name == "shape") {
      if (setter) {
//        lua_pushvalue(L,2);
//        lua_pushstring(L,"_shape");
//        lua_rawset(L,1);
        readonly=true;
      } else {
        lua_getfield(L,1,FIX_SHAPE);
      }
      ret = true;
    } else
    //sensor
    if (name == "sensor") {
      if (setter) {
        fixture->SetSensor(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,fixture->IsSensor());
      }
      ret = true;
    } else
    //body ROnly
    if (name == "body") {
      if (setter) {
        readonly=true;
      } else {
        b2Body* body = fixture->GetBody();
        if (body==NULL) {
          lua_pushnil(L);
        } else {
          PPBox2DWorld::findWorld(L,m->worldId);
          if (!PPBox2DWorld::findBody(L,body,lua_gettop(L))) {
            
          }
        }
      }
      ret = true;
    } else
    //next ROnly
    if (name == "next") {
      if (setter) {
        readonly=true;
      } else {
        b2Fixture* fix = fixture->GetNext();
        if (fix==NULL) {
          lua_pushnil(L);
        } else {
          PPBox2DWorld::findWorld(L,m->worldId);
          if (!PPBox2DWorld::findFixture(L,fix,lua_gettop(L))) {
            
          }
        }
      }
      ret = true;
    } else
//    //massData ROnly
//    if (name == "massData") {
//      if (setter) {
//         readonly=true;
//      } else {
//        b2MassData data;
//        m->fixture->GetMassData(&data);
//        lua_createtable(L,0,0);
//        int b = lua_gettop(L);
//        lua_pushnumber(L,data.mass);
//        lua_setfield(L,b,"mass");
//        lua_pushnumber(L,data.center.x);
//        lua_setfield(L,b,"x");
//        lua_pushnumber(L,data.center.y);
//        lua_setfield(L,b,"y");
//        lua_pushnumber(L,data.I);
//        lua_setfield(L,b,"I");
//      }
//      ret = true;
//    } else
    //density
    if (name == "density") {
      if (setter) {
        fixture->SetDensity(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,fixture->GetDensity());
      }
      ret = true;
    } else
    //friction
    if (name == "friction") {
      if (setter) {
        fixture->SetFriction(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,fixture->GetFriction());
      }
      ret = true;
    } else
    //restitution
    if (name == "restitution") {
      if (setter) {
        fixture->SetRestitution(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,fixture->GetRestitution());
      }
      ret = true;
    } else
    //categoryBits
    if (name == "categoryBits") {
      b2Filter filter;
      filter = fixture->GetFilterData();
      if (setter) {
        filter.categoryBits=lua_tointeger(L,3);
        fixture->SetFilterData(filter);
      } else {
        lua_pushinteger(L,filter.categoryBits);
      }
      ret = true;
    } else
    //maskBits
    if (name == "maskBits") {
      b2Filter filter;
      filter = fixture->GetFilterData();
      if (setter) {
        filter.maskBits=lua_tointeger(L,3);
        fixture->SetFilterData(filter);
      } else {
        lua_pushinteger(L,filter.maskBits);
      }
      ret = true;
    } else
    //groupIndex
    if (name == "groupIndex") {
      b2Filter filter;
      filter = fixture->GetFilterData();
      if (setter) {
        filter.groupIndex=lua_tointeger(L,3);
        fixture->SetFilterData(filter);
      } else {
        lua_pushinteger(L,filter.groupIndex);
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2FixtureSetter(lua_State* L)
{
  return funcb2FixtureAccessor(L,true);
}

static int funcb2FixtureGetter(lua_State* L)
{
  return funcb2FixtureAccessor(L);
}

//static int funcb2FixtureType(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//	switch (m->fixture->GetType()) {
//	case b2Shape::e_circle:
//		lua_pushstring(L,"circle");
//		break;
//	case b2Shape::e_edge:
//		lua_pushstring(L,"edge");
//		break;
//	case b2Shape::e_polygon:
//		lua_pushstring(L,"polygon");
//		break;
//	case b2Shape::e_chain:
//		lua_pushstring(L,"chain");
//		break;
//	default:
//		lua_pushnil(L);
//		break;
//	}
//	return 1;
//}

//static int funcb2FixtureShape(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//	lua_getfield(L,1,"_shape");
//	return 1;
//}

static int funcb2FixtureSensor(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	if (s->argCount > 0) {
		m->fixture->SetSensor(s->boolean(0));
    return 0;
	}
	lua_pushboolean(L,m->fixture->IsSensor());
	return 1;
}

static int funcb2FixtureFilterData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	b2Filter filter;
	if (s->argCount > 0) {
    filter = m->fixture->GetFilterData();
		filter.categoryBits = s->tableInteger(L,0,"categoryBits",filter.categoryBits);
		filter.maskBits = s->tableInteger(L,0,"maskBits",filter.maskBits);
		filter.groupIndex = s->tableInteger(L,0,"groupIndex",filter.groupIndex);
		m->fixture->SetFilterData(filter);
    return 0;
	}
	filter = m->fixture->GetFilterData();
	lua_createtable(L,0,0);
	lua_pushinteger(L,filter.categoryBits);
	lua_setfield(L,-2,"categoryBits");
	lua_pushinteger(L,filter.maskBits);
	lua_setfield(L,-2,"maskBits");
	lua_pushinteger(L,filter.groupIndex);
	lua_setfield(L,-2,"groupIndex");
	return 1;
}

static int funcb2FixtureRefilter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	m->fixture->Refilter();
	return 0;
}

//static int funcb2FixtureBody(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//  b2Body* body = m->fixture->GetBody();
//
//  PPBox2DWorld::findWorld(L,m->worldId);
//  PPBox2DWorld::findBody(L,body,lua_gettop(L));
//	return 1;
//}

//static int funcb2FixtureNext(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//  b2Fixture* fix = m->fixture->GetNext();
//  PPBox2DWorld::findWorld(L,m->worldId);
//  PPBox2DWorld::findFixture(L,fix,lua_gettop(L));
//	return 1;
//}

static int funcb2FixtureTestPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v;
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
		lua_pushboolean(L,m->fixture->TestPoint(v));
		return 1;
	}
	return 0;
}

static int funcb2FixtureRayCast(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	
	if (s->argCount > 1) {
	
		b2RayCastOutput output;
		b2RayCastInput input;
		input.p1.x = getNumber(L,2,0,"p1","x");
		input.p1.y = getNumber(L,2,0,"p1","y");
		input.p2.x = getNumber(L,2,0,"p2","x");
		input.p2.y = getNumber(L,2,0,"p2","y");
		input.maxFraction = getNumber(L,2,0,"maxFraction");
		int childIndex = (int)s->integer(1);
		
		bool value = m->fixture->RayCast(&output,input,childIndex);

    lua_pushboolean(L, value);
		lua_createtable(L,0,2);
		lua_createtable(L,0,2);
		lua_pushnumber(L,output.normal.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,output.normal.y);
		lua_setfield(L,-2,"y");
		lua_getglobal(L,"pppoint_mt");
		lua_setmetatable(L,-2);
		lua_setfield(L,-2,"normal");
		lua_pushnumber(L,output.fraction);
		lua_setfield(L,-2,"fraction");
		
    return 2;
	}

	return 0;
}

static int funcb2FixtureMassData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	b2MassData data;
	m->fixture->GetMassData(&data);
  pushMassData(L,data);
	return 1;
}

//static int funcb2FixtureDensity(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//	if (s->argCount > 0) {
//		m->fixture->SetDensity(s->number(0));
//    return 0;
//	}
//	lua_pushnumber(L,m->fixture->GetDensity());
//	return 1;
//}

//static int funcb2FixtureFriction(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//	if (s->argCount > 0) {
//		m->fixture->SetFriction(s->number(0));
//    return 0;
//	}
//	lua_pushnumber(L,m->fixture->GetFriction());
//	return 1;
//}

//static int funcb2FixtureRestitution(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DFixture* m = (PPBox2DFixture*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->fixture == NULL) return 0;
//	if (s->argCount > 0) {
//		m->fixture->SetRestitution(s->number(0));
//    return 0;
//	}
//	lua_pushnumber(L,m->fixture->GetRestitution());
//	return 1;
//}

static int funcb2FixtureAABB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	if (s->argCount > 0) {
		b2AABB aabb = m->fixture->GetAABB((int)s->integer(0));
		lua_createtable(L,0,0);
		lua_createtable(L,0,2);
		lua_pushnumber(L,aabb.lowerBound.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,aabb.lowerBound.y);
		lua_setfield(L,-2,"y");
		lua_getglobal(L,"pppoint_mt");
		lua_setmetatable(L,-2);
		lua_setfield(L,-2,"lowerBound");
		lua_createtable(L,0,2);
		lua_pushnumber(L,aabb.upperBound.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,aabb.upperBound.y);
		lua_setfield(L,-2,"y");
		lua_getglobal(L,"pppoint_mt");
		lua_setmetatable(L,-2);
		lua_setfield(L,-2,"upperBound");
		return 1;
	}
	return 0;
}

static int funcb2FixtureDump(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DFixture* m = (PPBox2DFixture*)PPLuaArg::UserData(L,PPBox2DFixture::className);
  PPUserDataAssert(m!=NULL);
	if (m->fixture == NULL) return 0;
	m->fixture->Dump((int)s->integer(0));
	return 0;
}

#pragma mark -

static int funcb2ContactAccessor(lua_State* L,bool setter=false)
{
  bool ret = false;
  bool readonly=false;
  PPBox2DContact* m = (PPBox2DContact*)PPLuaArg::UserData(L,PPBox2DContact::className,false);
  do {
    if (m == NULL) break;
    if (m->contact == NULL) break;
    std::string name = lua_tostring(L,2);
    b2Contact* contact = (b2Contact*)m->contact;

    //touching ROnly
    if (name == "touching") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushboolean(L,contact->IsTouching());
      }
      ret = true;
    } else
    //enable
    if (name == "enable") {
      if (setter) {
        contact->SetEnabled(lua_toboolean(L,3));
      } else {
        lua_pushboolean(L,contact->IsEnabled());
      }
      ret = true;
    } else
    //next ROnly
    if (name == "next") {
      if (setter) {
        readonly=true;
      } else {
        PPBox2DContact* contact = new PPBox2DContact(m->contact->GetNext());
        contact->prev = m;
        m->next = contact;
        contact->worldId = m->worldId;
        PPLuaScript::newObject(L,B2CONTACT,contact,funcDeleteContact);
        lua_getfield(L,1,"world");
        lua_setfield(L,-2,"world");
      }
      ret = true;
    } else
    //fixtureA ROnly
    if (name == "fixtureA") {
      if (setter) {
        readonly=true;
      } else {
        lua_getfield(L,1,"world");
        PPBox2DWorld::findFixture(L,m->contact->GetFixtureA(),lua_gettop(L));
      }
      ret = true;
    } else
    //childIndexA ROnly
    if (name == "childIndexA") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushinteger(L,m->contact->GetChildIndexA());
      }
      ret = true;
    } else
    //fixtureB ROnly
    if (name == "fixtureB") {
      if (setter) {
        readonly=true;
      } else {
        lua_getfield(L,1,"world");
        PPBox2DWorld::findFixture(L,m->contact->GetFixtureB(),lua_gettop(L));
      }
      ret = true;
    } else
    //childIndexB ROnly
    if (name == "childIndexB") {
      if (setter) {
        readonly=true;
      } else {
        lua_pushinteger(L,m->contact->GetChildIndexB());
      }
      ret = true;
    } else
    //friction
    if (name == "friction") {
      if (setter) {
        m->contact->SetFriction(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,m->contact->GetFriction());
      }
      ret = true;
    } else
    //restitution
    if (name == "restitution") {
      if (setter) {
        m->contact->SetRestitution(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,m->contact->GetRestitution());
      }
      ret = true;
    } else
    //tangentSpeed
    if (name == "tangentSpeed") {
      if (setter) {
        m->contact->SetTangentSpeed(lua_tonumber(L,3));
      } else {
        lua_pushnumber(L,m->contact->GetTangentSpeed());
      }
      ret = true;
    }

  } while (false);
  if (setter) {
    if (readonly) {
      return PPLuaArg::setterReadOnlyError(L,lua_tostring(L,2));
    }
    lua_pushboolean(L,ret);
  } else {
    if (!ret) {
      lua_pushnil(L);
    }
  }
  return 1;
}

static int funcb2ContactSetter(lua_State* L)
{
  return funcb2ContactAccessor(L,true);
}

static int funcb2ContactGetter(lua_State* L)
{
  return funcb2ContactAccessor(L);
}

static int funcDeleteContact(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DContact* m = (PPBox2DContact*)PPLuaArg::UserData(L,PPBox2DContact::className);
  PPUserDataAssert(m!=NULL);
  
  if (m->prev) m->prev->next=m->next;
  if (m->next) m->next->prev=m->prev;
  
  m->prev=NULL;
  m->next=NULL;
  m->contact=NULL;
  
  delete m;

//printf("delete Contact\n");

  return 0;
}

static int funcb2ContactManifold(lua_State *L) {
  return 0;
}

static int funcb2ContactWorldManifold(lua_State *L) {
  return 0;
}

//static int funcb2ContactIsTouching(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  lua_pushboolean(L,m->contact->IsTouching());
//  return 1;
//}

static int funcb2ContactEnabled(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DContact* m = (PPBox2DContact*)PPLuaArg::UserData(L,PPBox2DContact::className);
  PPUserDataAssert(m!=NULL);
	if (m->contact == NULL) return 0;
  if (lua_gettop(L)>1) {
    m->contact->SetEnabled(lua_toboolean(L,2));
    return 0;
  }
  lua_pushboolean(L,m->contact->IsEnabled());
  return 1;
}

//static int funcb2ContactNext(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  
//  PPBox2DContact* contact = new PPBox2DContact(m->contact->GetNext());
//  contact->prev = m;
//  m->next = contact;
//  contact->worldId = m->worldId;
//  PPLuaScript::newObject(L,B2CONTACT,contact,funcDeleteContact);
//  lua_getfield(L,1,"world");
//  lua_setfield(L,-2,"world");
//  
//  return 1;
//}

//static int funcb2ContactFixtureA(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  
//  lua_getfield(L,1,"world");
//  PPBox2DWorld::findFixture(L,m->contact->GetFixtureA(),lua_gettop(L));
//  
//  return 1;
//}

//static int funcb2ContactFixtureB(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  
//  lua_getfield(L,1,"world");
//  PPBox2DWorld::findFixture(L,m->contact->GetFixtureB(),lua_gettop(L));
//  
//  return 1;
//}

//static int funcb2ContactChildIndexA(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  lua_pushinteger(L,m->contact->GetChildIndexA());
//  return 1;
//}

//static int funcb2ContactChildIndexB(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  lua_pushinteger(L,m->contact->GetChildIndexB());
//  return 1;
//}

//static int funcb2ContactFriction(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  if (lua_gettop(L)>1) {
//    m->contact->SetFriction(lua_tonumber(L,2));
//    return 0;
//  }
//  lua_pushnumber(L,m->contact->GetFriction());
//  return 1;
//}
//
static int funcb2ContactResetFriction(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DContact* m = (PPBox2DContact*)PPLuaArg::UserData(L,PPBox2DContact::className);
  PPUserDataAssert(m!=NULL);
	if (m->contact == NULL) return 0;
  m->contact->ResetFriction();
  return 0;
}

//static int funcb2ContactRestitution(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  if (lua_gettop(L)>1) {
//    m->contact->SetRestitution(lua_tonumber(L,2));
//    return 0;
//  }
//  lua_pushnumber(L,m->contact->GetRestitution());
//  return 1;
//}

static int funcb2ContactResetRestitution(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	PPBox2DContact* m = (PPBox2DContact*)PPLuaArg::UserData(L,PPBox2DContact::className);
  PPUserDataAssert(m!=NULL);
	if (m->contact == NULL) return 0;
  m->contact->ResetRestitution();
  return 0;
}

//static int funcb2ContactTangentSpeed(lua_State *L) {
//	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPBox2DContact* m = (PPBox2DContact*)s->userdata;
//	if (m == NULL) return 0;
//	if (m->contact == NULL) return 0;
//  lua_pushnumber(L,m->contact->GetTangentSpeed());
//  return 1;
//}

static int funcb2ContactEvaluate(lua_State *L) {
  return 0;
}

#pragma mark -

PPBox2D* PPBox2D::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
  PPBox2D* box2d = new PPBox2D();

  PPBox2D::className=name;
	s->openModule(PPBox2D::className,box2d,0);
		s->addCommand("createWorld",funcCreateWorld);

    s->addIntegerValue("staticBody",b2_staticBody);
    s->addIntegerValue("dynamicBody",b2_dynamicBody);
    s->addIntegerValue("kinematicBody",b2_kinematicBody);
    s->addIntegerValue("maxManifoldPoints",b2_maxManifoldPoints);
    s->addIntegerValue("maxPolygonVertices",b2_maxPolygonVertices);
    s->addNumberValue("aabbExtension",b2_aabbExtension);
    s->addNumberValue("aabbMultiplier",b2_aabbMultiplier);
    s->addNumberValue("linearSlop",b2_linearSlop);
    s->addNumberValue("angularSlop",b2_angularSlop);
    s->addNumberValue("polygonRadius",b2_polygonRadius);
    s->addIntegerValue("maxSubSteps",b2_maxSubSteps);
    s->addIntegerValue("maxTOIContacts",b2_maxTOIContacts);
    s->addNumberValue("velocityThreshold",b2_velocityThreshold);
    s->addNumberValue("maxLinearCorrection",b2_maxLinearCorrection);
    s->addNumberValue("maxAngularCorrection",b2_maxAngularCorrection);
    s->addNumberValue("maxTranslation",b2_maxTranslation);
    s->addNumberValue("maxTranslationSquared",b2_maxTranslationSquared);
    s->addNumberValue("maxRotation",b2_maxRotation);
    s->addNumberValue("maxRotationSquared",b2_maxRotationSquared);
    s->addNumberValue("baumgarte",b2_baumgarte);
    s->addNumberValue("toiBaugarte",b2_toiBaugarte);
    s->addNumberValue("timeToSleep",b2_timeToSleep);
    s->addNumberValue("linearSleepTolerance",b2_linearSleepTolerance);
    s->addNumberValue("angularSleepTolerance",b2_angularSleepTolerance);

    s->addIntegerValue("revoluteJoint",e_revoluteJoint);
    s->addIntegerValue("prismaticJoint",e_prismaticJoint);
    s->addIntegerValue("distanceJoint",e_distanceJoint);
    s->addIntegerValue("pulleyJoint",e_pulleyJoint);
    s->addIntegerValue("mouseJoint",e_mouseJoint);
    s->addIntegerValue("gearJoint",e_gearJoint);
    s->addIntegerValue("wheelJoint",e_wheelJoint);
    s->addIntegerValue("weldJoint",e_weldJoint);
    s->addIntegerValue("frictionJoint",e_frictionJoint);
    s->addIntegerValue("ropeJoint",e_ropeJoint);
    s->addIntegerValue("motorJoint",e_motorJoint);
    
    s->addIntegerValue("inactiveLimit",e_inactiveLimit);
    s->addIntegerValue("atLowerLimit",e_atLowerLimit);
    s->addIntegerValue("atUpperLimit",e_atUpperLimit);
    s->addIntegerValue("equalLimits",e_equalLimits);
    
    s->addIntegerValue("circleShape",b2Shape::e_circle);
    s->addIntegerValue("edgeShape",b2Shape::e_edge);
    s->addIntegerValue("polygonShape",b2Shape::e_polygon);
    s->addIntegerValue("chainShape",b2Shape::e_chain);

	s->closeModule();

  PPBox2DWorld::className=B2WORLD;
	s->openModule(PPBox2DWorld::className.c_str(),NULL,0);
    s->addAccessor(funcb2WorldSetter,funcb2WorldGetter);
    //allowSleep
    //wormStarting
    //continuousPhysics
    //subStepping
    //proxyCount   ROnly
    //bodyCount    ROnly
    //jointCount   ROnly
    //contactCount ROnly
    //treeHeight   ROnly
    //treeBalance  ROnly
    //treeQuality  ROnly
    //locked       ROnly
    //autoClearForces
    s->addCommand("drawShape",functionb2WorldDrawShape);
    s->addCommand("drawJoint",functionb2WorldDrawJoint);
    s->addCommand("debugDrawShape",functionb2WorldDebugDrawShape);
    s->addCommand("debugDrawJoint",functionb2WorldDebugDrawJoint);
    s->addCommand("debugDrawAABB",functionb2WorldDebugDrawAABB);
    s->addCommand("debugDrawCOMs",functionb2WorldDebugDrawCOMs);
    s->addCommand("drawScale",functionb2WorldDrawScale);
		s->addCommand("createBody",funcb2WorldCreateBody);
		s->addCommand("destroyBody",funcb2WorldDestroyBody);
		s->addCommand("createJoint",funcb2WorldCreateJoint);
		s->addCommand("destroyJoint",funcb2WorldDestroyJoint);
		s->addCommand("step",funcb2WorldStep);
		s->addCommand("bodyList",funcb2WorldBodyList);
		s->addCommand("jointList",funcb2WorldJointList);
		s->addCommand("bodyTable",funcb2WorldBodyTable);
		s->addCommand("jointTable",funcb2WorldJointTable);
//		s->addCommand("contactList",funcb2WorldContactList);              //要検討
//		s->addCommand("allowSleeping",funcb2WorldAllowSleeping);          //allowSleep
//		s->addCommand("warmStarting",funcb2WorldWarmStarting);            //wormStarting
//		s->addCommand("continuousPhysics",funcb2WorldContinuousPhysics);  //continuousPhysics
//		s->addCommand("subStepping",funcb2WorldSubStepping);              //subStepping
//		s->addCommand("proxyCount",funcb2WorldProxyCount);                //proxyCount   ROnly
//		s->addCommand("bodyCount",funcb2WorldBodyCount);                  //bodyCount    ROnly
//		s->addCommand("jointCount",funcb2WorldJointCount);                //jointCount   ROnly
//		s->addCommand("contactCount",funcb2WorldContactCount);            //contactCount ROnly
//		s->addCommand("treeHeight",funcb2WorldTreeHeight);                //treeHeight   ROnly
//		s->addCommand("treeBalance",funcb2WorldTreeBalance);              //treeBalance  ROnly
//		s->addCommand("treeQuality",funcb2WorldTreeQuality);              //treeQuality  ROnly
		s->addCommand("gravity",funcb2WorldGravity);
//		s->addCommand("isLocked",funcb2WorldIsLocked);                    //locked       ROnly
//		s->addCommand("autoClearForces",funcb2WorldAutoClearForces);      //autoClearForces
		//s->addCommand("contactManager",funcb2WorldGetContactManager);
		s->addCommand("getProfile",funcb2WorldGetProfile);
		s->addCommand("queryAABB",funcb2WorldQueryAABB);
		s->addCommand("rayCast",funcb2WorldRayCast);
		s->addCommand("clearForces",funcb2WorldClearForces);
		s->addCommand("shiftOrigin",funcb2WorldShiftOrigin);
		s->addCommand("dump",funcb2WorldDump);
	s->closeModule();

  PPBox2DBody::className=B2BODY;
	s->openModule(PPBox2DBody::className,NULL,0);
    s->addAccessor(funcb2BodySetter,funcb2BodyGetter);
    //type ROnly
    //typename ROnly
    //x
    //y
    //angle
    //next ROnly
    //angularVelocity
    //mass ROnly
    //inertia ROnly
    //linearDamping
    //angularDamping
    //gravityScale
    //bullet
    //allowSleep
    //awake
    //active
    //fixedRotation
    //world ROnly
		s->addCommand("createFixture",funcb2BodyCreateFixture);
		s->addCommand("destroyFixture",funcb2BodyDestroyFixture);
		s->addCommand("transform",funcb2BodyTransform);                   //要検討
		s->addCommand("position",funcb2BodyPosition);
		s->addCommand("pos",funcb2BodyPosition);
		s->addCommand("move",funcb2BodyMove);
		s->addCommand("worldCenter",funcb2BodyWorldCenter);
		s->addCommand("localCenter",funcb2BodyLocalCenter);
		s->addCommand("applyForce",funcb2BodyApplyForce);
		s->addCommand("applyForceToCenter",funcb2BodyApplyForceToCenter);
		s->addCommand("applyTorque",funcb2BodyApplyTorque);
		s->addCommand("applyLinearImpulse",funcb2BodyApplyLinearImpulse);
		s->addCommand("applyAngularImpulse",funcb2BodyApplyAngularImpusle);
		s->addCommand("massData",funcb2BodyMassData);
		s->addCommand("resetMassData",funcb2BodyResetMassData);
		s->addCommand("worldPoint",funcb2BodyWorldPoint);
		s->addCommand("worldVector",funcb2BodyWorldVector);
		s->addCommand("localPoint",funcb2BodyLocalPoint);
		s->addCommand("localVector",funcb2BodyLocalVector);
		s->addCommand("linearVelocity",funcb2BodyLinearVelocity);
		s->addCommand("linearVelocityFromWorldPoint",funcb2BodyLinearVelocityFromWorldPoint);
		s->addCommand("linearVelocityFromLocalPoint",funcb2BodyLinearVelocityFromLocalPoint);
		s->addCommand("isBullet",funcb2BodyBullet);
		s->addCommand("isSleepingAllowed",funcb2BodySleepingAllowed);
		s->addCommand("isAwake",funcb2BodyAwake);
		s->addCommand("isActive",funcb2BodyActive);
		s->addCommand("isFixedRotation",funcb2BodyFixedRotation);
		s->addCommand("fixtureList",funcb2BodyFixtureList);
		s->addCommand("testPoint",funcb2BodyTestPoint);
		s->addCommand("dump",funcb2BodyDump);
	s->closeModule();

  PPBox2DJoint::className=B2JOINT;
	s->openModule(PPBox2DJoint::className,NULL,0);
    s->addAccessor(funcb2JointSetter,funcb2JointGetter);
    //type ROnly
    //typename ROnly
    //bodyA ROnly
    //bodyB ROnly
    //anchorA ROnly
    //anchorB ROnly
    //next ROnly
    //active ROnly
    //collideConnected ROnly
		s->addCommand("reactionForce",funcb2JointReactionForce);
		s->addCommand("reactionTorque",funcb2JointReactionTorque);
		s->addCommand("isActive",funcb2JointActive);                      
		s->addCommand("dump",funcb2JointDump);
		s->addCommand("shiftOrigin",funcb2JointShiftOrigin);
	s->closeModule();

	//motor
  PPBox2DJoint::motorJointClassName=B2MOTORJOINT;
	s->openModule(PPBox2DJoint::motorJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2MotorJointSetter,funcb2MotorJointGetter);
    //angularOffset
    //maxForce
    //maxTorque
    //correctionFactor
    s->addCommand("linearOffset",funcb2MotorJointLinearOffset);
  s->closeModule();
		
	//distance
  PPBox2DJoint::distanceJointClassName=B2DISTANCEJOINT;
	s->openModule(PPBox2DJoint::distanceJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2DistanceJointSetter,funcb2DistanceJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //length
    //frequency
    //dampingRatio
	s->closeModule();
		
	//friction
  PPBox2DJoint::frictionJointClassName=B2FRICTIONJOINT;
	s->openModule(PPBox2DJoint::frictionJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2FrictionJointSetter,funcb2FrictionJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //maxForce
    //maxTorque
	s->closeModule();
		
  //gear
  PPBox2DJoint::gearJointClassName=B2GEARJOINT;
	s->openModule(PPBox2DJoint::gearJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2GearJointSetter,funcb2GearJointGetter);
    //joint1 ROnly
    //joint2 ROnly
    //ratio
	s->closeModule();

	//prismatic
  PPBox2DJoint::prismaticJointClassName=B2PRISMATICJOINT;
  s->openModule(PPBox2DJoint::prismaticJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2PrismaticJointSetter,funcb2PrismaticJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //localAxisA ROnly
    //referenceAngle ROnly
    //jointTranslation ROnly
    //jointSpeed ROnly
    //enableLimit
    //lowerLimit ROnly
    //upperLimit ROnly
    //enableMotor
    //motorSpeed
    //maxMotorForce
		s->addCommand("isLimitEnabled",funcb2PrismaticJointEnableLimit);
		s->addCommand("limits",funcb2PrismaticJointLimits);
		s->addCommand("isMotorEnabled",funcb2PrismaticJointIsMotorEnabled);
		s->addCommand("motorForce",funcb2PrismaticJointMotorForce);
	s->closeModule();
		
  //pulley
  PPBox2DJoint::pulleyJointClassName=B2PULLEYJOINT;
  s->openModule(PPBox2DJoint::pulleyJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2PulleyJointSetter,funcb2PulleyJointGetter);
    //groundAnchorA ROnly
    //groundAnchorB ROnly
    //lengthA ROnly
    //lengthB ROnly
    //ratio
    //currentLengthA ROnly
    //currentLengthB ROnly
		s->addCommand("shiftOrigin",funcb2PulleyJointShiftOrigin);
	s->closeModule();
	
	//revolute
  PPBox2DJoint::revoluteJointClassName=B2REVOLUTEJOINT;
  s->openModule(PPBox2DJoint::revoluteJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2RevoluteJointSetter,funcb2RevoluteJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //referenceAngle ROnly
    //jointAngle ROnly
    //jointSpeed ROnly
    //enableLimit
    //lowerLimit
    //upperLimit
    //enableMotor
    //motorSpeed
    //maxMotorTorque
		s->addCommand("isLimitEnabled",funcb2RevoluteJointEnableLimit);
		s->addCommand("limits",funcb2RevoluteJointLimits);
		s->addCommand("isMotorEnabled",funcb2RevoluteJointIsMotorEnabled);
		s->addCommand("motorTorque",funcb2RevoluteJointMotorTorque);
	s->closeModule();

  //rope
  PPBox2DJoint::ropeJointClassName=B2ROPEJOINT;
  s->openModule(PPBox2DJoint::ropeJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2RopeJointSetter,funcb2RopeJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //maxLength
    //limitState ROnly
	s->closeModule();
		
  //weld
  PPBox2DJoint::weldJointClassName=B2WELDJOINT;
  s->openModule(PPBox2DJoint::weldJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2WeldJointSetter,funcb2WeldJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //referenceAngle ROnly
    //frequency
    //dampingRatio
	s->closeModule();

	//wheel
  PPBox2DJoint::wheelJointClassName=B2WHEELJOINT;
  s->openModule(PPBox2DJoint::wheelJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2WheelJointSetter,funcb2WheelJointGetter);
    //localAnchorA ROnly
    //localAnchorB ROnly
    //localAxisA ROnly
    //jointTranslation
    //jointSpeed
    //enableMotor
    //motorSpeed
    //maxMotorTorque
    //springFrequencyHz
    //springDampingRatio
		s->addCommand("isMotorEnabled",funcb2WheelJointIsMotorEnabled);
		s->addCommand("motorTorque",funcb2WheelJointMotorTorque);
	s->closeModule();

	//mouse
  PPBox2DJoint::mouseJointClassName=B2MOUSEJOINT;
  s->openModule(PPBox2DJoint::mouseJointClassName,NULL,0,B2JOINT);
    s->addAccessor(funcb2MouseJointSetter,funcb2MouseJointGetter);
    //maxForce
    //frequency
    //dampingRatio
    s->addCommand("target",funcb2MouseJointTarget);
		s->addCommand("shiftOrigin",funcb2MouseJointShiftOrigin);
	s->closeModule();

  PPBox2DFixture::className=B2FIXTURE;
	s->openModule(PPBox2DFixture::className,NULL,0);
    s->addAccessor(funcb2FixtureSetter,funcb2FixtureGetter);
    //type ROnly
    //typename ROnly
    //shape ROnly
    //sensor
    //body ROnly
    //next ROnly
    //density
    //friction ROnly
    //restitution
    //categoryBits
    //maskBits
    //groupIndex
		s->addCommand("isSensor",funcb2FixtureSensor);
		s->addCommand("filterData",funcb2FixtureFilterData);
		s->addCommand("refilter",funcb2FixtureRefilter);
		s->addCommand("testPoint",funcb2FixtureTestPoint);
		s->addCommand("rayCast",funcb2FixtureRayCast);
		s->addCommand("massData",funcb2FixtureMassData);
		s->addCommand("AABB",funcb2FixtureAABB);                        //要検討
		s->addCommand("dump",funcb2FixtureDump);
	s->closeModule();

  PPBox2DContact::className=B2CONTACT;
  s->openModule(PPBox2DContact::className,NULL,0);
    s->addAccessor(funcb2ContactSetter,funcb2ContactGetter);
    //touching ROnly
    //enable
    //next ROnly
    //fixtureA ROnly
    //childIndexA ROnly
    //fixtureB ROnly
    //childIndexB ROnly
    //friction
    //restitution
    //tangentSpeed
		s->addCommand("manifold",funcb2ContactManifold);                //要検討
		s->addCommand("worldManifold",funcb2ContactWorldManifold);      //要検討
		s->addCommand("isEnabled",funcb2ContactEnabled);
		s->addCommand("resetFriction",funcb2ContactResetFriction);
		s->addCommand("resetRestitution",funcb2ContactResetRestitution);
		s->addCommand("evaluate",funcb2ContactEvaluate);                //要検討
  s->closeModule();

  return box2d;
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
