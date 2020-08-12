#include "Graph.h"

/* ================ Constructor ================ */
Graph::Graph(sf::Vector2<float> graphPosition, sf::Vector2<unsigned int> graphSize)
{
    position = graphPosition;
    size = graphSize;
    
    // Creating the graph.
    defaultGraphImage.create(size.x, size.y, sf::Color(149, 202, 255));
    currentGraphImage.create(size.x, size.y, sf::Color(149, 202, 255));

    graphTexture = "graph";

    TextureManager::SetTexture(graphTexture, defaultGraphImage);
    graph.setTexture(TextureManager::GetTexture(graphTexture));
    graph.setPosition(position);

	// Creating the grid overlay.
	sf::Image gridImage;
	gridImage.create(size.x, size.y, sf::Color::Transparent);

    // Creates the grid's horizontal lines.
    for (unsigned int x = 0; x < size.x; x++)
        for (unsigned int y = size.y / 5; y < size.y; y += size.y / 5)
            gridImage.setPixel(x, size.y - y - 1, sf::Color(128, 128, 128));

    // Creates the grid's vertical lines.
    for (unsigned int y = 0; y < size.y; y++)
        for (unsigned int x = size.x / 6; x < size.x; x += size.x / 6)
            gridImage.setPixel(x, y, sf::Color(128, 128, 128));

    gridTextureName = "grid";
    TextureManager::SetTexture(gridTextureName, gridImage);
    grid.setTexture(TextureManager::GetTexture(gridTextureName));
    grid.setPosition(position);

    // Creates the graph outline.
    outline = sf::RectangleShape(sf::Vector2<float>((float)size.x, (float)size.y));
    outline.setOutlineThickness(2);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor(sf::Color::Black);
    outline.setPosition(position);

}

/* ================ Mutators ================ */
void Graph::setTitle(string text, sf::Font &font, unsigned int textSize, sf::Color color)
{
    title = sf::Text(text, font, textSize);
    title.setFillColor(color);
    title.setPosition(position.x + size.x / 2 - 110, position.y - 45);
}

void Graph::setYAxisIntervals(unsigned int maxAmount, unsigned int numIntervals, sf::Font &font, unsigned int textSize)
{
    yAxisIntervals = vector<sf::Text>(numIntervals);
    unsigned int posY = (unsigned int)position.y - 10U;
    unsigned int amountY = maxAmount;
    for (unsigned int i = 0; i < numIntervals; i++)
    {
        yAxisIntervals[i] = sf::Text(to_string(amountY), font, textSize);
        yAxisIntervals[i].setFillColor(sf::Color::Black);
        yAxisIntervals[i].setPosition(sf::Vector2<float>(position.x - yAxisIntervals[i].getGlobalBounds().width - 7, (float)posY));
        posY += size.y / numIntervals;
        amountY -= maxAmount / numIntervals;
        
    }
}

void Graph::setXAxisIntervals(unsigned int maxAmount, unsigned int numIntervals, sf::Font &font, unsigned int textSize)
{
    xAxisIntervals = vector<sf::Text>(numIntervals);
    unsigned int posX = 0;
    unsigned int amountX = 0;
    for (int i = 0; i < 7; i++)
    {
        xAxisIntervals[i] = sf::Text(to_string(amountX), font, textSize);
        xAxisIntervals[i].setFillColor(sf::Color(0, 0, 0));
        posX = unsigned int(position.x + i * size.x / (numIntervals - 1) - xAxisIntervals[i].getGlobalBounds().width / 2);
        xAxisIntervals[i].setPosition(sf::Vector2<float>((float)posX, position.y + size.y + 3));
        amountX += maxAmount / (numIntervals - 1);
    }
}

void Graph::setYAxisLabel(string text, sf::Font &font, unsigned int textSize)
{
    yAxisLabel = sf::Text(text, font, textSize);
    yAxisLabel.setFillColor(sf::Color::Black);
    yAxisLabel.rotate(270);
    yAxisLabel.setPosition(position.x - 85, position.y + size.y / 2 + 60);
}

void Graph::setXAxisLabel(string text, sf::Font &font, unsigned int textSize)
{
    xAxisLabel = sf::Text(text, font, textSize);
    xAxisLabel.setFillColor(sf::Color::Black);
    xAxisLabel.setPosition(position.x + size.x / 2 - 70, position.y + size.y + 20);
}

void Graph::reset()
{
    currentGraphImage.copy(defaultGraphImage, 0, 0);
    TextureManager::SetTexture(graphTexture, currentGraphImage);
    graph.setTexture(TextureManager::GetTexture(graphTexture));
}

void Graph::constructGraph(unsigned int** graphData, unsigned int days)
{
    unsigned int pixelPos = 0;

    for (unsigned int x = 0; x < days; x++)
    {
        for (unsigned int i = 0; i < size.x / days; i++)
        {
            for (unsigned int a = 0; a < graphData[x][2]; a++)
                currentGraphImage.setPixel(pixelPos, size.y - (unsigned int)((float)a / 100000 * size.y) - 1, sf::Color(70, 70, 70));

            for (unsigned int b = graphData[x][2]; b < graphData[x][1] + graphData[x][2]; b++)
                currentGraphImage.setPixel(pixelPos, size.y - (unsigned int)((float)b / 100000 * size.y) - 1, sf::Color(76, 153, 0));

            for (unsigned int c = graphData[x][1] + graphData[x][2]; c < graphData[x][0] + graphData[x][1] + graphData[x][2]; c++)
                currentGraphImage.setPixel(pixelPos, size.y - (unsigned int)((float)c / 100000 * size.y) - 1, sf::Color(215, 42, 42));
            pixelPos++;
        }
    }

    TextureManager::SetTexture(graphTexture, currentGraphImage);
    graph.setTexture(TextureManager::GetTexture(graphTexture));
}

void Graph::constructOneDay(unsigned int* dayData, unsigned int day, unsigned int numDays)
{
    unsigned int pixelsPerDay = size.x / numDays;
    unsigned int pixelPos = day * pixelsPerDay;

    for (unsigned int i = 0; i < pixelsPerDay; i++)
    {
        for (unsigned int a = 0; a < dayData[2]; a++)
            currentGraphImage.setPixel(pixelPos, size.y - (unsigned int)((float)a / 100000 * size.y) - 1, sf::Color(70, 70, 70));

        for (unsigned int b = dayData[2]; b < dayData[1] + dayData[2]; b++)
            currentGraphImage.setPixel(pixelPos, size.y - (unsigned int)((float)b / 100000 * size.y) - 1, sf::Color(76, 153, 0));

        for (unsigned int c = dayData[1] + dayData[2]; c < dayData[0] + dayData[1] + dayData[2]; c++)
            currentGraphImage.setPixel(pixelPos, size.y - (unsigned int)((float)c / 100000 * size.y) - 1, sf::Color(215, 42, 42));
        pixelPos++;
    }

    TextureManager::SetTexture(graphTexture, currentGraphImage);
    graph.setTexture(TextureManager::GetTexture(graphTexture));
}

/* ================ Other ================ */
void Graph::draw(sf::RenderTarget& target, sf::RenderStates) const
{
    target.draw(graph);
    target.draw(outline);
    target.draw(grid);
    target.draw(title);

    for (unsigned int i = 0; i < yAxisIntervals.size(); i++)
        target.draw(yAxisIntervals[i]);
    target.draw(yAxisLabel);

    for (unsigned int i = 0; i < xAxisIntervals.size(); i++)
        target.draw(xAxisIntervals[i]);
    target.draw(xAxisLabel);
}