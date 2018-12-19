#include "Camera.h"

namespace pg
{

Camera::Camera ( pg::Coord size, pg::Coord position , double rotation ) :
	hb ( new SquareHitBox ( position, size ) )
{
    HitBox* hb = this->getHitBox();


    hb->rotate(rotation);


}
void Camera::move(pg::Coord cord){


    double ang=hb->getAngle();

    cord.rotate(Coord(0,0),ang);

    hb->move(cord);
}

Camera::~Camera()
{
	if ( hb != nullptr ) {
		delete ( hb );
	}


}




}

