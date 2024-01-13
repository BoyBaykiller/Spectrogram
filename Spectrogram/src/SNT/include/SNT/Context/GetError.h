#pragma once
#include <SNT/SNT.h>

namespace SNT::Context
{
	ErrorCode PopErrorStack()
	{
		return (ErrorCode)alGetError();
	}
}