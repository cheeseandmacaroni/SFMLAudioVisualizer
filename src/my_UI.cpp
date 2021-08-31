#include "my_UI.hpp" 
#define Pi 3.141592653

void EventManager::subscribe(EventListener * listener)
{
	listeners.push_back(listener);
}

void EventManager::notify(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer &soundBuffer, Visualizer* &visualizer)
{
	for (int i = 0; i < listeners.size(); ++i)
		listeners[i]->update(event, mousePos, sound, state, soundBuffer, visualizer);
}

PlayButton::PlayButton(sf::Vector2f windowPos, sf::Vector2f windowSize)
{
	size.x = 20;
	size.y = 20;
	pos.x = windowPos.x + windowSize.x / 2 - size.x / 2;
	pos.y = windowPos.y + windowSize.y - size.y;
	body.setFillColor(sf::Color::White);
	playTexture.loadFromFile("images/playButton.jpg");
	pauseTexture.loadFromFile("images/pauseButton.jpg");
	body.setTexture(&playTexture);
	body.setPosition(pos);
	body.setSize(size);
}

void PlayButton::update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer &soundBuffer, Visualizer* &visualizer)
{
	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
	{
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			if (state.curState == GlobalState::state::pause)
			{
				body.setTexture(&pauseTexture);
				sound.play();
				state.curState = GlobalState::state::playing;
			}
			else if (state.curState == GlobalState::state::playing)
			{
				body.setTexture(&playTexture);
				sound.pause();
				state.curState = GlobalState::state::pause;
			}
		}
		onHover();
	}
	else onDefault();

}

void PlayButton::onHover() { body.setFillColor(sf::Color(251, 128, 55, 255)); };

void PlayButton::onDefault() { body.setFillColor(sf::Color::White); };

void PlayButton::draw(sf::RenderWindow &renderWindow) { renderWindow.draw(body); };

ProgressBar::ProgressBar(sf::Vector2f mainWindowSize, float offset)
{
	size.x = mainWindowSize.x - 2 * offset;
	size.y = (mainWindowSize.y - 3 * offset) * 0.015;
	pos.x = offset;
	pos.y = 1.5 * offset + (mainWindowSize.y - 3 * offset)*0.7;
	fillColor = sf::Color::Black;
	innerFillColor = sf::Color(251, 128, 55, 255);
	active = sf::Color(251, 92, 55, 255);
	body.setFillColor(fillColor);
	body.setPosition(pos);
	body.setSize(size);
	innerBody.setFillColor(innerFillColor);
	innerBody.setPosition(pos);
	innerBody.setSize(sf::Vector2f(0, size.y));
}

void ProgressBar::update(sf::Event &event, sf::Vector2f mousePos, sf::Sound &sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer)
{
	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y && !mouseButtonHold)
	{
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			float newSize;
			mousePos.x - innerBody.getPosition().x >= body.getSize().x ? newSize = body.getSize().x : newSize = mousePos.x - innerBody.getPosition().x;
			if (newSize < 0) newSize = 0;
			innerBody.setSize(sf::Vector2f(newSize, innerBody.getSize().y));
			sound.setPlayingOffset(sf::seconds(sound.getBuffer()->getDuration().asSeconds() * (float)(innerBody.getSize().x / body.getSize().x)));
			sound.getPlayingOffset();
			sound.pause();
			mouseButtonHold = true;
		}
		onHover();
	}
	else if (mouseButtonHold && event.type == sf::Event::MouseButtonReleased)
	{
		if(state.curState == GlobalState::state::playing)
		sound.play();
		mouseButtonHold = false;
	}
	else if (mouseButtonHold)
	{
		float newSize;
		mousePos.x - innerBody.getPosition().x >= body.getSize().x ? newSize = body.getSize().x : newSize = mousePos.x - innerBody.getPosition().x;
		if (newSize < 0) newSize = 0;
		innerBody.setSize(sf::Vector2f(newSize, innerBody.getSize().y));
		sound.setPlayingOffset(sf::seconds(sound.getBuffer()->getDuration().asSeconds() * (float)(innerBody.getSize().x / body.getSize().x)));
		onHover();
	}
	else onDefault();
	innerBody.setSize(sf::Vector2f(sound.getPlayingOffset().asSeconds() / sound.getBuffer()->getDuration().asSeconds() * body.getSize().x, innerBody.getSize().y));
}

