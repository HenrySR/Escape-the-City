#include <GameLogic.h>
#include <Door.h>
#include <Button.h>
#include <Hazards.h>
#include <Items.h>
#include <Plates.h>
#include <Platforms.h>
#include <cmath>
#include <string>
#include "tinyxml2.h"


GameLogic::GameLogic(ResourceManager& manager)
{
	fast_man = std::make_shared<PlayerChar>(true, manager);
	jump_man = std::make_shared<PlayerChar>(false, manager);
	exitPos = std::make_shared<Exit>(manager);
	//initialize all interactable lists here
	int hazard_num = 6;
	int door_num = 6;
	int item_num = 6;
	int platform_num = 6;
	//buttons
	for (int i = 0; i < door_num; i++) {
		std::shared_ptr<Interactables> ptr = std::make_shared<Button>(manager);
		//assign color
		buttons.push_back(ptr);
	}
	//pressure plates

	//hazards
	for (int i = 0; i < hazard_num; i++) {
		std::shared_ptr<Interactables> ptr = std::make_shared<Hazards>(manager);
		//assign color
		hazards.push_back(ptr);
	}
	//doors
	for (int i = 0; i < door_num; i++) {
		std::shared_ptr<Interactables> ptr = std::make_shared<Door>(manager);
		//assign color
		doors.push_back(ptr);
	}
	//items
	for (int i = 0; i < item_num; i++) {
		std::shared_ptr<Interactables> ptr = std::make_shared<Items>(manager, i);
		items.push_back(ptr);
	}
	for (int i = 0; i < platform_num; i++) {
		std::shared_ptr<Interactables> ptr = std::make_shared<Platforms>(manager);
		platforms.push_back(ptr);
	}
}


