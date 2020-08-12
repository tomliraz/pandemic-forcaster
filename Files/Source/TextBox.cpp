#include "TextBox.h"

/* ================ Constructor ================ */
TextBox::TextBox(sf::Vector2<float> position, sf::Vector2<float> size, sf::Color fillColor, const sf::Font& font, unsigned int fontSize) : Interactable(position, size, fillColor)
{
	text = sf::Text("", font, fontSize);
	text.setPosition(background.getPosition().x + 4, background.getPosition().y + background.getSize().y / 2 - text.getCharacterSize() / 2);
	text.setFillColor(sf::Color::White);
	
}

/* ================ Accessor ================ */
string TextBox::getInput()
{
	return text.getString();
}

bool TextBox::isInputEmpty()
{
	return text.getString().isEmpty();
}

/* ================ Mutator ================ */
void TextBox::addCharacter(char character)
{
	if(text.getString().getSize() < MAX_INPUT_LENGTH)
		text.setString(text.getString() + character);
}

void TextBox::deleteCharacter()
{
	if (text.getString().isEmpty())
		return;

	string currString = text.getString();

	text.setString(currString.substr(0, currString.size()-1));
}

void TextBox::clearInput()
{
	text.setString("");
}