#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "TextureManager.h"

using namespace std;

class Graph : public sf::Drawable
{
	sf::Vector2<float> position;
	sf::Vector2<unsigned int> size;
	
	sf::Image defaultGraphImage;	// Serves as blank slate to draw graph on.
	sf::Image currentGraphImage;
	string graphTexture;			// Key to access texture in TextureManager class.

	// Various Elements of the graph
	sf::Sprite graph;
	sf::RectangleShape outline;
	sf::Sprite grid;
	string gridTextureName;
	sf::Text title;
	vector<sf::Text> yAxisIntervals;
	sf::Text yAxisLabel;
	vector<sf::Text> xAxisIntervals;
	sf::Text xAxisLabel;


public:
	/* ================ Constructor ================ */
	Graph(sf::Vector2<float> graphPosition, sf::Vector2<unsigned int> graphSize);

	/* ================ Mutators ================ */
	//O(text)				Creates title object
	void setTitle(string text, sf::Font &font, unsigned int fontSize, sf::Color = sf::Color::Black);
	//O(numIntervals)		Creates yAxisIntervals object
	void setYAxisIntervals(unsigned int maxAmount, unsigned int numIntervals, sf::Font &font, unsigned int textSize);
	//O(numIntervals)		Creates xAxisIntervals object
	void setXAxisIntervals(unsigned int maxAmount, unsigned int numIntervals, sf::Font &font, unsigned int textSize);
	//O(text)				Creates yAxisLabel object
	void setYAxisLabel(string text, sf::Font &font, unsigned int textSize);
	//O(text)				Creates xAxisLabel object
	void setXAxisLabel(string text, sf::Font &font, unsigned int textSize);
	//O(TextureManager's unordered_map.size)
	//Sets graph texture to default.
	void reset();
	//O(size.x * size.y + (TextureManager's unordered_map.size))
	//Creates and sets graph texture
	void constructGraph(unsigned int ** graphData, unsigned int days);

	void constructOneDay(unsigned int* dayData, unsigned int day, unsigned int = 180);

	/* ================ Other ================ */
	//O(number of pixels in all graph elements)
	//Draws all elements of the graph to the screen.
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};

