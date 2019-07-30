#include <SFML/Graphics.hpp>
#include <assert.h>
#include <iostream>
#include <list>

#define GRID_WIDTH	WINDOW_WIDTH/16
#define GRID_HIGHT	WINDOW_HIGHT/16
#define WINDOW_WIDTH	480
#define WINDOW_HIGHT	480
#define BLOCK_PATH	"block.png"
#define FONT_PATH	"slkscr.ttf"


class Block : public sf::Drawable {
public:
	static Block * getInstance(std::string image_path = "") {
		if (!_instance) {
			_instance = new Block(image_path);
		}
		return _instance;
	}
	void setPosition(sf::Vector2u index) {
		_block_sprite->setPosition(index.x * _block_texture->getSize().x, index.y * _block_texture->getSize().y);
	}
	void setPosition(int x, int y) {
		_block_sprite->setPosition(x * _block_texture->getSize().x, y * _block_texture->getSize().y);
	}
	void setColor(const sf::Color & color) {
		_block_sprite->setColor(color);
	}
protected:
	void draw(sf::RenderTarget & target, sf::RenderStates states) const {
		target.draw(*_block_sprite);
	}

private:
	Block(std::string image_path) {
		if (image_path.empty()) {
			std::cout << "Block: Empty image path\n";
			exit(-1);
		}
		_block_texture = new sf::Texture();
		assert(_block_texture);
		if (!_block_texture->loadFromFile(image_path)) {
			exit(-1);
		}
		_block_sprite = new sf::Sprite(*_block_texture);
		assert(_block_texture);
	}
	~Block() {
		delete _instance;
		delete _block_texture;
		delete _block_sprite;
	}
	static Block *_instance ;
	sf::Texture * _block_texture;
	sf::Sprite * _block_sprite;
};
Block * Block::_instance = nullptr;

class Grid : public sf::Drawable {
public:
	Grid(Block* block, unsigned x = 100, unsigned y = 100,  sf::Color color = sf::Color::White)
		: _Xsize(x), _Ysize(y), _block_instance(block), _color(color)
	{}
	~Grid() {
		_Xsize = _Ysize = 0;
		_block_instance = nullptr;
	}
	void draw(sf::RenderTarget & 	target, sf::RenderStates states) const {
		for (int i =0; i< _Xsize ; i ++)
			for (int j = 0; j < _Ysize; j++) {
			_block_instance->setPosition(i,j);
			_block_instance->setColor(_color);
			target.draw(*_block_instance);
			}
	}
private:
	unsigned _Xsize, _Ysize;
	sf::Color _color;
	Block * _block_instance;
};

enum Direction {
	Up			=0,
	Down		=1,
	Right		=2,
	Left		=3
};
class Snake : public sf::Drawable {
public:
	Snake(Block * block, int x = 0, int y=0 , sf::Color color = sf::Color::Green) 
		: _block_instance (block), _color (color), _direction(Direction::Right), _last_tick_direction(_direction), _Xboundary(GRID_WIDTH -1 ), _Yboundary(GRID_HIGHT - 1),
		_dead(false), _toggle(false){
		SnakeList.push_back(sf::Vector2i(x+2, y));
		SnakeList.push_back(sf::Vector2i(x+1, y));
		SnakeList.push_back(sf::Vector2i(x , y));
	}
	void tick() {
		if (_dead) {
			_toggle = !_toggle;
			return;
		}
		switch (_direction) {
		case Direction::Up :
			if (SnakeList.front().y - 1 < 0)
				SnakeList.push_front(sf::Vector2i(SnakeList.front().x, _Yboundary));
			else 
				SnakeList.push_front(sf::Vector2i(SnakeList.front().x, SnakeList.front().y - 1));
			SnakeList.pop_back();
			break;
		case Direction::Down:
			if (SnakeList.front().y + 1 > _Yboundary)
				SnakeList.push_front(sf::Vector2i(SnakeList.front().x, 0));
			else
				SnakeList.push_front(sf::Vector2i(SnakeList.front().x, SnakeList.front().y + 1));
			SnakeList.pop_back();
			break;
		case Direction::Right:
			if (SnakeList.front().x + 1 > _Xboundary)
				SnakeList.push_front(sf::Vector2i(0, SnakeList.front().y));
			else
				SnakeList.push_front(sf::Vector2i(SnakeList.front().x + 1, SnakeList.front().y));
			SnakeList.pop_back();
			break;
		case Direction::Left:
			if (SnakeList.front().x - 1 < 0)
				SnakeList.push_front(sf::Vector2i(_Xboundary, SnakeList.front().y));
			else
				SnakeList.push_front(sf::Vector2i(SnakeList.front().x - 1, SnakeList.front().y));
			SnakeList.pop_back();
			break;
		default:
			std::cout << "Error: snake direction is undefined!\n";
			exit(-1);
			break;
		}
		_last_tick_direction = _direction;
		if (checkHit())
			_dead = true;

	}
	void goUp() {
		if(_last_tick_direction != Direction::Down)
			_direction = Direction::Up;
	}
	void goDown() {
		if (_last_tick_direction != Direction::Up)
			_direction = Direction::Down;
	}
	void goLeft() {
		if (_last_tick_direction != Direction::Right)
			_direction = Direction::Left;
	}
	void goRight() {
		if (_last_tick_direction != Direction::Left)
			_direction = Direction::Right;
	}
	sf::Vector2i getHeadPosition() {
		return SnakeList.front();
	}
	void growUp() {
		SnakeList.push_back(SnakeList.back());
	}
	~Snake() {
		_block_instance = nullptr;
	}
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		for ( std::list<sf::Vector2i>::const_iterator it = SnakeList.begin(); it != SnakeList.end(); it++) {
			_block_instance->setPosition(it->x, it->y);
				_block_instance->setColor(sf::Color::Red);
				if (_dead && _toggle) 
					_block_instance->setColor(sf::Color::White);
			target.draw(*_block_instance);
		}	
	}
