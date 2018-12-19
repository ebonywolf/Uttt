
#pragma GCC optimize("inline")
#pragma GCC optimize("O3")
 #pragma GCC optimize("omit-frame-pointer")
 
 
#ifndef MINMAXTREE_H_
#define MINMAXTREE_H_
#include <vector>
#include <unordered_map>
#include <list>
#include <iostream>
#include <memory>
#include <time.h>

typedef unsigned char uchar;

namespace wag
{
///just a base structure

struct Game {
    Game() = default;
    virtual ~Game()
    {
    }

    virtual bool move ( uchar t )
    {
        return 0;
    }
    ;
    virtual int getFitness()
    {
        return 0;
    }
    ;
    virtual bool getPlayer()
    {
        return 0;
    }
    ;
    Game* clone()
    {
        return new Game();
    }
};


struct TreeConfig {
    unsigned long limit=100000;
    int threads=1;
};

template<class T, class TYPE=uchar>
class MinMaxTree
{
    public:
        using GAME = std::shared_ptr<T>;
        static GAME game;

        unsigned short level = 0;
        static const bool MIN = false;
        static const bool MAX = true;
        static unsigned long long cont;


        TreeConfig config;
        struct Node : public std::enable_shared_from_this<Node> {
            std::unordered_map<TYPE, std::shared_ptr<Node> > sons;
            std::weak_ptr<Node> dad;
            bool type = 0;
            short fitness = 0;
            short trueFitness = 0;
            TYPE mov;
            TYPE root;
            TYPE transition;

            Node()
            {
                cont++;
            }
            Node ( Node* d , bool type  , int fit, TYPE transition  ) :
                type ( type ), fitness ( fit ), trueFitness ( fit ), root ( d->root ),
                transition( transition )
            {
                dad = d->shared_from_this();
                cont++;
            }
            ~Node()
            {
                cont--;

            }
            bool operator<(const Node& n )const{
                return this->fitness < n.fitness;
            }
            inline void acquireMoves(std::list<TYPE>& allMoves){
                if(!dad.expired()){
                    auto alce = dad.lock();
                    allMoves.push_front(transition);
                    alce->acquireMoves(allMoves);
                }
            }
            GAME createGame(){
                T* alce = static_cast<T*> ( game->clone() );
                GAME future = GAME ( alce );
                std::list<TYPE> moveList;
                acquireMoves(moveList);
                for(auto x: moveList){
                    future->move(x);
                }
                return future;
            }
            inline void updateFitness()
            {
                if ( sons.size() == 0 ) {
                    return;
                };
                auto it = sons.begin();

                fitness = it->second->fitness;
                mov = it->first;
                for ( auto& it : sons ) {
                    if ( type == MAX ) {
                        if ( it.second->fitness > fitness ) {
                            fitness = it.second->fitness;
                            mov = it.first;
                        } else if ( it.second->fitness == fitness && it.second->trueFitness > fitness ) {
                            mov = it.first;
                        }
                    } else {
                        if ( it.second->fitness < fitness ) {
                            fitness = it.second->fitness;
                            mov = it.first;
                        } else if ( it.second->fitness == fitness && it.second->trueFitness < fitness ) {
                            mov = it.first;
                        }
                    }
                }

                if ( !dad.expired() ) {
                    auto alce = dad.lock();
                    alce->updateFitness();
                }

            }


            inline void createSons ( std::list<std::shared_ptr<Node> >& next )
            {
                GAME tnow = createGame();
                std::list<TYPE> moves = tnow->getMoves ( type );
                for ( auto movement : moves ) {
                    T* future = static_cast<T*> ( tnow->clone() );
                   // GAME future = GAME ( alce );
                    future->move ( movement );

                    int fit = future->getFitness();

                    std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type , fit, movement ) );

