LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := box2d
SOURCE_PATH := ../../sources/Box2D_v2.2.1/Box2D

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
					$(SOURCE_PATH)/Collision/Shapes/b2ChainShape.cpp \
					$(SOURCE_PATH)/Collision/Shapes/b2CircleShape.cpp \
					$(SOURCE_PATH)/Collision/Shapes/b2EdgeShape.cpp \
					$(SOURCE_PATH)/Collision/Shapes/b2PolygonShape.cpp \
					$(SOURCE_PATH)/Collision/b2BroadPhase.cpp \
					$(SOURCE_PATH)/Collision/b2CollideCircle.cpp \
					$(SOURCE_PATH)/Collision/b2CollideEdge.cpp \
					$(SOURCE_PATH)/Collision/b2CollidePolygon.cpp \
					$(SOURCE_PATH)/Collision/b2Collision.cpp \
					$(SOURCE_PATH)/Collision/b2Distance.cpp \
					$(SOURCE_PATH)/Collision/b2DynamicTree.cpp \
					$(SOURCE_PATH)/Collision/b2TimeOfImpact.cpp \
					$(SOURCE_PATH)/Common/b2BlockAllocator.cpp \
					$(SOURCE_PATH)/Common/b2Draw.cpp \
					$(SOURCE_PATH)/Common/b2Math.cpp \
					$(SOURCE_PATH)/Common/b2Settings.cpp \
					$(SOURCE_PATH)/Common/b2StackAllocator.cpp \
					$(SOURCE_PATH)/Common/b2Timer.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2CircleContact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2Contact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2ContactSolver.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
					$(SOURCE_PATH)/Dynamics/Contacts/b2PolygonContact.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2DistanceJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2FrictionJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2GearJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2Joint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2MouseJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2PrismaticJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2PulleyJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2RevoluteJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2RopeJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2WeldJoint.cpp \
					$(SOURCE_PATH)/Dynamics/Joints/b2WheelJoint.cpp \
					$(SOURCE_PATH)/Dynamics/b2Body.cpp \
					$(SOURCE_PATH)/Dynamics/b2ContactManager.cpp \
					$(SOURCE_PATH)/Dynamics/b2Fixture.cpp \
					$(SOURCE_PATH)/Dynamics/b2Island.cpp \
					$(SOURCE_PATH)/Dynamics/b2World.cpp \
					$(SOURCE_PATH)/Dynamics/b2WorldCallbacks.cpp \
					$(SOURCE_PATH)/Rope/b2Rope.cpp

LOCAL_C_INCLUDES := \
                    $(LOCAL_PATH)/$(SOURCE_PATH)/../ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/Collision/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/Collision/Shapes/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/Common/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/Dynamics/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/Dynamics/Contacts/ \
					$(LOCAL_PATH)/$(SOURCE_PATH)/Dynamics/Joints/ \
 					$(LOCAL_PATH)/$(SOURCE_PATH)/Rope/ \
                    $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3

# it is used for ndk-r5  
# if you build with ndk-r4, comment it  
# because the new Windows toolchain doesn't support Cygwin's drive
# mapping (i.e /cygdrive/c/ instead of C:/)  
                           
include $(BUILD_SHARED_LIBRARY)
