#pragma once
#include <list>
#include "MinMaxTree.h"
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

    static  const int BIG_VICTORY = 9000;
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
            bool check=0;
             if(cont == 3 ){
                myfit = SMALL_VICTORY;
                retorno= 1;
                check=1;
            }
            if(cont ==2 && ncont==0){
                myfit = BIG_POINT;
                check=1;
            }
            if(ncont==2 && cont ==0){
                myfit = -BIG_POINT;
                check=1;
            }
            if(ncont ==3){
                myfit = -SMALL_VICTORY;
                retorno= 2;
                check=1;
            }
            if(check){
                if(mx==1 && mx==1){
                    myfit *= 5;
                }
                else{
                    if( !((mx==0 || mx==2)) && (my ==0 || my==2))myfit*=0.6;
                }
            }

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

        if (lastMove.x ==-1 || main[lastMove.x%3][lastMove.y%3] ||
            moves[lastMove.x%3][lastMove.y%3].size()==0   ){
                if(player==0){
                    total+=125;
                }else{
                    total-=125;
                }
            }else{
                if(lastMove.x%3==1 && lastMove.y%3==1){
                     if(player==0){
                        total+=70;
                    }else{
                        total-=70;
                    }
                }
                if( (lastMove.x%3==0 || lastMove.x%3==2 )  &&
                  (lastMove.y%3==0 || lastMove.y%3==2) ){
                     if(player==0){
                        total+=30;
                    }else{
                        total-=30;
                    }
                }
            }

        return total+myfit;
    }

    inline virtual bool getPlayer()
    {
        return player;
    }

    inline virtual std::vector<Move*> getMoves ( int player )
    {

        std::vector<Move*> allmoves;
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
            allmoves.insert(allmoves.begin(), moves[lastMove.x%3][lastMove.y%3].begin(), moves[lastMove.x%3][lastMove.y%3].end());
          //  return moves[lastMove.x%3][lastMove.y%3];
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
