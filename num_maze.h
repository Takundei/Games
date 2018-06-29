#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>
#include <iomanip>
#define INF 1000
enum Color {White,Gray, Black};
struct Vertex
{
    Color color = White;
    int d = INF;
    int id;
    int pi = -1;
    friend bool operator <(const Vertex& A,const Vertex& V)
    {//used by min_element
        return A.d < V.d;
    }
};

class PuzzleBoard
{
private:
    std::vector<std::vector<int>> Graph;//An adjacency matrix
    int size;//size of Graph
    int boardsize;
    std::pair<int,int> current = std::pair<int,int>(0,0);//stores the idx of the number we are currently at in the game
    int** Board;//the board
     int del = 0;
public:
    PuzzleBoard(int board_size, int **fields = NULL);
    ~PuzzleBoard();
    int makeMove_comp(int direction,int,int,int);
    bool makeMove(int direction);
    bool getResult();
    friend std::ostream& operator<<(std::ostream &os, const PuzzleBoard &m);
    int solve();
};
PuzzleBoard::~PuzzleBoard(){
	    if(del)
    {
        for(int i = 0; i < boardsize; i++)
            delete []Board[i];
        delete []Board;
    }
}
PuzzleBoard::PuzzleBoard(int board_size, int **fields)
{
    srand(time(NULL));
    boardsize = board_size;
    size = boardsize*boardsize;
    Graph.resize(size);//set size for graph
    for(int i = 0; i  < size; i ++)
        Graph[i].resize(size);
    if(!fields)
    {//Create random game
        del = 1;
        fields = new int*[boardsize];
        for(int i = 0 ; i < boardsize; i++)
        {
            fields[i] = new int[boardsize];
            for(int j = 0; j < boardsize; j++)
                fields[i][j] = rand() % (boardsize-1) + 1;
        }
    }
    Board = fields;
    //Create adj_matrix
    for(int i = 0 ; i < boardsize; i++)
    {
        for(int j = 0; j < boardsize; j++)
        {
            for(int dir = 0 ; dir < 4; dir++)
            {
                int k = makeMove_comp(dir,fields[i][j],i,j);
                if(k != -1)
                {
                    if (dir%2 == 0)
                    {
                        Graph[boardsize*(i)+j][boardsize*(k)+j] = 1;
                    }
                    else
                    {
                        Graph[boardsize*(i)+j][boardsize*(i)+k] = 1;
                    }
                }
            }
        }
    }
}
int PuzzleBoard::makeMove_comp(int direction,int distance,int row, int col)
{
    switch (direction)
    {
    case 0://Up
        return std::max(-1,row - distance);
        break;
    case 3://Left
        return std::max(-1,col - distance);
        break;
    case 2://Down
        return (boardsize > row + distance)? row+distance: -1;
        break;
    default://Right
        return (boardsize > col +distance) ? col +distance : -1;
        break;
    }
}
bool PuzzleBoard::makeMove(int direction){
	int i = current.first;
	int j = current.second;
	int distance  =  Board[i][j];
	int move = makeMove_comp(direction,distance,i,j);
	if(move != -1){
		if(direction % 2 == 0){
			current.first = move;
		}else{
			current.second = move;
		}
		return true;//Valid move made
	}
	return false ;//Move was invalid
}
bool PuzzleBoard::getResult()
{
	
    return current == std::pair<int,int>(boardsize-1,boardsize-1);
}
std::ostream& operator<<(std::ostream &os, const PuzzleBoard &m)
{	//Print board showing current possition
	int f = (m.current).first;
	int s = (m.current).second;
	for(int i = 0; i < m.boardsize; i++){
		for(int j = 0; j < m.boardsize; j++){
			if( j == s){
				if(f == i){
					std::cout<<"|";
					std::cout<<m.Board[i][j]<<"|";
				}else{
				std::cout<<" "<<m.Board[i][j]<<" ";
				}
		}else{
			std::cout<<m.Board[i][j]<<" ";
		}
	}
	std::cout<<std::endl;
}
	//os<<(m.Board)[i][j];
	os<<std::endl;
    return os;
}
int PuzzleBoard::solve()
{	//BFS
    std::vector<Vertex> V (size);
    std::vector<Vertex*> Q;
    for(int i = 0 ; i < size; i++)
    {
        V[i].id = i;
    }
    int u;
    int s = current.first * boardsize + current.second;
    V[s].d = 0;
    Q.push_back(&V[s]);
    while(!Q.empty())
    {
        auto dequeue =  std::min_element(Q.begin(),Q.end());
        u = (*dequeue)->id;
        Q.erase(dequeue);
        for(int i  = 0 ; i < size ; i++)
        {
            if(Graph[u][i] == 1 && (V[i].d == INF || V[i].d > (V[u].d+1)))
            {
                V[i].d = V[u].d + 1;
                V[i].pi = u;
                Q.push_back(&V[i]);
            }
        }

    }

    return V[size-1].d != INF ? V[size-1].d : -1;
}
