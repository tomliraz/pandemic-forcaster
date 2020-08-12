#include "People.h"

#include "Math.h"

unsigned int People::infected = 0;
unsigned int People::recovered = 0;
unsigned int People::dead = 0;
unsigned int People::healthy = 100000;

float People::deathRate;
unsigned int People::infectiousPeriod;

/* ================ Constructor ================ */
People::People(int _age, float _bmi, unsigned int group)
{
	age = _age;
	BMI = _bmi;
	recreationalGroup = group;
	status = InfectionStatus::HEALTHY;
	sickPeriod = 0;
	schedule = std::vector<unsigned int>();
	scheduleIndex = 0;
}

/* ================ Accessors ================ */
InfectionStatus People::GetStatus() { return status; }
int People::GetAge() { return age; }
float People::GetBMI() { return BMI; }
unsigned int People::GetGroup() { return recreationalGroup; }
unsigned int People::GetTotalHealthy() { return healthy; }
unsigned int People::GetTotalInfected() { return infected; }
unsigned int People::GetTotalRecovered() { return recovered; }
unsigned int People::GetTotalDead() { return dead; }

/* ================ Mutators ================ */
int People::ElapseDay()
{
	unsigned int result =  schedule[scheduleIndex];
	
	scheduleIndex++;
	if (scheduleIndex >= schedule.size())
		scheduleIndex = 0;
	if (status == InfectionStatus::INFECTED)
	{
		sickPeriod++;
		if (sickPeriod > infectiousPeriod)
		{
			status = InfectionStatus::RECOVERED;
			infected--;
			recovered++;
		}
		else if (Math::Random() < deathRate / infectiousPeriod)
		{
			status = InfectionStatus::DEAD;
			infected--;
			dead++;
		}
	}
	if(status == InfectionStatus::DEAD)
		return -1;
	return result;
}

void People::AddRecreation(unsigned int recreation)
{
	schedule.push_back(recreation);
}


void People::Infect() 
{ 
	status = InfectionStatus::INFECTED;
	healthy--;
	infected++;
}

void People::reset()
{
	sickPeriod = 0;
	status = InfectionStatus::HEALTHY;
	scheduleIndex = 0;
}

void People::resetStats()
{
	infected = 0;
	recovered = 0;
	dead = 0;
	healthy = 100000;

}