#ifndef CELL_HPP
#define CELL_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

enum class TeamColor {
    RED, GREEN, BLUE
};

class Cell {
public:
     Cell(const sf::Vector2f& position, float radius, TeamColor color);
    void draw(sf::RenderWindow& window) const;
    bool shouldDivide();
    void update(float windowHeight, float windowWidth);
      std::vector<Cell> divide(std::mt19937& gen, std::uniform_real_distribution<>& disOffset);
      sf::CircleShape& getShape() { return shape; }

private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool divided;

    std::mt19937 gen;
    std::uniform_real_distribution<> distrAngle;
};

class Simulation {
public:

    Simulation(sf::RenderWindow& window);
    void update(float windowHeight, float windowWidth);
    void display();
    void draw(sf::RenderWindow& window);
    void addCells(const std::vector<Cell>& newCells); // Only the declaration here
    void checkCollisionsAndSwallow() ;
    

private:
    std::vector<Cell> cells;
    sf::RenderWindow& window;

    std::mt19937 gen;
    std::uniform_real_distribution<> disRadius;
    std::uniform_real_distribution<> disPositionX;
    std::uniform_real_distribution<> disPositionY;
    std::uniform_real_distribution<> disOffset;

    
    

};

#endif // CELL_HPP
