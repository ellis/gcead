#ifndef __IDACDRIVERUSB24BASE_H
#define __IDACDRIVERUSB24BASE_H


#include <QtGlobal> // for quint8 and related types

#include <IdacDriver/IdacDriverUsb.h>
#include <IdacDriver/IdacSettings.h>


class IdacUsb;


class IdacDriverUsb24Base : public IdacDriverUsb
{
public:
	IdacDriverUsb24Base();
};

#endif