                    sons[movement] = novoson;
                    next.push_back ( novoson );
                    delete(future);
                }
                if ( sons.size() > 0 ) {
                    updateFitness();
                }

            }
            inline void firstUpdate ( std::unordered_map<TYPE, std::list<std::shared_ptr<Node>>>& next )
            {

                GAME tnow = createGame();
                std::list<TYPE> moves = tnow->getMoves ( type );
                for ( auto movement : moves ) {
                    T* future = static_cast<T*> ( tnow->clone() );

                    future->move ( movement );

                    int fit = future->getFitness();

                    std::shared_ptr<Node> novoson = std::shared_ptr<Node> ( new Node ( this , !type  , fit, movement ) );
                    novoson->root = movement;
                    sons[movement] = novoson;
                    next[movement].push_back ( novoson );
                    delete(future);
                }
                if ( sons.size() > 0 ) {updateFitness();}//TODO :novo update

            }
            inline void updateRoot ( TYPE novoRoot, bool first = 0 )
            {
                root = novoRoot;
                if ( !first ) {
                    for ( auto& it : sons ) {
                        it.second->updateRoot ( novoRoot );
                    }
                } else { ///first
                    for ( auto& it : sons ) {
                        it.second->updateRoot ( it.first );
                    }
                }
            }

        };
        using UpdateMap = std::unordered_map< TYPE , std::list<std::shared_ptr<Node>> >;

        std::shared_ptr<Node> start;
        //std::list<std::shared_ptr<Node> > nextUpdate;
        UpdateMap nextUpdate;

    public:

        MinMaxTree()
        {
            config.limit=-1;
            config.threads = 1;
        }

        MinMaxTree ( TreeConfig config ) :
            config ( config )
        {
        }
        inline void setGame ( T* g, bool xo=0 )
        {
            nextUpdate.clear();
            start =  std::shared_ptr<Node> ( new Node() );
            T* alce = static_cast<T*> ( g->clone() );
            game =  GAME ( alce );
            start->type = !g->getPlayer() ^xo;
            start->firstUpdate ( nextUpdate );
            // start->updateRoot(0,true );
        }
        inline int update ()
        {
            if(cont > config.limit)return 1;
            for ( auto& it : nextUpdate ) {
                std::list<std::shared_ptr<Node> >& next = it.second;
                std::list<std::shared_ptr<Node> > novo;
                for ( auto& node : next ) {
                    node->createSons ( novo );
                }
                next.swap ( novo );
                if( cont > config.limit)return 1;
            }
            return 0;
        }
        inline int update (unsigned long int time)
        {
            if(cont > config.limit)return 1;
            auto tnow = clock();

            while(true){
                if(nextUpdate.size()==0){
                   return 1;
                }
                for ( auto& it : nextUpdate ) {
                    std::list<std::shared_ptr<Node> >& next = it.second; //proximo nodos a atualizar
                    std::list<std::shared_ptr<Node> > novo;
                    auto beg = next.begin();
                    while(beg != next.end() ){//iterating over updates
                        auto n = *beg;
                        n->createSons(novo);
                        auto now = beg;
                        beg++;
                        next.erase(now);
                        if( clock() - tnow >= time || cont > config.limit ){
                            next.splice(next.end(), novo);
                            return 1;
                        }
                    }
                     next.swap ( novo );
                }
                 if( clock() - tnow >= time ){
                     return 1;
                 }

            }
            return 0;

        }

        inline TYPE getMovement()
        {
            std::cerr<< "fitness:" << start->fitness<<"cont:"<<cont  <<std::endl;
            return start->mov;
        }
        inline void sendCommand ( TYPE m )
        {

            std::shared_ptr<Node> next;

            if ( start->sons.count ( m ) ) {
                next = start->sons[m];
            } else {
                throw "FUUU";
            }

            auto& lista = nextUpdate[m];

            UpdateMap novoUpdate;
            if ( next->sons.size() ) { //has sons
                next->updateRoot ( 0, true );
                for ( auto& node : lista ) {
                    novoUpdate[ node->root ].push_back ( node );
                }

            } else {
                next->firstUpdate ( novoUpdate );
            }
            game->move( m );


            nextUpdate = novoUpdate;
            start = next;
        }
};
template<class T,class U>
unsigned long long MinMaxTree<T,U>::cont = 0;

