
#include "Entry.h"



	//*****************************//

	ForwardTableEntry::ForwardTableEntry() {}

	ForwardTableEntry::ForwardTableEntry(uint32_t next_hop_ip, uint32_t timestamp, GPS gps) :m_next_hop_ip(next_hop_ip), m_timestamp(timestamp),m_gps(gps) {}

	ForwardTableEntry::~ForwardTableEntry() {}

	uint32_t
	ForwardTableEntry::GetNextHopIP()
	{
		return m_next_hop_ip;
	}

	void
	ForwardTableEntry::SetNextHopIP(uint32_t node_ip)
	{
		m_next_hop_ip = node_ip;
	}

	uint32_t
	ForwardTableEntry::GetTimestamp()
	{
		return m_timestamp;
	}
	void
	ForwardTableEntry::SetTimestamp(uint32_t timestamp)
	{
		m_timestamp = timestamp;
	}

	GPS
	ForwardTableEntry::GetGPS()
	{
		return m_gps;
	}
	void
	ForwardTableEntry::SetGPS(GPS gps)
	{
		m_gps = gps;
	}
	//*****************************//

	NeighborEntry::NeighborEntry(GPS gps, uint32_t timestamp, Speed speed,uint32_t phy_ip) :m_gps(gps), m_timestamp(timestamp), m_speed(speed),m_phy_ip(phy_ip) {}

	NeighborEntry::~NeighborEntry() {}

	uint32_t
		NeighborEntry::GetTimestamp()
	{
		return m_timestamp;
	}

	void
		NeighborEntry::SetTimestamp(uint32_t timestamp)
	{
		m_timestamp = timestamp;
	}

	GPS
		NeighborEntry::GetGPS()
	{
		return m_gps;
	}

	void
		NeighborEntry::SetGPS(GPS gps)
	{
		m_gps = gps;
	}
	Speed
		NeighborEntry::GetSpeed()
	{
		return m_speed;
	}
	void
		NeighborEntry::SetSpeed(Speed speed)
	{
		m_speed = speed;
	}
	uint32_t
		NeighborEntry::GetPhyIP()
	{
		return m_phy_ip;
	}

	void
		NeighborEntry::SetPhyIP(uint32_t phy_ip)
	{
		m_phy_ip = phy_ip;
	}



	//*****************************//


	NounceEntry::NounceEntry(uint32_t timestamp) :m_timestamp(timestamp) {}

	NounceEntry::~NounceEntry() {}

	uint32_t
		NounceEntry::GetTimestamp()
	{
		return m_timestamp;
	}

	void
		NounceEntry::SetTimestamp(uint32_t timestamp)
	{
		m_timestamp = timestamp;
	}


