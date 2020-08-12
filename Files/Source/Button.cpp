#include "Button.h"

/* ================ Constructor ================ */
Button::Button(sf::Vector2<float> position, sf::Vector2<float> size, sf::Color fillColor) : Interactable(position, size, fillColor){}

/* ================ Mutators ================ */
void Button::setText(string textString, const sf::Font& font, unsigned int size)
{
	text = sf::Text(textString, font, size);
	sf::Rect<float> textCoords = text.getLocalBounds();
	text.setPosition(background.getPosition().x + background.getSize().x / 2 - textCoords.width / 2, background.getPosition().y + background.getSize().y / 2 - textCoords.height / 2 - 8);
	text.setFillColor(sf::Color::White);
}

void Button::setActivationState(bool state)
{
	if (activated != state)
	{
		if (state == false)
		{
			clear();
			background.setFillColor(sf::Color(0, 76, 153));
		}
		else
		{
			background.setFillColor(sf::Color(0, 128, 255));
		}

		activated = state;
	}
}