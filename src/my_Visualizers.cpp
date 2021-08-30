#include "my_Visualizers.hpp"

BasicVisualizer::BasicVisualizer(sf::Vector2f pos, sf::Vector2f size, float offset, float amplitudeMultiplier, int points)
{
	this->pos = pos;
	this->size = size;
	this->offset = offset;
	this->amplitudeMultiplier = amplitudeMultiplier;
	this->points = points;
	graph.setPrimitiveType(sf::PrimitiveType::LinesStrip);
	startPos.resize(points);
	graph.resize(points);
	for (int i = 0; i < points; ++i)
	{
		startPos[i].position = graph[i].position = sf::Vector2f(pos.x + offset + (size.x - 2 * offset) / points * i, pos.y + size.y / 2);
	}
}
void BasicVisualizer::update(FurierTransform &FT)
{
	samples = FT.getSamples();
	std::vector<double> FTbuffer = FT.getMagnitudes();
	std::vector<int> borders(points + 1);
	double baseOfTheLogarithm = pow(points + 1, 1. / (samples / 2));
	for (int i = 0; i < points + 1; i++)
	{
		borders[i] = (int)(log10(i + 1) / log10(baseOfTheLogarithm));
		borders[i] = (samples / 2) - borders[i];
	}
	std::reverse(borders.begin(), borders.end());
	for (int i = 0; i < points; i++)
	{
		float tmp = 0;
		if (borders[i] != borders[i + 1]) {
			for (int j = borders[i]; j < borders[i + 1]; j++)
			{
				tmp += FTbuffer[j];
			}
			tmp /= (borders[i + 1] - borders[i]);
			graph[i].position = sf::Vector2f(startPos[i].position.x, startPos[i].position.y - amplitudeMultiplier * (log10(i) / log10(points)) *(tmp));
		}
		else if (borders[i] == borders[i + 1])
		{
			graph[i].position = sf::Vector2f(startPos[i].position.x, startPos[i].position.y - amplitudeMultiplier * (log10(i) / log10(points)) *(FTbuffer[i]));
		}
		if (graph[i].position.y < pos.y + offset) graph[i].position.y = pos.y + offset;
	}
	/*float tmp;
	int k = 0;
	for (int i = 0; i < FT.samples / 2; i += FT.samples / 2 / points)
	{
		tmp = 0;
		for (int j = 0; j < FT.samples / 2 / points; j++)
		{
			tmp += FT.res[i + j];
		}
		tmp /= (FT.samples / 2 / points);
		FT.res[k++] = tmp;
	}
	int j = 0;
	for (int i = 0; i < points; i++)
	{
		finalPos[0][i].position = sf::Vector2f(startPos[0][i].position.x, startPos[0][i].position.y - amplitudeMultiplier * (FT.res[j++]));
	}*/
}

ThreeDimensionalVisualizer::ThreeDimensionalVisualizer(sf::Vector2f pos, sf::Vector2f size, float offset, float amplitudeMultiplier, int points)
{
	this->pos = pos;
	this->size = size;
	this->offset = offset;
	this->amplitudeMultiplier = amplitudeMultiplier;
	this->points = points;
	amountOfGraphs = size.y / 8;
	startPos.resize(amountOfGraphs);
	graphs.resize(amountOfGraphs);

	for (int i = 0; i < amountOfGraphs; ++i)
	{
		startPos[i].resize(points);
		graphs[i].resize(points);
		graphs[i].setPrimitiveType(sf::PrimitiveType::LinesStrip);
		for (int j = 0; j < points; ++j)
		{
			startPos[i][j].position = graphs[i][j].position = sf::Vector2f(pos.x + offset + (size.x - 2 * offset) / points * j, pos.y + size.y - offset - i * ((size.y - 2 * offset) / amountOfGraphs));
			if (i == 0) continue;
			graphs[i][j].color = sf::Color(255, 255, 255, 80);
		}
	}
}

