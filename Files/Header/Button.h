#pragma once
#include<string>
#include <SFML/Graphics.hpp>

#include "Interactable.h"

using namespace std;

class Button : public Interactable
{

public:
	/* ================ Constructor ================ */
	//O(1)
	Button(sf::Vector2<float> position, sf::Vector2<float> size, sf::Color fillColor);
	
	/* ================ Mutators ================ */
	//O(text)		Sets button's text
	void setText(string text, const sf::Font &font, unsigned int size);	
	//O(1)				Sets whether button is activated. Deactivation disables pressing and changes color.
	void setActivationState(bool state);
};