#ifndef ANGLE_H
#define ANGLE_H
#include "Coord.h"
#include <math.h>

#define PI 3.141592653589793238460d

namespace pg
{
 const double pi = 3.14159265358979323846264338327950288419716939937510;
 const double pi2 =2*3.14159265358979323846264338327950288419716939937510;

class Angle
{

	public:


		static double invertAngle ( double angle );
		static double shortestAngle ( double angle );
		static double degreeToRadian ( double angle );
		static double radianToDegree ( double angle );
		static double findangle ( pg::Coord a, pg::Coord b );
		static double turntoxy ( double angleStart, pg::Coord posStart, pg::Coord posDest );
		static double turntoangle ( double angleStart, double angleDest );
};
}
#endif
