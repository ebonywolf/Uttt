#include "Polygon.h"
#include "LineSeg.h"
#include <cmath>
#include "Angle.h"

using namespace pg;
using namespace std;
namespace pg
{

Polygon::Polygon ( vector<LineSeg> axis )
{
    if(axis.size()<1)throw "unable to make polygons with less than 1 line";
	this->axis = axis;
	calculateFurthestPoint();
	calculateCenter();
}
Polygon::Polygon ( vector<Coord> vertexs ) : axis ( vector<LineSeg>() )
{

	if ( vertexs.size() < 1 ) {throw "Cannot build polygon with no points";}


	axis = coordsToLine(vertexs);

	calculateFurthestPoint();
	calculateCenter();
}

Polygon::~Polygon()
{
	//dtor
}

 vector<LineSeg> Polygon::coordsToLine(vector<Coord> vertexs){
     vector<LineSeg> lines;
    if ( vertexs.size() == 1 ) {
		LineSeg line ( vertexs[0], vertexs[0] );
		lines.push_back ( line );
		return lines;
	}
	if ( vertexs.size() < 1 ) {return lines;}

	Coord last = vertexs[0];
	Coord firstCord = vertexs[0];
	bool isFirst = true;
	for ( auto x : vertexs ) {
		if ( isFirst ) {
			isFirst = false;
			last = x;
			continue;
		}
		LineSeg line = LineSeg ( last, x );

		lines.push_back ( line );
		last = x;
	}
	///fechando conexão do poligono
	LineSeg line = LineSeg ( last, firstCord );
	lines.push_back ( line );
	return lines;

 }
vector<LineSeg> Polygon::getLines() const
{

	return axis;
}

bool Polygon::pointBelongsTo ( Coord c )
{
	LineSeg seg = axis.front();

	Coord middle = seg.getVector() * 0.5 + seg.getStart();

	seg = LineSeg ( c, middle ); ///faz uma linha do ponto c até o meio de uma aresta do poligono.

	int count = 0;
	for ( auto w : axis ) {
		double time = w.timeToInterSect ( seg );
		double time2 = seg.timeToInterSect ( w );

		if ( time2 > 0  && time <= 1 && time >= 0 ) {
			count++;

		}
	}

	if ( count % 2 == 0 ) { return false; }
	return true;
}
vector<Polygon> Polygon::createBox ( double width, double height, double length, Coord pos )
{
    /*
	std::vector<LineSeg> segs;
	segs.push_back ( LineSeg ( pos  ,  pos + Coord ( 0, height, 0 ) ) );
	segs.push_back ( LineSeg ( pos + Coord ( 0, 0, length )  ,  pos + Coord ( 0, height, length ) ) );


	segs.push_back ( LineSeg ( pos + Coord ( width, height, 0 )  ,  pos + Coord ( width, 0, 0 ) ) );
	segs.push_back ( LineSeg ( pos + Coord ( width, height, length )  ,  pos + Coord ( width, 0, length ) ) );

	segs.push_back ( LineSeg ( pos  ,  pos + Coord ( width, 0, 0 ) ) );
	segs.push_back ( LineSeg ( pos  + Coord ( 0, 0, length ) ,  pos + Coord ( width, 0, length ) ) );


	segs.push_back ( LineSeg ( pos + Coord ( width, height, 0 )  ,  pos + Coord ( 0, height, 0 ) ) );
	segs.push_back ( LineSeg ( pos + Coord ( width, height, length )  ,  pos + Coord ( 0, height, length ) ) );


	segs.push_back ( LineSeg ( pos + Coord ( 0, 0, length ) , pos ) );
	segs.push_back ( LineSeg ( pos + Coord ( width, 0, length )   , pos + Coord ( width, 0, 0 ) ) );

	segs.push_back ( LineSeg ( pos + Coord ( width, height, 0 ) ,  pos + Coord ( width, height, length ) ) );
	segs.push_back ( LineSeg ( pos + Coord ( 0, height, 0 ) ,  pos + Coord ( 0, height, length ) ) );
	return Polygon ( segs );
	*/
	vector<vector<Coord>> faces;
	faces.resize(6);
	 faces[0] = { {0  ,   0   ,0},
                            {width,0,   0},
                            {width,height,0},
                            {0, height, 0}  };

    faces[1]  = { {0  ,   0   ,0},
                            {0,0,   length},
                            {0,height,length},
                            {0, height, 0}  };
     faces[2] = { {0  ,   0   ,0},
                            {0,0,   length},
                            {width,0,length},
                            {width, 0, 0}  };

     faces[3] = { {0  ,   height   ,0},
                            {0,height,   length},
                            {width,height,length},
                            {width, height, 0}  };

    faces[4] = { {width  ,   0   ,0},
                            {width,height,   0},
                            {width,height,length},
                            {width, 0, length}  };

    faces[5] = { {0  ,   0   ,length},
                            {width,0,   length},
                            {width,height,length},
                            {0, height, length}  };
    vector<Polygon> polies;

    for ( auto face : faces ){
        Polygon poly = Polygon(face);
        poly.move(pos);
        polies.push_back(poly);
    }
    return polies;



}


void Polygon::rotate ( double angle, Coord origin, pg::Coord::AXIS ax )
{
	///gambis de otimizaćão
	if ( angle < 0.0000000001 && angle > -0.0000000001 ) { return; }

	Coord trans = Coord ( cos ( angle ), sin ( angle ) );
int i=1;
	for ( auto &line : axis ) {
    i++;
    if(i==12){
        int debu=222;

    }
		Coord point = line.getStart();
		auto en = line.getEnd();

		point.rotate ( origin, angle, ax );
		line.setStart ( point );

        en.rotate(origin, angle, ax);
        line.setEnd(en);
        /*
		double ang = line.getAngle ( ax ) + angle;
		line.setAngle ( ang, ax );
		*/
	}
	calculateCenter();
}

void Polygon::scale ( pg::Coord value, Coord point )
{

	double modulo = value.modulo();
	if ( modulo < 0.0000000001 && modulo > -0.0000000001 ) { return; }


	for ( auto &x : axis ) {
		Coord c = x.getStart();

		c = c - point;
		Coord var = c * value;
		c = point + var;
		x.setStart ( c );
		auto alce = x.getVector() * value;
		x.setVector ( alce );

	}
	calculateCenter();
}
void Polygon::scaleOrigin ( pg::Coord value )
{
	double modulo = value.modulo();
	if ( modulo < 0.0000000001 && modulo > -0.0000000001 ) { return; }


	for ( auto &x : axis ) {
		Coord c = x.getStart();

		Coord var = c * value;
		x.setStart ( var );

		x.setVector ( x.getVector() * value );

	}
	calculateCenter();
}

void Polygon::move ( pg::Coord value )
{
	double modulo = value.modulo();
	if ( modulo < 0.0000000001 && modulo > -0.0000000001 ) { return; }


	for ( auto &x : axis ) {
		Coord c = x.getStart();
		x.setStart ( c + value );
	}
	calculateCenter();
}

std::vector<Coord> Polygon::getPoints() const
{
	std::vector<Coord> lista;
	for ( auto x : axis ) {
		lista.push_back ( x.getStart() );
	}

	return lista;

}

Polygon Polygon::create_square ( pg::Coord pos, pg::Coord size )
{

	std::vector<Coord> lista;
	lista.push_back ( pos );
	lista.push_back ( Coord ( pos.x + size.x , pos.y ) );
	lista.push_back ( Coord ( pos.x + size.x , pos.y + size.y ) );
	lista.push_back ( Coord ( pos.x , pos.y + size.y ) );
	return Polygon ( lista );


}
Polygon  Polygon::create_regular_polygon ( pg::Coord pos, pg::Coord size , unsigned sides )
{

	double angleMod = pg::pi2 / ( double ) sides;
	double angle = 0;
	vector<Coord> points;
	for ( int i = 0 ; i < sides ; i++ ) {
		Coord vetor = {cos ( angle ), sin ( angle )    } ;

		Coord ponto = pos + size * vetor;

		points.push_back ( ponto );
		angle += angleMod;
	}
	return Polygon ( points );

}


Coord Polygon::getCenter() const
{
	return center;
}
void Polygon::calculateCenter()
{
	Coord sum;
	for ( auto x : this->axis ) {
		sum += x.getStart();
	}
	sum /= axis.size();
	center = sum;
}

double Polygon::getRadius() const
{
	return axis[ furthestPoint ].getStart().distance ( getCenter() );
}
void Polygon::calculateFurthestPoint()
{
	int f = 0;
	auto center = getCenter();
	double dist = axis[0].getStart().distance ( center );
	int i = 0;
	for ( auto line : axis ) {
		double ndist = line.getStart().distance ( center );
		if ( ndist > dist ) {
			f = i;
			dist = ndist;
		}
		i++;
	}
	furthestPoint = i;
}
static int left_of ( Coord a, Coord b, Coord c )
{
	double x;

	a = b - a;
	b = c - b;

	x = a.cross ( b );
	return x < 0 ? -1 : x > 0;

}

static int line_sect ( Coord x0, Coord x1, Coord y0, Coord y1, Coord* res )
{
	Coord dx, dy, d;

	dx = x1 - x0;
	dy = y1 - y0;
	d = x0 - y0;


	double dyx = dy.cross ( dx );

	if ( !dyx ) { return 0; }

	dyx = d.cross ( dx ) / dyx;
	if ( dyx <= 0 || dyx >= 1 ) { return 0; }

	res->x = y0.x + dyx * dy.x;
	res->y = y0.y + dyx * dy.y;
	return 1;

}
static vector<Coord> poly_edge_clip ( const vector<Coord>& sub, Coord x0, Coord x1, int left )
{
	int side0;

	Coord v0 = sub[ sub.size() - 1];
	Coord tmp;
	vector<Coord> novo;

	side0 = left_of ( x0, x1, v0 );
	if ( side0 != -left ) { novo.push_back ( v0 ); }


	for ( int i = 0; i < sub.size(); i++ ) {
		Coord v1 = sub[i];

		int side1 = left_of ( x0, x1, v1 );
		if ( side0 + side1 == 0 && side0 ) {
			/* last point and current straddle the edge */
			if ( line_sect ( x0, x1, v0, v1, &tmp ) ) {
				novo.push_back ( tmp );
			}
		}
		if ( i == sub.size() - 1 ) { break; }
		if ( side1 != -left ) { novo.push_back ( v1 ); }
		v0 = v1;
		side0 = side1;
	}
	return novo;

}

vector<Coord> Polygon::clipTo ( const Polygon &p )
{

	vector<Coord> box = p.getPoints();
	vector<Coord> obj = this->getPoints();
	vector<Coord> tmp, p1;


	int dir = left_of ( box[0] , box[1], box[2] );

	vector<Coord> p2 = poly_edge_clip ( obj, box.back(), box[0], dir );

	for ( int i = 0; i < box.size() - 1; i++ ) {
		tmp = p2;
		p2 = p1;
		p1 = tmp;
		if ( p1.size() == 0 ) {
			break;
		}
		p2 = poly_edge_clip ( p1, box[i], box[i + 1], dir );
	}


	return p2;

}





}
