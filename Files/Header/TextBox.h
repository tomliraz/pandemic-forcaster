#pragma once
#include<string>
#include <SFML/Graphics.hpp>

#include "Interactable.h"

using namespace std;

class TextBox : public Interactable
{
	const unsigned int MAX_INPUT_LENGTH = 10U;

public:
	/* ================ Constructor ================ */
	//O(1)
	TextBox(sf::Vector2<float> position, sf::Vector2 <float> size, sf::Color fillColor, const sf::Font& font, unsigned int = 20U);
	
	/* ================ Accessor ================ */
	//O(1)				Returns input of TextBox
	string getInput();
	//O(1)				Returns whether input is empty
	bool isInputEmpty();

	/* ================ Mutator ================ */
	//O(1)				Adds given character to input
	void addCharacter(char character);
	//O(1)				Deletes last character from input
	void deleteCharacter();
	//O(1)				Clears input
	void clearInput();
};