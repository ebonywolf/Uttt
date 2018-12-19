#include "Angle.h"
using namespace pg;

namespace pg
{

static double roundAngle(double angle){
    double value = angle / pi2 ;
	if(value>1){
        int turns = value;
        angle = angle - pi2*(double)turns;
	}
	return angle;

}
double Angle::invertAngle ( double angle )
{
	angle += PI;

	angle = roundAngle(angle);
	return angle;
}
double Angle::shortestAngle ( double angle )
{
	bool sign = false;
	if ( angle < 0 ) {
		angle *= -1;
		sign = true;
	}

	angle = roundAngle(angle);

	if ( angle > PI ) {
		angle = 2 * PI - angle;
		angle *= -1;
	}

	if ( sign ) {
		angle *= -1;
	}
	return angle;
}
double Angle::degreeToRadian ( double angle )
{
	return angle * PI / 180;
}
double Angle::radianToDegree ( double angle )
{
	return angle * 180 / PI;
}
double Angle::findangle ( Coord a, Coord b )
{
	double x = b.x - a.x;
	double y = b.y - a.y;
	double angle;
	angle = atan ( y / x );
	if ( angle < 0 ) { angle = angle * -1; }
	if ( x < 0 && y > 0 ) {
		angle = PI - angle;
	}
	if ( x < 0 && y < 0 ) {
		angle = PI + angle;
	}
	if ( x > 0 && y < 0 ) {
		angle = 2 * PI - angle;
	}

	return angle;
}
double Angle::turntoxy ( double angleStart, Coord posStart, Coord posDest )
{
	double angle1 = findangle ( posStart, posDest );

	angleStart = angle1 - angleStart;

	angleStart = roundAngle(angleStart);

	if ( angleStart > PI ) { angleStart = angleStart - 2 * PI; }
	if ( angleStart < -PI ) { angleStart = 2 * PI + angleStart; }

	return angleStart;

}
double Angle::turntoangle ( double angleStart, double angleDest )
{
	double angle = angleDest - angleStart;
	if ( angle > PI ) { angle = angle - 2 * PI; }
	if ( angle < -PI ) { angle = 2 * PI + angle; }
	return angle;
}
}
