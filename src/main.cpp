#include "my_FT.hpp"
#include "my_UI.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <math.h>
#include <filesystem>
#include <Windows.h>
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32")

using namespace std;

class SettingsWindow
{
public:
	vector<MyButton*> buttons;
	SettingsWindow(sf::Vector2f mainWindowSize, float offset, sf::Font &font)
	{
		backgroundColor = sf::Color::Black;
		size.x = (mainWindowSize.x - 3 * offset) / 2;
		size.y = (mainWindowSize.y - 3 * offset) * 0.285;
		pos.x = (offset + mainWindowSize.x)/2;
		pos.y = mainWindowSize.y - offset - size.y;
		background.setPosition(pos);
		background.setSize(size);
		background.setFillColor(backgroundColor);
		visTypes = { "basic" , "3D", "fluid" };
		buttons.push_back(new UnfoldingList(sf::Vector2f(pos.x +offset, pos.y + offset), sf::Vector2f(size.x/3, size.y/12), visTypes, "Visualizer type:", font));
	}
	void renderEverything(sf::RenderWindow &window)
	{
		window.draw(background);
		for (MyButton* list : buttons)
			list->draw(window);
	}
private:
	vector<string> visTypes;
	sf::Vector2f pos;
	sf::Vector2f size;
	sf::RectangleShape background;
	sf::Color backgroundColor;
};

class VisualizerWindow
{
public:
	Visualizer *visualizer;
	vector<MyButton*> buttons;
	VisualizerWindow(sf::Vector2f mainWindowSize, float offset, int type)
	{
		backgroundColor = sf::Color::Black;
		pos.x = pos.y = offset;
		size.x = mainWindowSize.x - 2 * offset;
		size.y = (mainWindowSize.y - 3 * offset) * 0.7;
		background.setSize(sf::Vector2f(size.x, size.y));
		background.setPosition(sf::Vector2f(pos.x, pos.y));
		background.setFillColor(backgroundColor);
		buttons.push_back(new PlayButton(pos, size));
		buttons.push_back(new ProgressBar(mainWindowSize, offset));
		buttons.push_back(new SkipForward(pos, size));
		buttons.push_back(new SkipBackward(pos, size));
		switch (type)
		{
		case 0:
			visType = basic; visualizer = new BasicVisualizer(pos, size + sf::Vector2f(0, -20) , size.x * 0.01, 1000, size.x * 0.7); break;
		case 1:
			visType = threeD; visualizer = new ThreeDimensionalVisualizer(pos, size + sf::Vector2f(0, -20), size.x * 0.01, 1000, size.x * 0.5); break;
		}
	}
	void update(FurierTransform &FT) { visualizer->update(FT); }
	void renderEverything(sf::RenderWindow &window)
	{
		window.draw(background);
		visualizer->drawGraph(window);
		for (int i = 0; i < buttons.size(); ++i)
			buttons[i]->draw(window);
	}
private:
	enum type
	{
		basic,
		threeD
	};
	sf::Vector2f pos;
	sf::Vector2f size;
	sf::RectangleShape background;
	sf::Color backgroundColor;
	type visType;
};

class SoundManager
{
public:
	void updatePaths()
	{
		try
		{
			GetModuleFileNameW(NULL, exeDirectory, MAX_PATH);
			int j = 259;
			while (exeDirectory[j] != '\\')
			{
				--j;
			}
			std::string soundDirectoryPath;
			for (int i = 0; i < j; ++i)
			{
				soundDirectoryPath += exeDirectory[i];
			}
			int i = 0;
			for (const auto & entry : std::filesystem::directory_iterator(soundDirectoryPath + "\\" + soundDirectoryName))
			{
				if (extension(entry.path().string()) == "wav")
				{
					paths.push_back(entry.path().string());
					names.push_back(findName(entry.path().string()));
				}
			}
		}
		catch (std::exception &exception)
		{
			MessageBox(GetActiveWindow(), exception.what(), "Error", MB_ICONERROR);
			exit(4);
		}
	}
	const vector<string>& getPaths() { return paths; }
	const vector<string>& getNames() { return names; }
private:
	vector<string> paths;
	vector<string> names;
	sf::Font font;
	string soundDirectoryName = "sounds";
	WCHAR exeDirectory[260];
	string extension(const string &str)
	{
		std::string res;
		int i = str.length() - 1;
		while (i > 0 && str[i] != '.')
		{
			res += str[i--];
		}
		std::reverse(res.begin(), res.end());
		return res;
	}
	string findName(const string &path)
	{
		string res;
		int j = path.size() - 5;
		while (path[j] != '\\')
		{
			res += path[j];
			j--;
		}
		std::reverse(res.begin(), res.end());
		return res;
	}
};

