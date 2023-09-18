#include "cell.hpp"
#include <iostream>
#include <random>

float getDistance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}


// Define the helper function here
TeamColor mapSFColorToTeamColor(const sf::Color& color) {
    if (color == sf::Color::Red) return TeamColor::RED;
    if (color == sf::Color::Blue) return TeamColor::BLUE;
    if (color == sf::Color::Green) return TeamColor::GREEN;
    throw std::runtime_error("Unknown color!");  // Optional, as a safety check
}


Cell::Cell(const sf::Vector2f& position, float radius, TeamColor teamColor) 
: shape(radius),
divided(false),
gen(std::random_device{}()), 
distrAngle(0.0, 360.0) 
{
    shape.setPosition(position);
    shape.setRadius(radius);

    // Map TeamColor to sf::Color
    sf::Color fillColor;
    switch (teamColor) {
        case TeamColor::RED:
            fillColor = sf::Color::Red;
            break;
        case TeamColor::BLUE:
            fillColor = sf::Color::Blue;
            break;
        case TeamColor::GREEN:
            fillColor = sf::Color::Green;
            break;
        default:
            fillColor = sf::Color::White;  // Default color (shouldn't happen, but just in case)
    }
    shape.setFillColor(fillColor);

    shape.setOrigin(radius, radius); // This will set the center of the circle as its origin point.

    // Randomize the direction using the modern C++ random library
    float angle = distrAngle(gen) * (3.14159f / 180.0f); // Convert to radians
    velocity.x = 0.01f * std::cos(angle);
    velocity.y = 0.01f * std::sin(angle);
}


void Cell::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

bool Cell::shouldDivide() {
    // For simplicity, let's assume a cell divides if its radius > 50.
    return shape.getRadius() > 8;
}

void Cell::update(float windowWidth, float windowHeight){
    sf::Vector2f pos = shape.getPosition();
    
    // Add the movement
    pos += velocity;  // This will move the cell based on its velocity.


 // Bouncing effect
    float radius = shape.getRadius();  // Assuming your cell is a circle shape, get its radius.



    if (pos.x - radius <= 0 || pos.x + radius >= windowWidth) {
        velocity.x = -velocity.x;  // Reverse the x velocity
    }
    if (pos.y - radius <= 0 || pos.y + radius >= windowHeight) {
        velocity.y = -velocity.y;  // Reverse the y velocity
    }

    shape.setPosition(pos + velocity); // Update position with possibly adjusted velocity

    
}

std::vector<Cell> Cell::divide(std::mt19937& gen, std::uniform_real_distribution<>& disOffset) {
    std::vector<Cell> newCells;

    if (!divided) {
        divided = true;
        float newRadius = shape.getRadius() / 2;
        constexpr int numberOfCells = 10;
        float angleIncrement = 360.0f / numberOfCells;
        
        TeamColor currentTeam = mapSFColorToTeamColor(shape.getFillColor()); // Use the helper function

        for (int i = 0; i < numberOfCells; i++) {
            float angle = angleIncrement * i * (3.14159f / 180.0f); // Convert to radians
            sf::Vector2f newPos = shape.getPosition();
            
            newPos.x += cos(angle) * (newRadius * 2 + disOffset(gen));
            newPos.y += sin(angle) * (newRadius * 2 + disOffset(gen));

            newCells.emplace_back(newPos, newRadius, currentTeam); // Using TeamColor directly
        }
    }

    return newCells;
}




Simulation::Simulation(sf::RenderWindow& windowRef) 
    : window(windowRef), disRadius(5, 20), 
      disPositionX(5, windowRef.getSize().x - 20), 
      disPositionY(5, windowRef.getSize().y - 20),
      disOffset(-100, 100),  
      simulationRunning(false)
{
    gen.seed(std::random_device()());

    // Set up the start button
    startButton.setSize(sf::Vector2f(100, 50));  // Set the size of the button
    startButton.setFillColor(sf::Color::Green);  // Set the color of the button
    startButton.setPosition(windowRef.getSize().x / 2 - 50, windowRef.getSize().y / 2 - 25);  // Center the button


    // You can also add text to the button
    sf::Font font;
    if (font.loadFromFile("/Users/sam/Desktop/coding projects/Cell division/Cell-Divison/fonts/Arial.ttf")) { // Load a font (you may need to provide the correct font file)
        sf::Text buttonText("Start", font, 24); // Create a text object
        buttonText.setFillColor(sf::Color::White); // Set the text color
        buttonText.setPosition(startButton.getPosition().x + 20, startButton.getPosition().y + 10); // Adjust the text position
    }

    // Initialize 10 cells for each team:
    for (int i = 0; i < 10; ++i) {
        cells.emplace_back(sf::Vector2f(disPositionX(gen), disPositionY(gen)), disRadius(gen), TeamColor::RED);
        cells.emplace_back(sf::Vector2f(disPositionX(gen), disPositionY(gen)), disRadius(gen), TeamColor::GREEN);
        cells.emplace_back(sf::Vector2f(disPositionX(gen), disPositionY(gen)), disRadius(gen), TeamColor::BLUE);
    }
}




void Simulation::update(float windowHeight, float windowWidth) {
    
    if(simulationRunning){
    std::vector<Cell> newCells;
    for (Cell &cell : cells) {

     cell.update(windowWidth, windowHeight);
        // if (cell.shouldDivide()) {
        //     auto dividedCells = cell.divide(gen, disOffset);
        //     for (const auto& newCell : dividedCells) {
        //         newCells.push_back(newCell);
        //     }
        // }
    }

    for (const auto& newCell : newCells) {
        cells.push_back(newCell);
    }
    }
    
}

void Simulation::addCells(const std::vector<Cell>& newCells) {
    for (const auto& cell : newCells) {
        cells.push_back(cell);
    }
}



void Simulation::display() {
    for (const auto& cell : cells) {
        cell.draw(window);
    }
}

 void Simulation::checkCollisionsAndSwallow() {
    for (int i = 0; i < cells.size(); i++) {
        for (int j = i + 1; j < cells.size(); j++) {
            // Check if they're of different colors
            if (mapSFColorToTeamColor(cells[i].getShape().getFillColor()) !=
                mapSFColorToTeamColor(cells[j].getShape().getFillColor())) {

                float distance = getDistance(cells[i].getShape().getPosition(), cells[j].getShape().getPosition());
                float sumOfRadii = cells[i].getShape().getRadius() + cells[j].getShape().getRadius();

                if (distance < sumOfRadii) {  // They're colliding
                    if (cells[i].getShape().getRadius() < cells[j].getShape().getRadius()) {

                        // Remove cell[i]
                        cells.erase(cells.begin() + i);
                        i--;  // Decrement i to account for the removed cell
                        break;  // Exit the inner loop
                    } else {
                        // Remove cell[j]
                        cells.erase(cells.begin() + j);
                        j--;
                    }
                }
            }
        }
    }
}

bool Simulation:: isSimulationRunning() const {
    return simulationRunning;
}

const sf::RectangleShape& Simulation::getStartButton() const {
    return startButton;
}

void Simulation:: setSimulationRunningToTrue(){
    simulationRunning=true;
}
