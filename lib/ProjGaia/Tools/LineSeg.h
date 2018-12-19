#ifndef LINESEG_H
#define LINESEG_H
#include "Coord.h"

namespace pg
{
class LineSeg
{

	public:
		/** Default constructor */


		LineSeg();
		LineSeg ( pg::Coord start, pg::Coord end );
		LineSeg ( pg::Coord start, double angle, double speed );
		LineSeg ( pg::Coord vetor );
		LineSeg ( double angle, double speed );
		virtual ~LineSeg();

		void moveStart ( double time );
		void rotate ( double arc, Coord::AXIS = Coord::XY);
		bool isParallel ( LineSeg );
		bool pointBelongsTo ( pg::Coord );
		double getTimetoPoint ( pg::Coord );
		pg::Coord interSect ( LineSeg );
		double timeToInterSect ( LineSeg );


		void setVector ( pg::Coord );
		void setSpeed ( double );

		void setAngle ( double , Coord::AXIS = Coord::XY);

        void setEnd(pg::Coord coord);
		void setStart ( pg::Coord coord ) {start = coord;};

		pg::Coord getVector() const{return vector;}
		double getSpeed() const{return speed;}
		double getAngle(Coord::AXIS = Coord::XY) const;

		pg::Coord getStart() const{return start;}
		pg::Coord getEnd()const{return start+vector;}

		LineSeg operator+ ( LineSeg );
		LineSeg operator- ( LineSeg );
		bool operator== ( LineSeg );
		bool operator!= ( LineSeg );

	protected:
		pg::Coord vector;

		double speed;
		pg::Coord start;
		double angleXY;
		double angleXZ;
       double angleYZ;
	private:
		void updateSpeed();
		void updateVector();
		void updateAngle();
};
}
#endif // LINESEG_H
