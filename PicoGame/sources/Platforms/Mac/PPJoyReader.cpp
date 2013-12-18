/*-----------------------------------------------------------------------------------------------
	名前	PPJoyReader.cpp
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sysexits.h>
#include <IOKit/hid/IOHIDLib.h>
#include "PPJoyReader.h"

IOHIDManagerRef PPJoyReader::hidManager=NULL;
CFMutableArrayRef PPJoyReader::devArray=NULL;

static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
	CFTypeRef ref;
	int32_t value;
	
	ref = IOHIDDeviceGetProperty(device, key);
	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
			return value;
		}
	}
	return 0;
}

/*
static int get_string_property_utf8(IOHIDDeviceRef device, CFStringRef prop, char *buf, size_t len)
{
	CFStringRef str;
	if (!len)
		return 0;

	str = (CFStringRef)IOHIDDeviceGetProperty(device, prop);

	buf[0] = 0;

	if (str) {
		len--;

		CFIndex str_len = CFStringGetLength(str);
		CFRange range;
		range.location = 0;
		range.length = (str_len > len)? len: str_len;
		CFIndex used_buf_len;
		CFIndex chars_copied;
		chars_copied = CFStringGetBytes(str,
			range,
			kCFStringEncodingUTF8,
			(char)'?',
			FALSE,
			(UInt8*)buf,
			len,
			&used_buf_len);

		buf[chars_copied] = 0;
		return (int)used_buf_len;
	}
	else
		return 0;
}
*/

PPJoyReader::PPJoyReader()
{
	hidDev=NULL;
}

