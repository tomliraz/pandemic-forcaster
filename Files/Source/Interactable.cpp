#include "Interactable.h"
#include <iostream>

/* ================ Constructors ================ */
Interactable::Interactable(sf::Vector2<float> position, sf::Vector2<float> size, sf::Color fillColor)
{
	hoverOutline = 2;
	clickOutline = 4;

	box = sf::Rect<int>(sf::Vector2<int>(position), sf::Vector2<int>(size));

	background = sf::RectangleShape(size);
	background.setPosition(position);
	background.setFillColor(fillColor);
	background.setOutlineThickness((float)hoverOutline);
	background.setOutlineColor(sf::Color::Transparent);

	text = sf::Text();

	pressed = false;
	activated = true;
}

Interactable::~Interactable()
{}

/* ================ Accessors ================ */
bool Interactable::isHovering(const sf::Vector2<int>& point) const
{
	return box.contains(point);

}

bool Interactable::isPressed() const
{
	return pressed;
}

bool Interactable::isActivated() const
{
	return activated;
}

const sf::Vector2<float> &Interactable::getPosition() const
{
	return background.getPosition();
}

/* ================ Mutators ================ */
void Interactable::press()
{
	pressed = true;
	background.setOutlineThickness((float)clickOutline);
}

void Interactable::unpress()
{
	pressed = false;
	background.setOutlineThickness((float)hoverOutline);
}

void Interactable::clear()
{
	background.setOutlineColor(sf::Color::Transparent);
	background.setOutlineThickness((float)hoverOutline);
	pressed = false;
}

void Interactable::checkHover(sf::RenderWindow& window)
{
	if (!pressed)
		if (isHovering(sf::Mouse::getPosition(window)))
			background.setOutlineColor(sf::Color::Black);
		else
			background.setOutlineColor(sf::Color::Transparent);
}

bool Interactable::checkPress(sf::RenderWindow& window)
{
	if (activated && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (isHovering(sf::Mouse::getPosition(window)))
			if (!isPressed())
				press();
			else
				unpress();
		else
			return false;
	}
	return pressed;
}

/* ================ Other ================ */
void Interactable::draw(sf::RenderTarget& target, sf::RenderStates) const
{
		target.draw(background);
		target.draw(text);
}

