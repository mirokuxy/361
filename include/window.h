#pragma once

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// My own class headers
#include <cube.h>
#include <tile.h>

// ---- Game Window class-------

const int MAXWINDOWWIDTH = 15;
const int MAXWINDOWHEIGHT = 25;

const int DETECTNUM = 6;
const int DETECTLENGTH = 3;
const int DETECPATTERN[DETECTNUM][DETECTLENGTH][2] = {
	{ {-2,0},{-1,0},{0,0} },
	{ {-1,0},{0,0},{1,0} },
	{ {0,0},{1,0},{2,0} },

	{ {0,-2},{0,-1},{0,0} },
	{ {0,-1},{0,0},{0,1} },
	{ {0,0},{0,1},{0,2} }
};

const int pathNum = 4;

const int path[4][2] = {
	{-1,0},{1,0},{0,-1},{0,1}
};

class Window{
public:
	bool bitmap[MAXWINDOWWIDTH][MAXWINDOWHEIGHT];
	int type[MAXWINDOWWIDTH][MAXWINDOWHEIGHT];

	int width;
	int height;

	bool eliminate[MAXWINDOWWIDTH][MAXWINDOWHEIGHT];

	Window(int width = 10, int height = 20){
		this->width = width;
		this->height = height;

		for(int i=0;i<MAXWINDOWWIDTH;i++)
			for(int j=0;j<MAXWINDOWHEIGHT;j++){
				this->bitmap[i][j] = false;
				this->type[i][j] = 0;
				this->eliminate[i][j] = false;
			}
	}

	Window operator = (Window w){
		this->width = w.width;
		this->height = w.height;

		for(int i=0;i<w.width;i++)
			for(int j=0;j<MAXWINDOWHEIGHT;j++){
				this->bitmap[i][j] = w.bitmap[i][j];
				this->type[i][j] = w.type[i][j];
				this->eliminate[i][j] = w.eliminate[i][j];
			}
		return *this;
	}

	bool CheckTile(Tile t){
		for(int i=0;i<t.num;i++){
			glm::ivec3 pos = t.GetPos(i);
			if(pos.x < 0 || pos.x >= width || pos.y < 0) return false;
			if(bitmap[pos.x][pos.y]) return false;
		}
		return true;
	}

	bool CheckEnd(){
		for(int i=0;i<width;i++)
			if(bitmap[i][height]) return false;
		return true;
	}

	bool AddTile(Tile t){
		for(int i=0;i<t.num;i++){
			glm::ivec3 pos = t.GetPos(i);
			//if(pos.x >= 0 && pos.x <width && pos.y >= 0 && pos.y < height){
			if(pos.x >= 0 && pos.x <width && pos.y >= 0){
				bitmap[pos.x][pos.y] = true;
				type[pos.x][pos.y] = t[i].type;
			}
		}
	}

	void Detect(){
		// detect for row and column patterns
		for(int i=0;i<width;i++)
			//for(int j=0;j<height;j++) if(bitmap[i][j]){
			for(int j=0;j<MAXWINDOWHEIGHT;j++) if(bitmap[i][j]){

				for(int l=0;l<DETECTNUM;l++){  // for every pattern
					bool detected = true;
					int detectedType = -1;

					for(int k=0;k<DETECTLENGTH;k++){ // for every position in that pattern
						int x = i + DETECPATTERN[l][k][0];
						int y = j + DETECPATTERN[l][k][1];
						if(x < 0 || x >= width || y < 0) { detected = false; break; }
						if( bitmap[x][y] == false ) { detected = false; break; }
						if( detectedType == -1) detectedType = type[x][y];
						else if (detectedType != type[x][y]) {detected = false; break; }
					}

					if(! detected) continue;

					// set elimination flag
					for(int k=0;k<DETECTLENGTH;k++){
						int x = i + DETECPATTERN[l][k][0];
						int y = j + DETECPATTERN[l][k][1];
						eliminate[x][y] = true;
					}
				}
			}

		// detect for full rows
		for(int j=0;j<MAXWINDOWHEIGHT;j++){
			bool detected = true;
			for(int i=0;i<width;i++)
				if( bitmap[i][j] == false ) { detected = false; break; }	
			
			if(! detected) continue;

			// set elimination flag
			for(int i=0;i<width;i++)
				eliminate[i][j] = true;
		}
	}

	void Eliminate(){
		for(int i=0;i<width;i++)
			//for(int j=0;j<height;j++) if(eliminate[i][j]){
			for(int j=0;j<MAXWINDOWHEIGHT;j++) if(eliminate[i][j]){
				
				#ifdef DEBUG
				printf("Eliminate (%d,%d)\n",i,j);
				#endif

				eliminate[i][j] = false;
				bitmap[i][j] = false;
				type[i][j] = 0;
			}
	}

	// for CheckFreeTile function usage
	// -------------------------------------------------------
	bool visited[MAXWINDOWWIDTH][MAXWINDOWHEIGHT];
	bool onGround[MAXWINDOWWIDTH][MAXWINDOWHEIGHT];

	void CheckOnGround(int x,int y){
		visited[x][y] = true;
		onGround[x][y] = true;

		for(int i=0;i<pathNum;i++){
			int newx = x + path[i][0];
			int newy = y + path[i][1];

			if(newx < 0 || newx >= width || newy < 0 || newy >= MAXWINDOWHEIGHT) continue;
			if(bitmap[newx][newy] == false) continue;
			if(visited[newx][newy]) continue;

			CheckOnGround(newx,newy);
		}
	}

	void ConstructCube(Tile& tile, int x,int y){
		visited[x][y] = true;
		tile.cubes[tile.num++] = Cube(glm::ivec3(x,y,0), type[x][y]);
		//printf("FreeCube (%d,%d)\n",x,y);

		for(int i=0;i<pathNum;i++){
			int newx = x + path[i][0];
			int newy = y + path[i][1];

			if(newx < 0 || newx >= width || newy < 0 || newy >= MAXWINDOWHEIGHT) continue;
			if(bitmap[newx][newy] == false) continue;
			if(visited[newx][newy]) continue;
			
			ConstructCube(tile, newx,newy);
		}
	}

	TileLink* CheckFreeTile(TileLink* link){
		// initialize
		for(int i=0;i<width;i++)
			//for(int j=0;j<height;j++)
			for(int j=0;j<MAXWINDOWHEIGHT;j++)
				visited[i][j] = onGround[i][j] = false;

		// check fruits which have support
		for(int i=0;i<width;i++) if(bitmap[i][0] && ! visited[i][0])
			CheckOnGround(i,0);

		// find free tiles
		for(int i=0;i<width;i++)
			//for(int j=1;j<height;j++) if(bitmap[i][j] && ! visited[i][j]){
			for(int j=1;j<MAXWINDOWHEIGHT;j++) if(bitmap[i][j] && ! visited[i][j]){
				Tile tile;

				ConstructCube(tile,i,j);

				TileLink *tmp = new TileLink;
				tmp->next = link;
				tmp->tile = tile;

				link = tmp;
			}

		// eliminate free tiles
		for(int i=0;i<width;i++)
			//for(int j=0;j<height;j++) if(bitmap[i][j] && !onGround[i][j])
			for(int j=0;j<MAXWINDOWHEIGHT;j++) if(bitmap[i][j] && !onGround[i][j])
				bitmap[i][j] = false;

		return link;
	}

};