bool GameLogic::init(LevelState &level)
{
	this->level = &level;
	//Read any important values from level
	//eg hazards, enemies, etc

	std::vector<std::vector<int>> tileMap = level.getTileMap();
	int tileSize = level.getTileSize();

	std::vector<sf::Vector2f> hazard_pos = level.getHazardPos();
	std::vector<sf::Vector2f> door_pos = level.getDoorPos();
	std::vector<sf::Vector2f> button_pos = level.getButtonPos();
	//std::vector<sf::Vector2f> pressure_plate_pos = level.getPressurePlatePos();
	std::vector<sf::Vector2f> item_pos = level.getItemPos();
	std::vector<sf::Vector2f> platform_pos = level.getMovPlatformPos();


	//0 = no object
	//1 = hazard
	//2 = door
	int objects[6] = { 0,0,0,0,0,0 };

	//hazards
	for (int i = 0; i < hazard_pos.size(); i++) {
		sf::Vector2f this_hazard = hazard_pos[i];
		hazards[i]->setPos(this_hazard);
		if (int(this_hazard.x) != 0 || int(this_hazard.y) != 0) {
			objects[i] = 1;
			int size = 1;
			int cur_x = int(this_hazard.x) / tileSize;
			int cur_y = int(this_hazard.y) / tileSize;
			int id = tileMap[cur_x][cur_y];
			bool orientation = determineObjectLength(cur_x, cur_y, id, size);
			//set hazard size and orientation
			Hazards* haz = dynamic_cast<Hazards*>(hazards[i].get());
			haz->setSize(size);
			haz->setOrientation(orientation);
			haz->Reset();
		}
	}

	//doors
	for (int i = 0; i < door_pos.size(); i++) {
		sf::Vector2f this_door = door_pos[i];
		doors[i]->setPos(this_door);
		if (int(this_door.x) != 0 || int(this_door.y) != 0) {
			objects[i] = 2;
			int size = 1;
			int cur_x = int(this_door.x) / tileSize;
			int cur_y = int(this_door.y) / tileSize;
			int id = tileMap[cur_x][cur_y];
			bool orientation = determineObjectLength(cur_x, cur_y, id, size);
			//set hazard size and orientation
			Door* doo = dynamic_cast<Door*>(doors[i].get());
			doo->setSize(size);
			doo->setOrientation(orientation);
			doo->Reset();
		}
	}

	//platforms
	for (int i = 0; i < platform_pos.size(); i++) {
		sf::Vector2f this_platform = platform_pos[i];
		platforms[i]->setPos(this_platform);
		if (int(this_platform.x) != 0 || int(this_platform.y) != 0) {
			objects[i] = 3;
			int size = 1;
			int cur_x = int(this_platform.x) / tileSize;
			int cur_y = int(this_platform.y) / tileSize;
			int id = tileMap[cur_x][cur_y];
			bool orientation = determineObjectLength(cur_x, cur_y, id, size);
			//set platform size and orientation
			Platforms* platf = dynamic_cast<Platforms*>(platforms[i].get());
			platf->setSize(size);
			platf->setOrientation(orientation);
			platf->Reset();
		}
	}

	//buttons
	for (int i = 0; i < button_pos.size(); i++) {
		sf::Vector2f this_button = button_pos[i];
		buttons[i]->setPos(this_button);
		if (int(this_button.x) != 0 || int(this_button.y) != 0) {
			Button* but = dynamic_cast<Button*>(buttons[i].get());
			if (objects[i] == 1) {
				//link to hazards[i]
				but->setToggleable(hazards[i].get());
			}
			else if (objects[i] == 2) {
				//link to doors[i]
				but->setToggleable(doors[i].get());
			}
			else if (objects[i] == 3){
				but->setToggleable(platforms[i].get());
			}
			but->Reset();
		}
	}

	//items
	for (int i = 0; i < item_pos.size(); i++) {
		sf::Vector2f this_item = item_pos[i];
		items[i]->setPos(this_item);
		items[i]->Reset();
	}


	exitPos->setPos(level.getExitPt());

	//reset
	fast_man->useItem();
	jump_man->useItem();

	fast_man->setPos(level.getFastSpawnPt());
	fast_man->setSpawnPt(level.getFastSpawnPt());
	jump_man->setPos(level.getJumpSpawnPt());
	jump_man->setSpawnPt(level.getJumpSpawnPt());

	//sf::Vector2f grav_test_start(200, 200);
	//fast_man->setPos(grav_test_start);

	sf::Vector2f zeros(0, 0);
	fast_man->setVelocity(zeros);
	jump_man->setVelocity(zeros);
	//set starting positions and speeds
	//stored in level state

	//read in physics constants

	tinyxml2::XMLDocument config;
	tinyxml2::XMLError result = config.LoadFile("../config/constants.xml");
	if (result != tinyxml2::XML_SUCCESS)
		return false;

	tinyxml2::XMLElement* general = config.FirstChildElement("general");
	tinyxml2::XMLElement* fast = config.FirstChildElement("fastman");
	tinyxml2::XMLElement* jump = config.FirstChildElement("jumpman");

	GRAVITY = std::stof(general->FirstChildElement("gravity")->GetText());
	FRICTION = std::stof(general->FirstChildElement("friction")->GetText());

	float f_xmax = std::stof(fast->FirstChildElement("max_horiz_speed")->GetText());
	FAST_MAX_X = 2 * f_xmax * tileSize;
	float f_ymax = std::stof(fast->FirstChildElement("max_vert_speed")->GetText());
	FAST_MAX_Y = 2 * f_ymax * tileSize;
	float f_acceleration = std::stof(fast->FirstChildElement("accel")->GetText());
	//FAST_RUN = std::stof(fast->FirstChildElement("run_speed")->GetText());
	FAST_RUN = (FAST_MAX_X * FAST_MAX_X) / (2 * f_acceleration*tileSize);
	FAST_HEIGHT = std::stof(fast->FirstChildElement("jump_height")->GetText());
	FAST_AIR_MULT = std::stof(fast->FirstChildElement("air_multiplier")->GetText());
	FAST_VERT = std::sqrt(2 * GRAVITY * FAST_HEIGHT * tileSize);

	float j_xmax = std::stof(jump->FirstChildElement("max_horiz_speed")->GetText());
	JUMP_MAX_X = 2 * j_xmax * tileSize;
	float j_ymax = std::stof(jump->FirstChildElement("max_vert_speed")->GetText());
	JUMP_MAX_Y = 2 * j_ymax * tileSize;
	float j_acceleration = std::stof(jump->FirstChildElement("accel")->GetText());
	//JUMP_RUN = std::stof(jump->FirstChildElement("run_speed")->GetText());
	JUMP_RUN = (JUMP_MAX_X * JUMP_MAX_X) / (2 * j_acceleration*tileSize);
	JUMP_HEIGHT = std::stof(jump->FirstChildElement("jump_height")->GetText());
	JUMP_AIR_MULT = std::stof(jump->FirstChildElement("air_multiplier")->GetText());
	JUMP_VERT = std::sqrt(2 * GRAVITY * JUMP_HEIGHT * tileSize);

	MIN_VERT = std::sqrt(2*GRAVITY*tileSize/4);

	return true;
}


