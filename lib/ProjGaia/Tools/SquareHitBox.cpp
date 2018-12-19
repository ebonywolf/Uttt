#include "SquareHitBox.h"
#include "LineSeg.h"
#include "Angle.h"
#include "Utilities.h"
namespace pg
{
SquareHitBox::SquareHitBox ( pg::Coord pos , pg::Coord size ) :
	HitBox ( pg::Polygon::create_square ( pos, size ) )
{
	width = size.x;
	height = size.y;
	origin = polygon.getPoints() [0];
	angle=0;





}

SquareHitBox::~SquareHitBox()
{
	//dtor
}


void SquareHitBox::rotate ( double ang)
{


	HitBox::rotate ( ang );
	LineSeg seg = polygon.getLines() [0];
	origin = seg.getStart();

	angle += ang;

	if ( angle > pg::pi2 ) { angle -= pg::pi2; }
	if ( angle < -pg::pi2 ) { angle += pg::pi2; }

}
void SquareHitBox::scale ( pg::Coord value )
{

	HitBox::scale ( value );
	origin = polygon.getLines() [0].getStart();

	width *= value.x;
	height *= value.y;
}
void SquareHitBox::scaleOrigin ( pg::Coord value )
{

	polygon.scale ( value, origin );

	width *= value.x;
	height *= value.y;


}
///rotates around center
void SquareHitBox::rotateOrigin ( double ang )
{
	polygon.rotate ( ang, origin );
	angle += ang;

	if ( angle > pg::pi2 ) { angle -= pg::pi2; }
	if ( angle < -pg::pi2 ) { angle += pg::pi2; }

}
 void SquareHitBox::rotatePoint ( double ang, pg::Coord point )
{
    HitBox::rotate ( ang );
	origin = polygon.getLines() [0].getStart();

	angle += ang;

	if ( angle > pg::pi2 ) { angle -= pg::pi2; }
	if ( angle < -pg::pi2 ) { angle += pg::pi2; }

}
void SquareHitBox::move(pg::Coord cord){
    HitBox::move(cord);
    origin+= cord;

}

///rotates around point 0,0

bool SquareHitBox::pointBelongsTo ( pg::Coord coord )
{
    /// here we do basic square collision
    if(angle<0.000000001d && angle> -0.000000001d){
        if ( coord.x <= origin.x || coord.y <= origin.y ) { return false; } //verifica se esta acima ou a direita da caixa
        if ( coord.x >= origin.x + width ) { return false; } //verifica se está alem do eixo x
        if ( coord.y >= origin.y + height ) { return false; }
        return true;
    }else{
    ///if our square is inclined we do a triangle collision
        auto lista = polygon.getLines();
        std::vector<Coord> bothalf = {lista[0].getStart(),lista[1].getStart(),lista[2].getStart()   };

      //  if( pointInTriangle(coord,bothalf) )return true;

        std::vector<Coord> tophalf = {lista[0].getStart(),lista[3].getStart(),lista[2].getStart()   };

        //if( pointInTriangle(coord,tophalf) )return true;

        return false;

    }


}

bool SquareHitBox::collides ( HitBox* b )
{
//TODO : Collison

}
/**Gives the all the transformation info required to do a linear transformation of one area to another
    This method assumes both squares were created using the default constructor and that you didnt add or remove the polygons point
 */
TransformationInfo SquareHitBox::transformTo ( SquareHitBox* hb2 )
{
	TransformationInfo info;

	///Angle of the first line, second vector - first gives the disired change angle
	info.angle = hb2->angle - this->angle;
	info.offset = hb2->origin - this->origin;

    info.cameraOrigin = origin;
    info.canvasOrigin = hb2->origin;



  	///proportion assumes the polygon is a square shape.;
	/// speed of the vector is the module of its coordenates
	if ( this->width == 0  || this->height == 0 ) { throw "SquareHitBox::Transformto polygons have 0 area square"; }

	info.proportion.x = hb2->width / this->width;
	info.proportion.y = hb2->height / this->height;;
    info.proportion.z = 1;

	return info;

}



}