void ProgressBar::onHover() { innerBody.setFillColor(active); };

void ProgressBar::onDefault() { innerBody.setFillColor(innerFillColor); }

void ProgressBar::draw(sf::RenderWindow & renderWindow) { renderWindow.draw(body); renderWindow.draw(innerBody); };

FileButton::FileButton(sf::Vector2f pos, sf::Vector2f size, int index, const sf::Font &font, const std::string &path, const std::string &name)
{
	this->index = index;
	this->size = size;
	this->pos = pos;
	this->path = path;
	fillColor = activeTextColor = sf::Color(251, 128, 55, 255);
	textColor = sf::Color::White;
	body.setSize(size);
	body.setPosition(pos);
	body.setFillColor(sf::Color(0, 0, 0, 0));
	body.setOutlineThickness(-1.);
	body.setOutlineColor(fillColor);
	textToshow.setFont(font);
	textToshow.setCharacterSize(size.y - 4);
	textToshow.setPosition(sf::Vector2f(body.getPosition().x + 2, body.getPosition().y + 2));
	textToshow.setString(name);
}

void FileButton::update(sf::Event & event, sf::Vector2f mousePos, sf::Sound & sound, GlobalState &state, sf::SoundBuffer& soundBuffer, Visualizer* &visualizer)
{
	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y && canBePressed)
	{
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			try
			{
			if (state.currentSongIndex != index)
			{
				sound.pause();
				state.currentSongIndex = index;
				soundBuffer.loadFromFile(path);
				sound.setBuffer(soundBuffer);
				sound.play();
				if (state.curState == GlobalState::state::pause)
				{
					sound.pause();
				}
			}
			}
			catch (std::exception &error)
			{
				MessageBox(GetActiveWindow(), error.what(), "Error", MB_ICONERROR);
				exit(4);
			}
		}
		onHover();
	}
	else if (state.currentSongIndex == index)
	{
		onHover();
	}
	else
	{
		onDefault();
	}

}

void FileButton::onHover()
{
	textToshow.setFillColor(fillColor);
}

void FileButton::onDefault()
{
	textToshow.setFillColor(textColor);
}

void FileButton::draw(sf::RenderWindow & renderWindow)
{
	renderWindow.draw(textToshow);
	renderWindow.draw(body);
}

void FileButton::setPosition(sf::Vector2f newPosition)
{
	pos = newPosition;
	textToshow.setPosition(sf::Vector2f(newPosition.x + 2, newPosition.y + 2));
	body.setPosition(newPosition);
}

FileList::FileList(sf::Vector2f pos, sf::Vector2f size, const sf::Font &font, const std::vector<std::string> &paths, const std::vector<std::string> &names, int buttonsInWindow)
{
	this->size = size;
	this->pos = pos;
	this->buttonsInWindow = buttonsInWindow;
	for (int i = 0; i < names.size(); ++i)
	{
		buttons.push_back(new FileButton(sf::Vector2f(pos.x, pos.y + i * size.y / buttonsInWindow), sf::Vector2f(size.x, size.y / buttonsInWindow), i, font, paths[i], names[i]));
	}
}

void FileList::update(sf::Event & event, sf::Vector2f mousePos, sf::Sound & sound, GlobalState & state, sf::SoundBuffer & soundBuffer, Visualizer* &visualizer)
{
	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
	{
		if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.delta < 0 && buttons[buttons.size() - 1]->getPosition().y + size.y / buttonsInWindow > pos.y + size.y)
		{
			for (int i = 0; i < buttons.size(); ++i)
				buttons[i]->setPosition(sf::Vector2f(buttons[i]->getPosition().x, buttons[i]->getPosition().y - size.y / buttonsInWindow));
			firstButtonIndex++;
		}
		else if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.delta > 0 && buttons[0]->getPosition().y < pos.y)
		{
			for (int i = 0; i < buttons.size(); ++i)
				buttons[i]->setPosition(sf::Vector2f(buttons[i]->getPosition().x, buttons[i]->getPosition().y + size.y / buttonsInWindow));
			firstButtonIndex--;
		}
	}
}

