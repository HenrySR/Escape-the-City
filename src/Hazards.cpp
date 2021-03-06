#include <Hazards.h>
#include <SFML/Graphics.hpp>


Hazards::Hazards(ResourceManager& manager){
  this->toggled = false;
  this->orient = true;
  this->size = 1;

  sf::Vector2i coords = manager.getSpriteCoords(ResourceManager::Sprites::HAZARDS);
  onSpriteCoords.x = coords.x;
  onSpriteCoords.y = coords.y;
  coords = manager.getSpriteCoords(ResourceManager::Sprites::HAZARDS_OFF);
  offSpriteCoords.x = coords.x;
  offSpriteCoords.y = coords.y;
  spriteSheet = &manager.getSpriteSheet();
  //hazard.loadFromFile("..\\resources\\Spikes.png");
}

void Hazards::draw(sf::RenderWindow& window){
  sf::Vector2f position= pos;
  sf::Vector2i size(20, 20);
  sf::Sprite h;
  if (toggled == false) {
	  sf::IntRect spritePos(onSpriteCoords, size);
	  sf::Sprite temp(*spriteSheet, spritePos);
	  h = temp;
  }
  else {
	  sf::IntRect spritePos(offSpriteCoords, size);
	  sf::Sprite temp(*spriteSheet, spritePos);
	  h = temp;
  }

    //horizontal surface
  if(orient == true){
      for(int i = 0; i< this->size; i++){
        //h.setTexture(hazard);
        h.setRotation(0);
        h.setPosition(position);
      window.draw(h);
      position.x=position.x+20;
    }
  }
  //vertical surface
  else{
      position.x=position.x+20;
      for(int i = 0; i< this->size; i++){
		//h.setTexture(hazard);
        h.setRotation(90);
        h.setPosition(position);
      window.draw(h);
      position.y=position.y+20;
    }
  }
}



//set hazard's position only used at creation of a hazard
void Hazards::setPos(sf::Vector2f& pos){
  this->pos.x = pos.x;
  this->pos.y = pos.y;
}


//returns the position of the hazard
sf::Vector2f Hazards::getPos(){
  return pos;
}


//
void Hazards::Toggle(){
  this->toggled= !toggled;

}

void Hazards::Reset(){
  this->toggled= false;

}

void Hazards::setSize(int size){
  this->size = size;
}

void Hazards::setOrientation(bool orient){
  this->orient = orient;
}

void Hazards::PlayerContact(PlayerChar &player, int id){
  if(toggled==false){

  player.respawn();
}

}
