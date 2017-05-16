#ifndef ENTRY
#define ENTRY

#include <iostream>
#include <stdint.h>
#include <queue>
#include <string>
#include "GPS.h"
#include "Speed.h"

//*****************************//
class ForwardTableEntry
{
public:
	ForwardTableEntry();
	ForwardTableEntry(uint32_t next_hop_ip, uint32_t timestamp,GPS gps);
	~ForwardTableEntry();
	uint32_t GetNextHopIP();
	void SetNextHopIP(uint32_t node_ip);
	uint32_t GetTimestamp();
	void SetTimestamp(uint32_t timestamp);
	GPS GetGPS();
	void SetGPS(GPS gps);
public:
	GPS m_gps;
	uint32_t m_next_hop_ip;  //real_ip
	uint32_t m_timestamp;
};


//*****************************//
class NeighborEntry
{
public:
	NeighborEntry(GPS gps, uint32_t timestamp, Speed speed, uint32_t m_vip);
	~NeighborEntry();
	uint32_t GetTimestamp();
	void SetTimestamp(uint32_t timestamp);

	GPS GetGPS();
	void SetGPS(GPS gps);
	Speed GetSpeed();
	void SetSpeed(Speed speed);

	uint32_t GetPhyIP();
	void SetPhyIP(uint32_t phy_ip);
public:
	GPS m_gps;
	uint32_t m_timestamp;   // need modify
	Speed m_speed;
	uint32_t m_phy_ip;
	std::queue<GPS> gpsq;
};


//*****************************//
class NounceEntry
{
public:
	NounceEntry(uint32_t timestamp);
	~NounceEntry();
	uint32_t GetTimestamp();
	void SetTimestamp(uint32_t timestamp);
public:
	uint32_t m_timestamp;   // need modify
};


#endif // !ENTRY

