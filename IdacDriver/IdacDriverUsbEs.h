#ifndef IDACDRIVERUSBES_H
#define IDACDRIVERUSBES_H

#include "IdacDriverUsb.h"


class IdacDriverUsbEs : public IdacDriverUsb
{
    Q_OBJECT
public:
    explicit IdacDriverUsbEs(QObject *parent = 0);

public:
	virtual bool IdacZero(int iChar, bool bActivate) = 0;
	virtual bool IdacSetAdcZero(int iChan, int nOffset) = 0;
	virtual bool IdacSetInputZero(int iChan, int nOffset) = 0;
	virtual bool IdacNetfrequency(int iChan, bool bActivate) = 0;
	virtual bool IdacTuneNotch(int iChan, int nOffset) = 0;
	virtual bool ResetFpgaTestDll() = 0;
	virtual bool GetTransferData() = 0;
	virtual bool IdacConfigRead() = 0;
	virtual bool IdacConfigWrite() = 0;

	virtual int IdacLibVersion() = 0;
	virtual int IdacType() = 0;

	const IdacCaps* IdacCapabilities() { return caps(); }
	virtual QString IdacLock(const QString& sUserName) = 0;
	virtual void IdacUnlock() = 0;
	virtual void IdacPowerDown() = 0;

	virtual int IdacPresent(int nAddress) = 0;
	virtual int IdacBoot(const QString& sFilename, int nAddress) = 0;
	virtual bool IdacHasBooted() = 0;
	virtual int IdacGetDSPFirmwareVersion() = 0;
	virtual bool IdacProgramFPGA(const QString& sFilename) = 0;
	virtual bool IdacProgramFirmware(const QString& sFilename) = 0;
	virtual bool IdacProgramDSP(const QString& sFilename) = 0;
	virtual bool IdacGetUsbConfigDescriptor() = 0;
	virtual bool Idac() = 0;
	virtual bool Idac() = 0;
	virtual bool Idac() = 0;
	virtual bool Idac() = 0;
	virtual bool Idac() = 0;
};

#endif // IDACDRIVERUSBES_H