bool GameLogic::determineObjectLength(int x_start, int y_start, int id, int& size)
{
	std::vector<std::vector<int>> tileMap = level->getTileMap();
	int cur_x = x_start + 1;
	int cur_y = y_start + 1;
	while (tileMap[x_start][cur_y] == id || tileMap[cur_x][y_start] == id) {
		if(tileMap[cur_x][y_start] == id){
		cur_x += 1;
	}
		if(tileMap[x_start][cur_y] == id){
		cur_y += 1;
	}
		size += 1;
	}
	if (cur_y - y_start == size) {
		return false;
	}
	else {
		return true;
	}
}


bool GameLogic::update(float deltaMs)
{
	//level->printLevelState();

	updatePlayerPosition(*fast_man, deltaMs);
	updatePlayerPosition(*jump_man, deltaMs);

	updatePlayerState(*fast_man, deltaMs);
	updatePlayerState(*jump_man, deltaMs);

	//update other level objects
	for (int i = 0; i < platforms.size(); i++) {
			Platforms* platform_ptr = dynamic_cast<Platforms*>(platforms[i].get());
			platform_ptr->setTime(deltaMs);
		}


	bool level_end = exitPos->levelEnd();
	exitPos->Reset();
	return level_end;
}


std::vector<GameElements*> GameLogic::getDrawables() //IMPLEMENT WITH GAMEELEMENTS NEXT
{
	std::vector<GameElements*> drawables;

	//hazards
	for (int i = 0; i < hazards.size(); i++) {
		sf::Vector2f this_pos = hazards[i]->getPos();
		if (int(this_pos.x) != 0 || int(this_pos.y) != 0) {
			GameElements* game_ptr = hazards[i].get();
			drawables.push_back(game_ptr);
		}
	}
	//doors
	for (int i = 0; i < doors.size(); i++) {
		sf::Vector2f this_pos = doors[i]->getPos();
		if (int(this_pos.x) != 0 || int(this_pos.y) != 0) {
			GameElements* game_ptr = doors[i].get();
			drawables.push_back(game_ptr);
		}
	}
	//buttons
	for (int i = 0; i < buttons.size(); i++) {
		sf::Vector2f this_pos = buttons[i]->getPos();
		if (int(this_pos.x) != 0 || int(this_pos.y) != 0) {
			GameElements* game_ptr = buttons[i].get();
			drawables.push_back(game_ptr);
		}
	}
	//items
	for (int i = 0; i < items.size(); i++) {
		sf::Vector2f this_pos = items[i]->getPos();
		if (int(this_pos.x) != 0 || int(this_pos.y) != 0) {
			GameElements* game_ptr = items[i].get();
			drawables.push_back(game_ptr);
		}
	}
	//platforms
	for (int i = 0; i < platforms.size(); i++) {
		sf::Vector2f this_pos = platforms[i]->getPos();
		if (int(this_pos.x) != 0 || int(this_pos.y) != 0) {
			GameElements* game_ptr = platforms[i].get();
			drawables.push_back(game_ptr);
		}
	}
	//exit point
	GameElements* exit_ptr = exitPos.get();
	//players
	GameElements* fast_ptr = fast_man.get();
	GameElements* jump_ptr = jump_man.get();

	drawables.push_back(exit_ptr);
	drawables.push_back(fast_ptr);
	drawables.push_back(jump_ptr);

	//add all interactables

	return drawables;
}


