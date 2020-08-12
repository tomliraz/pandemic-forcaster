#include "Building.h"

/* ======================================================== Constructors =============================================*/
Building::Building(BuildingType _type, int capacity) : btype(_type), maxCapacity(capacity), numInfections(0)
{
	if (btype == BuildingType::RESIDENTIAL)
		numGroups = 1;
	else if (btype == BuildingType::INDUSTRY)
		numGroups = 2;
	else
		numGroups = 4;
	occupants = new std::vector<People*>[numGroups];

	for (unsigned int i = 0; i < numGroups; i++)
	{
		occupants[i] = std::vector<People*>();
	}
}
Building::~Building()
{
	delete[] occupants;
}

/* ======================================================== Accessors ================================================*/
unsigned int Building::GetMaxCapacity() const { return maxCapacity; }
unsigned int Building::GetNumberOfGroups() const { return numGroups; }
unsigned int Building::GetTotalInfections() const { return numInfections; }
BuildingType Building::GetBuildingType() const { return btype; }
unsigned int Building::GetNumOccupants(unsigned int group) const { return occupants[group].size(); }
unsigned int Building::GetNumInfected(unsigned int group) const
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < occupants[group].size(); i++)
		if (occupants[group][i]->GetStatus() == InfectionStatus::INFECTED)
			count++;
	return count;
}

/* ======================================================== Mutators =================================================*/
void Building::Populate(People* p, unsigned int group)
{
	occupants[group].push_back(p);
}

void Building::Infect(float ratePerPerson, unsigned int group)
{
	if (GetNumInfected(group) == 0)
		return;
	
	unsigned int numInfected= GetNumInfected(group);

	for (unsigned int i = 0; i < occupants[group].size(); i++)
	{
		if (occupants[group][i]->GetStatus() == InfectionStatus::HEALTHY) // Only healthy people can get infected.
		{
			float multiplier = 0.9f * numInfected; // Scale transmission chance with number of infected people in the building.
			if (occupants[group][i]->GetBMI() > 30 || occupants[group][i]->GetBMI() < 18.5) // If person has significant weight problems double transmission chance. 
			{
				multiplier *= 2;
			}

			if (std::abs(occupants[group][i]->GetAge() - 38) / 12.0f > 2) // If person is very young or old, scale transmission chance with deviation from the mean.
			{
				multiplier *= std::abs(occupants[group][i]->GetAge() - 38) / 12.0f;
			}
			if (Math::Random() < ratePerPerson * multiplier)
			{
				numInfections++;
				occupants[group][i]->Infect();
			}
		}
	}
}

void Building::Clear()
{
	for (unsigned int i = 0; i < numGroups; i++)
	{
		occupants[i].clear();
	}
}

void Building::ResetStats()
{
	numInfections = 0;
}