private:

	bool checkHit() {
		sf::Vector2i head = SnakeList.front();
		std::list<sf::Vector2i>::iterator it = SnakeList.begin();
		it++;
		for (; it != SnakeList.end(); it++) {
			if (*it == head)
				return true;
		}
		return false;
	}
	Direction _direction;
	Direction _last_tick_direction;
	Block* _block_instance;
	bool _toggle;
	bool _dead;
	int	_Xboundary, _Yboundary;
	sf::Color _color;
	std::list<sf::Vector2i>	SnakeList;
};

class Food : public sf::Drawable {
public:
	Food(Block* block, int x, int y, sf::Color color = sf::Color::Red)
		: _block_instance (block), _x(x), _y(y), _color(color), _toggle(false){
	}
	sf::Vector2i getPostition() {
		return sf::Vector2i(_x, _y);
	}
	void setPosition(int x, int y) {
		_x = x;
		_y = y;
	}
	void tick() {
		_toggle = !_toggle;
	}
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (_toggle)
			_block_instance->setColor(sf::Color::White);
		else
		_block_instance->setColor(_color);
		_block_instance->setPosition(_x, _y);
		target.draw(*_block_instance);
	}
private:
	int _x, _y;
	sf::Color _color;
	bool _toggle;
	Block* _block_instance;
};

class Score : public sf::Drawable {
public:
	Score(std::string font_path, int win_width = 0, int win_hight = 0 ) 
		: _window_width(win_width), _window_hight(win_hight), score(0){
		if (!font.loadFromFile(font_path))
		{
			std::cout << "FONT: couldn`t load font file\n";
			exit(1);
		}
		score_text.setFont(font);
		score_text.setFillColor(sf::Color::Black);
		score_text.setString(score_string + std::to_string(score));
		score_text.setPosition(0.7 * _window_width, 0 * _window_hight + 5);
		score_text.setCharacterSize(25);
	}
	void increment() {
		score++;
		score_text.setString(score_string + std::to_string(score));
	}
protected:
	void draw(sf::RenderTarget & target, sf::RenderStates states) const {
		target.draw(score_text);
	}
private:
	int _window_width, _window_hight;
	const std::string score_string = "score:";
	int score;
	sf::Text score_text;
	sf::Font font;
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HIGHT), "SNAKE GAME");
	Block * block = Block::getInstance(BLOCK_PATH);
	Grid grid(block, GRID_WIDTH, GRID_HIGHT);
	Snake snake(block);
	Food food (block, rand() % GRID_WIDTH, rand() % GRID_HIGHT);
	Score score(FONT_PATH, WINDOW_WIDTH, WINDOW_HIGHT);

	float tick_time = 0.1;
	sf::Clock timer;
	float passed_time = 0;
	while (window.isOpen())
	{
		passed_time += timer.getElapsedTime().asSeconds();
		timer.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) snake.goLeft();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  snake.goRight();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) snake.goUp();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) snake.goDown();

		if (passed_time > tick_time) {
			passed_time = 0;
			if (snake.getHeadPosition() == food.getPostition()) {
				snake.growUp();
				score.increment();
				food.setPosition(rand() % GRID_WIDTH, rand() % GRID_HIGHT);
			}
			snake.tick();
			food.tick();
		}
		window.clear();
		window.draw(grid);
		window.draw(snake);
		window.draw(food);
		window.draw(score);
		window.display();
	}

	return 0;
}