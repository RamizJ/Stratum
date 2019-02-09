#pragma once

#include "Wrapper.h"

class TextConverter
{
public:
	static void Convert_CP1251_To_UTF16(const char* a_In, Ogre::UTFString& a_Out);
	static void Convert_UTF16_To_CP1251(const Ogre::UTFString& a_In, char* a_Out);
};