bool GameLogic::buttonPress(Controller::Controls button, float deltaMs)
{
	float seconds = deltaMs;
	bool success = false;
	int powerUpF = fast_man->getPowerUp();
	int powerUpJ = jump_man->getPowerUp();

	sf::Vector2f fast_vel = fast_man->getVelocity();
	sf::Vector2f jump_vel = jump_man->getVelocity();

	switch (button)
	{
		case Controller::FAST_LEFT:
			if (!fast_man->isInAir()) {
				fast_vel.x -= (FAST_RUN + FRICTION) * seconds;
			}
			else {
				fast_vel.x -= FAST_RUN * FAST_AIR_MULT * seconds;
			}
			success = true;
			break;
		case Controller::FAST_RIGHT:
			if (!fast_man->isInAir()) {
				fast_vel.x += (FAST_RUN + FRICTION) * seconds;
			}
			else {
				fast_vel.x += FAST_RUN * FAST_AIR_MULT * seconds;
			}
			success = true;
			break;
		case Controller::FAST_JUMP:
			if (!fast_man->isInAir()) {
				fast_vel.y = (-1)*FAST_VERT;
				fast_man->setInAir(true);
				success = true;
			}
			success = true;
			break;
		case Controller::FAST_JUMP_RELEASE:
			if (fast_man->isInAir() && fast_vel.y < (-1)*MIN_VERT) {
				fast_vel.y = (-1)*MIN_VERT;
				success = true;
			}
			break;
		case Controller::FAST_DOWN:
			break;
		case Controller::FAST_USE:
		powerUpF = fast_man->getPowerUp();
		items[powerUpF]->Reset();
		fast_man->useItem();
		fast_vel = fast_man->getVelocity();
		success = true;
			break;
		case Controller::FAST_USE_RELEASE:
			fast_man->interact(true);
			break;
		case Controller::JUMP_LEFT:
			if (!jump_man->isInAir()) {
				jump_vel.x -= (JUMP_RUN + FRICTION) * seconds;
			}
			else {
				jump_vel.x -= JUMP_RUN * JUMP_AIR_MULT * seconds;
			}
			success = true;
			break;
		case Controller::JUMP_RIGHT:
			if (!jump_man->isInAir()) {
				jump_vel.x += (JUMP_RUN + FRICTION) * seconds;
			}
			else {
				jump_vel.x += JUMP_RUN * JUMP_AIR_MULT * seconds;
			}
			success = true;
			break;
		case Controller::JUMP_JUMP:
			if (!jump_man->isInAir()) {
				jump_vel.y = (-1)*JUMP_VERT;
				jump_man->setInAir(true);
				success = true;
			}
			break;
		case Controller::JUMP_JUMP_RELEASE:
			if (jump_man->isInAir() && jump_vel.y < (-1)*MIN_VERT) {
				jump_vel.y = (-1)*MIN_VERT;
				success = true;
			}
			break;
		case Controller::JUMP_DOWN:
			break;
		case Controller::JUMP_USE:
		powerUpJ = jump_man->getPowerUp();
		items[powerUpJ]->Reset();
		jump_man->useItem();
		jump_vel = jump_man->getVelocity();
		success = true;
			break;
		case Controller::JUMP_USE_RELEASE:
			jump_man->interact(true);
			break;
		case Controller::FAST_RESPAWN:
			//powerUpF = fast_man->getPowerUp();
			//items[powerUpF]->Reset();
			success = true;
			break;
		case Controller::FAST_RESPAWN_RELEASE:
			fast_man->respawn();
			success = true;
			break;
		case Controller::JUMP_RESPAWN:
			success = true;
			break;
		case Controller::JUMP_RESPAWN_RELEASE:
			//powerUpJ = jump_man->getPowerUp();
			//items[powerUpJ]->Reset();
			jump_man->respawn();
			success = true;
			break;
		default:
			break;
	}

	fast_man->setVelocity(fast_vel);
	jump_man->setVelocity(jump_vel);

	return success;
}