void ThreeDimensionalVisualizer::update(FurierTransform & FT)
{
	float dif = startPos[1][0].position.y - startPos[0][0].position.y;
	for (int i = amountOfGraphs - 1; i > 0; --i)
	{
		for (int j = 0; j < points; ++j)
		{
			graphs[i][j].position = sf::Vector2f(0,dif) + graphs[i - 1][j].position;
			if (graphs[i][j].position.y < pos.y) graphs[i][j].position.y = pos.y + 1;
		}
	}
	samples = FT.getSamples();
	std::vector<double> FTbuffer = FT.getMagnitudes();
	std::vector<int> borders(points + 1);
	double baseOfTheLogarithm = pow(points + 1, 1. / (samples / 2));
	for (int i = 0; i < points + 1; i++)
	{
		borders[i] = (int)(log10(i + 1) / log10(baseOfTheLogarithm));
		borders[i] = (samples / 2) - borders[i];
	}
	std::reverse(borders.begin(), borders.end());
	for (int i = 0; i < points; i++)
	{
		float tmp = 0;
		if (borders[i] != borders[i + 1]) {
			for (int j = borders[i]; j < borders[i + 1]; j++)
			{
				tmp += FTbuffer[j];
			}
			tmp /= (borders[i + 1] - borders[i]);
			graphs[0][i].position = sf::Vector2f(startPos[0][i].position.x, startPos[0][i].position.y - amplitudeMultiplier * (log10(i) / log10(points)) *(tmp));
		}
		else if (borders[i] == borders[i + 1])
		{
			graphs[0][i].position = sf::Vector2f(startPos[0][i].position.x, startPos[0][i].position.y - amplitudeMultiplier * (log10(i) / log10(points)) *(FTbuffer[i]));
		}
		if (graphs[0][i].position.y < pos.y + offset) graphs[0][i].position.y = pos.y + offset;
	}
}

FluidVisualizer::FluidVisualizer(sf::Vector2f pos, sf::Vector2f size, float offset, float amplitudeMultiplier, int points)
{
	this->offset = offset;
	this->amplitudeMultiplier = amplitudeMultiplier;
	this->points = points;
	this->size = size;
	this->pos = pos;
	nodesX = 250;
	radius = 90;
	nodeSize.x = (size.x - 2 * offset) / nodesX;
	nodeSize.y = nodeSize.x;
	nodesY = (int)(size.y - 2 * offset) / nodeSize.y;
	circleCentre.x = (nodesX / 2 + 1)*nodeSize.x + offset;
	circleCentre.y = (nodesY / 2 + 1)*nodeSize.y + offset;
	gridSize = (nodesX + 2) * (nodesY + 2);
	density.resize(gridSize);
	densityNext.resize(gridSize);
	U.resize(gridSize);
	Unext.resize(gridSize);
	V.resize(gridSize);
	Vnext.resize(gridSize);

	nodes.resize(gridSize);
	//vectorsToDraw.resize(gridSize);
	for (int i = 0; i < nodesX + 2; ++i)
		for (int j = 0; j < nodesY + 2; ++j)
		{
			density[IX(i, j)] = densityNext[IX(i, j)] = U[IX(i, j)] = V[IX(i, j)] = Unext[IX(i, j)] = Vnext[IX(i, j)] = 0;
			nodes[IX(i, j)].setPosition(pos.x + offset + (i - 1) * nodeSize.x, pos.y+ offset + (j - 1) * nodeSize.y);
			nodes[IX(i, j)].setSize(nodeSize);
			nodes[IX(i, j)].setFillColor(sf::Color(255, 255, 255, 0));
			nodes[IX(i, j)].setOutlineColor(sf::Color::White);

			/*vectorsToDraw[IX(i, j)].resize(2);
			vectorsToDraw[IX(i, j)].setPrimitiveType(sf::PrimitiveType::Lines);
			vectorsToDraw[IX(i, j)][0].position = vectorsToDraw[IX(i, j)][1].position = sf::Vector2f((i - 1) * nodeSize.x + nodeSize.x / 2, (j - 1) * nodeSize.y + nodeSize.y / 2);
			vectorsToDraw[IX(i, j)][0].color = vectorsToDraw[IX(i, j)][1].color = sf::Color::Red;*/
		}

}