int PPJoyReader::SetUpInterface(int joyId,IOHIDDeviceRef hidDev)
{
	this->joyId=joyId;

	for (int i=0;i<PPJoyReaderMaxNumAxis;i++) {
		axis[i].exist = false;
	}
	for (int i=0;i<PPJoyReaderMaxNumButton;i++) {
		button[i].exist = false;
	}
	for (int i=0;i<PPJoyReaderMaxNumHatSwitch;i++) {
		hatSwitch[i].exist = false;
	}
	
	if(hidDev!=NULL)
	{
		CFArrayRef elemAry=IOHIDDeviceCopyMatchingElements(hidDev,NULL,0);
		int nElem=(int)CFArrayGetCount(elemAry);
		int isMouse=0,isJoystick=0,isKeyboard=0,isGamePad=0;

		//--printf("This HID Device has %d elements.\n",nElem);

		int j;
		for(j=0; j<nElem; j++)
		{
			IOHIDElementRef elem=(IOHIDElementRef)CFArrayGetValueAtIndex(elemAry,j);
			IOHIDElementType elemType=IOHIDElementGetType(elem);
			unsigned int usage=IOHIDElementGetUsage(elem);
			unsigned int usagePage=IOHIDElementGetUsagePage(elem);

			//--printf("Element %3d",j);
			switch(elemType)
			{
			case kIOHIDElementTypeInput_ScanCodes:
				//--printf(" ScanCode  ");
				break;
			case kIOHIDElementTypeInput_Misc:
				//--printf(" Misc      ");
				break;
			case kIOHIDElementTypeInput_Button:
				//--printf(" Button    ");
				break;
			case kIOHIDElementTypeInput_Axis:
				//--printf(" Axis      ");
				break;
			case kIOHIDElementTypeOutput:
				//--printf(" Output    ");
				break;
			case kIOHIDElementTypeFeature:
				//--printf(" Feature   ");
				break;
			case kIOHIDElementTypeCollection:
				//--printf(" Collection");
				break;
			}

			//--printf("  Usage %3d  UsagePage %3d\n",usage,usagePage);

			if(kHIDPage_GenericDesktop==usagePage)
			{
				switch(usage)
				{
				case kHIDUsage_GD_Mouse:
					//--printf("    Can function as mouse\n");
					isMouse=1;
					break;
				case kHIDUsage_GD_Keyboard:
					//--printf("    Can function as Keyboard\n");
					isKeyboard=1;
					break;
				case kHIDUsage_GD_Joystick:
					//--printf("    Can function as Joystick\n");
					isJoystick=1;
					break;
				case kHIDUsage_GD_GamePad:
					//--printf("    Can function as GamePad\n");
					isGamePad=1;
					break;
				}
			}
		}

		if(0!=isJoystick)
		{
			int nAxis=0;
			int nHat=0;

			//char buf[256];
			//get_string_property_utf8(hidDev, CFSTR(kIOHIDManufacturerKey), buf, sizeof(buf));
			//printf("kIOHIDManufacturerKey %s\n",buf);
			//get_string_property_utf8(hidDev, CFSTR(kIOHIDProductKey), buf, sizeof(buf));
			//printf("kIOHIDProductKey %s\n",buf);
			//unsigned short productID = get_int_property(hidDev, CFSTR(kIOHIDProductIDKey));
			//printf("kIOHIDProductIDKey 0x%x\n",(int)productID);
			//unsigned short vendorID = get_int_property(hidDev, CFSTR(kIOHIDVendorIDKey));
			//printf("kIOHIDVendorIDKey 0x%x\n",(int)vendorID);

			productID = get_int_property(hidDev, CFSTR(kIOHIDProductIDKey));
			vendorID = get_int_property(hidDev, CFSTR(kIOHIDVendorIDKey));

			int j;
			for(j=0; j<nElem; j++)
			{
				IOHIDElementRef elem=(IOHIDElementRef)CFArrayGetValueAtIndex(elemAry,j);
				IOHIDElementType elemType=IOHIDElementGetType(elem);
				unsigned int usage=IOHIDElementGetUsage(elem);
				unsigned int usagePage=IOHIDElementGetUsagePage(elem);
				// The following two returned 0 and 255
				// IOHIDElementGetPhysicalMin(elem);
				// IOHIDElementGetPhysicalMax(elem);
				int min=(int)IOHIDElementGetLogicalMin(elem);
				int max=(int)IOHIDElementGetLogicalMax(elem);
				int scaledMin=min;
				int scaledMax=max;

				if(elemType==kIOHIDElementTypeInput_Misc ||
				   elemType==kIOHIDElementTypeInput_Button ||
				   elemType==kIOHIDElementTypeInput_Axis ||
				   elemType==kIOHIDElementTypeInput_ScanCodes)
				{
					switch(usagePage)
					{
					case kHIDPage_GenericDesktop:
						switch(usage)
						{
						case kHIDUsage_GD_Mouse:
							break;
						case kHIDUsage_GD_Keyboard:
							break;
						case kHIDUsage_GD_Joystick:
							break;
						case kHIDUsage_GD_GamePad:
							break;
						case kHIDUsage_GD_X:
							//--printf("    This element is for X-Axis (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Y:
							//--printf("    This element is for Y-Axis (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Z:
							//--printf("    This element is for Z-Axis (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Rx:
							//--printf("    This element is for Rx-Axis (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Ry:
							//--printf("    This element is for Ry-Axis (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Rz:
							//--printf("    This element is for Rz-Axis (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Slider:
							//--printf("    This element is for Slider (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							AddAxis(nAxis++,elem,min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Wheel:
							//--printf("    This element is for Wheel (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							break;
						case kHIDUsage_GD_Hatswitch:
							//--printf("    This element is for Hatswitch (%d->%d) Scaled(%d->%d)\n",min,max,scaledMin,scaledMax);
							if(nHat<PPJoyReaderMaxNumHatSwitch)
							{
								hatSwitch[nHat].exist=1;
								hatSwitch[nHat].elem=elem;
								CFRetain(elem);
								nHat++;
							}
							break;
						}
						break;
					case kHIDPage_Button:
						//--printf("    This element is for Button %d\n",usage-1);
						usage--;
						if(usage<PPJoyReaderMaxNumButton)
						{
							button[usage].exist=1;
							button[usage].elem=elem;
							CFRetain(elem);
						}
						break;
					}
				}
			}
			CFRelease(elemAry);
			this->hidDev=hidDev;
			return 1;

		}
		CFRelease(elemAry);
	}

	return 0;
}