std::vector<float> GameLogic::collisionCalculation(int tileXCoord, int tileYCoord, int prevTileX, int prevTileY, float posx, float posy, PointType corner)
{
	std::vector<std::vector<int>> tileMap = level->getCollisionMap();
	int tile_size = level->getTileSize();

	int x_diff, y_diff;

	//get to closest x tile border
	if ((int(posx) % tile_size) < (tile_size / 2))
		x_diff = -1 * (int(posx) % tile_size); //left side close
	else
		x_diff = tile_size - (int(posx) % tile_size); //right side close
	//get to closest y tile border
	if ((int(posy) % tile_size) < (tile_size / 2))
		y_diff = -1 * (int(posy) % tile_size); //top close
	else
		y_diff = tile_size - (int(posy) % tile_size); //bot close

	//check surrounding tiles
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;
	if (tileYCoord > 0)
		up = tileMap[tileXCoord][tileYCoord - 1];
	if (tileYCoord < tileMap[0].size()-1)
		down = tileMap[tileXCoord][tileYCoord + 1];
	if (tileXCoord > 0)
		left = tileMap[tileXCoord - 1][tileYCoord];
	if (tileXCoord < tileMap.size() - 1)
		right = tileMap[tileXCoord + 1][tileYCoord];

	std::vector<float> to_return;

	//flat ground
	if (left == 1 && right == 1) {
		to_return.push_back(posx);
		to_return.push_back(int(posy) + y_diff);
		to_return.push_back(1);
	}
	//flat wall
	else if (up == 1 && down == 1) {
		to_return.push_back(int(posx) + x_diff);
		to_return.push_back(posy);
		to_return.push_back(0);
	}
	//top exposed corner
	else if (up != 1 && (corner == bot_left || corner == bot_right) && (int(posy) % tile_size) < (tile_size / 4)) {
		to_return.push_back(posx);
		to_return.push_back(int(posy) + y_diff);
		to_return.push_back(1);
	}
	//bot exposed corner
	else if (down != 1 && (corner == top_left || corner == top_right) && tile_size - (int(posy) % tile_size) < (tile_size / 4)) {
		if (corner == top_left && prevTileX != tileXCoord && prevTileY == tileYCoord) {
			to_return.push_back(int(posx) + x_diff);
			to_return.push_back(posy);
			to_return.push_back(0);
		}
		else if (corner == top_right && prevTileX == tileXCoord && prevTileY == tileYCoord) {
			to_return.push_back(int(posx) + x_diff);
			to_return.push_back(posy);
			to_return.push_back(0);
		}
		else {
			to_return.push_back(posx);
			to_return.push_back(int(posy) + y_diff);
			to_return.push_back(1);
		}
	}
	//default to x collision
	else {
		to_return.push_back(int(posx) + x_diff);
		to_return.push_back(posy);
		to_return.push_back(0);
	}

	return to_return;
}


void GameLogic::updatePlayerState(PlayerChar& player, float deltaMs)
{
	std::vector<std::vector<int>> tileMap = level->getTileMap();
	int tile_size = level->getTileSize();

	sf::Vector2f pos = player.getPos();

	int x_pos = pos.x;
	int y_pos = pos.y;
	int player_height = player.getHeight();
	int player_width = player.getWidth();

	int id = 0;

	//bottom left corner
	if (tileMap[int(x_pos) / tile_size][(int(y_pos) + player_height) / tile_size] > 3) {
		id = tileMap[int(x_pos) / tile_size][(int(y_pos) + player_height) / tile_size];
	}
	//bottom right corner
	if (tileMap[int(x_pos + player_width) / tile_size][(int(y_pos) + player_height) / tile_size] > 3) {
		id = tileMap[int(x_pos + player_width) / tile_size][(int(y_pos) + player_height) / tile_size];
	}
	//top left corner
	if (tileMap[int(x_pos) / tile_size][int(y_pos) / tile_size] > 3) {
		id = tileMap[int(x_pos) / tile_size][int(y_pos) / tile_size];
	}
	//top right corner
	if (tileMap[int(x_pos + player_width) / tile_size][int(y_pos) / tile_size] > 3) {
		id = tileMap[int(x_pos + player_width) / tile_size][int(y_pos) / tile_size];
	}

	//exit position
	if (id == 4) {
		exitPos->PlayerContact(player, id);
	}
	//buttons and pressure plates
	if (id >= 5 && id <= 16) {
		buttons[id-5]->PlayerContact(player, id);
	}
	//doors
	if (id >= 17 && id <= 22) {
		doors[id-17]->PlayerContact(player, id);
	}
	//hazards
	if (id >= 23 && id <= 28) {
		hazards[id-23]->PlayerContact(player, id);
	}
	//items
	if (id >= 35 && id <= 40) {
		items[id - 35]->PlayerContact(player, id);
	}
	//Platforms
	if(id >= 29 && id <= 34){
		platforms[id - 29]->PlayerContact(player, id);
	}

	player.interact(false);
}


