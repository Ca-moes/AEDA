#include "maze.h"
#include <iostream>
#include <iomanip>
#include <utility>
#include <algorithm>

using namespace std;

DisjointSets Maze::getSets() const {
    return maze;
}

void Maze::printMaze() const
{
    cout << setw(3) << " ";
    for (int j=0; j<ncols; j++) {
        cout << setw(6) << "______";
    }
    for (int i=0; i<nrows; i++) {
        cout << endl;
        cout << setw(3) << "|";
        for (int j=0; j<ncols-1; j++) {
            int val = i*nrows+j;
            cout << setw(3) << val<<setw(3);
            if (noWalls.empty() || find(noWalls.begin(),noWalls.end(),pair<int,int>(val,val+1)) != noWalls.end())
                cout << "|";
            else
                cout <<" ";
        }
        int val = i*nrows+ncols-1;
        cout << setw(3) << val<<setw(3) << "|";
        cout <<endl;
        cout << setw(3) << " ";
        for (int j=0; j<ncols; j++) {
            int val = i*nrows+j;
            if (i<nrows-1) {
                if ( noWalls.empty() || find(noWalls.begin(), noWalls.end(), pair<int, int>(val, val + ncols)) != noWalls.end())
                    cout << setw(6) << "______";
                else
                    cout << setw(6) << " ";
            } else
                cout << setw(6) << "______";
        }

    }
    cout <<endl << endl;
}



// a implementar

Maze::Maze(int rows, int cols) {
    nrows = rows;
    ncols = cols;
    DisjointSets a(rows*cols);
    maze = a;

    //paredes nas linhas  cols=4 rows=3
    for (int i = 0; i < rows*cols; ++i) {
        pair<int,int> p;
        if (i%cols == cols -1) //se for ultimo valor da linha, não tem parede á frente
            continue;
        p.first = i;
        p.second = i+1;
        noWalls.push_back(p);
    }
    // 0-1 1-2 2-3 3-4
    // 5-6 6-7 7-8 8-9
    // 10-11 ... 13-14
    // 15-16 ... 18-19
    // 20-21 ... 23-24

    //paredes nas colunas
    for (int j = 0; j < rows*cols; ++j) {
        pair<int,int> p;
        if (j/cols == rows-1)  //se for ultimo valor da coluna, não tem parede para baixo
            continue;
        p.first = j;
        p.second = j+cols;
        noWalls.push_back(p);
    }
}

vector<int> Maze::getNeighbours(int x) const {
    // outro metodo : 4 if's, cada um verifica se tem vizinho em cima, baixo, lados

    vector<int> a;
    if (x == 0){  // canto superior esquerdo
        a.push_back(x+1);
        a.push_back(x+ncols);
        a.push_back(x-ncols);
        return a;
    }
    if (x == ncols-1)  // canto superior direito
    {
        a.push_back(x-1);
        a.push_back(x+ncols);
        return a;
    }
    if (x == ncols*nrows-ncols) // canto inferior esquerdo
    {
        a.push_back(x+1);
        a.push_back(x-ncols);
        return a;
    }
    if (x == ncols*nrows-1) // canto inferior direito
    {
        a.push_back(x-1);
        a.push_back(x-ncols);
        return a;
    }
}

void Maze::buildRandomMaze() {
    // ciclo while getNumberofSets != 1
}