void PPJoyReader::Read(void)
{
	int i;
	IOHIDValueRef valueRef;
	for(i=0; i<PPJoyReaderMaxNumAxis; i++)
	{
		if(axis[i].exist!=0)
		{
			axis[i].value = 0;
			if (IOHIDDeviceGetValue(hidDev,axis[i].elem,&valueRef) == kIOReturnSuccess) {
				axis[i].value=(int)IOHIDValueGetIntegerValue(valueRef);
			}
		}
	}
	for(i=0; i<PPJoyReaderMaxNumButton; i++)
	{
		if(button[i].exist!=0)
		{
			button[i].value = 0;
			if (IOHIDDeviceGetValue(hidDev,button[i].elem,&valueRef) == kIOReturnSuccess) {
				button[i].value=(int)IOHIDValueGetIntegerValue(valueRef);
			}
		}
	}
	for(i=0; i<PPJoyReaderMaxNumHatSwitch; i++)
	{
		if(hatSwitch[i].exist!=0)
		{
			hatSwitch[i].value = 0;
			if (IOHIDDeviceGetValue(hidDev,hatSwitch[i].elem,&valueRef) == kIOReturnSuccess) {

				double scaled=IOHIDValueGetScaledValue(valueRef,kIOHIDValueScaleTypePhysical);
				if(scaled<-0.001 || 359.999<scaled)
				{
					hatSwitch[i].value=0;
				}
				else
				{
					hatSwitch[i].value=1+(int)((scaled+22.5)/45.0);
				}

			}
		}
	}
}

//void PPJoyReader::ReleaseInterface(void)
//{
//	if(hidDev!=NULL)
//	{
//		// Honestly, I don't know what to do.
//		//
//		// Should I do
//		//   CFRelease(hidDev);
//		// ?
//		//
//		// This hidDev was copied from a copy of IOHIDManager's device list.
//		// Who owns it?  Why did I have to make a copy?
//		// 
//		// The Creare Rule implies that I have the ownership.
//		// http://developer.apple.com/mac/library/documentation/CoreFoundation/Conceptual/CFMemoryMgmt/Concepts/Ownership.html#//apple_ref/doc/uid/20001148-SW1
//		//
//		// Then, I suppose I should release it.  Am I right?
//		//CFRelease(hidDev);
//		hidDev=NULL;
//	}
//}

void PPJoyReader::AddAxis(int axisId,IOHIDElementRef elem,int min,int max,int scaledMin,int scaledMax)
{
	if(0<=axisId && axisId<PPJoyReaderMaxNumAxis)
	{
		axis[axisId].exist=1;
		axis[axisId].elem=elem;
		axis[axisId].min=min;
		axis[axisId].max=max;
//		axis[axisId].scaledMin=scaledMin;
//		axis[axisId].scaledMax=scaledMax;

		axis[axisId].calibCenter=(min+max)/2;
		axis[axisId].calibMin=min;
		axis[axisId].calibMax=max;

		CFRetain(elem);
	}
}

static void CFSetCopyCallBack(const void *value,void *context);
void CFSetCopyCallBack(const void *value,void *context)
{
	CFArrayAppendValue((CFMutableArrayRef)context,value);
}

int PPJoyReader::SetUpJoystick(int &nJoystick,PPJoyReader joystick[],int maxNumJoystick)
{
	nJoystick=0;

	if(NULL==hidManager)
	{
		hidManager=IOHIDManagerCreate(kCFAllocatorDefault,kIOHIDOptionsTypeNone);
	}

	if(NULL!=hidManager)
	{
		IOHIDManagerSetDeviceMatching(hidManager,NULL);  // Just enumrate all devices
		IOHIDManagerOpen(hidManager,kIOHIDOptionsTypeNone);

		CFSetRef copyOfDevices=IOHIDManagerCopyDevices(hidManager);
		if(NULL!=devArray)
		{
			CFRelease(devArray);
			devArray=NULL;
		}
		devArray=CFArrayCreateMutable(kCFAllocatorDefault,0,&kCFTypeArrayCallBacks);
		CFSetApplyFunction(copyOfDevices,CFSetCopyCallBack,(void *)devArray);

		CFIndex nDev=CFArrayGetCount(devArray);

		//--printf("%d devices found\n",(int)nDev);

		CFRelease(copyOfDevices);

		int i;
		for(i=0; i<nDev && nJoystick<maxNumJoystick; i++)
		{
			IOHIDDeviceRef hidDev=(IOHIDDeviceRef)CFArrayGetValueAtIndex(devArray,i);
			if(joystick[nJoystick].SetUpInterface(nJoystick,hidDev)!=0)
			{
				nJoystick++;
				// CFRelease(hidDev);  // Doesn't it destroy integrity of devArray?
			}
		}
	}

	return nJoystick;
}

int PPJoyReaderSetUpJoystick(int &nJoystick,PPJoyReader joystick[],int maxNumJoystick)
{
	return PPJoyReader::SetUpJoystick(nJoystick,joystick,maxNumJoystick);
}

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
