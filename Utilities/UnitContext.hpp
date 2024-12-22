#pragma once

enum UnitStatus
{
	DISCONNECTED = 0,
	IDLE = 1,
	MOVING = 2,
	UNSPECIFIED = 3
};

struct IpAddress
{
	std::string ip{ "" };
	short port{};
};

struct Position
{
	float x{ 0.0 };
	float y{ 0.0 };
};

struct DroneContext {
	unsigned short id{ 0 };
	UnitStatus status{ UnitStatus::DISCONNECTED };
	Position position{ 0.f, 0.f };
	float orientation{ 0.f };
};