void FluidVisualizer::update(FurierTransform & FT)
{
	samples = FT.getSamples();
	std::vector<double> FTbuffer = FT.getMagnitudes();
	std::vector<int> borders(points + 1);
	double baseOfTheLogarithm = pow(points + 1, 1. / (samples / 2));
	for (int i = 0; i < points + 1; i++)
	{
		borders[i] = (int)(log10(i + 1) / log10(baseOfTheLogarithm));
		borders[i] = (samples / 2) - borders[i];
	}
	std::reverse(borders.begin(), borders.end());
	for (int i = 0; i < points; ++i)
	{
		float tmp = 0;
		int x, y;
		float x_d, y_d;
		x = (int)(circleCentre.x + radius * cos(-Pi / 2 + Pi / points * i) - pos.x) / nodeSize.x - 1;
		y = (int)(circleCentre.y + radius * sin(-Pi / 2 + Pi / points * i) - pos.y) / nodeSize.y - 1;
		x_d = circleCentre.x + radius * cos(-Pi / 2 + Pi / points * i);
		y_d = circleCentre.x + radius * sin(-Pi / 2 + Pi / points * i);
		if (borders[i] != borders[i + 1]) {
			for (int j = borders[i]; j < borders[i + 1]; ++j)
			{
				tmp += FTbuffer[j];
			}
			tmp /= (borders[i + 1] - borders[i]);
			addDensity(sf::Vector2i(x, y));
			addVelocity(sf::Vector2i(x, y), (sf::Vector2f(x*nodeSize.x, y*nodeSize.y) - circleCentre)*tmp, amplitudeMultiplier);
		}
		else if (borders[i] == borders[i + 1])
		{
			addDensity(sf::Vector2i(x, y));
			addVelocity(sf::Vector2i(x, y), (sf::Vector2f(x*nodeSize.x, y*nodeSize.y) - circleCentre) * (float)FTbuffer[i], amplitudeMultiplier);
		}
	}
	for (int i = 0; i < points; ++i)
	{
		float tmp = 0;
		int x, y;
		float x_d, y_d;
		x = (int)(circleCentre.x + radius * cos(Pi / 2 + Pi / points * i) - pos.x) / nodeSize.x - 1;
		y = (int)(circleCentre.y + radius * sin(Pi / 2 + Pi / points * i) - pos.y) / nodeSize.y - 1;
		x_d = circleCentre.x + radius * cos(Pi / 2 + Pi / points * i);
		y_d = circleCentre.x + radius * sin(Pi / 2 + Pi / points * i);
		if (borders[i] != borders[i + 1]) {
			for (int j = borders[i]; j < borders[i + 1]; ++j)
			{
				tmp += FTbuffer[j];
			}
			tmp /= (borders[i + 1] - borders[i]);
			addDensity(sf::Vector2i(x, y));
			addVelocity(sf::Vector2i(x, y), (sf::Vector2f(x*nodeSize.x, y*nodeSize.y) - circleCentre)*tmp, amplitudeMultiplier);
		}
		else if (borders[i] == borders[i + 1])
		{
			addDensity(sf::Vector2i(x, y));
			addVelocity(sf::Vector2i(x, y), (sf::Vector2f(x*nodeSize.x, y*nodeSize.y) - circleCentre) * (float)FTbuffer[i], amplitudeMultiplier);
		}
	}
	solve(0.2, 0.0001, 0.001);
}

void FluidVisualizer::drawGraph(sf::RenderWindow & window)
{
	for (int i = 1; i < nodesX + 1; ++i)
		for (int j = 1; j < nodesY + 1; ++j)
		{
			if (density[IX(i, j)] > 1)
			{
				nodes[IX(i, j)].setFillColor(sf::Color(255, 255, 255, 255));
			}
			else if (density[IX(i, j)] < 0)
			{
				nodes[IX(i, j)].setFillColor(sf::Color(255, 255, 255, 0));
			}
			else
			{
				nodes[IX(i, j)].setFillColor(sf::Color(255, 255, 255, 255 * density[IX(i, j)]));
			}
			window.draw(nodes[IX(i, j)]);
			/*vectorsToDraw[IX(i, j)][1].position = vectorsToDraw[IX(i, j)][0].position + sf::Vector2f(U[IX(i, j)] * nodeSize.x, V[IX(i, j)] * nodeSize.x);
			window.draw(vectorsToDraw[IX(i, j)]);*/
		}
}

void FluidVisualizer::addDensity(sf::Vector2i pos)
{
	density[IX(pos.x, pos.y)] = 1;
}

void FluidVisualizer::addVelocity(sf::Vector2i pos, sf::Vector2f direction, float scale)
{
	U[IX(pos.x, pos.y)] += direction.x * scale;
	V[IX(pos.x, pos.y)] += direction.y * scale;
}

