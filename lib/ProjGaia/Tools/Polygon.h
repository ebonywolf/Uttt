#ifndef POLYGON_H
#define POLYGON_H
#include <vector>


#include <iostream>
#include "LineSeg.h"


namespace pg
{

class Polygon
{
	public:
		/** Default constructor */
		Polygon() = default;
		Polygon ( std::vector<pg::LineSeg> axis );
		Polygon ( std::vector<pg::Coord> vertexs );

		void rotate ( double angle, pg::Coord point, pg::Coord::AXIS = pg::Coord::AXIS::XY );
		void scale ( pg::Coord value , pg::Coord  point );
		void scaleOrigin ( pg::Coord value);
		void move ( pg::Coord value );
		void setOrigin ( pg::Coord point );

		std::vector<Coord> clipTo(const Polygon &p);
		void lineClip(const Polygon& outside);


		static Polygon create_square ( pg::Coord pos, pg::Coord size );
		static Polygon create_regular_polygon ( pg::Coord pos, pg::Coord size , unsigned sides );
        static std::vector<Polygon> createBox(double width, double height, double length, Coord pos);
        static std::vector<LineSeg> coordsToLine(std::vector<Coord> cords);


		Coord getCenter()const;

		virtual ~Polygon();

		virtual bool pointBelongsTo ( pg::Coord coord );
		double getRadius() const;
		std::vector<pg::LineSeg> getLines() const;
		std::vector<pg::Coord> getPoints() const;
		friend std::ostream& operator<< ( std::ostream& os, const Polygon& p )
		{
			for ( auto x : p.axis ) {
				os << x.getStart() << "\n";
			}
			return os;
		}

    std::vector<pg::LineSeg>* __getAxis(){return &axis;};

	protected:

		 std::vector<pg::LineSeg> axis;
	private:
	    pg::Coord center;
        int furthestPoint;
        void calculateCenter();
	    void calculateFurthestPoint();

};

}
#endif // POLYGON_H