void FileList::draw(sf::RenderWindow &renderWindow)
{
	for (int i = 0; i < buttons.size(); ++i)
		buttons[i]->canBePressed = false;
	int upBorder = 0;
	buttonsInWindow > buttons.size() - firstButtonIndex ? upBorder = buttons.size() : upBorder = buttonsInWindow + firstButtonIndex;
	for (int i = firstButtonIndex; i < upBorder; ++i)
	{
		buttons[i]->draw(renderWindow);
		buttons[i]->canBePressed = true;
	}
}

void SkipForward::onHover() { body.setFillColor(sf::Color(251, 128, 55, 255)); };

void SkipForward::onDefault() { body.setFillColor(fillColor); };

void SkipForward::draw(sf::RenderWindow &renderWindow) { renderWindow.draw(body); };

SkipForward::SkipForward(sf::Vector2f windowPos, sf::Vector2f windowSize)
{
	size.x = 20.;
	size.y = 20.;
	pos.x = windowPos.x + windowSize.x / 2 + size.x / 2;
	pos.y = windowPos.y + windowSize.y - size.y;
	fillColor = sf::Color::White;
	texture.loadFromFile("images/skipButton.jpg");
	body.setTexture(&texture);
	body.setPosition(pos);
	body.setSize(size);
	body.setFillColor(fillColor);
}

void SkipForward::update(sf::Event & event, sf::Vector2f mousePos, sf::Sound & sound, GlobalState & state, sf::SoundBuffer & soundBuffer, Visualizer* &visualizer)
{
	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
	{
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{

			state.currentSongIndex = (state.currentSongIndex + 1) % state.getNames().size();
			sound.pause();
			soundBuffer.loadFromFile(state.getPaths()[state.currentSongIndex]);
			sound.setBuffer(soundBuffer);
			sound.play();
			if (state.curState == GlobalState::state::pause)
			{
				sound.pause();
			}
		}
		onHover();
	}
	else onDefault();
}

SkipBackward::SkipBackward(sf::Vector2f windowPos, sf::Vector2f windowSize)
{
	size.x = 20.;
	size.y = 20.;
	pos.x = windowPos.x + windowSize.x / 2 - 1.5 * size.x;
	pos.y = windowPos.y + windowSize.y - size.y;
	fillColor = sf::Color::White;
	texture.loadFromFile("images/skipButton.jpg");
	body.setTexture(&texture);
	body.setScale(-1, 1);
	body.setPosition(sf::Vector2f(pos.x + size.x, pos.y));
	body.setSize(size);
	body.setFillColor(fillColor);
}

void SkipBackward::update(sf::Event & event, sf::Vector2f mousePos, sf::Sound & sound, GlobalState & state, sf::SoundBuffer & soundBuffer, Visualizer* &visualizer)
{
	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y)
	{
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			state.currentSongIndex == 0 ? state.currentSongIndex = state.getPaths().size() - 1 : --state.currentSongIndex;
			sound.pause();
			soundBuffer.loadFromFile(state.getPaths()[state.currentSongIndex]);
			sound.setBuffer(soundBuffer);
			sound.play();
			if (state.curState == GlobalState::state::pause)
			{
				sound.pause();
			}
		}
		onHover();
	}
	else onDefault();
}

void SkipBackward::onHover() { body.setFillColor(sf::Color(251, 128, 55, 255)); };

void SkipBackward::onDefault() { body.setFillColor(fillColor); };

void SkipBackward::draw(sf::RenderWindow & renderWindow) { renderWindow.draw(body); };

UnfoldingList::UnfoldingList(sf::Vector2f pos, sf::Vector2f buttonSize, std::vector<std::string>& items, std::string headline, sf::Font & font)
{
	folded = true;
	this->font = font;
	this->items = items;
	this->pos = pos;
	this->buttonSize = buttonSize;
	this->size = sf::Vector2f(buttonSize.x, (items.size() + 1)*buttonSize.y);
	textColor = sf::Color::White;
	activeTextColor = outlineColor = sf::Color(251, 128, 55, 255);
	textheadline.setString(headline);
	textheadline.setPosition(pos);
	textheadline.setFillColor(textColor);
	textheadline.setCharacterSize(buttonSize.y - 2);
	textheadline.setFont(font);
	for (int i = 0; i < items.size(); ++i)
	{
		texts.push_back(sf::Text());
		bodies.push_back(sf::RectangleShape());
		texts[i].setString(items[i]);
		texts[i].setPosition(sf::Vector2f(pos.x, pos.y + (i + 1) * buttonSize.y));
		texts[i].setFont(font);
		texts[i].setFillColor(textColor);
		texts[i].setCharacterSize(buttonSize.y - 2.);
		bodies[i].setFillColor(sf::Color::Black);
		bodies[i].setOutlineThickness(-1.);
		bodies[i].setOutlineColor(outlineColor);
		bodies[i].setPosition(texts[i].getPosition());
		bodies[i].setSize(buttonSize);
	}
	triangle.setPointCount(3);
	triangle.setRadius(buttonSize.y / 2 * 0.5);
	triangle.setPosition(sf::Vector2f(pos.x + buttonSize.x - 9 + 2 * triangle.getRadius()*cos(Pi/6), pos.y + buttonSize.y + 4 + 2 * triangle.getRadius()*cos(Pi / 6)));
	triangle.setRotation(180);
	triangle.setFillColor(sf::Color::White);
}

