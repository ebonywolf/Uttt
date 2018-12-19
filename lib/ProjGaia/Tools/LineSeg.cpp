#include "LineSeg.h"
#include <math.h>
#include "Angle.h"

#define M_PI		3.14159265358979323846

using namespace pg;

static double abso ( double a )
{
	if ( a < 0 ) { return a * -1; }
	return a;
}

namespace pg
{
LineSeg::LineSeg() :
	vector ( Coord ( 0, 0 ) ), start ( Coord ( 0, 0 ) ), speed ( 0 ),  angleXY(), angleYZ()
{

}
LineSeg::LineSeg ( Coord start, Coord end ) :
	start ( start ), vector ( end - start )
{

	updateSpeed();
	updateAngle();

}
LineSeg::LineSeg ( Coord start, double angle, double speed )
	: start ( start ), speed ( speed ), angleXY ( angle )
{
	updateVector();
}
LineSeg::LineSeg ( Coord vetor ) :
	vector ( vetor ), start ( Coord() )
{
	updateAngle();
	updateSpeed();

}
LineSeg::LineSeg ( double angle, double speed )
	: angleXY ( angle ), speed ( speed ), start ( Coord() )
{
	updateVector();
}

LineSeg::~LineSeg()
{

}
double LineSeg::getAngle ( Coord::AXIS axis ) const
{
	switch ( axis ) {
		case Coord::XY :
			return angleXY;
		case Coord::XZ :
			return angleXZ;

		case Coord::YZ :
			return angleYZ;


		default:
			return 0;

	}



}
void LineSeg::setVector ( Coord coord )
{
	vector = coord;
	updateSpeed();
	updateAngle();

}
void LineSeg::setSpeed ( double _speed )
{
	speed = _speed;
	updateVector();

}

void LineSeg::setEnd ( Coord en )
{
	vector = en - start;
	updateSpeed();
	updateAngle();

}
void LineSeg::setAngle ( double _angle , Coord::AXIS axis )
{

	switch ( axis ) {
		case Coord::XY :
			{angleXY = _angle; break;}
		case Coord::XZ :
			{angleXZ = _angle; break;}

		case Coord::YZ :
			{angleYZ = _angle; break;}


		default:
			break;
	}

	updateVector();
}

LineSeg LineSeg::operator+ ( LineSeg line )
{
	LineSeg novo;
	novo.setVector ( this->vector + line.vector );
	novo.setStart ( this->start + line.start );
	novo.updateAngle();
	novo.updateSpeed();
	return novo;

}
LineSeg LineSeg::operator- ( LineSeg line )
{
	LineSeg novo;
	novo.setVector ( this->vector - line.vector );
	novo.setStart ( this->start - line.start );
	novo.updateAngle();
	novo.updateSpeed();
	return novo;
}

bool LineSeg::operator== ( LineSeg line )
{
	if ( vector == line.vector && start == line.start ) { return true; }
	return false;
}
bool LineSeg::operator!= ( LineSeg line )
{
	return ! ( *this == line );
}
void  LineSeg::updateSpeed()
{
	speed = vector.modulo();
}

void  LineSeg::updateVector()
{
	vector.x =  speed * ( cos ( angleXY ) * abso ( cos ( angleXZ ) )  );
	vector.y =  speed * ( sin ( angleXY ) * abso ( cos ( angleYZ ) ) );
	vector.z =  speed * ( sin ( angleYZ ) * abso ( sin ( angleXZ ) ) );

}
void  LineSeg::updateAngle()
{


	angleXY = atan ( vector.y / vector.x );
	if ( ! ( angleXY == angleXY ) ) { angleXY = 0; }


	angleYZ = atan ( vector.z / vector.y );
	if ( ! ( angleYZ == angleYZ ) ) { angleYZ = 0; }

    angleXZ = atan ( vector.z / vector.x );
	if ( ! ( angleXZ == angleXZ ) ) { angleXZ = 0; }



	if ( vector.x < 0 ) {
      angleXY = Angle::invertAngle ( angleXY );
	//	angleXZ = Angle::invertAngle ( angleXZ );
	}
	if ( vector.y < 0 ) {
		angleYZ = Angle::invertAngle ( angleYZ );

		//  angleXZ = Angle::invertAngle ( angleXZ );
	}
	if ( vector.z < 0 ) {
		// angleYZ = Angle::invertAngle ( angleYZ );
	}


}
void LineSeg::moveStart ( double time )
{
	start = start + vector * time;


}
void LineSeg::rotate ( double arc, Coord::AXIS axis )
{
	double* angle ;

	switch ( axis ) {
		case Coord::XY : {
				angle = &angleXY;
				break;
			}
		case Coord::XZ : {
//				angle = &angleXZ;
				break;
			}
		case Coord::YZ : {
				angle = &angleYZ;
				break;
			}

		default:
			break;
	}


	*angle = *angle + arc;
	while ( *angle >= 2 * PI ) {
		*angle -= 2 * PI;
	}
	while ( *angle <= -2 * PI ) {
		*angle += 2 * PI;
	}
}
bool LineSeg::isParallel ( LineSeg line )
{
	if ( Angle::shortestAngle ( angleXY ) == Angle::shortestAngle ( line.angleXY ) ) { return true; }
	return false;
}
bool LineSeg::pointBelongsTo ( Coord coord )
{
	double a = getTimetoPoint ( coord );
	if ( isinf ( a ) ) { return false; }
	return true;

}
double LineSeg::getTimetoPoint ( Coord coord )
{
	/** c*V + start= Coord
	c*V.x + start.x= Coord.x
	c*V.y + start.y= Coord.y
	 */
	Coord var = ( coord - start );
	double f = var.x / vector.x;
	if ( var.x == 0 ) { f = 0; }

	double g = var.y / vector.y;
	if ( var.y == 0 ) { g = 0; }

	if ( f == g ) { return f; }

	if ( vector * g + start == coord ) { return g; }
	if ( vector * f + start == coord ) { return f; }

	return 1.0 / 0.0;

}
Coord LineSeg::interSect ( LineSeg line )
{
	double j = timeToInterSect ( line );
	Coord cross = vector * j + start;
	return cross;
}
double LineSeg::timeToInterSect ( LineSeg line )
{
	double m[2][3];

	m[0][0] = this->vector.x;
	m[1][0] = this->vector.y;

	m[0][1] = -line.vector.x;
	m[1][1] = -line.vector.y;
	//TODO :Consertar Gambiarra
	//GAMBIARRA
	for ( int i = 0; i < 2; i++ ) {
		for ( int j = 0; j < 2; j++ ) {
			if ( m[i][j] == 0 ) { m[i][j] += 0.0000001; }

		}


	}
	//FIM DE GAMBIARRA


	//det verifica se é paralelo
	int det = m[0][0] * m[1][1] - ( m[1][0] * m[0][1] );
	if ( det == 0 ) { return 1.0 / 0.0; }

	m[0][2] = -this->start.x + line.start.x;
	m[1][2] = -this->start.y + line.start.y;



	//descobre y
	double mult = m[1][0] / m[0][0];
	for ( int i = 0; i < 3; i++ ) {
		m[1][i] = m[1][i] - m[0][i] * mult;
	}
	//descobre x
	mult = m[0][1] / m[1][1];
	for ( int i = 0; i < 3; i++ ) {
		m[0][i] = m[0][i] - m[1][i] * mult;

	}
	double j = m[0][2] / m[0][0];
	// double u=m[1][2]/m[1][1];
	return j;
}
}
