#include "GameEngine.h"
#include "Assets.h"
#include "Scene_Frogger.h"
#include "Scene_Menu.h"
#include "Command.h"
#include <fstream>
#include <memory>
#include <cstdlib>
#include <iostream>


GameEngine::GameEngine(const std::string& path)
{
    Assets::getInstance().loadFromFile("../config.txt") ;
	init(path);
}


void GameEngine::init(const std::string& path)
{
    unsigned int width;
    unsigned int height;
    loadConfigFromFile(path, width, height);


    _window.create(sf::VideoMode(width, height), "FROGGER");

    _statisticsText.setFont(Assets::getInstance().getFont("main"));
    _statisticsText.setPosition(15.0f, 5.0f);
    _statisticsText.setCharacterSize(15);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

void GameEngine::loadConfigFromFile(const std::string &path, unsigned int &width, unsigned int &height) const {
    std::ifstream config(path);
    if (config.fail()) {
        std::cerr << "Open file " << path << " failed\n";
        config.close();
        exit(1);
    }
    std::string token{""};
    config >> token;
    while (!config.eof()) {
        if (token == "Window") {
            config >> width >> height;
        } else if (token[0] == '#') {
            std::string tmp;
            std::getline(config, tmp);
            std::cout << tmp << "\n";
        }

        if (config.fail()) {
            config.clear(); // clear error on stream
            std::cout << "*** Error reading config file\n";
        }
        config >> token;
    }
    config.close();
}


void GameEngine::sUserInput()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)  
			quit();  

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().contains(event.key.code))
			{
				const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
				currentScene()->doAction( Command(currentScene()->getActionMap().at(event.key.code), actionType) );
			}
		}
	}
}


std::shared_ptr<Scene> GameEngine::currentScene()
{
	return _sceneMap.at(_currentScene);
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{


	if (endCurrentScene) {
		// remove scene from map
		_sceneMap.erase(_currentScene);
	}

	if (!_sceneMap.contains(sceneName)) {
		// if scene not in map alrady put new one in
		// otherwise use existing scene arleady in map
		_sceneMap[sceneName] = scene;
	}

	_currentScene = sceneName;
}


void GameEngine::quit()
{
	_window.close();
}


void GameEngine::run()
{
	const sf::Time SPF = sf::seconds(1.0f / 60.f);  // seconds per frame for 60 fps 

	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (isRunning())
	{
		sUserInput();								// get user input

		timeSinceLastUpdate += clock.restart();		 
		while (timeSinceLastUpdate > SPF)
		{
			currentScene()->update(SPF);			// update world
			timeSinceLastUpdate -= SPF;
		}

		currentScene()->sRender();					// render world

		// draw stats

		// display
		window().display();
	}
}

void GameEngine::quitLevel() {
	changeScene("MENU", nullptr, true);
}

void GameEngine::backLevel() {
	changeScene("MENU", nullptr, false);

}


sf::RenderWindow& GameEngine::window()
{
	return _window;
}

sf::Vector2f GameEngine::windowSize() const {
	return sf::Vector2f{_window.getSize()};
}


bool GameEngine::isRunning()
{
	return (_running && _window.isOpen());
}
