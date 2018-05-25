#include "num_maze.h"
#include <iostream>
using namespace std;
int main()
{
    int size;
    int direction;
    bool move;
    int k;
    int moves = 0;
    cout<<"Enter boardsize : ";
     cin>>size;
     
    PuzzleBoard *Puzzle;
    
    cout<<"Generating a solvable puzzle..."<<endl;
    	do{
    		Puzzle = new PuzzleBoard (size);
    		k = Puzzle->solve();
    		if(k == -1){
    			delete Puzzle;
    		}
    	}while(k == -1);
    	cout<<"# moves "<<k<<endl;	
    	
    cout<<*Puzzle;
    while(!Puzzle->getResult()){
    		cout<<"MOVES:  U: 0 ; L:3 ; D:2; R:1"<<endl;
    		cout<<">>> ";
    		cin>>direction;
    		move = Puzzle->makeMove(direction);
    		if(!move){
    			cout<<"error : invalid move"<<endl;
    		}else{
    			moves++;
    			cout<<"---\n"<<*Puzzle;
    		}
    		k = Puzzle->solve();
    		if(k == -1){
    		cout<<"Puzzle can no longer be solved"<<endl;
    		//return 0;
    		}else{
    		cout<<"Number of moves required to win from current position: "<<k<<endl;
    		}
    }
    cout<<"PUZZLE SOLVED IN "<<moves<<" MOVES!"<<endl;
    return 0;
}
