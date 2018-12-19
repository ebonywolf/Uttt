//#pragma GCC optimize("inline")
//#pragma GCC optimize("O3")
//#pragma GCC optimize("omit-frame-pointer")

#include "Uttt.h"

#include "Uttt2.h"
#include "MinMaxTree.h"
#include <vector>
#include <thread>
#include <memory>
#include <time.h>
#include <mutex>


using namespace wag;
using namespace std;
template <class T>
ostream& operator<<(ostream& os, list<T>& lista){
    for(auto x: lista){
        os<< x<<" "<<endl;
    }
    return os;
}

template <class T>
void showList(ostream& os, list<T>& lista){
    for(auto& x: lista){
        os<< *x<<" "<<endl;
    }
}

int main()
{
	Uttt game = Uttt ( 6 );
	Uttt2 game2 = Uttt2 ( 6 );
	TreeConfig config;
	config.limit = 500000;
	config.minimalScore=800;
	config.chosenMovePercent=0.1;

	MinMaxTree<Uttt, Uttt::Move* > tree(config);
	MinMaxTree<Uttt2, Uttt2::Move* > tree2(config);

	game.player = 0;


	tree.setGame ( &game, 0 );
    tree2.setGame(&game2,0);

	tree2.update();
	tree.update();

	//tree.update();

    int opponentRow;
    int opponentCol;
     int validActionCount;
    int row=0;
    int col=0;
    Uttt::Move* mov ;
    Uttt2::Move* mov2 ;

    tree2.update(800);
    tree.update(800);

    while (1) {
       tree2.update(80);
       tree.update(80);

        if(game.player){
            mov2 = tree2.getMovement();
            mov = Uttt::everyMove[mov2->x][mov2->y] ;

        }else{
            mov = tree.getMovement();
            mov2 = Uttt2::everyMove[mov->x][mov->y] ;
        }

        tree.sendCommand(mov);
        tree2.sendCommand(mov2);


        game.move(mov);
        game2.move(mov2);
        std::cout << "Player:"<<game.player<<" plays:"<<*mov << std::endl;
        std::cout << game << std::endl;
        std::cout  << std::endl;
        if(game.done)return 0;


    }

}
