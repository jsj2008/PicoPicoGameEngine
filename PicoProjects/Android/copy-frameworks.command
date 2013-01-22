#!/bin/bash
cd `dirname $0`

cp ../../FullBuild/Android/libs/armeabi/* ./jni/armeabi/
cp ../../FullBuild/Android/libs/armeabi-v7a/* ./jni/armeabi-v7a/
cp ../../FullBuild/Android/libs/x86/* ./jni/x86/
