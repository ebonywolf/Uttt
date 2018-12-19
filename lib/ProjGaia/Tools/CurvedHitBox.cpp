#include "CurvedHitBox.h"
namespace pg{
CurvedHitBox::CurvedHitBox ( std::vector<Coord> anchors, double accuracy ):
    anchors(anchors),accuracy(accuracy)
{
    updatePolygon();
    closed=false;

}

CurvedHitBox::~CurvedHitBox()
{

}

void CurvedHitBox::rotate ( double angle )
{
    for ( auto& cord: anchors ){
        cord.rotate(position,angle);
    }
    needsupdate=true;

}
void CurvedHitBox::rotatePoint ( double angle, pg::Coord point )
{

     for ( auto& cord: anchors ){
        cord.rotate(point,angle);
    }
    position.rotate(point,angle);
    needsupdate=true;

}
void CurvedHitBox::scale ( pg::Coord value )
{
    auto point = position;
    for ( auto &c : anchors ) {
		c = c - point;
		Coord var = c* value;
		c = point+var;

	}
	position = ((position - point)*value)+point;

	needsupdate=true;

}

pg::Polygon CurvedHitBox::getPolygon(){
    if(needsupdate){
        updatePolygon();
        needsupdate=false;
    }
    return polygon;
}
void CurvedHitBox::move ( pg::Coord vetor ){
    for ( auto& cord: anchors ){
        cord+=vetor;
    }
    needsupdate=true;

}


double inline interpolate ( double n1, double n2, double prec )
{
	return n1 + ( ( n2 - n1 ) * prec );
}


pg::Polygon  CurvedHitBox::getClippedPolygon(HitBox* hb){

	if ( anchors.size() <= 2 ) {
		return Polygon(anchors);
	}

	std::vector<Coord> end;
	if(hb->pointBelongsTo(anchors[0])){
        end.push_back ( anchors[0] );
	}

	for ( double c = 0; c < accuracy; c++ ) {
		double i = c / accuracy;
		std::vector<Coord> temp;
		for ( unsigned int j = 1; j < anchors.size(); ++j ) {
            auto cord=Coord ( interpolate ( anchors[j - 1].x, anchors[j].x, i ),
                                 interpolate ( anchors[j - 1].y, anchors[j].y, i ) ) ;
			temp.push_back ( cord);
		}
		while ( temp.size() > 1 ) {
			std::vector<Coord> temp2;

			for ( unsigned int j = 1; j < temp.size(); ++j ){
			       auto cord =Coord ( interpolate ( temp[j - 1].x, temp[j].x, i ),
				                          interpolate ( temp[j - 1].y, temp[j].y, i ) );
                    temp2.push_back ( cord );

			}

			temp = temp2;
		}


        if(hb->pointBelongsTo(temp[0])){
            end.push_back ( temp[0] );
        }
	}

	return Polygon(end);



}

void CurvedHitBox::updatePolygon ()
{
	if ( anchors.size() <= 2 ) {
		polygon = Polygon(anchors);
		position = polygon.getCenter();
		return;
	}

	std::vector<Coord> end;
	end.push_back ( anchors[0] );
	for ( double c = 0; c < accuracy; c++ ) {
		double i = c / accuracy;
		std::vector<Coord> temp;
		for ( unsigned int j = 1; j < anchors.size(); ++j ) {
            auto cord=Coord ( interpolate ( anchors[j - 1].x, anchors[j].x, i ),
                                 interpolate ( anchors[j - 1].y, anchors[j].y, i ) ) ;
			temp.push_back ( cord);
		}
		while ( temp.size() > 1 ) {
			std::vector<Coord> temp2;

			for ( unsigned int j = 1; j < temp.size(); ++j ){
			       auto cord =Coord ( interpolate ( temp[j - 1].x, temp[j].x, i ),
				                          interpolate ( temp[j - 1].y, temp[j].y, i ) );
                    temp2.push_back ( cord );

			}

			temp = temp2;
		}
		end.push_back ( temp[0] );
	}

	polygon = Polygon(end);
    position = polygon.getCenter();
}

}
