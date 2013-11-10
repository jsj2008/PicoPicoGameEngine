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
//#include "QBGame.h"

class ContactPoint
{
public:
	ContactPoint() : fixtureA(NULL),fixtureB(NULL),pointCount(0),points(NULL),state1(NULL),state2(NULL) {
	}
	virtual ~ContactPoint() {
		if (points) free(points);
		if (state1) free(state1);
		if (state2) free(state2);
	}
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	int pointCount;
	b2Vec2* points;
	b2Vec2 velocityA;
	b2Vec2 velocityB;
	b2PointState* state1;
	b2PointState* state2;
};

class PPBox2DListener : public b2ContactListener
{
public:
	static const int32 k_maxContactPoints = 2048;

	PPBox2DListener() {
		m_pointCount = 0;
	}
	virtual ~PPBox2DListener() {
	}

	virtual void BeginContact(b2Contact* contact) {
		B2_NOT_USED(contact);
	}
	virtual void EndContact(b2Contact* contact) {
		B2_NOT_USED(contact);
	}
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
		const b2Manifold* manifold = contact->GetManifold();
		if (manifold->pointCount == 0)
		{
			return;
		}
		
		if (m_pointCount >= k_maxContactPoints) return;
		
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();
		b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
		b2GetPointStates(state1,state2,oldManifold,manifold);
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		const b2Body* bodyA = contact->GetFixtureA()->GetBody();
		const b2Body* bodyB = contact->GetFixtureB()->GetBody();
		b2Vec2 point = worldManifold.points[0];
		b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
		b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);

		ContactPoint* cp = m_points + m_pointCount;
		cp->fixtureA = fixtureA;
		cp->fixtureB = fixtureB;
		cp->normal = worldManifold.normal;

		if (cp->points) free(cp->points);
		if (cp->state1) free(cp->state1);
		if (cp->state2) free(cp->state2);
		
		cp->pointCount = manifold->pointCount;
		cp->points = (b2Vec2*)malloc(manifold->pointCount*sizeof(b2Vec2));
		cp->state1 = (b2PointState*)malloc(manifold->pointCount*sizeof(b2PointState));
		cp->state2 = (b2PointState*)malloc(manifold->pointCount*sizeof(b2PointState));
		cp->velocityA = vA;
		cp->velocityB = vB;

		for (int32 i = 0; i < manifold->pointCount ; ++i)
		{
			cp->points[i] = worldManifold.points[i];
			cp->state1[i] = state1[i];
			cp->state2[i] = state2[i];
		}

		++m_pointCount;
	}
	virtual void PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse) {
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}
	void startListener() {
		for (int i=0;i<m_pointCount;i++) {
			if (m_points[i].points) free(m_points[i].points);
			if (m_points[i].state1) free(m_points[i].state1);
			if (m_points[i].state2) free(m_points[i].state2);
			m_points[i].points = NULL;
			m_points[i].state1 = NULL;
			m_points[i].state2 = NULL;
			m_points[i].pointCount = 0;
		}
		m_pointCount = 0;
	}
	int m_pointCount;
	ContactPoint m_points[k_maxContactPoints];
};

class PPBox2DWorld : public b2World
{
public:
	PPBox2DWorld(const b2Vec2& gravity) : b2World(gravity) {
		pplistener = NULL;
	}
	virtual ~PPBox2DWorld() {
		if (pplistener) delete pplistener;
	}
	PPBox2DListener* pplistener;
};

static int funcDeleteWorld(lua_State *L)
{
	PPBox2DWorld* w = (PPBox2DWorld*)(PPLuaScript::DeleteObject(L));
	delete w;
//printf("delete world\n");
	return 0;
}

static int funcNewWorld(lua_State *L)
{
//	PPBox2DWorld* m = (PPBox2DWorld*)PPLuaScript::UserData(L);
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::SharedScript(m->world(),L);
	PPPoint g = s->getPoint(L,0);
	PPBox2DWorld* obj = new PPBox2DWorld(b2Vec2(g.x,g.y));
	obj->pplistener = new PPBox2DListener();
	obj->SetContactListener(obj->pplistener);
	PPLuaScript::newObject(L,"PPBox2DWorld",obj,funcDeleteWorld);
	return 1;
}

static int funcScale(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	QBGame* g = (QBGame*)PPWorld::sharedWorld();
//	PPPoint sc = g->GetScale();
	return s->returnPoint(L,PPPoint(10,10));
//	return s->returnPoint(L,PPPoint(10*sc.x,10*sc.y));
}

static int funcb2WorldDump(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript_sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	m->Dump();
	return 0;
}

static int funcb2WorldStep(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	m->pplistener->m_pointCount=0;
	m->Step(s->number(0),(int)s->integer(1),(int)s->integer(2));
	return 0;
}

static int findObjectIndex(lua_State* L,int n,void* data)
{
	int top = lua_gettop(L);
#ifdef __LUAJIT__
	lua_objlen(L,n);
	int e=(int)lua_tointeger(L,-1);
	lua_settop(L,top);
#else
	int e = luaL_len(L,n);
#endif
	for (int i=0;i<e;i++) {
		lua_rawgeti(L,n,i+1);
		lua_getmetatable(L,-1);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		if (data == lua_touserdata(L,-1)) {
			lua_settop(L,top);
			return i+1;
		}
		lua_settop(L,top);
	}
	return 0;
}
static void removeTable(lua_State* L,int n,int pos)
{
#ifdef __LUAJIT__
	int top = lua_gettop(L);
	lua_objlen(L,n);
	int e=(int)lua_tointeger(L,-1);
	lua_settop(L,top);
#else
	int e = luaL_len(L,n);
#endif
//	lua_rawgeti(L,n,pos);  /* result = t[pos] */
	for ( ;pos<e; pos++) {
		lua_rawgeti(L,n,pos+1);
		lua_rawseti(L,n-1,pos);  /* t[pos] = t[pos+1] */
	}
	lua_pushnil(L);
	lua_rawseti(L,n-1,e);  /* t[e] = nil */
}
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
					def = lua_tointeger(L,-1);
				}
			} else {
				def = lua_toboolean(L,-1);
			}
		}
	}
	lua_settop(L,top);
	return def;
}
static PPBox2DWorld* getWorld(lua_State* L)
{
	int top = lua_gettop(L);
	PPBox2DWorld* world = NULL;
	lua_getfield(L,1,"_world");
	if (lua_istable(L,-1)) {
		lua_getmetatable(L,-1);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		if (!lua_isnil(L,-1)) {
			world = (PPBox2DWorld*)lua_touserdata(L,-1);
		}
	}
	lua_settop(L,top);
	return world;
}
static bool findObject(lua_State* L,const char* field,void* tbody,int n=-1)
{
	bool found = false;
	int top = lua_gettop(L);
	if (!lua_isnil(L,n) && lua_istable(L,n)) {
		lua_getfield(L,n,field);
		int result = lua_gettop(L);
#ifdef __LUAJIT__
		size_t len = lua_objlen(L,-1);
#else
		size_t len = lua_rawlen(L,-1);
#endif
		for (int i=0;i<len;i++) {
			int vtop = lua_gettop(L);
			lua_rawgeti(L,-1,i+1);
			if (lua_istable(L,-1)) {
				lua_getmetatable(L,-1);
				lua_getfield(L,-1,PPGAMEINSTNACE);
				if (!lua_isnil(L,-1)) {
					void* body = lua_touserdata(L,-1);
					if (body == tbody) {
						lua_settop(L,vtop+1);
						lua_replace(L,result);
						found = true;
						break;
					}
				}
			}
			lua_settop(L,vtop);
		}
	}
	if (!found) {
		lua_settop(L,top);
	}
	return found;
}
static bool findB2Object(lua_State* L,const char* field,void* tbody)
{
	bool found = true;
	int top = lua_gettop(L);
	getWorld(L);
	int result = lua_gettop(L);
	if (findObject(L,field,tbody)) {
		lua_replace(L,result);
		lua_settop(L,result);
	} else {
		lua_settop(L,top);
	}
	return found;
}
static b2Joint* getJoint(lua_State* L,int n,b2Joint* def,const char* field1)
{
	int top = lua_gettop(L);
	if (lua_istable(L,n)) {
		lua_getfield(L,n,field1);
		if (!lua_isnil(L,-1)) {
			lua_getmetatable(L,-1);
			lua_getfield(L,-1,PPGAMEINSTNACE);
			def = (b2Joint*)lua_touserdata(L,-1);
		}
	}
	lua_settop(L,top);
	return def;
}
static void pushContact(lua_State* L,b2Contact* c)
{
	lua_createtable(L,0,0);
	int table = lua_gettop(L);

	const b2Manifold* manifold = c->GetManifold();
	b2WorldManifold worldManifold;
	c->GetWorldManifold(&worldManifold);
	lua_createtable(L,manifold->pointCount+1,0);
	for (int32 i=0;i<manifold->pointCount;++i) {
		lua_createtable(L,0,2);
		lua_pushnumber(L,worldManifold.points[i].x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,worldManifold.points[i].y);
		lua_setfield(L,-2,"y");
		lua_rawseti(L,-2,i+1);
	}
	lua_setfield(L,table,"points");

	lua_createtable(L,0,2);
	lua_pushnumber(L,worldManifold.normal.x);
	lua_setfield(L,-2,"x");
	lua_pushnumber(L,worldManifold.normal.y);
	lua_setfield(L,-2,"y");
	lua_setfield(L,table,"normal");

	{
		int top = lua_gettop(L);
		b2Fixture* fix = c->GetFixtureA();
		b2Body* body = fix->GetBody();
		lua_pushvalue(L,1);
		if (findObject(L,"_body",body)) {
			if (findObject(L,"_fixture",fix)) {
				lua_setfield(L,table,"fixtureA");
			}
		}
		lua_settop(L,top);
	}

	{
		int top = lua_gettop(L);
		b2Fixture* fix = c->GetFixtureB();
		b2Body* body = fix->GetBody();
		lua_pushvalue(L,1);
		if (findObject(L,"_body",body)) {
			if (findObject(L,"_fixture",fix)) {
				lua_setfield(L,table,"fixtureB");
			}
		}
		lua_settop(L,top);
	}
	
	lua_pushinteger(L,c->GetChildIndexA());
	lua_setfield(L,table,"childIndexA");
	lua_pushinteger(L,c->GetChildIndexB());
	lua_setfield(L,table,"childIndexB");
}
static void funcJointRevolute(lua_State* L,int n,b2RevoluteJointDef* def)
{
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
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
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->localAxisA.x = getNumber(L,n,def->localAxisA.x,"localAxisA","x");
	def->localAxisA.y = getNumber(L,n,def->localAxisA.y,"localAxisA","y");
	def->referenceAngle = getNumber(L,n,def->referenceAngle,"referenceAngle");
	def->enableLimit = getBool(L,n,def->enableLimit,"enableLimit");
	def->lowerTranslation = getNumber(L,n,def->lowerTranslation,"lowerTranslation");
	def->upperTranslation = getNumber(L,n,def->upperTranslation,"upperTranslation");
	def->enableMotor = getBool(L,n,def->enableMotor,"enableMotor");
	def->maxMotorForce = getNumber(L,n,def->maxMotorForce,"maxMotorForce");
	def->motorSpeed = getNumber(L,n,def->motorSpeed,"motorSpeed");
}
static void funcJointDistance(lua_State* L,int n,b2DistanceJointDef* def)
{
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->length = getNumber(L,n,def->length,"length");
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
//	def->groundAnchorA.x = getNumber(L,n,def->groundAnchorA.x,"groundAnchorA","x");
//	def->groundAnchorA.y = getNumber(L,n,def->groundAnchorA.y,"groundAnchorA","y");
//	def->groundAnchorB.x = getNumber(L,n,def->groundAnchorB.x,"groundAnchorB","x");
//	def->groundAnchorB.y = getNumber(L,n,def->groundAnchorB.y,"groundAnchorB","y");
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->lengthA = getNumber(L,n,def->lengthA,"lengthA");
	def->lengthB = getNumber(L,n,def->lengthB,"lengthB");
	def->ratio = getNumber(L,n,def->ratio,"ratio");
}
static void funcJointWheel(lua_State* L,int n,b2WheelJointDef* def)
{
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->localAxisA.x = getNumber(L,n,def->localAxisA.x,"localAxisA","x");
	def->localAxisA.y = getNumber(L,n,def->localAxisA.y,"localAxisA","y");
	def->enableMotor = getBool(L,n,def->enableMotor,"enableMotor");
	def->maxMotorTorque = getNumber(L,n,def->maxMotorTorque,"maxMotorTorque");
	def->motorSpeed = getNumber(L,n,def->motorSpeed,"motorSpeed");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}
