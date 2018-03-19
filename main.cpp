//#pragma GCC optimize("inline")
//#pragma GCC optimize("O3")
//#pragma GCC optimize("omit-frame-pointer")

#include "Uttt.h"
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
	TreeConfig config;
	config.limit = 500000;

	MinMaxTree<Uttt, Uttt::Move* > tree(config);
	game.player = 1;

	tree.setGame ( &game, 0 );

	tree.update();
	//tree.update();

    int opponentRow;
    int opponentCol;
     int validActionCount;
    int row=0;
    int col=0;
  Uttt::Move* mov ;

  tree.update(80);

    while (1) {

        cin >> opponentRow >> opponentCol; cin.ignore();

        cin >> validActionCount; cin.ignore();

        for (int i = 0; i < validActionCount; i++) {
            cin >> row >> col; cin.ignore();
        }


        if(opponentRow== -1){
           game.player=!game.player;
        }else{
            mov = Uttt::everyMove[opponentCol][opponentRow] ;
            game.move(mov);
            tree.sendCommand(mov);
        }

        try{
        tree.update(70);

        mov = tree.getMovement();
        game.move(mov);
        tree.sendCommand(mov);
    cerr<<*mov<<endl;
        std::cout<< mov->y <<" "<<mov->x <<std::endl;
        }catch(...){
            cout<<row<<" "<<col<<endl;
        }


    }

}