class PlayListWindow
{
public:
	SoundManager soundManager;
	FileList *filelist;
	PlayListWindow(sf::Vector2f mainWindowSize, float offset, sf::Font &font)
	{
		size.x = (mainWindowSize.x - 3 * offset) / 2;
		size.y = (mainWindowSize.y - 3 * offset) * 0.285;
		pos.x = offset;
		pos.y = mainWindowSize.y - offset - size.y;
		backgroundColor = sf::Color::Black;
		background.setFillColor(backgroundColor);
		background.setPosition(pos);
		background.setSize(size);
		hideRight.setFillColor(sf::Color(131, 135, 136, 255));
		hideRight.setPosition(sf::Vector2f(pos.x + size.x, 0));
		hideRight.setSize(sf::Vector2f(mainWindowSize.x - size.x, mainWindowSize.y));

		soundManager.updatePaths();
		amountOfSongs = soundManager.getNames().size();

		textFont = font;

		filelist = new FileList(pos, size, font, soundManager.getPaths(), soundManager.getNames(), buttonsInWindow);
	}
	void renderEverything(sf::RenderWindow &window)
	{
		window.draw(background);
		filelist->draw(window);
		window.draw(hideRight);
	}
private:
	int amountOfSongs;
	int buttonsInWindow = 10;
	sf::Font textFont;
	sf::Vector2f pos;
	sf::Vector2f size;
	sf::RectangleShape background;
	sf::RectangleShape hideRight; //if file name is too long and can't be fit in button
	sf::Color backgroundColor;
};

class MainWindow
{
public:
	GlobalState songState;
	EventManager eventManager;
	MainWindow(sf::Vector2f size, string windowName, int maxFPS, float offset, int antialiasing)
	{
		try
		{
			this->size = size;
			this->offset = offset;
			this->windowName = windowName;
			textFont.loadFromFile("fonts/LEMONMILK-Medium.otf");
			settings.antialiasingLevel = antialiasing;
			renderWindow.create(sf::VideoMode(size.x, size.y), windowName, sf::Style::Close, settings);
			renderWindow.setFramerateLimit(maxFPS);
			renderWindow.setVerticalSyncEnabled(true);
			renderWindow.setKeyRepeatEnabled(false);
			visualizerWindow = new VisualizerWindow(size, offset, 0);
			for (MyButton* button : visualizerWindow->buttons)
			{
				eventManager.subscribe(button);
			}
			playlistWindow = new PlayListWindow(size, offset, textFont);
			eventManager.subscribe(playlistWindow->filelist);
			for (MyButton* button : playlistWindow->filelist->getButtonsArray())
			{
				eventManager.subscribe(button);
			}
			songState.init(playlistWindow->soundManager.getPaths(), playlistWindow->soundManager.getNames());
			settingsWindow = new SettingsWindow(size, offset, textFont);
			for (MyButton* list : settingsWindow->buttons)
			{
				eventManager.subscribe(list);
			}
			if (size.x < minSize.x || size.y < minSize.y)	throw std::exception("Invalid window size");
			if (antialiasing < 0)	throw std::exception("Invalid antialiasing value");
		}
		catch (std::exception& error)
		{
			MessageBox(GetActiveWindow(), error.what(), "Error", MB_ICONERROR);
			exit(1);
		}
	}
	sf::RenderWindow & getWindow() { return renderWindow; }
	void updateVisualizer(FurierTransform& FT) { visualizerWindow->visualizer->update(FT); }
	void myRender()
	{
		renderWindow.clear(backgroundColor);
		playlistWindow->renderEverything(renderWindow);
		visualizerWindow->renderEverything(renderWindow);
		settingsWindow->renderEverything(renderWindow);
		renderWindow.display();
	}
	vector<string> getSongPaths()
	{
		return playlistWindow->soundManager.getPaths();
	}
	Visualizer* &getVisualizer() { return visualizerWindow->visualizer; };
private:
	float offset;
	sf::Vector2f size;
	sf::Vector2f minSize = sf::Vector2f(600, 600);
	sf::Font textFont;
	string windowName;
	sf::Color backgroundColor = sf::Color(131, 135, 136, 255);
	sf::ContextSettings settings;
	sf::RenderWindow renderWindow;
	VisualizerWindow* visualizerWindow;
	PlayListWindow* playlistWindow;
	SettingsWindow* settingsWindow;
};

