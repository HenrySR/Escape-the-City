#include <GameLogic.h>

#include <cmath>


GameLogic::GameLogic()
{
	fast_man = std::make_shared<PlayerChar>(true);
	jump_man = std::make_shared<PlayerChar>(false);
}


bool GameLogic::init(LevelState &level)
{
	this->level = &level;
	//Read any important values from level
	//eg hazards, enemies, etc

	fast_man->setPos(level.getFastSpawnPt());
	jump_man->setPos(level.getJumpSpawnPt());

	//sf::Vector2f grav_test_start(200, 200);
	//fast_man->setPos(grav_test_start);

	sf::Vector2f zeros(0, 0);
	fast_man->setVelocity(zeros);
	jump_man->setVelocity(zeros);
	//set starting positions and speeds
	//stored in level state

	//read in physics constants
	GRAVITY = 1000;
	FRICTION = 500;

	FAST_MAX_X = 2500;
	FAST_MAX_Y = 2500;
	FAST_RUN = 1000;
	FAST_VERT = 500;

	JUMP_MAX_X = 1000;
	JUMP_MAX_Y = 2000;
	JUMP_RUN = 500;
	JUMP_VERT = 1000;

	return true;
}


bool GameLogic::update(float deltaMs)
{
	//level->printLevelState();

	//update player state

	updatePlayerPosition(*fast_man, deltaMs);
	updatePlayerPosition(*jump_man, deltaMs);

	//update other level objects
	return true;
}


std::vector<GameElements*> GameLogic::getDrawables() //IMPLEMENT WITH GAMEELEMENTS NEXT
{
	std::vector<GameElements*> drawables;

	GameElements* fast_ptr = fast_man.get();
	GameElements* jump_ptr = jump_man.get();

	drawables.push_back(fast_ptr);
	drawables.push_back(jump_ptr);

	//add all interactables

	return drawables;
}


void GameLogic::buttonPress(Controller::Controls button, float deltaMs)
{
	float seconds = deltaMs;

	sf::Vector2f fast_vel = fast_man->getVelocity();
	sf::Vector2f jump_vel = jump_man->getVelocity();

	switch (button)
	{
		case Controller::FAST_LEFT:
			fast_vel.x -= FAST_RUN * seconds;
			break;
		case Controller::FAST_RIGHT:
			fast_vel.x += FAST_RUN * seconds;
			break;
		case Controller::FAST_JUMP:
			if (!fast_man->isInAir()) {
				fast_vel.y = (-1)*FAST_VERT;
				fast_man->setInAir(true);
			}
			break;
		case Controller::FAST_DOWN:
			break;
		case Controller::FAST_USE:
			break;
		case Controller::JUMP_LEFT:
			jump_vel.x -= JUMP_RUN * seconds;
			break;
		case Controller::JUMP_RIGHT:
			jump_vel.x += JUMP_RUN * seconds;
			break;
		case Controller::JUMP_JUMP:
			if (!jump_man->isInAir()) {
				jump_vel.y = (-1)*JUMP_VERT;
				jump_man->setInAir(true);
			}
			break;
		case Controller::JUMP_DOWN:
			break;
		case Controller::JUMP_USE:
			break;

		default:
			break;
	}

	fast_man->setVelocity(fast_vel);
	jump_man->setVelocity(jump_vel);

}


std::vector<float> GameLogic::collisionCalculation(float x, float y)
{
	int tile_size = level->getTileSize();

	int x_diff, y_diff;

	//get to closest x tile border
	if ((int(x) % tile_size) < (tile_size / 2))
		x_diff = -1 * (int(x) % tile_size);
	else
		x_diff = tile_size - (int(x) % tile_size);
	//get to closest y tile border
	if ((int(y) % tile_size) < (tile_size / 2))
		y_diff = -1 * (int(y) % tile_size);
	else
		y_diff = tile_size - (int(y) % tile_size);

	std::vector<float> to_return;
	//chooses shortest distance to fix to, defaults to horiz when equal
	if (std::abs(y_diff) < std::abs(x_diff)) {
		to_return.push_back(x);
		to_return.push_back(int(y) + y_diff);
		to_return.push_back(1);
	}
	else {
		to_return.push_back(int(x) + x_diff);
		to_return.push_back(y);
		to_return.push_back(0);
	}

	return to_return;
}


void GameLogic::updatePlayerPosition(PlayerChar& player, float deltaMs)
{
	float seconds = deltaMs;

	sf::Vector2f pos = player.getPos();
	sf::Vector2f vel = player.getVelocity();

	float new_vx = vel.x;
	float new_vy = vel.y;

	//apply gravity
	new_vy += GRAVITY * seconds;

	//limit to max vert speed
	if (std::abs(int(new_vy)) > FAST_MAX_Y) {
		if (player.getType()) { //if fast char
			new_vy = FAST_MAX_Y;
		}
		else {
			if (std::abs(int(new_vy)) > JUMP_MAX_Y) {
				new_vy = JUMP_MAX_Y;
			}
		}
	}

	//apply friction on ground

	int tile_size = level->getTileSize();
	int player_height = player.getHeight();
	int player_width = player.getWidth();
	std::vector<std::vector<int>> level_layout = level->getTileMap();

	float deltaX = vel.x * seconds;
	float deltaY = vel.y * seconds;

	float new_x = pos.x + deltaX;
	float new_y = pos.y + deltaY;

	//top left corner
	if (level_layout[int(new_x)/tile_size][int(new_y)/tile_size] == 1) {
		std::vector<float> shift = collisionCalculation(new_x, new_y);
		new_x = shift[0];
		new_y = shift[1];
		if (shift[2] == 0) {
			new_vx = 0;
		}
		else {
			new_vy = 0;
			player.setInAir(false);
		}
	}
	//top right corner
	if (level_layout[(int(new_x) + player_width) / tile_size][int(new_y) / tile_size] == 1) {
		std::vector<float> shift = collisionCalculation(new_x + player_width, new_y);
		new_x = shift[0] - player_width;
		new_y = shift[1];
		if (shift[2] == 0) {
			new_vx = 0;
		}
		else {
			new_vy = 0;
			player.setInAir(false);
		}
	}
	//bottom left corner
	if (level_layout[int(new_x) / tile_size][(int(new_y) + player_height) / tile_size] == 1) {
		std::vector<float> shift = collisionCalculation(new_x, new_y + player_height);
		new_x = shift[0];
		new_y = shift[1] - player_height;
		if (shift[2] == 0) {
			new_vx = 0;
		}
		else {
			new_vy = 0;
			player.setInAir(false);
		}
	}
	//bottom right corner
	if (level_layout[(int(new_x) + player_width) / tile_size][(int(new_y) + player_height) / tile_size] == 1) {
		std::vector<float> shift = collisionCalculation(new_x + player_width, new_y + player_height);
		new_x = shift[0] - player_width;
		new_y = shift[1] - player_height;
		if (shift[2] == 0) {
			new_vx = 0;
		}
		else {
			new_vy = 0;
			player.setInAir(false);
		}
	}

	sf::Vector2f new_pos = sf::Vector2f(new_x, new_y);
	sf::Vector2f new_vel = sf::Vector2f(new_vx, new_vy);

	player.setPos(new_pos);
	player.setVelocity(new_vel);

}


