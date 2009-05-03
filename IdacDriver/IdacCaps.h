#ifndef __IDACCAPS_H
#define __IDACCAPS_H

/// IDAC capabilities
class IdacCaps
{
public:
	/// Has highcut filter
	bool bHighcut;
	/// Range can be adjusted individuall per channel
	bool bRangePerChannel;
};

#endif
