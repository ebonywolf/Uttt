#include "Sprite.h"
#include "../Tools/SquareHitBox.h"

using namespace pg;

Sprite::Sprite ( HitBox* h ) : hitbox ( h )
{

}
Sprite::Sprite ( std::vector<Coord> lista ) :
	hitbox ( new HitBox ( lista ) )
{


}
Sprite::Sprite( const Sprite &s):Sprite(s.getHitBox()->getPolygon().getPoints())
{



}

Sprite::Sprite ( Coord position = Coord(), Coord scale = Coord ( 1, 1 ), float height = 1 ) :
	hitbox ( new SquareHitBox ( position, scale ) )
{

	hitbox->z = height;
}

Sprite::~Sprite()
{
	if ( hitbox != nullptr && _deleteHitbox ) {
		delete ( hitbox );
	}
}
