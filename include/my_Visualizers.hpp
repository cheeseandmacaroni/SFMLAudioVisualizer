#pragma once
#include "my_FT.hpp"
#include <SFML/Graphics.hpp>
#define Pi 3.14159265359
#define IX(i,j) (i + j * (nodesX + 2))

class Visualizer
{
public:
	virtual void update(FurierTransform &FT) = 0;
	virtual void drawGraph(sf::RenderWindow &window) = 0;
	const sf::Vector2f& getSize() { return size; }
	const sf::Vector2f& getPos() { return pos; }
	const int getMultiplier() { return amplitudeMultiplier; };
	const int getOffset() { return offset; };
protected:
	sf::Vector2f pos;
	sf::Vector2f size;
	float offset;
	float amplitudeMultiplier;
	int points;
	int samples;
};

class BasicVisualizer :public Visualizer
{
public:
	BasicVisualizer(sf::Vector2f pos, sf::Vector2f size, float offset, float amplitudeMultiplier, int points);
	void update(FurierTransform &FT);
	void drawGraph(sf::RenderWindow &window) { window.draw(graph); };
private:
	sf::VertexArray startPos;
	sf::VertexArray graph;
};

class ThreeDimensionalVisualizer : public Visualizer
{
public:
	ThreeDimensionalVisualizer(sf::Vector2f pos, sf::Vector2f size, float offset, float amplitudeMultiplier, int points);
	void update(FurierTransform &FT);
	void drawGraph(sf::RenderWindow &window) { for(sf::VertexArray graph:graphs) window.draw(graph); };
private:
	int amountOfGraphs;
	std::vector<sf::VertexArray> startPos;
	std::vector<sf::VertexArray> graphs;
};

class FluidVisualizer :public Visualizer
{
public:
	FluidVisualizer(sf::Vector2f pos, sf::Vector2f size, float offset, float amplitudeMultiplier, int points);
	void update(FurierTransform &FT);
	void drawGraph(sf::RenderWindow &window);
private:
	int nodesX;
	int nodesY;
	int gridSize;//includes boundary cells
	std::vector<float> density;//density of each cell, must be between 1 and 0
	std::vector<float> densityNext;
	std::vector<float> U;//horisontal part of velocity
	std::vector<float> Unext;
	std::vector<float> V;//vertical part of velocity
	std::vector<float> Vnext;
	sf::Vector2f nodeSize;
	sf::Vector2f circleCentre;
	std::vector<sf::RectangleShape> nodes;//drawable
	std::vector<sf::VertexArray> vectorsToDraw;//debug vector field
	//vector<sf::VertexArray> vectorsToDraw;
	float radius;//in pixels
	void addDensity(sf::Vector2i pos);
	void addVelocity(sf::Vector2i pos, sf::Vector2f direction, float scale);
	void diffuse(int b, std::vector<float> &next, std::vector<float> &cur, float diff, float dt);
	void project(std::vector<float> &u, std::vector<float> &v, std::vector<float> &p, std::vector<float> &div);
	void advect(int b, std::vector<float> &next, std::vector<float> &cur, std::vector<float> &u, std::vector<float> &v, float dt);
	void setBoundary(int b, std::vector<float> &x);
	void solve(float dt, float visc, float diff);
	void decreaseDensity(float rate);
};