template<class T,class U>
std::shared_ptr<T> MinMaxTree<T,U>::game=std::shared_ptr<T>();


} /* namespace wag */

#endif /* MINMAXTREE_H_ */

#include <list>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
using uchar = unsigned char;
using namespace std;

struct Uttt: public wag::Game {
    static const int POINT= 1;
    static const int BIG_POINT=5;
    static const int SMALL_VICTORY = 50;
    static const int HUGE_POINT=200;
    
    static  const int BIG_VICTORY = 900;
    struct Move {
        int x, y;
        inline bool operator== ( const Move& m ) const
        {
            return x == m.x && y == m.y;
        }
        friend ostream& operator<< ( ostream& os,const Move& g )
        {
            os << g.x << " " << g.y ;
            return os;

        }
    };
    uchar board[9][9];
    uchar main[3][3];
    list<Move*> moves[3][3];
    static unordered_map<int, unordered_map<int, Move*>> everyMove;
    int boardFitness[3][3];

    bool done = 0;
    bool player = 0;
    bool calledFit = 0;
    Move lastMove = { -1, -1};
    Uttt (  )=default;
    Uttt ( int )
    {
        for ( int i = 0; i < 9; i++ ) {
            for ( int j = 0; j < 9; j++ ) {
                board[i][j] = 0;
            }
        }
        for ( int i = 0; i < 3; i++ ) {
            for ( int j = 0; j < 3; j++ ) {
                main[i][j] = 0;
                boardFitness[i][j]=0;
            }
        }
        for ( int i = 0; i < 3; i++ ) {
            for ( int j = 0; j < 3; j++ ) {
                for ( int w = 0; w < 9; w++ ) {
                    Move* m = new Move ( { i * 3 + w % 3, j * 3 + w / 3} );
                    moves[i][j].push_back ( m );
                    everyMove[m->x][m->y] = m;
                }
            }
        }

    }
    virtual ~Uttt()
    {
    }

    inline bool checkVictory ( uchar mx, uchar my, uchar c )
    {
        int minI = mx * 3;
        int maxI = mx * 3 + 3;
        int minJ = my * 3;
        int maxJ = my * 3 + 3;
        for ( int i = minI; i < maxI; i++ ) {
            for ( int j = minJ; j < maxJ ; j++ ) {
                if ( i > minI && i < maxI - 1 )
                    if ( board[i][j] == c   &&  board[i - 1][j] == c &&  board[i + 1][j] == c ) { return true; }

                if ( j > minJ && j < maxJ - 1 )
                    if ( board[i][j] == c   &&  board[i][j + 1] == c &&  board[i][j - 1] == c ) { return true; }
                if ( j > minJ && j < maxJ - 1 && i > minI && i < maxI - 1 ) {
                    if ( board[i][j] == c   &&  board[i - 1][j - 1] == c &&  board[i + 1][j + 1] == c ) { return true; }
                    if ( board[i][j] == c   &&  board[i - 1][j + 1] == c &&  board[i + 1][j - 1] == c ) { return true; }
                }
            }
        }
        return false;
    }
    inline bool checkBigVictory(int c){
        int minI = 0;
        int maxI = 3;
        int minJ = 0;
        int maxJ = 3;
        for ( int i = minI; i < maxI; i++ ) {
            for ( int j = minJ; j < maxJ ; j++ ) {
                if ( i > minI && i < maxI - 1 )
                    if ( main[i][j] == c   &&  main[i - 1][j] == c &&  main[i + 1][j] == c ) { return true; }

                if ( j > minJ && j < maxJ - 1 )
                    if ( main[i][j] == c   &&  main[i][j + 1] == c &&  main[i][j - 1] == c ) { return true; }
                if ( j > minJ && j < maxJ - 1 && i > minI && i < maxI - 1 ) {
                    if ( main[i][j] == c   &&  main[i - 1][j - 1] == c &&  main[i + 1][j + 1] == c ) { return true; }
                    if ( main[i][j] == c   &&  main[i - 1][j + 1] == c &&  main[i + 1][j - 1] == c ) { return true; }
                }
            }
        }
        return false;
    }