void FluidVisualizer::diffuse(int b, std::vector<float> &next, std::vector<float> &cur, float diff, float dt)
{
	float a = dt * diff * nodesX * nodesY;
	for (int k = 0; k < 5; ++k)
	{
		for (int i = 1; i <= nodesX; ++i)
			for (int j = 1; j <= nodesY; ++j)
				next[IX(i, j)] = (cur[IX(i, j)] + a * (next[IX(i - 1, j)] + next[IX(i + 1, j)] + next[IX(i, (j - 1))] + next[IX(i, (j + 1))])) / (1 + 4 * a);

		setBoundary(b, next);
	}
}

void FluidVisualizer::project(std::vector<float> &u, std::vector<float> &v, std::vector<float> &p, std::vector<float> &div)
{
	for (int i = 1; i <= nodesX; ++i) {
		for (int j = 1; j <= nodesY; ++j) {
			div[IX(i, j)] = -0.5 * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
				v[IX(i, (j + 1))] - v[IX(i, (j - 1))]);
			p[IX(i, j)] = 0;
		}
	}
	setBoundary(0, div); setBoundary(0, p);
	for (int k = 0; k < 5; ++k) {
		for (int i = 1; i <= nodesX; ++i) {
			for (int j = 1; j <= nodesY; ++j) {
				p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] +
					p[IX(i, (j - 1))] + p[IX(i, (j + 1))]) / 4;
			}
		}
		setBoundary(0, p);
	}
	for (int i = 1; i <= nodesX; ++i) {
		for (int j = 1; j <= nodesY; ++j) {
			u[IX(i, j)] -= 0.5*(p[IX((i + 1), j)] - p[IX((i - 1), j)]);
			v[IX(i, j)] -= 0.5*(p[IX(i, (j + 1))] - p[IX(i, (j - 1))]);
		}
	}
	setBoundary(1, u); setBoundary(2, v);
}

void FluidVisualizer::advect(int b, std::vector<float> &next, std::vector<float> &cur, std::vector<float> &u, std::vector<float> &v, float dt)
{
	int i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;
	//dt0 = dt * (nodesX + nodesY / 2.);
	for (int i = 1; i <= nodesX; ++i) {
		for (int j = 1; j <= nodesY; ++j) {
			x = i - dt * u[IX(i, j)]; y = j - dt * v[IX(i, j)];
			if (x < 0.5) x = 0.5; if (x > nodesX + 0.5) x = nodesX + 0.5; i0 = (int)x; i1 = i0 + 1;
			if (y < 0.5) y = 0.5; if (y > nodesY + 0.5) y = nodesY + 0.5; j0 = (int)y; j1 = j0 + 1;
			s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
			next[IX(i, j)] = s0 * (t0 * cur[IX(i0, j0)] + t1 * cur[IX(i0, j1)]) +
				s1 * (t0 * cur[IX(i1, j0)] + t1 * cur[IX(i1, j1)]);
		}
	}
	setBoundary(b, next);
}

void FluidVisualizer::setBoundary(int b, std::vector<float> &x)
{
	for (int i = 1; i <= nodesY; ++i) {
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX((nodesX + 1), i)] = b == 1 ? -x[IX(nodesX, i)] : x[IX(nodesX, i)];
	}
	for (int i = 1; i <= nodesX; ++i) {
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, (nodesY + 1))] = b == 2 ? -x[IX(i, nodesY)] : x[IX(i, nodesY)];
	}
	x[IX(0, 0)] = 0.5*(x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, (nodesY + 1))] = 0.5*(x[IX(1, (nodesY + 1))] + x[IX(0, nodesY)]);
	x[IX((nodesX + 1), 0)] = 0.5*(x[IX(nodesX, 0)] + x[IX((nodesX + 1), 1)]);
	x[IX((nodesX + 1), (nodesY + 1))] = 0.5*(x[IX(nodesX, (nodesY + 1))] + x[IX((nodesX + 1), nodesY)]);
}

void FluidVisualizer::solve(float dt, float visc, float diff)
{
	diffuse(1, Unext, U, visc, dt);
	diffuse(2, Vnext, V, visc, dt);

	project(Unext, Vnext, U, V);

	advect(1, U, Unext, Unext, Vnext, dt);
	advect(2, V, Vnext, Unext, Vnext, dt);

	project(U, V, Unext, Vnext);

	diffuse(0, densityNext, density, diff, dt);
	advect(0, density, densityNext, U, V, dt);

	decreaseDensity(dt/(points / 5));
}

void FluidVisualizer::decreaseDensity(float rate)
{
	for (int i = 0; i < gridSize; ++i)
	{
		density[i] -= rate;
		if (density[i] < 0) density[i] = 0;
	}
}