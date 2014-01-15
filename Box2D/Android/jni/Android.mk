LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libbox2d
B2_SOURCE_PATH := ../../sources/Box2D_v2.3.0/Box2D/Box2D

BOX2D_MODULE := 1

JNI_CFLAGS := \
				-D_STLP_NO_EXCEPTIONS \
				-D_STLP_USE_SIMPLE_NODE_ALLOC

LOCAL_CFLAGS := $(JNI_CFLAGS)

LOCAL_SRC_FILES := \
					$(B2_SOURCE_PATH)/Collision/Shapes/b2ChainShape.cpp \
					$(B2_SOURCE_PATH)/Collision/Shapes/b2CircleShape.cpp \
					$(B2_SOURCE_PATH)/Collision/Shapes/b2EdgeShape.cpp \
					$(B2_SOURCE_PATH)/Collision/Shapes/b2PolygonShape.cpp \
					$(B2_SOURCE_PATH)/Collision/b2BroadPhase.cpp \
					$(B2_SOURCE_PATH)/Collision/b2CollideCircle.cpp \
					$(B2_SOURCE_PATH)/Collision/b2CollideEdge.cpp \
					$(B2_SOURCE_PATH)/Collision/b2CollidePolygon.cpp \
					$(B2_SOURCE_PATH)/Collision/b2Collision.cpp \
					$(B2_SOURCE_PATH)/Collision/b2Distance.cpp \
					$(B2_SOURCE_PATH)/Collision/b2DynamicTree.cpp \
					$(B2_SOURCE_PATH)/Collision/b2TimeOfImpact.cpp \
					$(B2_SOURCE_PATH)/Common/b2BlockAllocator.cpp \
					$(B2_SOURCE_PATH)/Common/b2Draw.cpp \
					$(B2_SOURCE_PATH)/Common/b2Math.cpp \
					$(B2_SOURCE_PATH)/Common/b2Settings.cpp \
					$(B2_SOURCE_PATH)/Common/b2StackAllocator.cpp \
					$(B2_SOURCE_PATH)/Common/b2Timer.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2CircleContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2Contact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2ContactSolver.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Contacts/b2PolygonContact.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2DistanceJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2FrictionJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2GearJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2MotorJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2Joint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2MouseJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2PrismaticJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2PulleyJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2RevoluteJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2RopeJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2WeldJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/Joints/b2WheelJoint.cpp \
					$(B2_SOURCE_PATH)/Dynamics/b2Body.cpp \
					$(B2_SOURCE_PATH)/Dynamics/b2ContactManager.cpp \
					$(B2_SOURCE_PATH)/Dynamics/b2Fixture.cpp \
					$(B2_SOURCE_PATH)/Dynamics/b2Island.cpp \
					$(B2_SOURCE_PATH)/Dynamics/b2World.cpp \
					$(B2_SOURCE_PATH)/Dynamics/b2WorldCallbacks.cpp \
					$(B2_SOURCE_PATH)/Rope/b2Rope.cpp

LOCAL_C_INCLUDES := \
          $(LOCAL_PATH)/$(B2_SOURCE_PATH)/../ \
					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Collision/ \
					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Collision/Shapes/ \
					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Common/ \
					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Dynamics/ \
					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Dynamics/Contacts/ \
					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Dynamics/Joints/ \
 					$(LOCAL_PATH)/$(B2_SOURCE_PATH)/Rope/ \
          $(LOCAL_PATH)/

LOCAL_CFLAGS += -O3
                           
include $(BUILD_STATIC_LIBRARY)
