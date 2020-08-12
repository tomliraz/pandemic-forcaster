#pragma once
#include <SFML/Graphics.hpp>

using namespace std;

class Interactable : public sf::Drawable
{
	sf::Rect<int> box;
	
	bool pressed;
	unsigned int hoverOutline;
	unsigned int clickOutline;

	/* ================ Mutators ================ */
	//O(1)				Sets pressed to true and changes outline accordingly.
	void press();
	//O(1)				Sets pressed to false and changes outline accordingly.
	void unpress();

protected:
	bool activated; // Controls whether pressing is activated

	sf::RectangleShape background;
	sf::Text text;	

public:
	/* ================ Constructors ================ */
	//O(1)				Constructor
	Interactable(sf::Vector2<float> = sf::Vector2<float>(0,0), sf::Vector2<float> = sf::Vector2<float>(0, 0), sf::Color = sf::Color(0,0,0));
	//O(1)				Destructor - Interactable should never be used to instantiate an object.
	virtual ~Interactable() = 0;

	/* ================ Accessors ================ */
	//O(1)				Checks whether a point is within the Interactable's bounds
	bool isHovering(const sf::Vector2<int>& point) const;
	//O(1)				Returns whether pressed
	bool isPressed() const;
	//O(1)				Returns whether activated
	bool isActivated() const;
	//O(1)				Returns position
	const sf::Vector2<float>& getPosition() const;

	/* ================ Mutators ================ */
	//O(1)				Clears any hover or pressed condition
	void clear();
	//O(1)				Handles hovering behavior with respect to the cursor
	void checkHover(sf::RenderWindow& window);
	//O(1)				Handles pressing behavior when left mouse button is pressed - only works when the Interactable is activated
	bool checkPress(sf::RenderWindow& window);

	/* ================ Other ================ */
	//O(background.getSize().x * background.getSize().x + number of pixels in text)
	//Draws all elements of the graph to the screen.
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	
};