    inline void updateFitness ( uchar mx , uchar my)
    {
        int& myfit = boardFitness[mx][my];
        int cont =0;
        int ncont =0;
        auto func = [ &cont, &ncont, &myfit, this, mx, my](){
            int retorno=0;
             if(cont == 3 ){
                myfit = SMALL_VICTORY;  
                retorno= 1;          
            }
            if(cont ==2 && ncont==0){
                myfit = BIG_POINT;            
            }
            if(ncont==2 && cont ==0){
                myfit = -BIG_POINT;            
            }
            if(ncont ==3){ 
                myfit = -SMALL_VICTORY;
                retorno= 2;
            }
            if(mx==1 && mx==1)myfit*=1.3;
            cont =0;
            ncont=0;
            
            if( retorno!=0 ){
                main[mx][my]= retorno;
                moves[mx][my].clear();  
                return 1;
            }
            return 0;
            
        };
        int nx = mx*3;
        int ny = my*3;
        
        for(int i=0; i< 3; i++){
            //horizontal
            for(int j = 0; j< 3; j++){
                if( board[nx + i][ny + j] == 1) cont++;
                if( board[nx + i][ny + j] == 2) ncont++;
            }
            if(func()){
                    return;
            }
            //vertical
            for(int j = 0; j< 3; j++){
                if( board[nx + j][ny + i] == 1) cont++;
                if( board[nx + j][ny + i] == 2) ncont++;
            }
            if(func()){
                   
                    return;
            }
            
        }
        for(int i =0; i <3; i++){
            if( board[nx + i][ny + i] == 1) cont++;
            if( board[nx + i][ny + i] == 2) ncont++; 
        }
       
        if(func()){return;}

        for(int i =0; i <3; i++){
            if( board[nx + i][ny + 2-i] == 1) cont++;
            if( board[nx + i][ny + 2-i] == 2) ncont++; 
        }
        if(func()){return;}
    }
    inline virtual bool move ( const  Move* m )
    {
        calledFit = 0;
        auto& lista = moves[ m->x / 3 ][m->y / 3];
        bool found = false;
        for ( auto it = lista.begin(); it != lista.end(); it++ ) {
            Move* i = *it;
            if ( ( *i ) == *m ) {

                lista.erase ( it );
                found = true;
                break;
            }
        }
        if ( !found ) {
            stringstream s;
            s<< "Move not found in list:";
            s<< *m << "\nAvailable";
            for(auto x: lista){
                s<<*m<<" "<<m<<"\n";
            }
            throw runtime_error ( s.str() );
        }
        board[m->x][m->y] = player + 1;

        ///checking smallboard
        lastMove = *m;

        updateFitness ( m->x / 3, m->y / 3 );
        bool vic = checkBigVictory(player+1);

        if( vic){
            done = 1;
        }
        player = !player;
        return 0;
    }