int main(int argc, char* argv[])
{
	int maxFramesPerSecond = 60;

	//setting the graph updates per second and render window fps gives a better perfomance
	sf::Time minimalDeltaTime = sf::seconds(1./maxFramesPerSecond);//keeps the minimal time interval between visualizer updates (myWindow.updateVisualizer func)
	sf::Clock timeBetweenUpdates;//measures the time between visualizer updates
	sf::SoundBuffer buffer;
	sf::Sound sound;

	FurierTransform mainFFT(8192);
	MainWindow myWindow(sf::Vector2f(800, 600), "MV", maxFramesPerSecond, 8., 8);

	try
	{
		if (!buffer.loadFromFile(myWindow.getSongPaths()[0]))
		{
			throw std::exception("Couldn't load the .WAV file, check the /sounds directory or your audio device");
		}

		sound.setBuffer(buffer);
		sound.setLoop(0);
		sound.play();
		sound.pause();
		while (myWindow.getWindow().isOpen())
		{
			timeBetweenUpdates.restart();
			sf::Event event;
			sf::Vector2f mousePos;
			mousePos.x = sf::Mouse::getPosition(myWindow.getWindow()).x;
			mousePos.y = sf::Mouse::getPosition(myWindow.getWindow()).y;
			while (myWindow.getWindow().pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
					myWindow.getWindow().close();
					break;
				default:
					myWindow.eventManager.notify(event, mousePos, sound, myWindow.songState, buffer, myWindow.getVisualizer());
					break;
				}
			}
			if (!myWindow.getWindow().pollEvent(event))//to make progressbar updating
			{
				event.type = sf::Event::MouseMoved;
				myWindow.eventManager.notify(event, mousePos, sound, myWindow.songState, buffer, myWindow.getVisualizer());
			}
			if ((sound.getPlayingOffset().asSeconds() * buffer.getSampleRate() + mainFFT.getSamples()) * buffer.getChannelCount() <= buffer.getSampleCount() && sound.getStatus() == sf::Sound::Status::Playing)
			{
				mainFFT.copyToMagnitudeBuffer(buffer.getSamples() + sizeof(sf::Int16) * (int)(sound.getPlayingOffset().asSeconds() * buffer.getSampleRate()), buffer.getChannelCount());
				mainFFT.processTheBuffer();
				myWindow.updateVisualizer(mainFFT);
			}
			if (sound.getStatus() == sf::Sound::Status::Stopped)
			{
				myWindow.songState.currentSongIndex = (myWindow.songState.currentSongIndex + 1) % myWindow.songState.getPaths().size();
				buffer.loadFromFile(myWindow.songState.getPaths()[myWindow.songState.currentSongIndex]);
				sound.setBuffer(buffer);
				sound.play();
			}
			while (timeBetweenUpdates.getElapsedTime().asSeconds() < minimalDeltaTime.asSeconds())
			{
				Sleep(0.001);
			}
			myWindow.getWindow().setTitle(to_string(1. / timeBetweenUpdates.getElapsedTime().asSeconds()));//for fps check
			myWindow.myRender();
		}
	}
	catch (std::exception& error)
	{
		MessageBox(GetActiveWindow(), error.what(), "Error", MB_ICONERROR);
		exit(2);
	}

	return 0;
}