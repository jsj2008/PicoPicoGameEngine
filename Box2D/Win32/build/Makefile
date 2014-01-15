CC = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-gcc
AR = /Developer/Cocotron/1.0/Windows/i386/gcc-4.3.1/bin/i386-mingw32msvc-ar

CFLAGS = -DFT2_BUILD_LIBRARY -DFT_CONFIG_MODULES_H="<ftmodule.h>" -D_WIN32 -DIGNORE_EXPORT -DGLEW_NO_GLU -D__MINGW32__ -DGLEW_STATIC -DREGEX_STATIC -DLUA_COMPAT_ALL -DCC_UNDER_WIN32 -DGL_GLEXT_PROTOTYPES=1 -DWIN32 -D_WINDOWS -DNDEBUG \
-O3 -DWINDOWS

BUILD_DIR = objects
SRC_PATH=../../sources/Box2D_v2.3.0/Box2D

LIBNAME = libbox2d.ar

CFLAGS += \
-I$(SRC_PATH)/ \
-I$(SRC_PATH)/Box2D/ \
-I$(SRC_PATH)/Box2D/Collision/ \
-I$(SRC_PATH)/Box2D/Collision/Shapes/ \
-I$(SRC_PATH)/Box2D/Common/ \
-I$(SRC_PATH)/Box2D/Dynamics/ \
-I$(SRC_PATH)/Box2D/Dynamics/Contacts/ \
-I$(SRC_PATH)/Box2D/Dynamics/Joints/ \
-I$(SRC_PATH)/Box2D/Rope/

VPATH = \
$(SRC_PATH)/Box2D/:\
$(SRC_PATH)/Box2D/Collision/:\
$(SRC_PATH)/Box2D/Collision/Shapes/:\
$(SRC_PATH)/Box2D/Common/:\
$(SRC_PATH)/Box2D/Dynamics/:\
$(SRC_PATH)/Box2D/Dynamics/Contacts/:\
$(SRC_PATH)/Box2D/Dynamics/Joints/:\
$(SRC_PATH)/Box2D/Rope/:

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
b2MotorJoint.o \
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
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBNAME): setup $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(AR) r $@ $(addprefix $(BUILD_DIR)/,$(OBJS))

setup:
	mkdir -p $(BUILD_DIR)
	rm -f $(LINKFILE)

clean:
	rm -f *.exe *.ar
	rm -fr $(BUILD_DIR)
