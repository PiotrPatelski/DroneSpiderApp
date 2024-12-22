#pragma once
#include <vector>

class UserIdProvider
{
public:
	UserIdProvider();
	unsigned short generateNextUserId();
	void freeUserId(unsigned short userId);
private:
	void incrementUserId();

	unsigned short nextUserId;
	std::vector<unsigned short> idsInUse;
};