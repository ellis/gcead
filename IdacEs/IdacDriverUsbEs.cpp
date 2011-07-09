#include "IdacDriverUsbEs.h"

#include <Idac/IdacFactory.h>

IdacDriverUsbEs::IdacDriverUsbEs()
{
	m_manager = IdacFactory::getDriverManager(true);

}
