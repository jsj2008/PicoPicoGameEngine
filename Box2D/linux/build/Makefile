CC = gcc
CPP = g++
AR = ar

CFLAGS = -O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -ffast-math -fstrict-aliasing -fomit-frame-pointer

BUILD_DIR = objects

LIBNAME = libbox2d.ar

CFLAGS += \
-I../../sources/Box2D_v2.2.1/ \
-I../../sources/Box2D_v2.2.1/Box2D/ \
-I../../sources/Box2D_v2.2.1/Box2D/Collision/ \
-I../../sources/Box2D_v2.2.1/Box2D/Collision/Shapes/ \
-I../../sources/Box2D_v2.2.1/Box2D/Common/ \
-I../../sources/Box2D_v2.2.1/Box2D/Dynamics/ \
-I../../sources/Box2D_v2.2.1/Box2D/Dynamics/Contacts/ \
-I../../sources/Box2D_v2.2.1/Box2D/Dynamics/Joints/ \
-I../../sources/Box2D_v2.2.1/Box2D/Rope/

VPATH = \
../../sources/Box2D_v2.2.1/Box2D/:\
../../sources/Box2D_v2.2.1/Box2D/Collision/:\
../../sources/Box2D_v2.2.1/Box2D/Collision/Shapes/:\
../../sources/Box2D_v2.2.1/Box2D/Common/:\
../../sources/Box2D_v2.2.1/Box2D/Dynamics/:\
../../sources/Box2D_v2.2.1/Box2D/Dynamics/Contacts/:\
../../sources/Box2D_v2.2.1/Box2D/Dynamics/Joints/:\
../../sources/Box2D_v2.2.1/Box2D/Rope/:

OBJS = b2BroadPhase.o \
b2CollideCircle.o \
b2CollideEdge.o \
b2CollidePolygon.o \
b2Collision.o \
b2Distance.o \
b2DynamicTree.o \
b2TimeOfImpact.o \
b2ChainShape.o \
b2CircleShape.o \
b2EdgeShape.o \
b2PolygonShape.o \
b2BlockAllocator.o \
b2Draw.o \
b2Math.o \
b2Settings.o \
b2StackAllocator.o \
b2Timer.o \
b2Body.o \
b2ContactManager.o \
b2Fixture.o \
b2Island.o \
b2World.o \
b2WorldCallbacks.o \
b2ChainAndCircleContact.o \
b2ChainAndPolygonContact.o \
b2CircleContact.o \
b2Contact.o \
b2ContactSolver.o \
b2EdgeAndCircleContact.o \
b2EdgeAndPolygonContact.o \
b2PolygonAndCircleContact.o \
b2PolygonContact.o \
b2DistanceJoint.o \
b2FrictionJoint.o \
b2GearJoint.o \
b2Joint.o \
b2MouseJoint.o \
b2PrismaticJoint.o \
b2PulleyJoint.o \
b2RevoluteJoint.o \
b2RopeJoint.o \
b2WeldJoint.o \
b2WheelJoint.o \
b2Rope.o

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
