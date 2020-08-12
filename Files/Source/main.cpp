#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>
#include <unordered_map>
#include <thread>

#include "RobotoFonts.h"
#include "Logo.h"
#include "Icon.h"

#include "Button.h"
#include "TextBox.h"
#include "Graph.h"
#include "CityMap.h"
#include "building.h"

#define DATA_SIZE 100000

using namespace std;

// SFML does not provide time complexity documentation for its function, therefore I have made the following assumptions
// Assuming sf::Sprite::setTexture is O(1)
// Assuming sf::IntRect::contains is O(1)
// Assuming sf::Texture::loadFromImage is O(number of pixels)
// Assuming sf::Drawable::draw is O(number of pixels)
// Assuming any function that sets a string scales linearly with string's size
// Other functions not mentioned likely assumed to be O(1)

// Populates the city vector with the given capacities for each type of buildings.
// Stores the index bound of each type of building in the passed pointer (ie. resedential buildings are indices 0 through 1249)
// O(city.size())
void populateCity(vector<Building*>& city, unsigned int* bounds, int resedential, int industrial, int recreational)
{

    bounds[0] = city.size();

    int pop = 0;
    while (pop < DATA_SIZE)
    {
        Building* building = new Building(BuildingType::RESIDENTIAL, resedential);
        pop += building->GetNumberOfGroups() * resedential;
        city.push_back(building);
    }

    bounds[1] = city.size();
    
    pop = 0;
    while (pop < DATA_SIZE)
    {
        Building* building = new Building(BuildingType::INDUSTRY, industrial);
        pop += building->GetNumberOfGroups() * industrial;
        city.push_back(building);
    }

    bounds[2] = city.size();

    pop = 0;
    while (pop < DATA_SIZE)
    {
        Building* building = new Building(BuildingType::RECREATIONAL, recreational + 100); // Since recreational is the average number of people per building I added 100 extra spots to allow for variance.
        pop += building->GetNumberOfGroups() * recreational;
        city.push_back(building);
    }

    bounds[3] = city.size();
};

// Populates an unordered_map with pointers of People. Places People in a residential and industrial building and creates a weekly recreational schedule.
//O((number of people) * (size of their schedule))
void populatePeople(unordered_map<int, People*>& population, vector<Building*>& city, unsigned int* bounds)
{
    normal_distribution<float> bmiDist(25.5f, 3.7f);
    normal_distribution<float> ageDist(38, 12);

    default_random_engine engine((unsigned)std::chrono::system_clock::now().time_since_epoch().count());

    unsigned int industryBuilding = bounds[1] - 1;

    unsigned int recreationBuilding = bounds[2] - 1;

    // Created to safeguard against the unlikely but problematic occurance that a recreational building will exceed max capacity.
    unsigned short *weekScheduler[250][4]; // Dynamically allocated to offload memory from the stack.
    for (unsigned int a = 0; a < 250; a++)
    {
        for (unsigned int b = 0; b < 4; b++)
        {
            weekScheduler[a][b] = new unsigned short[7];
            for (unsigned int c = 0; c < 7; c++)
                weekScheduler[a][b][c] = 0;
        }
    }

    // Created to safeguard against the unlikely but problematic occurance where one recreational group has significantly more people than the others.
    unsigned int groupCount[4] = { 0 };

    for (unsigned int i = 0; i < DATA_SIZE; i++)
    {

        int age = (int)ageDist(engine);
        float bmi = bmiDist(engine);
        
        unsigned int randGroup;

        do // Find a non-full group in a recreational building.
        {
            randGroup = unsigned int(Math::Random() * 4);
        } while (randGroup >= 4 || groupCount[randGroup] > 28000);

        groupCount[randGroup]++;

        People *person = new People(age, bmi, randGroup);

        population[i] = person;

        city[i / city[0]->GetMaxCapacity()]->Populate(person, 0); // Residential buildings only have group 0

        if (i % 200 == 0)
            industryBuilding++;
        
        city[industryBuilding]->Populate(person, i % 2);

        if (i % 400 == 0)
            recreationBuilding++;

        unsigned int randBuilding = 0;

         

        for (int a = 0; a < 7; a++)
        {
            do // Find a non-full group in a recreational building.
            {
                randBuilding = unsigned int(Math::Random() * (bounds[3] - bounds[2]) + bounds[2]);
            } while (randBuilding >= bounds[3] || weekScheduler[randBuilding-1750][person->GetGroup()][a] > (unsigned short)city[randBuilding]->GetMaxCapacity());
            weekScheduler[randBuilding - bounds[2]][person->GetGroup()][a]++;

           person->AddRecreation(randBuilding);
        }

    }

    for (unsigned int a = 0; a < 250; a++)
    {
        for (unsigned int b = 0; b < 4; b++)
        {
            delete[] weekScheduler[a][b];
        }
    }
};