void UnfoldingList::update(sf::Event & event, sf::Vector2f mousePos, sf::Sound & sound, GlobalState & state, sf::SoundBuffer & soundBuffer, Visualizer* &visualizer)
{

	if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y + buttonSize.y && mousePos.y <= pos.y + 2 * buttonSize.y && folded)
	{
		activebuttonindex = 0;
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			folded = false;
			state.allListsFolded = false;
		}
	}
	else if(mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y + buttonSize.y && mousePos.y <= pos.y + 2 * buttonSize.y && !folded)
	{
		activebuttonindex = 0;
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			folded = true;
			state.allListsFolded = true;
		}
	}
	else if (mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y + 2 * buttonSize.y && mousePos.y <= pos.y + size.y && !folded)
	{
		for (int i = 1; i < items.size(); ++i)
		{
			if (mousePos.y >= pos.y + buttonSize.y * (i + 1) && mousePos.y <= pos.y + buttonSize.y * (i + 2))
				activebuttonindex = i;
		}
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			sf::Vector2f visSize = visualizer->getSize();
			sf::Vector2f visPos = visualizer->getPos();
			//int multiplier = visualizer->getMultiplier();
			float offset = visualizer->getOffset();
			if (items[activebuttonindex] == "3D")
			{
				delete visualizer;
				visualizer = new ThreeDimensionalVisualizer(visPos, visSize, offset, 1000, visSize.x * 0.5);
			}
			else if (items[activebuttonindex] == "basic")
			{
				delete visualizer;
				visualizer = new BasicVisualizer(visPos, visSize, offset, 1000, visSize.x * 0.7);
			}
			else if (items[activebuttonindex] == "fluid")
			{
				delete visualizer;
				visualizer = new FluidVisualizer(visPos, visSize, offset, 100, 128);
			}
			swap(items[0], items[activebuttonindex]);
			swapTexts(texts[0], texts[activebuttonindex]);
		}
		
	}
	else
	{
		activebuttonindex = -1;
		state.allListsFolded = true;
		folded = true;
	}
	onHover();
	onDefault();
}

void UnfoldingList::onHover()
{
	if (activebuttonindex >= 0)
	{
		bodies[activebuttonindex].setFillColor(sf::Color(outlineColor));
		texts[activebuttonindex].setFillColor(sf::Color::Black);
		if (activebuttonindex == 0)
			triangle.setFillColor(sf::Color::Black);
	}
	
}

void UnfoldingList::onDefault()
{
	for (int i = 0; i < items.size(); ++i)
	{
		if (i == activebuttonindex) continue;
		bodies[i].setFillColor(sf::Color::Black);
		texts[i].setFillColor(sf::Color::White);
		if (activebuttonindex != 0)
			triangle.setFillColor(sf::Color::White);
	}
}

void UnfoldingList::draw(sf::RenderWindow & renderWindow)
{
	renderWindow.draw(textheadline);
	if(!folded)
	for (int i = 0; i < items.size(); ++i)
	{
		renderWindow.draw(bodies[i]);
		renderWindow.draw(texts[i]);
	}
	else if (folded)
	{
		renderWindow.draw(bodies[0]);
		renderWindow.draw(texts[0]);
	}
	renderWindow.draw(triangle);
}

void UnfoldingList::swapTexts(sf::Text &a, sf::Text &b)
{
	std::string tmp = a.getString();
	a.setString(b.getString());
	b.setString(tmp);
}
