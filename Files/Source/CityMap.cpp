#include "CityMap.h"
/* ================ Constructors ================ */
CityMap::CityMap(sf::Vector2<float> position, unsigned int row, unsigned int col, unsigned int squareSize)
{
	rowSize = row;
	colSize = col;

	map = new sf::RectangleShape*[rowSize];

	for (unsigned int i = 0; i < rowSize; i++)
	{
		map[i] = new sf::RectangleShape[colSize];
		for (unsigned int j = 0; j < colSize; j++)
		{
			map[i][j] = sf::RectangleShape(sf::Vector2<float>((float)squareSize, (float)squareSize));
			map[i][j].setPosition(sf::Vector2<float>(position.x + i * squareSize , position.y + j * squareSize ));

			map[i][j].setOutlineColor(sf::Color::Black);
			map[i][j].setOutlineThickness(-1);
		}
	}
}

CityMap::~CityMap()
{
	for (unsigned int i = 0; i < rowSize; i++)
	{
		delete[] map[i];
	}
	delete[] map;
}

/* ================ Mutators ================ */
void CityMap::loadMap(unsigned int *infectionCount, unsigned int maxCases)
{	
	
	for (unsigned int j = 0; j < colSize; j++)
		for (unsigned int i = 0; i < rowSize; i++)
		{
			unsigned int fraction = (unsigned int)(255 * (1 - ((float)infectionCount[j * rowSize + i] / maxCases)));

			map[i][j].setFillColor(sf::Color(255, (sf::Uint8)fraction, (sf::Uint8)fraction));
		}
}

void CityMap::reset()
{
	for (unsigned int i = 0; i < rowSize; i++)
		for (unsigned int j = 0; j < colSize; j++)
		{
			map[i][j].setFillColor(sf::Color(255, 255, 255));
		}
}

/* ================ Other ================ */
void CityMap::draw(sf::RenderTarget& target, sf::RenderStates) const
{
	for(unsigned int i = 0; i < rowSize; i++)
		for (unsigned int j = 0; j < colSize; j++)
		{
			target.draw(map[i][j]);
		}
}