static void funcJointWeld(lua_State* L,int n,b2WeldJointDef* def)
{
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->referenceAngle = getNumber(L,n,def->referenceAngle,"referenceAngle");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}
static void funcJointFriction(lua_State* L,int n,b2FrictionJointDef* def)
{
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->maxForce = getNumber(L,n,def->maxForce,"maxForce");
	def->maxTorque = getNumber(L,n,def->maxTorque,"maxTorque");
}
static void funcJointRope(lua_State* L,int n,b2RopeJointDef* def)
{
	def->localAnchorA.x = getNumber(L,n,def->localAnchorA.x,"localAnchorA","x");
	def->localAnchorA.y = getNumber(L,n,def->localAnchorA.y,"localAnchorA","y");
	def->localAnchorB.x = getNumber(L,n,def->localAnchorB.x,"localAnchorB","x");
	def->localAnchorB.y = getNumber(L,n,def->localAnchorB.y,"localAnchorB","y");
	def->maxLength = getNumber(L,n,def->maxLength,"maxLength");
}
static void funcJointMouse(lua_State* L,int n,b2MouseJointDef* def)
{
	def->target.x = getNumber(L,n,def->target.x,"target","x");
	def->target.y = getNumber(L,n,def->target.y,"target","y");
	def->maxForce = getNumber(L,n,def->maxForce,"maxForce");
	def->frequencyHz = getNumber(L,n,def->frequencyHz,"frequencyHz");
	def->dampingRatio = getNumber(L,n,def->dampingRatio,"dampingRatio");
}
static int funcb2WorldCreateBody(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m && s->argCount > 0) {
		b2BodyDef def;
		int n=0;
		if (strcmp(s->args(n),"static") == 0) {
			def.type = b2_staticBody;
		} else
		if (strcmp(s->args(n),"dynamic") == 0) {
			def.type = b2_dynamicBody;
		} else
		if (strcmp(s->args(n),"kinematic") == 0) {
			def.type = b2_kinematicBody;
		}
		n++;
		if (s->argCount > n) {
			if (s->isTable(L,n)) {
				def.position.x = s->tableNumber(L,n,"x",0);
				def.position.y = s->tableNumber(L,n,"y",0);
				n++;
			} else {
				def.position.x = s->number(n+0);
				def.position.y = s->number(n+1);
				n+=2;
			}
			if (s->argCount > n) {
				if (s->isTable(L,n)) {
					def.angularVelocity = s->tableNumber(L,n,"angularVelocity",def.angularVelocity);
					def.linearDamping = s->tableNumber(L,n,"linearDamping",def.linearDamping);
					def.angularDamping = s->tableNumber(L,n,"angularDamping",def.angularDamping);
					def.allowSleep = s->tableBoolean(L,n,"allowSleep",def.allowSleep);
					def.awake = s->tableBoolean(L,n,"awake",def.awake);
					def.fixedRotation = s->tableBoolean(L,n,"fixedRotation",def.fixedRotation);
					def.bullet = s->tableBoolean(L,n,"bullet",def.bullet);
					def.active = s->tableBoolean(L,n,"active",def.active);
					def.gravityScale = s->tableBoolean(L,n,"gravityScale",def.gravityScale);
				}
			}
			
			lua_getfield(L,1,"_body");
			if (lua_isnil(L,-1)) {
				lua_createtable(L,0,0);
				lua_setfield(L,1,"_body");
				lua_pop(L,1);
			}
			lua_getfield(L,1,"_body");
			
#ifdef __LUAJIT__
			size_t len = lua_objlen(L,-1);
#else
			size_t len = lua_rawlen(L,-1);
#endif
			b2Body* body = m->CreateBody(&def);
			PPLuaScript::newObject(L,"b2Body",body,NULL);
			lua_pushvalue(L,1);
			lua_setfield(L,-2,"_world");
			lua_rawseti(L,-2,(int)len+1);
			lua_rawgeti(L,-1,(int)len+1);

		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2WorldDestroyBody(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		int t = 2;
		lua_getmetatable(L,t);
		int p = lua_gettop(L);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		void* data = lua_touserdata(L,-1);
		if (data) {
			lua_pop(L,1);
			lua_getfield(L,1,"_body");
			if (!lua_isnil(L,-1)) {
				int index = findObjectIndex(L,-1,data);
				if (index > 0) {
					lua_pushnil(L);
					lua_setfield(L,p,PPGAMEINSTNACE);
					removeTable(L,-1,index);
				}
			}
		}
	}
	return 0;
}
static int funcb2WorldCreateJoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
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
		int n=1;
		if (s->argCount > n) {
			if (lua_istable(L,n+2)) {
				if (s->argCount > n+1) {
					lua_getmetatable(L,n+2);
					lua_getfield(L,-1,PPGAMEINSTNACE);
					bodyA = (b2Body*)lua_touserdata(L,-1);
					lua_pop(L,1);

					lua_getmetatable(L,n+3);
					lua_getfield(L,-1,PPGAMEINSTNACE);
					bodyB = (b2Body*)lua_touserdata(L,-1);
					lua_pop(L,1);
					
					n+=2;
				}
			}
		}
		n+=2;
		if (bodyA != NULL && bodyB != NULL) {
			if (strcmp(s->args(0),"revolute") == 0) {
				if (lua_istable(L,n)) {
					def = &revolute;
					anchor.x = getNumber(L,n,anchor.x,"anchor","x");
					anchor.y = getNumber(L,n,anchor.y,"anchor","y");
					revolute.Initialize(bodyA,bodyB,anchor);
					funcJointRevolute(L,n,&revolute);
					n++;
				}
			} else
			if (strcmp(s->args(0),"pulley") == 0) {
				if (lua_istable(L,n)) {
					def = &pulley;
					groundAnchorA.x = getNumber(L,n,groundAnchorA.x,"groundAnchorA","x");
					groundAnchorA.y = getNumber(L,n,groundAnchorA.y,"groundAnchorA","y");
					groundAnchorB.x = getNumber(L,n,groundAnchorB.x,"groundAnchorB","x");
					groundAnchorB.y = getNumber(L,n,groundAnchorB.y,"groundAnchorB","y");
					anchorA.x = getNumber(L,n,anchorA.x,"anchorA","x");
					anchorA.y = getNumber(L,n,anchorA.y,"anchorA","y");
					anchorB.x = getNumber(L,n,anchorB.x,"anchorB","x");
					anchorB.y = getNumber(L,n,anchorB.y,"anchorB","y");
					ratio = getNumber(L,n,0,"ratio");
					pulley.Initialize(bodyA,bodyB,groundAnchorA,groundAnchorB,anchorA,anchorB,ratio);
					funcJointPulley(L,n,&pulley);
					n++;
				}
			} else
			if (strcmp(s->args(0),"prismatic") == 0) {
				if (lua_istable(L,n)) {
					def = &prismatic;
					anchor.x = getNumber(L,n,anchor.x,"anchor","x");
					anchor.y = getNumber(L,n,anchor.y,"anchor","y");
					axis.x = getNumber(L,n,axis.x,"axis","x");
					axis.y = getNumber(L,n,axis.y,"axis","y");
					prismatic.Initialize(bodyA,bodyB,anchor,axis);
					funcJointPrismatic(L,n,&prismatic);
					n++;
				}
			} else
			if (strcmp(s->args(0),"distance") == 0) {
				if (lua_istable(L,n)) {
					def = &distance;
					anchorA.x = getNumber(L,n,anchorA.x,"anchorA","x");
					anchorA.y = getNumber(L,n,anchorA.y,"anchorA","y");
					anchorB.x = getNumber(L,n,anchorB.x,"anchorB","x");
					anchorB.y = getNumber(L,n,anchorB.y,"anchorB","y");
					distance.Initialize(bodyA,bodyB,anchorA,anchorB);
					funcJointDistance(L,n,&distance);
					n++;
				}
			} else
			if (strcmp(s->args(0),"gear") == 0) {
				if (lua_istable(L,n)) {
					gear.bodyA = bodyA;
					gear.bodyB = bodyB;
					funcJointGear(L,n,&gear);
					if (gear.joint1 != NULL && gear.joint2 != NULL) {
						def = &gear;
					}
					n++;
				}
			} else
			if (strcmp(s->args(0),"wheel") == 0) {
				if (lua_istable(L,n)) {
					def = &wheel;
					anchor.x = getNumber(L,n,anchor.x,"anchor","x");
					anchor.y = getNumber(L,n,anchor.y,"anchor","y");
					axis.x = getNumber(L,n,axis.x,"axis","x");
					axis.y = getNumber(L,n,axis.y,"axis","y");
					wheel.Initialize(bodyA,bodyB,anchor,axis);
					funcJointWheel(L,n,&wheel);
					n++;
				}
			} else
			if (strcmp(s->args(0),"weld") == 0) {
				if (lua_istable(L,n)) {
					def = &weld;
					anchor.x = getNumber(L,n,anchor.x,"anchor","x");
					anchor.y = getNumber(L,n,anchor.y,"anchor","y");
					weld.Initialize(bodyA,bodyB,anchor);
					funcJointWeld(L,n,&weld);
					n++;
				}
			} else
			if (strcmp(s->args(0),"friction") == 0) {
				if (lua_istable(L,n)) {
					def = &friction;
					anchor.x = getNumber(L,n,anchor.x,"anchor","x");
					anchor.y = getNumber(L,n,anchor.y,"anchor","y");
					friction.Initialize(bodyA,bodyB,anchor);
					funcJointFriction(L,n,&friction);
					n++;
				}
			} else
			if (strcmp(s->args(0),"rope") == 0) {
				if (lua_istable(L,n)) {
					def = &rope;
					funcJointRope(L,n,&rope);
					n++;
				}
			} else
			if (strcmp(s->args(0),"mouse") == 0) {
				if (lua_istable(L,n)) {
					def = &mouse;
					funcJointMouse(L,n,&mouse);
					n++;
				}
			}

			if (def) {
				def->collideConnected = getBool(L,n,def->collideConnected,"collideConnected");

				lua_getfield(L,1,"_joint");
				if (lua_isnil(L,-1)) {
					lua_createtable(L,0,0);
					lua_setfield(L,1,"_joint");
					lua_pop(L,1);
				}
				lua_getfield(L,1,"_joint");

#ifdef __LUAJIT__
				size_t len = lua_objlen(L,-1);
#else
				size_t len = lua_rawlen(L,-1);
#endif
				b2Joint* joint = m->CreateJoint(def);
				PPLuaScript::newObject(L,"b2Joint",joint,NULL);
				lua_pushvalue(L,1);
				lua_setfield(L,-2,"_world");
				lua_rawseti(L,-2,(int)len+1);
				lua_rawgeti(L,-1,(int)len+1);
			}
		}
		
	} else {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2WorldDestroyJoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		int t = 2;
		lua_getmetatable(L,t);
		int p = lua_gettop(L);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		void* data = lua_touserdata(L,-1);
		if (data) {
			lua_pop(L,1);
			lua_getfield(L,1,"_joint");
			if (!lua_isnil(L,-1)) {
				int index = findObjectIndex(L,-1,data);
				if (index > 0) {
					lua_pushnil(L);
					lua_setfield(L,p,PPGAMEINSTNACE);
					removeTable(L,-1,index);
				}
			}
		}
	}
	return 0;
}
static int funcb2WorldBodyList(lua_State *L) {
	lua_getfield(L,1,"_body");
	return 1;
}
static int funcb2WorldJointList(lua_State *L) {
	lua_getfield(L,1,"_joint");
	return 1;
}
static int funcb2WorldContactList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_createtable(L,0,0);
	int n=1;
	for (b2Contact* c=m->GetContactList();c;c=c->GetNext()) {
		const b2Manifold* manifold = c->GetManifold();
		if (manifold->pointCount > 0) {
			pushContact(L,c);
			lua_rawseti(L,-2,n);
			n++;
		}
	}
	return 1;
}
static int funcb2WorldPreSolveContactList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_createtable(L,m->pplistener->m_pointCount,0);
	int rtable = lua_gettop(L);
	int n=1;
	for (int i=0;i<m->pplistener->m_pointCount;i++) {
		ContactPoint* c = &m->pplistener->m_points[i];
		lua_createtable(L,0,5);
		int table = lua_gettop(L);

		{
			int top = lua_gettop(L);
			b2Fixture* fix = c->fixtureA;
			b2Body* body = fix->GetBody();
			lua_pushvalue(L,1);
			if (findObject(L,"_body",body)) {
				if (findObject(L,"_fixture",fix)) {
					lua_setfield(L,table,"fixtureA");
				}
			}
			lua_settop(L,top);
		}

		{
			int top = lua_gettop(L);
			b2Fixture* fix = c->fixtureB;
			b2Body* body = fix->GetBody();
			lua_pushvalue(L,1);
			if (findObject(L,"_body",body)) {
				if (findObject(L,"_fixture",fix)) {
					lua_setfield(L,table,"fixtureB");
				}
			}
			lua_settop(L,top);
		}

		lua_createtable(L,0,2);
		lua_pushnumber(L,c->normal.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,c->normal.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,table,"normal");

		lua_createtable(L,0,2);
		lua_pushnumber(L,c->velocityA.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,c->velocityA.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,table,"velocityA");

		lua_createtable(L,0,2);
		lua_pushnumber(L,c->velocityB.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,c->velocityB.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,table,"velocityB");

		lua_createtable(L,c->pointCount,0);
		int pt = lua_gettop(L);
		for (int i=0;i<c->pointCount;i++) {
			lua_createtable(L,0,4);
			lua_pushnumber(L,c->points[i].x);
			lua_setfield(L,-2,"x");
			lua_pushnumber(L,c->points[i].y);
			lua_setfield(L,-2,"y");

			switch (c->state1[i]) {
			case b2_nullState:
				lua_pushstring(L,"null");
				break;
			case b2_addState:
				lua_pushstring(L,"add");
				break;
			case b2_persistState:
				lua_pushstring(L,"persist");
				break;
			case b2_removeState:
				lua_pushstring(L,"remove");
				break;
			}
			lua_setfield(L,-2,"state1");

			switch (c->state2[i]) {
			case b2_nullState:
				lua_pushstring(L,"null");
				break;
			case b2_addState:
				lua_pushstring(L,"add");
				break;
			case b2_persistState:
				lua_pushstring(L,"persist");
				break;
			case b2_removeState:
				lua_pushstring(L,"remove");
				break;
			}
			lua_setfield(L,-2,"state2");
			lua_rawseti(L,pt,i+1);
		}
		lua_setfield(L,table,"points");
		lua_rawseti(L,rtable,n);
		n++;
	}
	return 1;
}
static int funcb2WorldAllowSleeping(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetAllowSleeping(s->boolean(0));
	}
	lua_pushboolean(L,m->GetAllowSleeping());
	return 1;
}
static int funcb2WorldWarmStarting(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetWarmStarting(s->boolean(0));
	}
	lua_pushboolean(L,m->GetWarmStarting());
	return 1;
}
static int funcb2WorldContinuousPhysics(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetContinuousPhysics(s->boolean(0));
	}
	lua_pushboolean(L,m->GetContinuousPhysics());
	return 1;
}
static int funcb2WorldSubStepping(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetSubStepping(s->boolean(0));
	}
	lua_pushboolean(L,m->GetSubStepping());
	return 1;
}
static int funcb2WorldProxyCount(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetProxyCount());
	return 1;
}
static int funcb2WorldBodyCount(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetBodyCount());
	return 1;
}
static int funcb2WorldJointCount(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetJointCount());
	return 1;
}
static int funcb2WorldContactCount(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetContactCount());
	return 1;
}
static int funcb2WorldTreeHeight(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetTreeHeight());
	return 1;
}
static int funcb2WorldTreeBalance(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetTreeBalance());
	return 1;
}
static int funcb2WorldTreeQuality(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushnumber(L,m->GetTreeQuality());
	return 1;
}
static int funcb2WorldGravity(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 g;
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		g.x = p.x;
		g.y = p.y;
		m->SetGravity(g);
	}
	g = m->GetGravity();
	return s->returnPoint(L,PPPoint(g.x,g.y));
}
static int funcb2WorldIsLocked(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	lua_pushboolean(L,m->IsLocked());
	return 1;
}
static int funcb2WorldAutoClearForces(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetAutoClearForces(s->boolean(0));
	}
	lua_pushboolean(L,m->GetAutoClearForces());
	return 1;
}
static int funcb2WorldGetContactManager(lua_State *L) {
	return 0;
}
static int funcb2WorldGetProfile(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
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

class PointQueryCallback : public b2QueryCallback
{
public:
	PointQueryCallback(const b2Vec2& point,int count,lua_State* _L) {
		L = _L;
		m_point = point;
		m_count = count;
		lua_createtable(L,0,0);
	}
 
	bool ReportFixture(b2Fixture* fixture) {
		if (fixture->TestPoint(m_point)) {
			b2Body* body = fixture->GetBody();
			int top = lua_gettop(L);
			lua_pushvalue(L,1);
			if (findObject(L,"_body",body)) {
#ifdef __LUAJIT__
				int n = (int)lua_objlen(L,top);
#else
				int n = (int)lua_rawlen(L,top);
#endif
				lua_rawseti(L,top,n+1);
			}
			lua_settop(L,top);
			if (m_count > 0) m_count --;
			if (m_count == 0) {
				return false;
			}
		}
		return true;
	}

	lua_State* L;
	b2Vec2 m_point;
	int m_count;
};

class AABBQueryCallback : public b2QueryCallback
{
public:
	AABBQueryCallback(int count,lua_State* _L) {
		L = _L;
		m_count = count;
		lua_createtable(L,0,0);
	}
 
	bool ReportFixture(b2Fixture* fixture) {
		b2Body* body = fixture->GetBody();
		int top = lua_gettop(L);
		lua_pushvalue(L,1);
		if (findObject(L,"_body",body)) {
#ifdef __LUAJIT__
			int n = (int)lua_objlen(L,top);
#else
			int n = (int)lua_rawlen(L,top);
#endif
			lua_rawseti(L,top,n+1);
		}
		lua_settop(L,top);
		if (m_count > 0) m_count --;
		if (m_count == 0) {
			return false;
		}
		return true;
	}

	lua_State* L;
	int m_count;
};

static int funcb2WorldQueryAABB(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2AABB aabb;
		aabb.lowerBound.x = getNumber(L,2,aabb.lowerBound.x,"lowerBound","x");
		aabb.lowerBound.y = getNumber(L,2,aabb.lowerBound.y,"lowerBound","y");
		aabb.upperBound.x = getNumber(L,2,aabb.upperBound.x,"upperBound","x");
		aabb.upperBound.y = getNumber(L,2,aabb.upperBound.y,"upperBound","y");
		if (s->argCount > 1 && s->isTable(L,1)) {
			b2Vec2 p;
			p.x = getNumber(L,3,p.x,"x");
			p.y = getNumber(L,3,p.y,"y");
			int count = -1;
			if (s->argCount > 2) {
				count = (int)s->integer(2);
			}
			PointQueryCallback callback(p,count,L);
			m->QueryAABB(&callback,aabb);
		} else {
			int count = -1;
			if (s->argCount > 1) {
				count = (int)s->integer(1);
			}
			AABBQueryCallback callback(count,L);
			m->QueryAABB(&callback,aabb);
		}
	}
	return 1;
}
class RayCastMultipleCallback : public b2RayCastCallback
{
public:
	RayCastMultipleCallback(int maxCount) {
		m_count = 0;
		m_maxCount = maxCount;
		m_points = (b2Vec2*)malloc(maxCount*sizeof(b2Vec2));
		m_normals = (b2Vec2*)malloc(maxCount*sizeof(b2Vec2));
		m_body = (b2Body**)malloc(maxCount*sizeof(b2Body*));
	}
	virtual ~RayCastMultipleCallback() {
		if (m_points) free(m_points);
		if (m_normals) free(m_normals);
		if (m_body) free(m_body);
	}
	float32 ReportFixture(b2Fixture* fixture,const b2Vec2& point,const b2Vec2& normal,float32 fraction) {
		b2Body* body = fixture->GetBody();
#if 0
		void* userData = body->GetUserData();
		if (userData)
		{
			int32 index = *(int32*)userData;
			if (index == 0)
			{
				// filter
				return -1.0f;
			}
		}
#endif
		m_body[m_count] = body;
		m_points[m_count] = point;
		m_normals[m_count] = normal;
		++m_count;

		if (m_count == m_maxCount)
		{
			return 0.0f;
		}

		return 1.0f;
	}
	b2Body** m_body;
	b2Vec2* m_points;
	b2Vec2* m_normals;
	int32 m_maxCount;
	int32 m_count;
};
static int funcb2WorldRayCast(lua_State* L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	PPBox2DWorld* m = (PPBox2DWorld*)s->userdata;
	if (m == NULL) return 0;
	
	if (s->argCount < 2) return 0;
	
	b2Vec2 p1,p2;
	PPPoint t1,t2;
	int n=0;
	t1 = s->getPoint(L,0);
	n++;
	if (s->isTable(L,0)) {
		n++;
	}
	t2 = s->getPoint(L,n);
	n++;
	p1.x = t1.x;
	p1.y = t1.y;
	p2.x = t2.x;
	p2.y = t2.y;
	
	int maxCount = 3;
	if (n < s->argCount) {
		maxCount = (int)s->integer(n);
	}
	
	RayCastMultipleCallback callback(maxCount);
	m->RayCast(&callback,p1,p2);

	lua_createtable(L,callback.m_count,0);
	int table = lua_gettop(L);
	for (int i=0;i<callback.m_count;i++) {
		lua_createtable(L,3,0);

		if (findObject(L,"_body",callback.m_body[i],1)) {
			lua_setfield(L,-2,"body");
		}

		lua_createtable(L,2,0);
		lua_pushnumber(L,callback.m_points[i].x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,callback.m_points[i].y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"point");

		lua_createtable(L,2,0);
		lua_pushnumber(L,callback.m_normals[i].x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,callback.m_normals[i].y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"normal");
		lua_rawseti(L,table,i+1);
		
		lua_settop(L,table);
	}

	return 1;
}