void GameLogic::updatePlayerPosition(PlayerChar& player, float deltaMs)
{
	float seconds = deltaMs;

	sf::Vector2f pos = player.getPos();
	sf::Vector2f vel = player.getVelocity();

	float new_vx = vel.x;
	float new_vy = vel.y;

	//apply friction on ground
	if (!player.isInAir() && new_vx != 0.0) {
		int sign;
		if (std::isless(new_vx, 0)) {
			sign = 1;
			new_vx += FRICTION * seconds;
		}
		else if(std::isgreater(new_vx, 0)) {
			sign = 0;
			new_vx -= FRICTION * seconds;
		}
		//passed zero
		if (sign != std::signbit(new_vx)) {
			new_vx = 0;
		}
	}

	//limit to max horizontal speed
	if (std::abs(int(new_vx)) > JUMP_MAX_X) {
		if (!player.getType()) { //if jump char
			new_vx = std::pow(-1,std::signbit(new_vx))*JUMP_MAX_X;
		}
		else {
			if (std::abs(int(new_vx)) > FAST_MAX_X) {
				new_vx = std::pow(-1, std::signbit(new_vx))*FAST_MAX_X;
			}
		}
	}

	//apply gravity
	new_vy += GRAVITY * seconds;
	player.setInAir(true);

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

	int tile_size = level->getTileSize();
	int player_height = player.getHeight();
	int player_width = player.getWidth();
	std::vector<std::vector<int>> level_layout = level->getCollisionMap();

	float deltaX = vel.x * seconds;
	float deltaY = vel.y * seconds;

	float new_x = pos.x + deltaX;
	float new_y = pos.y + deltaY;

	int prev_x = int(pos.x);
	int prev_y = int(pos.y);

	//bottom left corner
	if (level_layout[int(new_x) / tile_size][(int(new_y) + player_height) / tile_size] == 1) {
		std::vector<float> shift = collisionCalculation(int(new_x) / tile_size, (int(new_y) + player_height) / tile_size, prev_x / tile_size, (prev_y + player_height) / tile_size, new_x, new_y + player_height, bot_left);
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
		std::vector<float> shift = collisionCalculation((int(new_x) + player_width) / tile_size, (int(new_y) + player_height) / tile_size, (prev_x + player_width) / tile_size, (prev_y + player_height) / tile_size, new_x + player_width, new_y + player_height, bot_right);
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
	//top left corner
	if (level_layout[int(new_x)/tile_size][int(new_y)/tile_size] == 1) {
		std::vector<float> shift = collisionCalculation(int(new_x) / tile_size, int(new_y) / tile_size, prev_x / tile_size, prev_y / tile_size, new_x, new_y, top_left);
		new_x = shift[0];
		new_y = shift[1];
		if (shift[2] == 0) {
			new_vx = 0;
		}
		else {
			new_vy = 0;
			//player.setInAir(false);
		}
	}
	//top right corner
	if (level_layout[(int(new_x) + player_width) / tile_size][int(new_y) / tile_size] == 1) {
		std::vector<float> shift = collisionCalculation((int(new_x) + player_width) / tile_size, int(new_y) / tile_size, (prev_x + player_width) / tile_size, prev_y / tile_size, new_x + player_width, new_y, top_right);
		new_x = shift[0] - player_width;
		new_y = shift[1];
		if (shift[2] == 0) {
			new_vx = 0;
		}
		else {
			new_vy = 0;
			//player.setInAir(false);
		}
	}


	sf::Vector2f new_pos = sf::Vector2f(new_x, new_y);
	sf::Vector2f new_vel = sf::Vector2f(new_vx, new_vy);

	player.setPos(new_pos);
	player.setVelocity(new_vel);

}
//if bad
//then git gud
