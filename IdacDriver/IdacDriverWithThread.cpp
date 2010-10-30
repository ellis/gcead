#include "IdacDriverWithThread.h"

#include <Check.h>

#include "IdacDriverSamplingThread.h"
#include "Sleeper.h"


const int g_nSampleMax = 600;


IdacDriverWithThread::IdacDriverWithThread(QObject* parent)
	: IdacDriver(parent)
{
	m_bSampling = false;
	m_iSampleRead = 0;
	m_iSampleWrite = 0;
	m_nSamplesInBuffer = 0;
	m_recordThread = NULL;

	m_samplesDigital = new short[g_nSampleMax];
	m_samplesAnalog1 = new short[g_nSampleMax];
	m_samplesAnalog2 = new short[g_nSampleMax];
}

IdacDriverWithThread::~IdacDriverWithThread() {
	delete[] m_samplesDigital;
	delete[] m_samplesAnalog1;
	delete[] m_samplesAnalog2;
}

void IdacDriverWithThread::startSamplingThread() {
	CHECK_PRECOND_RET(m_recordThread == NULL);

	m_bSampling = true;
	m_iSampleRead = 0;
	m_iSampleWrite = 0;
	m_nSamplesInBuffer = 0;
	m_recordThread = new IdacDriverSamplingThread(this);
	m_recordThread->start(QThread::TimeCriticalPriority);
}

void IdacDriverWithThread::stopSampling()
{
	stopSamplingThread();
}

void IdacDriverWithThread::stopSamplingThread() {
	CHECK_PRECOND_RET(m_recordThread != NULL);

	m_bSampling = false;
	if (m_recordThread->wait(5000))
	{
		delete m_recordThread;
		m_recordThread = NULL;
	}
}

void IdacDriverWithThread::msleep(unsigned long msecs) {
	Sleeper::msleep(msecs);
}

bool IdacDriverWithThread::addSample(short digital, short analog1, short analog2) {
	if (m_nSamplesInBuffer < g_nSampleMax - 1) {
		m_sampleMutex.lock();
		m_samplesDigital[m_iSampleWrite] = digital;
		m_samplesAnalog1[m_iSampleWrite] = analog1;
		m_samplesAnalog2[m_iSampleWrite] = analog2;
		m_iSampleWrite++;
		if (m_iSampleWrite == g_nSampleMax)
			m_iSampleWrite = 0;
		m_nSamplesInBuffer++;
		m_sampleMutex.unlock();
		return true;
	}
	return false;
}

int IdacDriverWithThread::takeData(short* digital, short* analog1, short* analog2, int maxSize)
{
	int size = 0;
	while (size < maxSize && m_nSamplesInBuffer > 0)
	{
		size++;
		*digital++ = m_samplesDigital[m_iSampleRead];
		*analog1++ = m_samplesAnalog1[m_iSampleRead];
		*analog2++ = m_samplesAnalog2[m_iSampleRead];
		m_iSampleRead++;
		if (m_iSampleRead == g_nSampleMax)
			m_iSampleRead = 0;

		m_sampleMutex.lock();
		m_nSamplesInBuffer--;
		m_sampleMutex.unlock();
	}
	return size;
}