    inline virtual int getFitness()
    {

        if(done){
            if( checkBigVictory( 1 ) )
                return BIG_VICTORY;
            else{
                return -BIG_VICTORY;
            }
        }

        int total=0;
        for (int i=0;i<3;i++){
            for (int j=0;j<3;j++){
                total+= boardFitness[i][j];
            }
        }
        
        int myfit = 0;
        int cont =0;
        int ncont =0;
        auto func = [ &cont, &ncont, &myfit, this](){
            int retorno=0;
             if(cont == 3 ){
                myfit = BIG_VICTORY;  
                retorno= 1;          
            }
            if(cont ==2 && ncont==0){
                myfit = HUGE_POINT;            
            }
            if(ncont==2 && cont ==0){
                myfit = -HUGE_POINT;            
            }
            if(ncont ==3){ 
                myfit = -BIG_VICTORY;
                retorno= 2;
            }
            cont =0;
            ncont=0;
            if( retorno!=0 ){
                return 1;
            }
            return 0;
            
        };
        int nx = 0;
        int ny = 0;
        
        for(int i=0; i< 3; i++){
            //horizontal
            for(int j = 0; j< 3; j++){
                if( main[nx + i][ny + j] == 1) cont++;
                if( main[nx + i][ny + j] == 2) ncont++;
            }
            if(func()){
                    return myfit;
            }
            //vertical
            for(int j = 0; j< 3; j++){
                if( main[nx + j][ny + i] == 1) cont++;
                if( main[nx + j][ny + i] == 2) ncont++;
            }
            if(func()){
                   
                     return myfit;
            }
            
        }
        for(int i =0; i <3; i++){
            if( main[nx + i][ny + i] == 1) cont++;
            if( main[nx + i][ny + i] == 2) ncont++; 
        }
       
        if(func()){
                 return myfit;
        }

        for(int i =0; i <3; i++){
            if( main[nx + i][ny + 2-i] == 1) cont++;
            if( main[nx + i][ny + 2-i] == 2) ncont++; 
        }
        if(func()){
             return myfit;        
        }
        
        

        return total+myfit;
    }

    inline virtual bool getPlayer()
    {
        return player;
    }

    inline virtual std::list<Move*> getMoves ( int player )
    {

        std::list<Move*> allmoves;
        if(done)return allmoves;
        int mx,my;

        if (lastMove.x ==-1 || main[lastMove.x%3][lastMove.y%3] ||
            moves[lastMove.x%3][lastMove.y%3].size()==0   ){
            for(auto& listax: moves){
                for(auto& listas: listax ){

                     allmoves.insert(allmoves.begin(), listas.begin(), listas.end());
                }
            }
        }else{
            return moves[lastMove.x%3][lastMove.y%3];
        }

        return allmoves;
    }

    Game* clone()
    {
        Uttt* g = new Uttt();
        *g = *this;
        return g;
    }
    friend std::ostream& operator<< ( std::ostream& os, Uttt& c )
    {
        for ( int i = 0; i < 21; i++ ) {
            os << "=";
        }
        os << std::endl;
        for ( int j = 0; j < 9; j++ ) {
            for ( int i = 0; i < 9; i++ ) {
                
                if(c.main[i/3][j/3]!=0){
                     if(c.main[i/3][j/3] == 1)os<<"X"<<" ";
                     if(c.main[i/3][j/3] == 2)os<<"O"<<" ";
                }else{
                    
                    if(c.board[i][j] == 1)os<<"X"<<" ";
                    if(c.board[i][j] == 2)os<<"O"<<" ";
                    if(c.board[i][j] == 0)os<<"-"<<" ";
                }

                if( (i+1)%3==0)os<<"|";
            }
            os << std::endl;
            if((j+1)%3==0){
                for ( int i = 0; i < 21; i++ ) {
                    os << "_" ;
                }
                os << std::endl;
            }
        }
        for ( int i = 0; i < 21; i++ ) {
            os << "=" ;
        }
        return os;

    }
};

unordered_map<int, unordered_map<int, Uttt::Move*>> Uttt::everyMove = unordered_map<int, unordered_map<int, Uttt::Move*>>();


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

  tree.update(800000);
 // tree.update();

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
        tree.update(70000);
       // tree.update();

        mov = tree.getMovement();
        game.move(mov);
        tree.sendCommand(mov);
      //   tree.update();
    cerr<<*mov<<endl;
        std::cout<< mov->y <<" "<<mov->x <<std::endl;
        }catch(...){
            cout<<row<<" "<<col<<endl;
        }


    }

}