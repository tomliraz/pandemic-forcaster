#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>

enum class InfectionStatus { DEAD = -1, HEALTHY = 0, INFECTED = 1, RECOVERED = 2 };

class People {
private:
	// Gathers statistics during the simulation
	static unsigned int healthy;
	static unsigned int infected;
	static unsigned int recovered;
	static unsigned int dead;
	
	InfectionStatus status;							//determine infection. -1=dead, 0=clear, 1=infected, 2=recovered
						
	unsigned int scheduleIndex;

	unsigned int recreationalGroup;

	unsigned int sickPeriod;						// If infected, stores number of days infected;
	int age;
	float BMI;

public:
	static float deathRate;							// User Input
	static unsigned int infectiousPeriod;			// User Input

	std::vector<unsigned int> schedule;				// Stores recreational schedule.

	/* ================ Constructor ================ */
	//O(1)
	People(int age, float bmi, unsigned int group);
	
	/* ================ Accessors ================ */
	//O(1)				Returns the InfectionStatus
	InfectionStatus GetStatus();
	//O(1)				Returns age
	int GetAge();
	//O(1)				Returns BMI
	float GetBMI();
	//O(1)				Returns recreational group
	unsigned int GetGroup();
	//O(1)				Returns total healthy People
	static unsigned int GetTotalHealthy();
	//O(1)				Returns total infected People
	static unsigned int GetTotalInfected();
	//O(1)				Returns total recovered People
	static unsigned int GetTotalRecovered();
	//O(1)				Returns total dead People
	static unsigned int GetTotalDead();

	/* ================ Mutators ================ */
	//O(1)				Handles disease progression if infected and returns next recreational building on schedule if not dead.
	int ElapseDay();
	//O(1)				Adds a recreation to the schedule
	void AddRecreation(unsigned int recreation);
	//O(1)				Sets infected status
	void Infect();
	//O(1)				Resets global stat variables.
	static void resetStats();
	//O(1)				Resets disease progress and schedule index
	void reset();
};



