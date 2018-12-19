#ifndef CURVEDHITBOX_H
#define CURVEDHITBOX_H
#include "HitBox.h"
#include "Coord.h"

namespace pg{
class CurvedHitBox : public HitBox
{
    public:

        CurvedHitBox(std::vector<Coord> anchors, double accuracy=10000.0);

        virtual void rotate ( double angle ) ;
        virtual void rotatePoint ( double angle, pg::Coord point ) ;
		virtual void scale ( pg::Coord vetor ) ;

		virtual void move ( pg::Coord vetor ) ;


        virtual pg::Polygon getPolygon();
        virtual pg::Polygon getClippedPolygon(HitBox*);


        virtual ~CurvedHitBox();

    protected:
        bool needsupdate=false;
        void updatePolygon();
        std::vector<Coord> anchors;
        double accuracy = 10000;


    private:
};

}
#endif // CURVEDHITBOX_H
