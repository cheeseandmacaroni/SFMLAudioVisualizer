#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include "my_Visualizers.hpp"
#include "global_state.cpp"

class EventListener
{
public:
	virtual void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer) = 0;
};

class EventManager
{
public:
	void subscribe(EventListener *listener);
	void notify(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
private:
	std::vector<EventListener*> listeners;
};

class MyButton: public EventListener
{
public:
	virtual void onHover() = 0;
	virtual void onDefault() = 0;
	virtual void draw(sf::RenderWindow &renderWindow) = 0;
protected:
	sf::Vector2f size;
	sf::Vector2f pos;
	sf::RectangleShape body;
	sf::Color fillColor;
};

class PlayButton: public MyButton
{
public:
	PlayButton(sf::Vector2f windowPos, sf::Vector2f windowSize);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover();
	void onDefault();
	void draw(sf::RenderWindow &renderWindow);
private:
	sf::Texture playTexture;
	sf::Texture pauseTexture;
};

class SkipForward:public MyButton
{
public:
	SkipForward(sf::Vector2f windowPos, sf::Vector2f windowSize);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover();
	void onDefault();
	void draw(sf::RenderWindow &renderWindow);
private:
	sf::Texture texture;
};

class SkipBackward:public MyButton
{
public:
	SkipBackward(sf::Vector2f pos, sf::Vector2f Size);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover();
	void onDefault();
	void draw(sf::RenderWindow &renderWindow);
private:
	sf::Texture texture;
};

class ProgressBar : public MyButton
{
public:
	ProgressBar(sf::Vector2f mainWindowSize, float offset);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover();
	void onDefault();
	void draw(sf::RenderWindow &renderWindow);
private:
	bool mouseButtonHold = false;
	sf::Color fillColor;
	sf::Color innerFillColor;
	sf::Color active;
	sf::RectangleShape innerBody;
};

class FileButton :public MyButton
{
public:
	bool canBePressed = true;
	FileButton(sf::Vector2f pos, sf::Vector2f size, int index, const sf::Font &font, const std::string &path, const std::string &name);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover();
	void onDefault();
	void draw(sf::RenderWindow &renderWindow);
	void setPosition(sf::Vector2f newPosition);
	const sf::Vector2f getPosition(){ return pos; }
private:
	std::string path;
	int index;
	sf::Color textColor;
	sf::Color activeTextColor;
	sf::Text textToshow;
};

class FileList :public MyButton
{
public:
	FileList(sf::Vector2f pos, sf::Vector2f size, const sf::Font &font, const std::vector<std::string> &paths, const std::vector<std::string> &names, int maxButtonsToRender);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover() { };
	void onDefault() { };
	void draw(sf::RenderWindow &renderWindow);
	const std::vector<FileButton*> getButtonsArray() { return buttons; }
private:
	int firstButtonIndex = 0;
	int buttonsInWindow;
	sf::Vector2f size;
	sf::Vector2f pos;
	std::vector<FileButton*> buttons;
};

class UnfoldingList:public MyButton
{
public:
	UnfoldingList(sf::Vector2f pos, sf::Vector2f buttonSize, std::vector<std::string> &items, std::string headline, sf::Font &font);
	void update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer);
	void onHover();
	void onDefault();
	void draw(sf::RenderWindow &renderWindow);
private:
	bool folded;
	int activebuttonindex;
	std::vector<std::string> items;
	std::vector<sf::Text> texts;
	std::vector<sf::RectangleShape> bodies;
	sf::Vector2f buttonSize;
	sf::Text textheadline;
	sf::Font font;
	sf::Color textColor;
	sf::Color activeTextColor;
	sf::Color outlineColor;
	sf::CircleShape triangle;
	void swapTexts(sf::Text &a, sf::Text &b);
};