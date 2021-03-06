#include "Game.h"

Game::Game()
{
	gameState = Uninit;
}

void Game::start()
{
	if (gameState != Uninit)
		return;

	srand(static_cast<unsigned>(time(NULL)));
	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Flop Box", sf::Style::Titlebar | sf::Style::Close);
	mainWindow.setPosition(sf::Vector2i(400, 10));
	mainWindow.setKeyRepeatEnabled(false);
	mainWindow.setFramerateLimit(31);

	box.load(BOX_IMAGE, sf::IntRect(0, 0, 24, 24)); //load the box
	box.getSprite().setOrigin(12, 12);
	box.setPosition(SCREEN_WIDTH / 5 + 25, SCREEN_HEIGHT / 2 - 25);

	if (!font.loadFromFile("res/scoreFont.ttf"))
		assert(false);

	gameState = Menu;

	while (!isExiting())
		gameLoop();

	mainWindow.close();
}

bool Game::isExiting()
{
	if (gameState == Exiting)
		return true;
	return false;
}

void Game::gameLoop()
{
	int speed = 175;
	float moveY = 0.0f;
	double boxRotation = 0;
	sf::Event currentEvent;
	sf::Time frameTime;

	switch (gameState)
	{
		case Menu:
		{
			while (true)
			{
				if (mainWindow.pollEvent(currentEvent))
				{
					mainWindow.clear(sf::Color::Black);

					if (currentEvent.type == sf::Event::Closed)
					{
						gameState = Exiting;
						break;
					}
					if (currentEvent.type == sf::Event::MouseButtonPressed && currentEvent.mouseButton.button == sf::Mouse::Left)
					{
						gameState = Playing;
						break;
					}
				}

				sf::Text getReady("Get Ready!", font, 45);
				getReady.setColor(sf::Color::White);
				sf::FloatRect getReadyRect = getReady.getLocalBounds();
				getReady.setOrigin(getReadyRect.left + getReadyRect.width / 2.0f, getReadyRect.top + getReadyRect.height / 2.0f);
				getReady.setPosition(SCREEN_WIDTH / 2, 125);

				sf::Text instructions("(Use your mouse to control the box)", font, 15);
				instructions.setColor(sf::Color::White);
				sf::FloatRect instructionsRect = instructions.getLocalBounds();
				instructions.setOrigin(instructionsRect.left + instructionsRect.width / 2.0f, instructionsRect.top + instructionsRect.height / 2.0f);
				instructions.setPosition(SCREEN_WIDTH / 2, 160);

				frameTime = frameClock.restart();

				//UPDATE
				map.update("ground", speed * frameTime.asSeconds());

				//DRAW
				map.draw(mainWindow, 0);
				box.draw(mainWindow);
				mainWindow.draw(getReady);
				mainWindow.draw(instructions);
				drawScore(mainWindow);
				mainWindow.display();
			}
			break;
		}
	
		case Playing:
		{
			while (true)
			{
				frameTime = frameClock.restart();
				if (count == 1)
				{
					moveY = -(400 * frameTime.asSeconds());
					count = 2;
				}
				if (mainWindow.pollEvent(currentEvent))
				{
					mainWindow.clear(sf::Color::Black);

					if (currentEvent.type == sf::Event::Closed)
					{
						gameState = Exiting;
						break;
					}

					if (currentEvent.type == sf::Event::MouseButtonPressed)
					{
						if ((currentEvent.mouseButton.button == sf::Mouse::Left) && !keyPressed)
						{
							//bird up and down movements
							moveY -= moveY + (290 * frameTime.asSeconds());
							keyPressed = true;
						}
						else
						{
							keyPressed = false;
						}
					}
					else
					{
						keyPressed = false;
					}

				}

				if (moveY < 20 && !keyPressed)
					moveY += (45*frameTime.asSeconds());

				boxRotation += 5;

				box.getSprite().move(0.0f, moveY);
				box.getSprite().setRotation(boxRotation);

				if (map.isColliding(box.getSprite().getGlobalBounds()))
				{
					reset();
					gameState = Menu;
					break;
				}

				//UPDATE
				map.update("ground", speed * frameTime.asSeconds()); //ground movement
				map.update("pipes", speed * frameTime.asSeconds()); //pipes movement
				if (map.isBoxThrough()) //score update
					updateScore();

				//DRAW
				map.draw(mainWindow, 1); //draw ground and pipes
				drawScore(mainWindow); //draw score
				box.draw(mainWindow); //draw box
				mainWindow.display(); //draw window
			}
			break;
		}
	}
}

void Game::updateScore()
{
	score++;
}

void Game::drawScore(sf::RenderWindow & window)
{
	string strScore;
	ostringstream scoreConverter;
	scoreConverter << score;
	strScore = scoreConverter.str();

	sf::Text score(strScore, font, 41);
	score.setColor(sf::Color::White);

	sf::FloatRect scoreRect = score.getLocalBounds();
	score.setOrigin(scoreRect.left + scoreRect.width / 2.0f, scoreRect.top + scoreRect.height / 2.0f);
	score.setPosition(SCREEN_WIDTH/2, 50);

	window.draw(score);
}

void Game::reset()
{
	score = 0;
	keyPressed = false;
	box.setPosition(SCREEN_WIDTH / 5 + 25, SCREEN_HEIGHT / 2 - 25);
	box.getSprite().setRotation(0);
	map.reset();
	count = 1;
}