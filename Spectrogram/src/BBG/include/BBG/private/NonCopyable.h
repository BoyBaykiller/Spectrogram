#pragma once


#define BBG_NON_COPYABLE(ClassTypeName) ClassTypeName(const ClassTypeName&) = delete; ClassTypeName& operator=(const ClassTypeName&) = delete;