static int funcb2BodyCreateFixture(lua_State *L)
{
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m && s->argCount > 0) {
		b2CircleShape circleShape;
		b2PolygonShape polygonShape;
		b2EdgeShape edgeShape;
		b2ChainShape chainShape;
		b2FixtureDef def;
		int n=0;
		if (strcmp(s->args(n),"circle") == 0) {
			n++;
			if (s->argCount > n) {
				circleShape.m_radius = s->number(n);
				def.shape = &circleShape;
				n++;
			}
		} else
		if (strcmp(s->args(n),"edge") == 0) {
			n++;
			PPPoint p[2];
			if (s->isTable(L,n)) {
#ifdef __LUAJIT__
				int len = (int)lua_objlen(L,n+2);
#else
				int len = (int)lua_rawlen(L,n+2);
#endif
				int top = lua_gettop(L);
				for (int i=0,m=0;i<len&m<2;i+=2,m++) {
					lua_rawgeti(L,n+2,i+1);
					p[m].x = luaL_optnumber(L,-1,0);
					lua_rawgeti(L,n+2,i+2);
					p[m].y = luaL_optnumber(L,-1,0);
					lua_settop(L,top);
				}
				edgeShape.Set(b2Vec2(p[0].x,p[0].y),b2Vec2(p[1].x,p[1].y));
				def.shape = &edgeShape;
				n++;
			}
		} else
		if (strcmp(s->args(n),"box") == 0) {
			n++;
			PPPoint p;
			if (s->isTable(L,n)) {
#ifdef __LUAJIT__
				int len = (int)lua_objlen(L,n+2);
#else
				int len = (int)lua_rawlen(L,n+2);
#endif
				if (len >= 2) {
					lua_rawgeti(L,n+2,1);
					p.x = luaL_optnumber(L,-1,0);
					lua_rawgeti(L,n+2,2);
					p.y = luaL_optnumber(L,-1,0);
					polygonShape.SetAsBox(p.x,p.y);
					def.shape = &polygonShape;
				}
				n++;
			}
		} else
		if (strcmp(s->args(n),"polygon") == 0) {
			n++;
			if (s->argCount > n) {
#ifdef __LUAJIT__
				int len = (int)lua_objlen(L,n+2);
#else
				int len = (int)lua_rawlen(L,n+2);
#endif
				int top = lua_gettop(L);
				b2Vec2* p = (b2Vec2*)malloc((len/2)*sizeof(b2Vec2));
				if (p) {
					for (int i=0,m=0;i<len&m<(len/2);i+=2,m++) {
						lua_rawgeti(L,n+2,i+1);
						p[m].x = luaL_optnumber(L,-1,0);
						lua_rawgeti(L,n+2,i+2);
						p[m].y = luaL_optnumber(L,-1,0);
						lua_settop(L,top);
//printf("p%d %f,%f\n",m,p[m].x,p[m].y);
					}
					polygonShape.Set(p,len/2);
					def.shape = &polygonShape;
					free(p);
				}
				n++;
			}
		} else
		if (strcmp(s->args(n),"chain") == 0) {
			n++;
			if (s->argCount > n) {
#ifdef __LUAJIT__
				int len = (int)lua_objlen(L,n+2);
#else
				int len = (int)lua_rawlen(L,n+2);
#endif
				int top = lua_gettop(L);
				b2Vec2* p = (b2Vec2*)malloc((len/2)*sizeof(b2Vec2));
				if (p) {
					for (int i=0,m=0;i<len&m<(len/2);i+=2,m++) {
						lua_rawgeti(L,n+2,i+1);
						p[m].x = luaL_optnumber(L,-1,0);
						lua_rawgeti(L,n+2,i+2);
						p[m].y = luaL_optnumber(L,-1,0);
						lua_settop(L,top);
					}
					chainShape.CreateChain(p,len/2);
					def.shape = &chainShape;
					free(p);
				}
				n++;
			}
		}
		if (def.shape) {
			if (s->argCount > n) {
				if (s->isTable(L,n)) {
					def.friction = s->tableNumber(L,n,"friction",def.friction);
					def.restitution = s->tableNumber(L,n,"restitution",def.restitution);
					def.density = s->tableNumber(L,n,"density",def.density);
					def.isSensor = s->tableBoolean(L,n,"isSensor",def.isSensor);
					def.filter.categoryBits = s->tableInteger(L,n,"categoryBits",def.filter.categoryBits);
					def.filter.maskBits = s->tableInteger(L,n,"maskBits",def.filter.maskBits);
					def.filter.groupIndex = s->tableInteger(L,n,"groupIndex",def.filter.groupIndex);
				}
			}

			lua_getfield(L,1,"_fixture");
			if (lua_isnil(L,-1)) {
				lua_createtable(L,0,0);
				lua_setfield(L,1,"_fixture");
				lua_pop(L,1);
			}
			lua_getfield(L,1,"_fixture");
			
#ifdef __LUAJIT__
			size_t len = lua_objlen(L,-1);
#else
			size_t len = lua_rawlen(L,-1);
#endif
			b2Fixture* fix = m->CreateFixture(&def);
			PPLuaScript::newObject(L,"b2Fixture",fix,NULL);
			PPLuaScript::newObject(L,"b2Shape",fix->GetShape(),NULL);
			lua_setfield(L,-2,"_shape");
			lua_rawseti(L,-2,(int)len+1);
			lua_rawgeti(L,-1,(int)len+1);

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
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		int t = 2;
		lua_getmetatable(L,t);
		int p = lua_gettop(L);
		lua_getfield(L,-1,PPGAMEINSTNACE);
		void* data = lua_touserdata(L,-1);
		if (data) {
			lua_pop(L,1);
			lua_getfield(L,1,"_fixture");
			if (!lua_isnil(L,-1)) {
				int index = findObjectIndex(L,-1,data);
				if (index > 0) {
					lua_pushnil(L);
					lua_setfield(L,p,PPGAMEINSTNACE);
					removeTable(L,-1,index);
				}
			}
		}
	}
	return 0;
}
static int funcb2BodyTransform(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v(0,0);
		float angle=0;
		if (s->isTable(L,0)) {
			v.x = getNumber(L,2,v.x,"x");
			v.y = getNumber(L,2,v.y,"y");
			angle = getNumber(L,2,angle,"angle");
		} else {
			PPPoint p = s->getPoint(L,0);
			if (s->isTable(L,0)) {
				angle = s->number(1);
			} else {
				angle = s->number(2);
			}
			v.x = p.x;
			v.y = p.y;
		}
		m->SetTransform(v,angle);
	}
	b2Transform t = m->GetTransform();
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
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v(0,0);
		float angle=m->GetAngle();
		if (s->isTable(L,0)) {
			v.x = getNumber(L,2,v.x,"x");
			v.y = getNumber(L,2,v.y,"y");
		} else {
			PPPoint p = s->getPoint(L,0);
			v.x = p.x;
			v.y = p.y;
		}
		m->SetTransform(v,angle);
	}
	b2Vec2 position = m->GetPosition();
	return s->returnPoint(L,PPPoint(position.x,position.y));
}
static int funcb2BodyScaledPosition(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	QBGame* g = (QBGame*)PPWorld::sharedWorld();
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	float scale = 10;
	if (s->argCount > 0) {
		scale = s->number(0);
	}
	b2Vec2 position = m->GetPosition();
//	PPPoint sc = g->GetScale();
//	return s->returnPoint(L,PPPoint(position.x*scale*sc.x,position.y*scale*sc.y));
	return s->returnPoint(L,PPPoint(position.x*scale,position.y*scale));
}
static int funcb2BodyAngle(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v = m->GetPosition();
		float angle=0;
		if (s->isTable(L,0)) {
			angle = getNumber(L,2,angle,"angle");
		} else {
			angle = s->number(0);
		}
		m->SetTransform(v,angle);
	}
	lua_pushnumber(L,m->GetAngle());
	return 1;
}
static int funcb2BodyWorldCenter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 position = m->GetWorldCenter();
	return s->returnPoint(L,PPPoint(position.x,position.y));
}
static int funcb2BodyLocalCenter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 position = m->GetLocalCenter();
	return s->returnPoint(L,PPPoint(position.x,position.y));
}
static int funcb2BodyLinearVelocity(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v;
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
		m->SetLinearVelocity(v);
	}
	v = m->GetLinearVelocity();
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyAngularVelocity(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetAngularVelocity(s->number(0));
	}
	lua_pushnumber(L,m->GetAngularVelocity());
	return 1;
}
static int funcb2BodyApplyForce(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 1) {
		PPPoint p = s->getPoint(L,0);
		b2Vec2 v1(p.x,p.y);
		if (s->isTable(L,0)) {
			p = s->getPoint(L,1);
		} else {
			p = s->getPoint(L,2);
		}
		b2Vec2 v2(p.x,p.y);
		m->ApplyForce(v1,v2);
	}
	return 0;
}
static int funcb2BodyApplyForceToCenter(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		b2Vec2 v(p.x,p.y);
		m->ApplyForceToCenter(v);
	}
	return 0;
}
static int funcb2BodyApplyTorque(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->ApplyTorque(s->number(0));
	}
	return 0;
}
static int funcb2BodyApplyLinearImpulse(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 1) {
		PPPoint p = s->getPoint(L,0);
		b2Vec2 v1(p.x,p.y);
		if (s->isTable(L,0)) {
			p = s->getPoint(L,1);
		} else {
			p = s->getPoint(L,2);
		}
		b2Vec2 v2(p.x,p.y);
		m->ApplyLinearImpulse(v1,v2);
	}
	return 0;
}
static int funcb2BodyApplyAngularImpusle(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->ApplyAngularImpulse(s->number(0));
	}
	return 0;
}
static int funcb2BodyMass(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	lua_pushnumber(L,m->GetMass());
	return 1;
}
static int funcb2BodyInertia(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	lua_pushnumber(L,m->GetInertia());
	return 1;
}
static int funcb2BodyMassData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2MassData data;
	if (s->argCount > 0) {
		if (s->isTable(L,0)) {
			data.mass = s->tableNumber(L,0,"mass",data.mass);
			data.center.x = s->tableNumber(L,0,"x",data.center.x);
			data.center.y = s->tableNumber(L,0,"y",data.center.y);
			data.I = s->tableNumber(L,0,"I",data.I);
		} else {
			data.mass = s->number(0,data.mass);
			data.center.x = s->number(1,data.center.x);
			data.center.y = s->number(2,data.center.y);
			data.I = s->number(3,data.I);
		}
	}
	m->GetMassData(&data);
	lua_createtable(L,0,0);
	int b = lua_gettop(L);
	lua_pushnumber(L,data.mass);
	lua_setfield(L,b,"mass");
	lua_pushnumber(L,data.center.x);
	lua_setfield(L,b,"x");
	lua_pushnumber(L,data.center.y);
	lua_setfield(L,b,"y");
	lua_pushnumber(L,data.I);
	lua_setfield(L,b,"I");
	return 1;
}
static int funcb2BodyResetMassData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	m->ResetMassData();
	return 0;
}
static int funcb2BodyWorldPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->GetWorldPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyWorldVector(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->GetWorldVector(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyLocalPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->GetLocalPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyLocalVector(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->GetLocalVector(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyLinearVelocityFromWorldPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->GetLinearVelocityFromWorldPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyLinearVelocityFromLocalPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	if (s->argCount > 0) {
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
	}
	v = m->GetLinearVelocityFromLocalPoint(v);
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2BodyLinearDamping(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetLinearDamping(s->number(0));
	}
	lua_pushnumber(L,m->GetLinearDamping());
	return 1;
}
static int funcb2BodyAngularDamping(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetAngularDamping(s->number(0));
	}
	lua_pushnumber(L,m->GetAngularDamping());
	return 1;
}
static int funcb2BodyGravityScale(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetGravityScale(s->number(0));
	}
	lua_pushnumber(L,m->GetGravityScale());
	return 1;
}
static int funcb2BodyType(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		if (strcmp(s->args(0),"static") == 0) {
			m->SetType(b2_staticBody);
		} else
		if (strcmp(s->args(0),"dynamic") == 0) {
			m->SetType(b2_dynamicBody);
		} else
		if (strcmp(s->args(0),"kinematic") == 0) {
			m->SetType(b2_kinematicBody);
		}
	}
	switch (m->GetType()) {
	case b2_staticBody:
		lua_pushstring(L,"static");
		break;
	case b2_dynamicBody:
		lua_pushstring(L,"dynamic");
		break;
	case b2_kinematicBody:
		lua_pushstring(L,"kinematic");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}
static int funcb2BodyBullet(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetBullet(s->boolean(0));
	}
	lua_pushboolean(L,m->IsBullet());
	return 1;
}
static int funcb2BodySleepingAllowed(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetSleepingAllowed(s->boolean(0));
	}
	lua_pushboolean(L,m->IsSleepingAllowed());
	return 1;
}
static int funcb2BodyAwake(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetAwake(s->boolean(0));
	}
	lua_pushboolean(L,m->IsAwake());
	return 1;
}
static int funcb2BodyActive(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetActive(s->boolean(0));
	}
	lua_pushboolean(L,m->IsActive());
	return 1;
}
static int funcb2BodyFixedRotation(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetFixedRotation(s->boolean(0));
	}
	lua_pushboolean(L,m->IsFixedRotation());
	return 1;
}
static int funcb2BodyFixtureList(lua_State *L) {
	lua_getfield(L,1,"_fixture");
	return 1;
}
static int funcb2BodyJointList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
//	PPLuaScript* s = PPLuaScript::sharedScript(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2JointEdge* joint = m->GetJointList();
	
	return 0;
}
static int funcb2BodyContactList(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2ContactEdge* contact = m->GetContactList();
	
	return 0;
}
static int funcb2BodyNext(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	b2Body* tbody = m->GetNext();
	if (!findB2Object(L,"_body",tbody)) {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2BodyUserData(lua_State *L) {
	return 0;
}
static int funcb2BodyWorld(lua_State *L) {
	lua_getfield(L,1,"_world");
	return 1;
}
static int funcb2BodyTestPoint(lua_State *L) {
	bool hit=false;
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v;
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
		b2Fixture* fix = m->GetFixtureList();
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
	b2Body* m = (b2Body*)s->userdata;
	if (m == NULL) return 0;
	m->Dump();
	return 0;
}
static int funcb2JointType(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		lua_pushstring(L,"revolute");
		break;
	case e_pulleyJoint:
		lua_pushstring(L,"pulley");
		break;
	case e_prismaticJoint:
		lua_pushstring(L,"prismatic");
		break;
	case e_distanceJoint:
		lua_pushstring(L,"distance");
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
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}
static int funcb2JointBodyA(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Body* tbody = m->GetBodyA();
	if (!findB2Object(L,"_body",tbody)) {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2JointBodyB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Body* tbody = m->GetBodyB();
	if (!findB2Object(L,"_body",tbody)) {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2JointAnchorA(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	v = m->GetAnchorA();
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2JointAnchorB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v(0,0);
	v = m->GetAnchorB();
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2JointReactionForce(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount <= 0) return 0;
	b2Vec2 v(0,0);
	v = m->GetReactionForce(s->number(0));
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2JointReactionTorque(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount <= 0) return 0;
	lua_pushnumber(L,m->GetReactionTorque(s->number(0)));
	return 1;
}
static int funcb2JointNext(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Joint* tbody = m->GetNext();
	if (!findB2Object(L,"_joint",tbody)) {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2JointUserData(lua_State *L) {
	return 1;
}
static int funcb2JointActive(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	lua_pushboolean(L,m->IsActive());
	return 1;
}
static int funcb2JointCollideConnected(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	lua_pushboolean(L,m->GetCollideConnected());
	return 1;
}
static int funcb2JointDump(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	m->Dump();
	return 0;
}
static int funcb2JointLocalAnchorA(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetLocalAnchorA();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetLocalAnchorA();
		break;
	case e_distanceJoint:
		v = ((b2DistanceJoint*)m)->GetLocalAnchorA();
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		v = ((b2WheelJoint*)m)->GetLocalAnchorA();
		break;
	case e_weldJoint:
		v = ((b2WeldJoint*)m)->GetLocalAnchorA();
		break;
	case e_frictionJoint:
		v = ((b2FrictionJoint*)m)->GetLocalAnchorA();
		break;
	case e_ropeJoint:
		v = ((b2RopeJoint*)m)->GetLocalAnchorA();
		break;
	default:
		break;
	}
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2JointLocalAnchorB(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetLocalAnchorB();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetLocalAnchorB();
		break;
	case e_distanceJoint:
		v = ((b2DistanceJoint*)m)->GetLocalAnchorB();
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		v = ((b2WheelJoint*)m)->GetLocalAnchorB();
		break;
	case e_weldJoint:
		v = ((b2WeldJoint*)m)->GetLocalAnchorB();
		break;
	case e_frictionJoint:
		v = ((b2FrictionJoint*)m)->GetLocalAnchorB();
		break;
	case e_ropeJoint:
		v = ((b2RopeJoint*)m)->GetLocalAnchorB();
		break;
	default:
		break;
	}
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2JointLength(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		v = ((b2DistanceJoint*)m)->GetLength();
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointFrequency(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		if (s->argCount > 0) {
			((b2DistanceJoint*)m)->SetFrequency(s->number(0));
		}
		v = ((b2DistanceJoint*)m)->GetFrequency();
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		if (s->argCount > 0) {
			((b2WeldJoint*)m)->SetFrequency(s->number(0));
		}
		v = ((b2WeldJoint*)m)->GetFrequency();
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointDampingRatio(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		if (s->argCount > 0) {
			((b2DistanceJoint*)m)->SetDampingRatio(s->number(0));
		}
		v = ((b2DistanceJoint*)m)->GetDampingRatio();
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		if (s->argCount > 0) {
			((b2WeldJoint*)m)->SetDampingRatio(s->number(0));
		}
		v = ((b2WeldJoint*)m)->GetDampingRatio();
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMaxForce(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		if (s->argCount > 0) {
			((b2FrictionJoint*)m)->SetMaxForce(s->number(0));
		}
		v = ((b2FrictionJoint*)m)->GetMaxForce();
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMaxTorque(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		if (s->argCount > 0) {
			((b2FrictionJoint*)m)->SetMaxTorque(s->number(0));
		}
		v = ((b2FrictionJoint*)m)->GetMaxTorque();
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointJoint1(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		{
			b2Joint* j = ((b2GearJoint*)m)->GetJoint1();
			if (!findB2Object(L,"_joint",j)) {
				lua_pushnil(L);
			}
			return 1;
		}
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	return 0;
}
static int funcb2JointJoint2(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		{
			b2Joint* j = ((b2GearJoint*)m)->GetJoint2();
			if (!findB2Object(L,"_joint",j)) {
				lua_pushnil(L);
			}
			return 1;
		}
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	return 0;
}
static int funcb2JointRatio(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		v = ((b2PulleyJoint*)m)->GetRatio();
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		if (s->argCount > 0) {
			((b2GearJoint*)m)->SetRatio(s->number(0));
		}
		v = ((b2GearJoint*)m)->GetRatio();
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointLocalAxisA(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	b2Vec2 v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetLocalAxisA();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		v = ((b2WheelJoint*)m)->GetLocalAxisA();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	return s->returnPoint(L,PPPoint(v.x,v.y));
}
static int funcb2JointReferenceAngle(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetReferenceAngle();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetReferenceAngle();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		v = ((b2WeldJoint*)m)->GetReferenceAngle();
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointJointTranslation(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetJointTranslation();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		v = ((b2WheelJoint*)m)->GetJointTranslation();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointJointSpeed(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetJointSpeed();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetJointSpeed();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		v = ((b2WheelJoint*)m)->GetJointSpeed();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointLimitEnabled(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	bool v=false;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		if (s->argCount > 0) {
			((b2PrismaticJoint*)m)->EnableLimit(s->boolean(0));
		}
		v = ((b2PrismaticJoint*)m)->IsLimitEnabled();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushboolean(L,v);
	return 1;
}
static int funcb2JointEnableLimit(lua_State* L) {
	return funcb2JointLimitEnabled(L);
}
static int funcb2JointLowerLimit(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetLowerLimit();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetLowerLimit();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointUpperLimit(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetUpperLimit();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetUpperLimit();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointLimits(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount < 2) return 0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		((b2RevoluteJoint*)m)->SetLimits(s->number(0),s->number(1));
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		((b2PrismaticJoint*)m)->SetLimits(s->number(0),s->number(1));
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	return 0;
}
static int funcb2JointEnableMotor(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	bool v=false;
	switch (m->GetType()) {
	case e_revoluteJoint:
		if (s->argCount > 0) {
			((b2RevoluteJoint*)m)->EnableMotor(s->boolean(0));
		}
		v = ((b2RevoluteJoint*)m)->IsMotorEnabled();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		if (s->argCount > 0) {
			((b2PrismaticJoint*)m)->EnableMotor(s->boolean(0));
		}
		v = ((b2PrismaticJoint*)m)->IsMotorEnabled();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		if (s->argCount > 0) {
			((b2WheelJoint*)m)->EnableMotor(s->boolean(0));
		}
		v = ((b2WheelJoint*)m)->IsMotorEnabled();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushboolean(L,v);
	return 1;
}
static int funcb2JointMotorSpeed(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		if (s->argCount > 0) {
			((b2RevoluteJoint*)m)->SetMotorSpeed(s->number(0));
		}
		v = ((b2RevoluteJoint*)m)->GetMotorSpeed();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		if (s->argCount > 0) {
			((b2PrismaticJoint*)m)->SetMotorSpeed(s->number(0));
		}
		v = ((b2PrismaticJoint*)m)->GetMotorSpeed();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		if (s->argCount > 0) {
			((b2WheelJoint*)m)->SetMotorSpeed(s->number(0));
		}
		v = ((b2WheelJoint*)m)->GetMotorSpeed();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMaxMotorForce(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		if (s->argCount > 0) {
			((b2PrismaticJoint*)m)->SetMaxMotorForce(s->number(0));
		}
		v = ((b2PrismaticJoint*)m)->GetMaxMotorForce();
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMaxMotorTorque(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		if (s->argCount > 0) {
			((b2RevoluteJoint*)m)->SetMaxMotorTorque(s->number(0));
		}
		v = ((b2RevoluteJoint*)m)->GetMaxMotorTorque();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		if (s->argCount > 0) {
			((b2WheelJoint*)m)->SetMaxMotorTorque(s->number(0));
		}
		v = ((b2WheelJoint*)m)->GetMaxMotorTorque();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMotorTorque(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		if (s->argCount > 0) {
			v = ((b2RevoluteJoint*)m)->GetMotorTorque(s->number(0));
		}
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		if (s->argCount > 0) {
			v = ((b2WheelJoint*)m)->GetMotorTorque(s->number(0));
		}
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMotorForce(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		v = ((b2PrismaticJoint*)m)->GetMotorForce(s->number(0));
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointLengthA(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		v = ((b2PulleyJoint*)m)->GetLengthA();
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointLengthB(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		v = ((b2PulleyJoint*)m)->GetLengthB();
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointMaxLength(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		v = ((b2RopeJoint*)m)->GetMaxLength();
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointJointAngle(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		v = ((b2RevoluteJoint*)m)->GetJointAngle();
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointLimitState(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		{
			b2LimitState v = ((b2RopeJoint*)m)->GetLimitState();
			switch (v) {
			case e_inactiveLimit:
				lua_pushstring(L,"inactiveLimit");
				break;
			case e_atLowerLimit:
				lua_pushstring(L,"atLowerLimit");
				break;
			case e_atUpperLimit:
				lua_pushstring(L,"atUpperLimit");
				break;
			case e_equalLimits:
				lua_pushstring(L,"equalLimits");
				break;
			}
			return 1;
		}
		break;
	default:
		break;
	}
	return 0;
}
static int funcb2JointSpringFrequencyHz(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		if (s->argCount > 0) {
			((b2WheelJoint*)m)->SetSpringFrequencyHz(s->number(0));
		}
		v = ((b2WheelJoint*)m)->GetSpringFrequencyHz();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2JointSpringDampingRatio(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Joint* m = (b2Joint*)s->userdata;
	if (m == NULL) return 0;
	float v=0;
	switch (m->GetType()) {
	case e_revoluteJoint:
		break;
	case e_pulleyJoint:
		break;
	case e_prismaticJoint:
		break;
	case e_distanceJoint:
		break;
	case e_gearJoint:
		break;
	case e_wheelJoint:
		if (s->argCount > 0) {
			((b2WheelJoint*)m)->SetSpringDampingRatio(s->number(0));
		}
		v = ((b2WheelJoint*)m)->GetSpringDampingRatio();
		break;
	case e_weldJoint:
		break;
	case e_frictionJoint:
		break;
	case e_ropeJoint:
		break;
	default:
		break;
	}
	lua_pushnumber(L,v);
	return 1;
}
static int funcb2ShapeClone(lua_State *L) {
	return 0;
}
static int funcb2ShapeType(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Shape* m = (b2Shape*)s->userdata;
	if (m == NULL) return 0;
	switch (m->GetType()) {
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
		lua_pushnil(L);
		break;
	}
	return 1;
}
static int funcb2ShapeChildCount(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Shape* m = (b2Shape*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetChildCount());
	return 1;
}
static int funcb2ShapeTestPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Shape* m = (b2Shape*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 1) {
		b2Transform xf;
		b2Vec2 p;
		if (s->isTable(L,0)) {
			xf.p.x = s->tableNumber(L,0,"x",xf.p.x);
			xf.p.y = s->tableNumber(L,0,"y",xf.p.y);
			xf.q.s = s->tableNumber(L,0,"sin",xf.q.s);
			xf.q.c = s->tableNumber(L,0,"cos",xf.q.c);
			PPPoint _p = s->getPoint(L,1);
			p.x = _p.x;
			p.y = _p.y;
			lua_pushboolean(L,m->TestPoint(xf,p));
			return 1;
		}
	}
	return 0;
}
static int funcb2ShapeRayCast(lua_State *L) {
	return 0;
}
static int funcb2ShapeComputeAABB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Shape* m = (b2Shape*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 1) {
		b2AABB aabb;
		b2Transform xf;
		xf.p.x = s->tableNumber(L,0,"x",xf.p.x);
		xf.p.y = s->tableNumber(L,0,"y",xf.p.y);
		xf.q.s = s->tableNumber(L,0,"sin",xf.q.s);
		xf.q.c = s->tableNumber(L,0,"cos",xf.q.c);
		int childIndex=(int)s->integer(1);
		m->ComputeAABB(&aabb,xf,childIndex);
		lua_createtable(L,0,0);
		lua_createtable(L,0,2);
		lua_pushnumber(L,aabb.lowerBound.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,aabb.lowerBound.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"lowerBound");
		lua_createtable(L,0,2);
		lua_pushnumber(L,aabb.upperBound.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,aabb.upperBound.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"upperBound");
		return 1;
	}
	return 0;
}
static int funcb2ShapeComputeMass(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Shape* m = (b2Shape*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2MassData data;
		float density = s->number(0);
		m->ComputeMass(&data,density);
		lua_createtable(L,0,0);
		int b = lua_gettop(L);
		lua_pushnumber(L,data.mass);
		lua_setfield(L,b,"mass");
		lua_pushnumber(L,data.center.x);
		lua_setfield(L,b,"x");
		lua_pushnumber(L,data.center.y);
		lua_setfield(L,b,"y");
		lua_pushnumber(L,data.I);
		lua_setfield(L,b,"I");
		return 1;
	}
	return 0;
}
static int funcb2ShapeRadius(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Shape* m = (b2Shape*)s->userdata;
	if (m == NULL) return 0;
	lua_pushnumber(L,m->m_radius);
	return 1;
}
static int funcb2FixtureType(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	switch (m->GetType()) {
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
		lua_pushnil(L);
		break;
	}
	return 1;
}
static int funcb2FixtureShape(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	lua_getfield(L,1,"_shape");
	return 1;
}
static int funcb2FixtureSensor(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetSensor(s->boolean(0));
	}
	lua_pushboolean(L,m->IsSensor());
	return 1;
}
static int funcb2FixtureFilterData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	b2Filter filter;
	if (s->argCount > 0) {
		filter.categoryBits = s->tableInteger(L,0,"categoryBits",filter.categoryBits);
		filter.maskBits = s->tableInteger(L,0,"maskBits",filter.maskBits);
		filter.groupIndex = s->tableInteger(L,0,"groupIndex",filter.groupIndex);
		m->SetFilterData(filter);
	}
	filter = m->GetFilterData();
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
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	m->Refilter();
	return 0;
}
static int funcb2FixtureBody(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (!findB2Object(L,"_body",m->GetBody())) {
		lua_pushnil(L);
	}
	return 1;
}
static int funcb2FixtureNext(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (!findB2Object(L,"_body",m->GetBody())) {
		lua_pushnil(L);
		return 1;
	}
	if (!findObject(L,"_fixture",m->GetNext())) {
		lua_pushnil(L);
		return 1;
	}
	return 1;
}
static int funcb2FixtureUserData(lua_State *L) {
	return 0;
}
static int funcb2FixtureTestPoint(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2Vec2 v;
		PPPoint p = s->getPoint(L,0);
		v.x = p.x;
		v.y = p.y;
		lua_pushboolean(L,m->TestPoint(v));
		return 1;
	}
	return 0;
}
static int funcb2FixtureRayCast(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	
	if (s->argCount > 1) {
	
		b2RayCastOutput output;
		b2RayCastInput input;
		input.p1.x = getNumber(L,2,0,"p1","x");
		input.p1.y = getNumber(L,2,0,"p1","y");
		input.p2.x = getNumber(L,2,0,"p2","x");
		input.p2.y = getNumber(L,2,0,"p2","y");
		input.maxFraction = getNumber(L,2,0,"maxFraction");
		int childIndex = (int)s->integer(1);
		
		if (m->RayCast(&output,input,childIndex)) {
			
		}

		lua_createtable(L,0,2);
		lua_createtable(L,0,2);
		lua_pushnumber(L,output.normal.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,output.normal.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"normal");
		lua_pushnumber(L,output.fraction);
		lua_setfield(L,-2,"fraction");
		
	}

	return 1;
}
static int funcb2FixtureMassData(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	b2MassData data;
	m->GetMassData(&data);
	lua_createtable(L,0,0);
	int b = lua_gettop(L);
	lua_pushnumber(L,data.mass);
	lua_setfield(L,b,"mass");
	lua_pushnumber(L,data.center.x);
	lua_setfield(L,b,"x");
	lua_pushnumber(L,data.center.y);
	lua_setfield(L,b,"y");
	lua_pushnumber(L,data.I);
	lua_setfield(L,b,"I");
	return 1;
}
static int funcb2FixtureDensity(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetDensity(s->number(0));
	}
	lua_pushnumber(L,m->GetDensity());
	return 1;
}
static int funcb2FixtureFriction(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetFriction(s->number(0));
	}
	lua_pushnumber(L,m->GetFriction());
	return 1;
}
static int funcb2FixtureRestitution(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetRestitution(s->number(0));
	}
	lua_pushnumber(L,m->GetRestitution());
	return 1;
}
static int funcb2FixtureAABB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		b2AABB aabb = m->GetAABB((int)s->integer(0));
		lua_createtable(L,0,0);
		lua_createtable(L,0,2);
		lua_pushnumber(L,aabb.lowerBound.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,aabb.lowerBound.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"lowerBound");
		lua_createtable(L,0,2);
		lua_pushnumber(L,aabb.upperBound.x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,aabb.upperBound.y);
		lua_setfield(L,-2,"y");
		lua_setfield(L,-2,"upperBound");
		return 1;
	}
	return 0;
}
static int funcb2FixtureDump(lua_State* L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Fixture* m = (b2Fixture*)s->userdata;
	if (m == NULL) return 0;
	m->Dump((int)s->integer(0));
	return 0;
}
static int funcb2ContactManifold(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	//pushManifold(L,m->GetManifold());
	return 0;
}
static int funcb2ContactWorldManifold(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	b2WorldManifold worldManifold;
	m->GetWorldManifold(&worldManifold);
	lua_createtable(L,0,0);
	lua_createtable(L,0,0);
	for (int i=0;i<b2_maxManifoldPoints;i++) {
		lua_createtable(L,0,0);
		lua_pushnumber(L,worldManifold.points[i].x);
		lua_setfield(L,-2,"x");
		lua_pushnumber(L,worldManifold.points[i].y);
		lua_setfield(L,-2,"y");
		lua_rawseti(L,-2,i);
	}
	lua_setfield(L,-2,"points");
	lua_createtable(L,0,0);
	lua_pushnumber(L,worldManifold.normal.x);
	lua_setfield(L,-2,"x");
	lua_pushnumber(L,worldManifold.normal.y);
	lua_setfield(L,-2,"y");
	lua_setfield(L,-2,"normal");
	return 0;
}
static int funcb2ContactTouching(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	lua_pushboolean(L,m->IsTouching());
	return 1;
}
static int funcb2ContactEnabled(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetEnabled(s->boolean(0));
	}
	lua_pushboolean(L,m->IsEnabled());
	return 1;
}
static int funcb2ContactNext(lua_State *L) {
	return 1;
}
static int funcb2ContactFixtureA(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	b2Fixture* fix = m->GetFixtureA();
	b2Body* body = fix->GetBody();
	if (findB2Object(L,"_body",body)) {
		if (findObject(L,"_fixture",fix)) {
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int funcb2ContactChildIndexA(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetChildIndexA());
	return 1;
}
static int funcb2ContactFixtureB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	b2Fixture* fix = m->GetFixtureB();
	b2Body* body = fix->GetBody();
	if (findB2Object(L,"_body",body)) {
		if (findObject(L,"_fixture",fix)) {
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int funcb2ContactChildIndexB(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	lua_pushinteger(L,m->GetChildIndexB());
	return 1;
}
static int funcb2ContactFriction(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetFriction(s->number(0));
	}
	lua_pushnumber(L,m->GetFriction());
	return 1;
}
static int funcb2ContactResetFriction(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	m->ResetFriction();
	return 1;
}
static int funcb2ContactRestitution(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	if (s->argCount > 0) {
		m->SetRestitution(s->number(0));
	}
	lua_pushnumber(L,m->GetRestitution());
	return 1;
}
static int funcb2ContactResetRestitution(lua_State *L) {
	PPLuaArg arg(NULL);PPLuaArg* s=&arg;s->init(L);
	b2Contact* m = (b2Contact*)s->userdata;
	if (m == NULL) return 0;
	m->ResetRestitution();
	return 1;
}
static int funcb2ContactEvaluate(lua_State *L) {
	return 0;
}

void PPBox2D::openLibrary(PPLuaScript* s,const char* name,const char* superclass)
{
	s->openModule(name,NULL,0);
		s->addCommand("newWorld",funcNewWorld);
		s->addCommand("scale",funcScale);
	s->closeModule();
	
	s->openModule("PPBox2DWorld",NULL,0);
		s->addCommand("createBody",funcb2WorldCreateBody);
		s->addCommand("destroyBody",funcb2WorldDestroyBody);
		s->addCommand("createJoint",funcb2WorldCreateJoint);
		s->addCommand("destroyJoint",funcb2WorldDestroyJoint);
		s->addCommand("step",funcb2WorldStep);
		s->addCommand("bodyList",funcb2WorldBodyList);
		s->addCommand("jointList",funcb2WorldJointList);
		s->addCommand("contactList",funcb2WorldContactList);
		s->addCommand("preSolveContactList",funcb2WorldPreSolveContactList);
		s->addCommand("allowSleeping",funcb2WorldAllowSleeping);
		s->addCommand("warmStarting",funcb2WorldWarmStarting);
		s->addCommand("continuousPhysics",funcb2WorldContinuousPhysics);
		s->addCommand("subStepping",funcb2WorldSubStepping);
		s->addCommand("proxyCount",funcb2WorldProxyCount);
		s->addCommand("bodyCount",funcb2WorldBodyCount);
		s->addCommand("jointCount",funcb2WorldJointCount);
		s->addCommand("contactCount",funcb2WorldContactCount);
		s->addCommand("treeHeight",funcb2WorldTreeHeight);
		s->addCommand("treeBalance",funcb2WorldTreeBalance);
		s->addCommand("treeQuality",funcb2WorldTreeQuality);
		s->addCommand("gravity",funcb2WorldGravity);
		s->addCommand("isLocked",funcb2WorldIsLocked);
		s->addCommand("autoClearForces",funcb2WorldAutoClearForces);
		s->addCommand("getContactManager",funcb2WorldGetContactManager);
		s->addCommand("getProfile",funcb2WorldGetProfile);
		s->addCommand("queryAABB",funcb2WorldQueryAABB);
		s->addCommand("rayCast",funcb2WorldRayCast);
		s->addCommand("dump",funcb2WorldDump);
	s->closeModule();

	s->openModule("b2Body",NULL,0);
		s->addCommand("createFixture",funcb2BodyCreateFixture);
		s->addCommand("destroyFixture",funcb2BodyDestroyFixture);
		s->addCommand("transform",funcb2BodyTransform);
		s->addCommand("position",funcb2BodyPosition);
		s->addCommand("pos",funcb2BodyPosition);
		s->addCommand("scaledPosition",funcb2BodyScaledPosition);
		s->addCommand("angle",funcb2BodyAngle);
		s->addCommand("worldCenter",funcb2BodyWorldCenter);
		s->addCommand("localCenter",funcb2BodyLocalCenter);
		s->addCommand("linearVelocity",funcb2BodyLinearVelocity);
		s->addCommand("AngularVelocity",funcb2BodyAngularVelocity);
		s->addCommand("applyForce",funcb2BodyApplyForce);
		s->addCommand("applyForceToCenter",funcb2BodyApplyForceToCenter);
		s->addCommand("applyTorque",funcb2BodyApplyTorque);
		s->addCommand("applyLinearImpulse",funcb2BodyApplyLinearImpulse);
		s->addCommand("applyAngularImpulse",funcb2BodyApplyAngularImpusle);
		s->addCommand("mass",funcb2BodyMass);
		s->addCommand("inertia",funcb2BodyInertia);
		s->addCommand("massData",funcb2BodyMassData);
		s->addCommand("resetMassData",funcb2BodyResetMassData);
		s->addCommand("worldPoint",funcb2BodyWorldPoint);
		s->addCommand("worldVector",funcb2BodyWorldVector);
		s->addCommand("localPoint",funcb2BodyLocalPoint);
		s->addCommand("localVector",funcb2BodyLocalVector);
		s->addCommand("linearVelocityFromWorldPoint",funcb2BodyLinearVelocityFromWorldPoint);
		s->addCommand("linearVelocityFromLocalPoint",funcb2BodyLinearVelocityFromLocalPoint);
		s->addCommand("linearDamping",funcb2BodyLinearDamping);
		s->addCommand("angularDamping",funcb2BodyAngularDamping);
		s->addCommand("gravityScale",funcb2BodyGravityScale);
		s->addCommand("type",funcb2BodyType);
		s->addCommand("bullet",funcb2BodyBullet);
		s->addCommand("sleepingAllowed",funcb2BodySleepingAllowed);
		s->addCommand("awake",funcb2BodyAwake);
		s->addCommand("active",funcb2BodyActive);
		s->addCommand("fixedRotation",funcb2BodyFixedRotation);
		s->addCommand("fixtureList",funcb2BodyFixtureList);
		s->addCommand("jointList",funcb2BodyJointList);
		s->addCommand("contactList",funcb2BodyContactList);
		s->addCommand("next",funcb2BodyNext);
		s->addCommand("userData",funcb2BodyUserData);
		s->addCommand("world",funcb2BodyWorld);
		s->addCommand("testPoint",funcb2BodyTestPoint);
		s->addCommand("dump",funcb2BodyDump);
	s->closeModule();

	s->openModule("b2Joint",NULL,0);
		s->addCommand("type",funcb2JointType);
		s->addCommand("bodyA",funcb2JointBodyA);
		s->addCommand("bodyB",funcb2JointBodyB);
		s->addCommand("anchorA",funcb2JointAnchorA);
		s->addCommand("anchorB",funcb2JointAnchorB);
		s->addCommand("reactionForce",funcb2JointReactionForce);
		s->addCommand("reactionTorque",funcb2JointReactionTorque);
		s->addCommand("next",funcb2JointNext);
		s->addCommand("userData",funcb2JointUserData);
		s->addCommand("active",funcb2JointActive);
		s->addCommand("collideConnected",funcb2JointCollideConnected);
		s->addCommand("dump",funcb2JointDump);
		
		//Distance
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("length",funcb2JointLength);
		s->addCommand("frequency",funcb2JointFrequency);
		s->addCommand("dampingRatio",funcb2JointDampingRatio);
		
		//Friction
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("maxForce",funcb2JointMaxForce);
		s->addCommand("maxTorque",funcb2JointMaxTorque);
		
		//Gear
		s->addCommand("joint1",funcb2JointJoint1);
		s->addCommand("joint2",funcb2JointJoint2);
		s->addCommand("ratio",funcb2JointRatio);
		
		//Prismatic
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("localAxisA",funcb2JointLocalAxisA);
		s->addCommand("referenceAngle",funcb2JointReferenceAngle);
		s->addCommand("jointTranslation",funcb2JointJointTranslation);
		s->addCommand("jointSpeed",funcb2JointJointSpeed);
		s->addCommand("limitEnabled",funcb2JointLimitEnabled);
		s->addCommand("enableLimit",funcb2JointEnableLimit);
		s->addCommand("lowerLimit",funcb2JointLowerLimit);
		s->addCommand("upperLimit",funcb2JointUpperLimit);
		s->addCommand("limits",funcb2JointLimits);
		s->addCommand("enableMotor",funcb2JointEnableMotor);
		s->addCommand("motorSpeed",funcb2JointMotorSpeed);
		s->addCommand("maxMotorTorque",funcb2JointMaxMotorForce);
		s->addCommand("motorForce",funcb2JointMotorForce);
		
		//PulleyJoint
		s->addCommand("groundAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("groundAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("lengthA",funcb2JointLengthA);
		s->addCommand("lengthB",funcb2JointLengthB);
		s->addCommand("ratio",funcb2JointRatio);
	
		//RevoluteJoint
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("referenceAngle",funcb2JointReferenceAngle);
		s->addCommand("jointAngle",funcb2JointJointAngle);
		s->addCommand("jointSpeed",funcb2JointJointSpeed);
		s->addCommand("enableLimit",funcb2JointEnableLimit);
		s->addCommand("lowerLimit",funcb2JointLowerLimit);
		s->addCommand("uppperLimit",funcb2JointUpperLimit);
		s->addCommand("limits",funcb2JointLimits);
		s->addCommand("enableMotor",funcb2JointEnableMotor);
		s->addCommand("motorSpeed",funcb2JointMotorSpeed);
		s->addCommand("maxMotorTorque",funcb2JointMaxMotorTorque);
		s->addCommand("motorTorque",funcb2JointMotorTorque);

		//RopeJoint
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("maxLength",funcb2JointMaxLength);
		s->addCommand("limitState",funcb2JointLimitState);
		
		//WeldJoint
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("referenceAngle",funcb2JointReferenceAngle);
		s->addCommand("frequency",funcb2JointFrequency);
		s->addCommand("dampingRatio",funcb2JointDampingRatio);

		//WheelJoint
		s->addCommand("localAnchorA",funcb2JointLocalAnchorA);
		s->addCommand("localAnchorB",funcb2JointLocalAnchorB);
		s->addCommand("localAxisA",funcb2JointLocalAxisA);
		s->addCommand("jointTranslation",funcb2JointJointTranslation);
		s->addCommand("jointSpeed",funcb2JointJointSpeed);
		s->addCommand("enableMotor",funcb2JointEnableMotor);
		s->addCommand("motorSpeed",funcb2JointMotorSpeed);
		s->addCommand("maxMotorTorque",funcb2JointMaxMotorTorque);
		s->addCommand("motorTorque",funcb2JointMotorTorque);
		s->addCommand("springFrequencyHz",funcb2JointSpringFrequencyHz);
		s->addCommand("springDampingRatio",funcb2JointSpringDampingRatio);
	s->closeModule();

	s->openModule("b2Shape",NULL,0);
		s->addCommand("clone",funcb2ShapeClone);
		s->addCommand("type",funcb2ShapeType);
		s->addCommand("childCount",funcb2ShapeChildCount);
		s->addCommand("testPoint",funcb2ShapeTestPoint);
		s->addCommand("rayCast",funcb2ShapeRayCast);
		s->addCommand("computeAABB",funcb2ShapeComputeAABB);
		s->addCommand("computeMass",funcb2ShapeComputeMass);
		s->addCommand("radius",funcb2ShapeRadius);
	s->closeModule();

	s->openModule("b2Fixture",NULL,0);
		s->addCommand("type",funcb2FixtureType);
		s->addCommand("shape",funcb2FixtureShape);
		s->addCommand("sensor",funcb2FixtureSensor);
		s->addCommand("filterData",funcb2FixtureFilterData);
		s->addCommand("refilter",funcb2FixtureRefilter);
		s->addCommand("body",funcb2FixtureBody);
		s->addCommand("next",funcb2FixtureNext);
		s->addCommand("userData",funcb2FixtureUserData);
		s->addCommand("testPoint",funcb2FixtureTestPoint);
		s->addCommand("rayCast",funcb2FixtureRayCast);
		s->addCommand("massData",funcb2FixtureMassData);
		s->addCommand("density",funcb2FixtureDensity);
		s->addCommand("friction",funcb2FixtureFriction);
		s->addCommand("restitution",funcb2FixtureRestitution);
		s->addCommand("AABB",funcb2FixtureAABB);
		s->addCommand("dump",funcb2FixtureDump);
	s->closeModule();

#if 0
	s->openModule("b2Contact",NULL,0);
		s->addCommand("manifold",funcb2ContactManifold);
		s->addCommand("worldManifold",funcb2ContactWorldManifold);
		s->addCommand("touching",funcb2ContactTouching);
		s->addCommand("enabled",funcb2ContactEnabled);
		s->addCommand("next",funcb2ContactNext);
		s->addCommand("fixtureA",funcb2ContactFixtureA);
		s->addCommand("childIndexA",funcb2ContactChildIndexA);
		s->addCommand("fixtureB",funcb2ContactFixtureB);
		s->addCommand("childIndexB",funcb2ContactChildIndexB);
		s->addCommand("friction",funcb2ContactFriction);
		s->addCommand("resetFriction",funcb2ContactResetFriction);
		s->addCommand("restitution",funcb2ContactRestitution);
		s->addCommand("resetRestitution",funcb2ContactResetRestitution);
		s->addCommand("evaluate",funcb2ContactEvaluate);
	s->closeModule();
#endif
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
