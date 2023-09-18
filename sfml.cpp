#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include "cell.hpp"


// Constants
const float MIN_RADIUS = 5.0f;
const float MAX_RADIUS = 20.0f;
const int NUM_CELLS_PER_TEAM = 10;

// Utility Functions
sf::Vector2f randomPosition(unsigned int windowWidth, unsigned int windowHeight) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> distrX(0, windowWidth);
    std::uniform_real_distribution<float> distrY(0, windowHeight);

    return sf::Vector2f(distrX(gen), distrY(gen));
}

float randomRadius() {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> distr(MIN_RADIUS, MAX_RADIUS);

    return distr(gen);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 600), "Cell Simulation");
    Simulation simulation(window);  

    sf::Vector2u windowSize = window.getSize();
    unsigned int windowWidth = windowSize.x;
    unsigned int windowHeight = windowSize.y;

    std::cout << "Window Width: " << windowWidth << ", Window Height: " << windowHeight << std::endl;
   
    // Initialization of cells
    std::vector<Cell> initialCells; 
    for (int i = 0; i < NUM_CELLS_PER_TEAM; ++i) {
        initialCells.push_back(Cell(randomPosition(windowWidth, windowHeight), randomRadius(), TeamColor::RED));
        initialCells.push_back(Cell(randomPosition(windowWidth, windowHeight), randomRadius(), TeamColor::BLUE));
        initialCells.push_back(Cell(randomPosition(windowWidth, windowHeight), randomRadius(), TeamColor::GREEN));
    }
    simulation.addCells(initialCells);

    // Run initial updates on simulation
    for (int i = 0; i <= 1; ++i) {
        simulation.update(window.getSize().x, window.getSize().y);
        simulation.display();
    }


 bool simulationStarted = false;


// Main loop
while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        // Handle button click event
        if (!simulationStarted && event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (simulation.getStartButton().getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                simulation.setSimulationRunningToTrue();
                simulationStarted = true;
            }
        }
    }

    window.clear(); // Clear the window

    if (simulation.isSimulationRunning()) {
        simulation.update(window.getSize().x, window.getSize().y);
        simulation.checkCollisionsAndSwallow();  // Check for collisions and handle them
        simulation.display();
    } else {
        window.draw(simulation.getStartButton());

// Load the font and create the text object
sf::Font font;
if (font.loadFromFile("/Users/sam/Desktop/coding projects/Cell division/Cell-Divison/fonts/Arial.ttf")) {
    sf::Text buttonText("START", font, 24);
    buttonText.setFillColor(sf::Color::Black);

    // Calculate the center position for the text
    float textPosX = simulation.getStartButton().getPosition().x +
                     (simulation.getStartButton().getSize().x - buttonText.getLocalBounds().width) / 2;
    float textPosY = simulation.getStartButton().getPosition().y +
                     (simulation.getStartButton().getSize().y - buttonText.getLocalBounds().height) / 2;
    
    // Vertically center the text
    textPosY -= buttonText.getLocalBounds().top;
    
    buttonText.setPosition(textPosX, textPosY);

    // Draw the text on the start button
    window.draw(buttonText);
}


    }

    window.display();
}





    return 0;
}
