#pragma once
#include "Math.h"
#include "People.h"
#include <vector>

enum class BuildingType { RESIDENTIAL, INDUSTRY, RECREATIONAL };

class Building {
	unsigned int maxCapacity;
	unsigned int numInfections;
	BuildingType btype;

	unsigned int numGroups;
	std::vector<People*>* occupants;
public:
	/* ================ Constructors ================ */
	//O(1)					Constructor
	Building(BuildingType _type, int capacity);
	//O(1)					Destructor
	~Building();
	
	/* ================ Accessors ================ */
	//O(1)					Returns the building's max capacity
	unsigned int GetMaxCapacity() const;
	//O(1)					Returns the building's number of groups
	unsigned int GetNumberOfGroups() const;
	//O(1)					Returns how many people have been infected in the building
	unsigned int GetTotalInfections() const;
	//O(1)					Returns the building type
	BuildingType GetBuildingType() const;
	//O(1)					Returns how many occupants are in the given group.
	unsigned int GetNumOccupants(unsigned int group) const;
	//O(maxCapacity)		Returns how many people who are currently infected are in the given group
	unsigned int GetNumInfected(unsigned int group) const;

	/* ================ Mutators ================ */
	//O(1)					Adds input p to the given group of occupants
	void Populate(People* p, unsigned int group);
	//O(maxCapacity)		Infects random people in the given group.
	void Infect(float rate, unsigned int group);
	//O(group * maxCapacity)Empties the building's groups.
	void Clear();
	//O(1)					Resets numInfections counter
	void ResetStats();
};