// Elapses a day in the program. Runs the infect algorithm on every group of every building.
// Time is amorphous within a single day, since the algorithm considers people to be in their residential, industrial, and recreational buildings at the same time.
// However, as the program focuses on a period of many days this hardly affects the program's accuracy.
// O(number of people)
void day(unordered_map<int, People*>& population, vector<Building*>& city, unsigned int* bounds, float infectionRate)
{
    unordered_map<int, People*>::iterator iter;

    for (iter = population.begin(); iter != population.end(); iter++)
    {
        int building = iter->second->ElapseDay();

        if (building != -1)
            city[building]->Populate(iter->second, iter->second->GetGroup()); // Add person to the building.
    }

    for (unsigned int i = 0; i < city.size(); i++)
    {
        for (unsigned int j = 0; j < city[i]->GetNumberOfGroups(); j++) // Run infection calculation on all buildings.
        {
            city[i]->Infect(infectionRate, j);
        }
    }

    for (unsigned int i = bounds[2]; i < bounds[3]; i++)
        city[i]->Clear(); // Clears recreational buildings after infecting them since they change daily.

}

int main()
{
    float r0;
    unsigned int daysinfected;
    float deathRate;
    
    unsigned int resedential = 80;  // 1250 total buildings
    int industrial = 100;           // 500 total buildings
    int recreational = 100;         // 250 total buildings

    unsigned int days = 0;
    bool isRunning = false;
    bool isReset = true;

    float infectionChance = 0;

    unsigned int** dailyStats = new unsigned int* [180]; // Keeps track of infected, recovered, and dead daily.
    unsigned int* finalInfectionCounts = new unsigned int[2000]; // Keeps track of total infections in each building.

    for (int i = 0; i < 180; i++)
        dailyStats[i] = new unsigned int[3];

    unsigned int infectionsOfBuildingTypes[3] = { 0 }; // Keeps track of the total infections of each building type.

    unordered_map<int, People*> population;
    vector<Building*> city;
    unsigned int* bounds = new unsigned int[4];

    populateCity(city, bounds, resedential, industrial, recreational);
    populatePeople(population, city, bounds);

    sf::RenderWindow window(sf::VideoMode(1400, 800), "Pandemic Forcaster");
    sf::Image icon;
    icon.loadFromMemory(&pandemic_icon_png, pandemic_icon_png_len);
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    // -------------------------------------- Creates Graph -------------------------------------- //
    // Defines the size and position of the graph. All graph elements rely on these values.
    const sf::Vector2<unsigned int> GRAPH_SIZE(720, 300);
    const sf::Vector2<float> GRAPH_POSITION(655, 50);

    sf::Font robotoLight;
    sf::Font robotoRegular;
    sf::Font robotoMedium;
    
    robotoLight.loadFromMemory(&Roboto_Light_ttf, Roboto_Light_ttf_len);
    robotoRegular.loadFromMemory(&Roboto_Regular_ttf, Roboto_Regular_ttf_len);
    robotoMedium.loadFromMemory(&Roboto_Medium_ttf, Roboto_Medium_ttf_len);

    Graph graph(GRAPH_POSITION, GRAPH_SIZE);

    graph.setYAxisIntervals(100000, 5, robotoLight, 15);
    graph.setYAxisLabel("Number of People", robotoRegular, 20);
    graph.setXAxisIntervals(180, 7, robotoLight, 15);
    graph.setXAxisLabel("Number of Days", robotoRegular, 20);
    graph.setTitle("Infectivity Graph", robotoMedium, 30);

    const sf::Vector2<float> KEY_SIZE(20, 20);

    // Creates the a key below the graph that explains what each color represents.
    sf::RectangleShape deadKey(KEY_SIZE);
    unsigned int spacing = unsigned int((GRAPH_SIZE.x - 4 * deadKey.getSize().x) / 8);

    deadKey.setFillColor(sf::Color(70, 70, 70));
    deadKey.setOutlineThickness(2);
    deadKey.setOutlineColor(sf::Color::Black);
    deadKey.setPosition(sf::Vector2<float>(GRAPH_POSITION.x + spacing - 30, GRAPH_POSITION.y + GRAPH_SIZE.y + 55));
    sf::Text deadText("Dead", robotoRegular, 20);
    deadText.setPosition(sf::Vector2<float>(deadKey.getGlobalBounds().left - deadText.getGlobalBounds().width - 8, GRAPH_POSITION.y + GRAPH_SIZE.y + 53));

    cout << deadText.getGlobalBounds().width << endl;

    sf::RectangleShape recoveredKey(KEY_SIZE);
    recoveredKey.setFillColor(sf::Color(76, 153, 0));
    recoveredKey.setOutlineThickness(2);
    recoveredKey.setOutlineColor(sf::Color::Black);
    recoveredKey.setPosition(sf::Vector2<float>(GRAPH_POSITION.x + 3 * spacing + KEY_SIZE.x - 5, GRAPH_POSITION.y + GRAPH_SIZE.y + 55));
    sf::Text recoveredText("Recovered", robotoRegular, 20);
    recoveredText.setPosition(sf::Vector2<float>(recoveredKey.getGlobalBounds().left - recoveredText.getGlobalBounds().width - 8, GRAPH_POSITION.y + GRAPH_SIZE.y + 53));

    sf::RectangleShape infectedKey(KEY_SIZE);
    infectedKey.setFillColor(sf::Color(215, 42, 42));
    infectedKey.setOutlineThickness(2);
    infectedKey.setOutlineColor(sf::Color::Black);
    infectedKey.setPosition(sf::Vector2<float>(GRAPH_POSITION.x + 5 * spacing + 2 * KEY_SIZE.x + 10, GRAPH_POSITION.y + GRAPH_SIZE.y + 55));
    sf::Text infectedText("Infected", robotoRegular, 20);
    infectedText.setPosition(sf::Vector2<float>(infectedKey.getGlobalBounds().left - infectedText.getGlobalBounds().width - 8, GRAPH_POSITION.y + GRAPH_SIZE.y + 53));


    sf::RectangleShape healthyKey(KEY_SIZE);
    healthyKey.setFillColor(sf::Color(149, 202, 255));
    healthyKey.setOutlineThickness(2);
    healthyKey.setOutlineColor(sf::Color::Black);
    healthyKey.setPosition(sf::Vector2<float>(GRAPH_POSITION.x + 7 * spacing + 3 * KEY_SIZE.x + 20, GRAPH_POSITION.y + GRAPH_SIZE.y + 55));
    sf::Text healthyText("Healthy", robotoRegular, 20);
    healthyText.setPosition(sf::Vector2<float>(healthyKey.getGlobalBounds().left - healthyText.getGlobalBounds().width - 8, GRAPH_POSITION.y + GRAPH_SIZE.y + 53));


    sf::Text deadStat("0", robotoRegular, 20);
    deadStat.setPosition(deadKey.getPosition().x + deadKey.getGlobalBounds().width + 3, GRAPH_POSITION.y + GRAPH_SIZE.y + 53);

    sf::Text recoveredStat("0", robotoRegular, 20);
    recoveredStat.setPosition(recoveredKey.getPosition().x + recoveredKey.getGlobalBounds().width + 3, GRAPH_POSITION.y + GRAPH_SIZE.y + 53);

    sf::Text infectedStat("0", robotoRegular, 20);
    infectedStat.setPosition(infectedKey.getPosition().x + infectedKey.getGlobalBounds().width + 3, GRAPH_POSITION.y + GRAPH_SIZE.y + 53);
    
    sf::Text healthyStat("100000", robotoRegular, 20);
    healthyStat.setPosition(healthyKey.getPosition().x + healthyKey.getGlobalBounds().width + 3, GRAPH_POSITION.y + GRAPH_SIZE.y + 53);


    // -------------------------------------- Creates Interactables -------------------------------------- //
    sf::Color textBoxColor = sf::Color(149, 202, 255);
    sf::Color buttonColor = sf::Color(0, 128, 255);

    // Creates textboxes to input r-number, infectious period, and death rate.
    TextBox rValueInput(sf::Vector2<float>(GRAPH_POSITION.x + GRAPH_SIZE.x / 10, GRAPH_POSITION.y + GRAPH_SIZE.y + 130), sf::Vector2<float>((float)(2 * GRAPH_SIZE.x / 10), 30), textBoxColor, robotoRegular);
    TextBox infectiousPeriodInput(sf::Vector2<float>(GRAPH_POSITION.x + 4 * GRAPH_SIZE.x / 10, GRAPH_POSITION.y + GRAPH_SIZE.y + 130), sf::Vector2<float>((float)(2 * GRAPH_SIZE.x / 10), 30), textBoxColor, robotoRegular);
    TextBox deathRateInput(sf::Vector2<float>(GRAPH_POSITION.x + 7 * GRAPH_SIZE.x / 10, GRAPH_POSITION.y + GRAPH_SIZE.y + 130), sf::Vector2<float>((float)(2 * GRAPH_SIZE.x / 10), 30), textBoxColor, robotoRegular);

    // Creates R0 value label.
    sf::Text rValueLabel("R0 Value:", robotoRegular, 20);
    rValueLabel.setPosition(rValueInput.getPosition() - sf::Vector2<float>(0, rValueLabel.getGlobalBounds().height + 13));
    rValueLabel.setFillColor(sf::Color::Black);

    // Creates infectious period label.
    sf::Text infectiousPeriodLabel("Infectious Period:", robotoRegular, 20);
    infectiousPeriodLabel.setPosition(infectiousPeriodInput.getPosition() - sf::Vector2<float>(0, infectiousPeriodLabel.getGlobalBounds().height + 13));
    infectiousPeriodLabel.setFillColor(sf::Color::Black);

    // Creates R0 value label.
    sf::Text deathRateLabel("Mortality Rate:", robotoRegular, 20);
    deathRateLabel.setPosition(deathRateInput.getPosition() - sf::Vector2<float>(0, deathRateLabel.getGlobalBounds().height + 10));
    deathRateLabel.setFillColor(sf::Color::Black);

    // Creates run button
    Button run(sf::Vector2<float>(GRAPH_POSITION.x + 276, GRAPH_POSITION.y + GRAPH_SIZE.y + 190), sf::Vector2<float>(168, 40), buttonColor);
    run.setText("Run", robotoRegular, 30);
    run.setActivationState(false);

    // Commends on invalid or incomplete input
    sf::Text inputWarning("Please enter all inputs", robotoRegular, 20);
    inputWarning.setFillColor(sf::Color::Black);
    inputWarning.setPosition(GRAPH_POSITION.x + GRAPH_SIZE.x/2 - inputWarning.getGlobalBounds().width/2, GRAPH_POSITION.y + GRAPH_SIZE.y + 250);
    inputWarning.setString("");

    // -------------------------------------- Create Heat Map -------------------------------------- //
    const sf::Vector2<float> MAP_POS(20, 120);

    CityMap heatMap(MAP_POS, 50, 40, 10);

    sf::Text mapText("Relative Transmission Across The City", robotoMedium, 25);
    mapText.setPosition(MAP_POS.x + 250 -  mapText.getGlobalBounds().width/2, MAP_POS.y-mapText.getGlobalBounds().height-12);
    mapText.setFillColor(sf::Color::Black);

    sf::Text residentialInfections("", robotoRegular, 20U);
    residentialInfections.setPosition(MAP_POS.x, MAP_POS.y + 410);

    sf::Text industrialInfections("", robotoRegular, 20U);
    industrialInfections.setPosition(MAP_POS.x, MAP_POS.y + 440);

    sf::Text recreationalInfections("", robotoRegular, 20U);
    recreationalInfections.setPosition(MAP_POS.x, MAP_POS.y + 470);

    // -------------------------------------- Creates Other Elements -------------------------------------- //
    sf::Text loading("", robotoRegular, 20U);
    loading.setPosition(900, GRAPH_POSITION.y + GRAPH_SIZE.y + 250);
    loading.setFillColor(sf::Color::Black);

    sf::Image logoImage;
    string logoKey = "logo";
    logoImage.loadFromMemory(pandemic_logo_png, pandemic_logo_png_len);
    TextureManager::SetTexture(logoKey, logoImage);
    sf::Sprite logo(TextureManager::GetTexture(logoKey));
    logo.setPosition(MAP_POS.x + 250 - logo.getGlobalBounds().width / 2, 5);

    sf::Text programDescription("This is a software-based model designed to predict the\nprogression and outcomes of a pandemic spread. It assumes\na city of 100,000 inhabitants, and a time frame of 180 days.\n\nInstructions: type in the R0 value (indicating contagiousness), \ninfectious period, and the mortality rate, then click \"Run\".", robotoRegular, 20);
    programDescription.setFillColor(sf::Color::Black);
    programDescription.setPosition(MAP_POS.x, 630);

    sf::Text sampleInput("COVID19 - R0: 2.5 | Infectious Period: 14 days | Mortality Rate: 0.038\nEbola - R0: 1.7 | Infectious Period: 10 days | Mortality Rate: 0.325\nMeasles - R0: 15 | Infectious Period: 5 days | Mortality Rate: 0.002", robotoRegular, 20);
    sampleInput.setFillColor(sf::Color::Black);
    sampleInput.setPosition(unsigned int(GRAPH_POSITION.x + GRAPH_SIZE.x/2 - sampleInput.getGlobalBounds().width/2), 680);

    sf::Text sampleInputTitle("Sample inputs to try out:", robotoMedium, 20);
    sampleInputTitle.setFillColor(sf::Color::Black);
    sampleInputTitle.setPosition(unsigned int(unsigned int(GRAPH_POSITION.x + GRAPH_SIZE.x / 2 - sampleInput.getGlobalBounds().width / 2)), 650);

    bool runThreads = true;
    // Lambda experssion to be used in a thread.
    auto runForcaster = [&]() { 
        while (window.isOpen() && runThreads)
        {
            if (isRunning)
            {
                string loadingString = "Loading: Day " + to_string(days + 1) + " of 180";
                loading.setString(loadingString);

                day(population, city, bounds, infectionChance);
                dailyStats[days][0] = People::GetTotalInfected();
                dailyStats[days][1] = People::GetTotalRecovered();
                dailyStats[days][2] = People::GetTotalDead();
                
                graph.constructOneDay(dailyStats[days], days);

                deadStat.setString(to_string(People::GetTotalDead()));
                recoveredStat.setString(to_string(People::GetTotalRecovered()));
                infectedStat.setString(to_string(People::GetTotalInfected()));
                healthyStat.setString(to_string(People::GetTotalHealthy()));

                days++;
                
                if (days == 180)
                {
                    unsigned int max = 0;
                    for (int i = 0; i < 2000; i++)
                    {
                        finalInfectionCounts[i] = city[i]->GetTotalInfections();
                        if (finalInfectionCounts[i] > max)
                            max = finalInfectionCounts[i];
                        if (i < 1250)
                            infectionsOfBuildingTypes[0] += finalInfectionCounts[i];
                        else if (i < 1750)
                            infectionsOfBuildingTypes[1] += finalInfectionCounts[i];
                        else
                            infectionsOfBuildingTypes[2] += finalInfectionCounts[i];
                    }

                    heatMap.loadMap(finalInfectionCounts, max);

                    run.clear();
                    run.setText("Reset", robotoRegular, 30);

                    loading.setString("Loading Complete: 180 Days");

                    isRunning = false;

                    residentialInfections.setString("Residential: " + to_string(infectionsOfBuildingTypes[0] / 1250U) + " infections per building");
                    industrialInfections.setString("Industrial: " + to_string(infectionsOfBuildingTypes[1] / 500U) + " infections per building");
                    recreationalInfections.setString("Recreational: " + to_string(infectionsOfBuildingTypes[2] / 250U) + " infections per building");
                }
            }
        }
    };

    thread t1(runForcaster); // Runs model in seperate thread

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                runThreads = false;
                t1.join();
                window.close();
            }

            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (event.type == sf::Event::MouseMoved )
            {
                run.checkHover(window);
                rValueInput.checkHover(window);
                infectiousPeriodInput.checkHover(window);
                deathRateInput.checkHover(window);
            }

            if (!isRunning && event.type == sf::Event::MouseButtonPressed)
            {
                if (isReset)
                {
                    if (rValueInput.checkPress(window))
                    {
                        infectiousPeriodInput.clear();

                        deathRateInput.clear();
                    }

                    else if (infectiousPeriodInput.checkPress(window))
                    {
                        rValueInput.clear();

                        deathRateInput.clear();
                    }

                    else if (deathRateInput.checkPress(window))
                    {
                        rValueInput.clear();

                        infectiousPeriodInput.clear();
                    }

                    else if (run.checkPress(window)) // Runs program
                    {
                        isRunning = true;

                        isReset = false;

                        r0 = stof(rValueInput.getInput());
                        daysinfected = stoi(infectiousPeriodInput.getInput());
                        deathRate = stof(deathRateInput.getInput());

                        rValueInput.clear();
                        infectiousPeriodInput.clear();
                        deathRateInput.clear();

                        unsigned int avgMetPeopleDaily = resedential + industrial + recreational;
                        
                        // Formula that drives the program. Takes R0 and turns it into an infection chance for one person that interacts with one infected person.
                        infectionChance = (float)r0 / daysinfected / avgMetPeopleDaily;

                        People::deathRate = deathRate;
                        People::infectiousPeriod = daysinfected;

                        for (unsigned int i = 0; i < 2000; i++) // Starts of with 2000 people infected.
                        {
                            population[i * 50]->Infect(); // Actually infects both maps of people since they point to the same memory.
                        }

                    }

                    else if (run.isHovering(sf::Mouse::getPosition(window))) 
                    {
                        if (rValueInput.isInputEmpty() || infectiousPeriodInput.isInputEmpty() || deathRateInput.isInputEmpty())
                            inputWarning.setString("Please enter all inputs");
                        else
                            inputWarning.setString("Please enter valid inputs");
                    }
                }
                else if (!isRunning && run.checkPress(window)) // Resets program
                {
                    isReset = true;
                    run.setText("Run", robotoRegular, 30);

                    rValueInput.clearInput();
                    infectiousPeriodInput.clearInput();
                    deathRateInput.clearInput();

                    graph.reset();
                    heatMap.reset();

                    for (auto iter = population.begin(); iter != population.end(); iter++)
                        iter->second->reset();
                    People::resetStats();

                    for (unsigned int i = 0; i < city.size(); i++)
                        city[i]->ResetStats();
                    days = 0;

                    infectionsOfBuildingTypes[0] = 0;
                    infectionsOfBuildingTypes[1] = 0;
                    infectionsOfBuildingTypes[2] = 0;

                    residentialInfections.setString("");
                    industrialInfections.setString("");
                    recreationalInfections.setString("");

                    loading.setString("");

                    deadStat.setString("0");
                    recoveredStat.setString("0");
                    infectedStat.setString("0");
                    healthyStat.setString("100000");
                }
            }

            if (!isRunning && isReset && event.type == sf::Event::TextEntered)
            {
                // Used ASCII to control user input.
                // 48 to 57 are the digits
                // 46 is the decimal point
                // 8 is backspace
                if (rValueInput.isPressed())
                {
                    if (event.text.unicode == 46 || (event.text.unicode >= 48 && event.text.unicode <= 57)) 
                        rValueInput.addCharacter(static_cast<char>(event.text.unicode));
                    else if (event.text.unicode == 8)
                        rValueInput.deleteCharacter();
                }
                else if (infectiousPeriodInput.isPressed())
                {
                    if (event.text.unicode >= 48 && event.text.unicode <= 57)
                        infectiousPeriodInput.addCharacter(static_cast<char>(event.text.unicode));
                    else if (event.text.unicode == 8)
                        infectiousPeriodInput.deleteCharacter();
                }
                else if (deathRateInput.isPressed())
                {
                    if (event.text.unicode == 46 || (event.text.unicode >= 48 && event.text.unicode <= 57))
                        deathRateInput.addCharacter(static_cast<char>(event.text.unicode));
                    else if (event.text.unicode == 8)
                        deathRateInput.deleteCharacter();
                }
            }

            if (!rValueInput.isInputEmpty() && !infectiousPeriodInput.isInputEmpty() && !deathRateInput.isInputEmpty()) // Checks for proper inputs
            {
                try
                {
                    r0 = stof(rValueInput.getInput());
                    daysinfected = stoi(infectiousPeriodInput.getInput());
                    deathRate = stof(deathRateInput.getInput());
                    if (!run.isActivated())
                    {
                        run.setActivationState(true);
                        inputWarning.setString("");
                    }
                }
                catch (...)
                {
                    if (run.isActivated())
                        run.setActivationState(false);
                }
            }
            else if (run.isActivated())
                run.setActivationState(false);

        }

        // Linked the running of the algorithm to the drawing of the screen.
        // This way I can have a loading progress indicator and maintain the responsiveness of the GUI
        // Uses a seperate thread to further maintain GUI responsiveness

        window.clear(sf::Color(102, 178, 255, 255));

        window.draw(graph);

        window.draw(heatMap);
        window.draw(mapText);

        window.draw(deadKey);
        window.draw(deadText);
        window.draw(recoveredKey);
        window.draw(recoveredText);
        window.draw(infectedKey);
        window.draw(infectedText);
        window.draw(healthyKey);
        window.draw(healthyText);

        window.draw(rValueInput);
        window.draw(infectiousPeriodInput);
        window.draw(deathRateInput);

        window.draw(rValueLabel);
        window.draw(infectiousPeriodLabel);
        window.draw(deathRateLabel);

        window.draw(run);
        window.draw(inputWarning);

        window.draw(loading);
        window.draw(residentialInfections);
        window.draw(industrialInfections);
        window.draw(recreationalInfections);
        window.draw(logo);

        window.draw(deadStat);
        window.draw(recoveredStat);
        window.draw(infectedStat);
        window.draw(healthyStat);
        
        window.draw(programDescription);
        window.draw(sampleInputTitle);
        window.draw(sampleInput);

        window.display();
    }

    // Delete stat counters
    delete[] finalInfectionCounts;
    for (unsigned int i = 0; i < 180; i++)
        delete[] dailyStats[i];
    delete[] dailyStats;

    delete[] bounds;

    for (unsigned int i = 0; i < city.size(); i++) // Delete buildings.
        delete city[i];
    for (auto iter = population.begin(); iter != population.end(); iter++)
        delete iter->second;

    return 0;
};