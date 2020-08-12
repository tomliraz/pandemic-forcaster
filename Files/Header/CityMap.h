#pragma once
#include <SFML/Graphics.hpp>

#include "building.h"

using namespace std;

class CityMap : public sf::Drawable
{
	sf::RectangleShape **map;
	
	unsigned int rowSize;
	unsigned int colSize;

	sf::Vector2<float> position;


public:
	/* ================ Constructors ================ */
	//O(rowSize * colSize)				Constructor
	CityMap(sf::Vector2<float> position, unsigned int row, unsigned int col, unsigned int squareSize);
	//O(rowSize * colSize)				Destructor
	~CityMap();

	/* ================ Mutators ================ */
	//O(rowSize * colSize)				Returns the building's max capacity
	void loadMap(unsigned int* infectionCount, unsigned int maxCases);
	//O(rowSize * colSize)				Resets color to white in all cells
	void reset();

	/* ================ Other ================ */
	//O((size of squares)^2 * rowSize * colSize)
	//Draws the squares of the map to the screen.
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	
};

