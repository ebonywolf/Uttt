#include "HitBox.h"


namespace pg
{

HitBox::HitBox ( std::vector<pg::Coord> lista ) : polygon ( Polygon ( lista ) ) {
 position = polygon.getCenter();

}

HitBox::HitBox ( pg::Polygon pos ) : polygon ( pos )
{
	position = polygon.getCenter();
}


HitBox::~HitBox()
{
	//dtor
}
bool HitBox::circularCollision(HitBox* hb) const{
    double distance = this->getRadius()+ hb->getRadius();
    double max = position.distance( hb->position );
    if( distance>=max)return true;
    return false;
}
bool HitBox::collides ( HitBox* b )
{
//TODO : Collison

}
pg::Polygon HitBox::getClippedPolygon(HitBox* hb){
    auto cords = polygon.clipTo(hb->getPolygon());
    return Polygon(cords);

}


}
