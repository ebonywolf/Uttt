#include "HitBox3d.h"

using namespace std;
namespace pg
{





HitBox3d::HitBox3d ( std::vector<Polygon> faces ) :
	faces ( faces ), usingFaces ( true )
{
	updatePosition();
}


void HitBox3d::rotate ( double angle, Coord::AXIS axis  )
{
    if(!usingFaces){
        polygon.rotate ( angle, position, axis );
        position = polygon.getCenter();
    }else{
        for ( auto& poly : faces ){
              poly.rotate ( angle, position, axis );
        }
        updatePosition();

    }
}
 double  HitBox3d::getRadius()const {
     if(!usingFaces)return polygon.getRadius();
     else{
        double biggest=0;
        for ( auto face : faces ){
            if(face.getRadius()>biggest)biggest = face.getRadius();
        }
        return biggest;


     }

}
void HitBox3d::rotatePoint ( double angle, pg::Coord point, Coord::AXIS axis )
{
    if(!usingFaces){
        polygon.rotate ( angle, point,axis );
        position = polygon.getCenter();
    }
    else{
            for ( auto& poly : faces ){
              poly.rotate ( angle, point,axis );
        }
        updatePosition();
    }
}
void HitBox3d::scale ( pg::Coord vetor )
{
    if(!usingFaces){
        polygon.scale ( vetor, position );
        position = polygon.getCenter();
    }
    else{
        for ( auto& poly : faces ){
              poly.scale ( vetor, position );
        }
        updatePosition();
    }
}
void HitBox3d::move ( pg::Coord vetor )
{
    if(!usingFaces){
        polygon.move ( vetor );
        position += vetor;
    }
      else{
        for ( auto& poly : faces ){
              poly.move ( vetor );
        }
        updatePosition();
    }
}


HitBox3d::HitBox3d ( std::vector<LineSeg> lines )
{
	this->polygon = Polygon ( lines );
	position = polygon.getCenter();

}
void HitBox3d::updatePosition()
{
	if ( usingFaces ) {
		Coord sum;
		for ( auto& poly : faces ) {
			sum += poly.getCenter();
		}
		sum /= faces.size();
		position = sum;
	} else {
		position = polygon.getCenter();
	}


}


pg::Polygon HitBox3d::getClippedPolygon ( HitBox* hb )
{
	if ( !usingFaces ) {
		return this->polygon;
	} else {
	   vector<LineSeg> cords;
		auto hitPoly= hb->getPolygon();
		for ( auto& poly : faces ) {

			auto clippedFace = poly.clipTo ( hitPoly );

			auto clippedLines = Polygon::coordsToLine ( clippedFace );
			if ( clippedLines.size() >= 1 ) {
				auto it = cords.end();
				cords.insert ( it, clippedLines.begin(), clippedLines.end() );

			}

		}

		return Polygon ( cords );

	}



}
HitBox3d::HitBox3d ( Polygon p )
{
	this->polygon = p;
	position = position = polygon.getCenter();


}


}
