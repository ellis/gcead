#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <IdacEs/IdacExports.h>

#include <iostream>
using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn1_clicked()
{
	cout << "IdacLibVersion(): " << IdacLibVersion() << endl;
	cout << "IdacPresent(-1): " << IdacPresent(-1) << endl;
	/*
	DRIVER_EXPORT	DWORD			WINAPI	IdacDataAvail				();
	DRIVER_EXPORT	BOOL			WINAPI	IdacEnableChannel			(DWORD Chan, BOOL bEnable);
	DRIVER_EXPORT	DWORD			WINAPI	IdacGetChannelDecimation	(DWORD Chan);
	DRIVER_EXPORT	DWORD			WINAPI	IdacGetDSPInfo				(PWORD pBuffer, WORD wSignalCount, DWORD dwBufCount);
	DRIVER_EXPORT	LPDWORD			WINAPI	IdacGetRanges				();
	DRIVER_EXPORT	BOOL			WINAPI	IdacHighPass				(DWORD Chan, DWORD Index);
	DRIVER_EXPORT	void			WINAPI	IdacHighPassStrings			(int *NrOfStrings, const char **ppFilterStrings[]);
	DRIVER_EXPORT	BOOL			WINAPI	IdacIsChannelEnabled		(DWORD Chan);
	DRIVER_EXPORT	DWORD			WINAPI	IdacLibVersion				();
	DRIVER_EXPORT	LPCSTR			WINAPI	IdacLock					(LPCSTR UserName);
	DRIVER_EXPORT	BOOL			WINAPI	IdacLowPass					(DWORD Chan, DWORD Index);
	DRIVER_EXPORT	void			WINAPI	IdacLowPassStrings			(int *NrOfStrings, const char **ppFilterStrings[]);
	DRIVER_EXPORT	UINT			WINAPI	IdacNrOfAnChannelEnabled	();
	DRIVER_EXPORT	void			WINAPI	IdacPowerDown				();
	DRIVER_EXPORT	BOOL			WINAPI	IdacScaleRange				(DWORD Chan, DWORD Index);
	DRIVER_EXPORT	void			WINAPI	IdacSetBufferEvent			(HANDLE hEvent);
	DRIVER_EXPORT	BOOL			WINAPI	IdacSetDecimation			(DWORD Chan, DWORD dwDecimation);
	DRIVER_EXPORT	BOOL			WINAPI	IdacSetOffsetAnalogIn		(DWORD Chan, DWORD Offset);
	DRIVER_EXPORT	BOOL			WINAPI	IdacSmpStart				();
	DRIVER_EXPORT	BOOL			WINAPI	IdacSmpStop					();
	DRIVER_EXPORT	void			WINAPI	IdacUnlock					();

	DRIVER_EXPORT	LPCDD32_SAMPLE	WINAPI	IdacLockReadBuffer			(DWORD* pdwCount);
	DRIVER_EXPORT	void			WINAPI	IdacUnlockReadBuffer		();
	DRIVER_EXPORT	DWORD			WINAPI	IdacType					();
	DRIVER_EXPORT	DWORD			WINAPI	IdacBoot					(LPCSTR FileName, LONG Address);
	*/
}

void MainWindow::on_btn2_clicked()
{
	if (!IdacPresent(-1))
		return;
	cout << "IdacType(): " << IdacType() << endl;
	cout << "IdacBoot(): " << IdacBoot("", 0) << endl;
}
