// C Lib for random number
#include <cstdio>      /* NULL */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

// C++ 
#include <iostream>
using namespace std;

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// freeglut
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>

// GLM lib for matrix calculation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL lib for loading pictures
#include <SOIL/SOIL.h>

// Personal classes
#include <shader.h> // My shader class
#include <camera.h> // My camera class
#include <cube.h>   // My Cube class
#include <tile.h>   // My Tetris Tile class
#include <window.h> // My game Window class
#include <robot.h>  // My Robot Arm class

// ---------- Define global variables ------------
// -----------------------------------------------
// Cannot use initialisers here which require the use of opengl functions.
// Because glewInit() hasn't been called yet.
// -----------------------------------------------

// ---- 3D basics ------

Shader tileShader,gridShader,robotShader,lightShader;
GLuint tileVAO,gridVAO,robotVAO,lightVAO;
GLuint texture[5];

int screenWidth;
int screenHeight;

Camera myCamera(glm::vec3(5.0f,10.0f,0.0f));     // Define camera with position

GLfloat lastX,lastY;
bool firstMouse = true;

bool keys[1024];
GLfloat lastFrame = 0.0f;
bool camLeft,camRight,camUp,camDown;

// ------ Tile --------------

const int CUBENUM = 4;

const int TOTFORMS = 3;
const int transNum[TOTFORMS] = {2,2,4};
glm::ivec3 forms[TOTFORMS][4] = {
    {
        glm::ivec3(-2,0,0),
        glm::ivec3(-1,0,0),
        glm::ivec3(0,0,0),
        glm::ivec3(1,0,0)
    },
    {
        glm::ivec3(-1,-1,0),
        glm::ivec3(0,-1,0),
        glm::ivec3(0,0,0),
        glm::ivec3(1,0,0)
    },
    {
        glm::ivec3(-1,-1,0),
        glm::ivec3(-1,0,0),
        glm::ivec3(0,0,0),
        glm::ivec3(1,0,0)
    }
};

const int TOTCOLORS = 5;
glm::vec3 colors[TOTCOLORS] = {
    glm::vec3(1.0f,0.0f,1.0f),  // Purple, Grape
    glm::vec3(1.0f,0.0f,0.0f),  // Red, Apple
    glm::vec3(1.0f,1.0f,0.0f),  // Yellow, Banana
    glm::vec3(0.0f,1.0f,0.0f),  // Green, Pear
    glm::vec3(1.0f,0.648f,0.0f) // Orange, Orange
};

Tile myTile;
bool hasTile;

// --------Game Window----------
Window myWindow;

bool hasFreeTile;
TileLink* freeTiles;

GLfloat interval = 0.0f;
bool gameSuspend = false;
bool gameEnd = false;
bool gameRestart = false;

// ---------Robot Arm-----------
Robot robot;
bool tileOnTip;

// lighting
glm::vec3 lightColor(1.0f,1.0f,1.0f);
glm::vec3 lightPos;
glm::vec3 lightScale(2.0f,2.0f,2.0f);

//------------------------------------------------------

Tile newTile(int x1,int x2){
    Cube cubes[CUBENUM+1];
    int form = rand() % TOTFORMS;
    int trans = rand() % transNum[form];

    // make a tile of original form
    // --------------------------

    // make a cube array of form "form"
    for(int i=0;i<CUBENUM;i++)
        cubes[i] = Cube(forms[form][i],rand() % TOTCOLORS);
    // get random position in [x1,x2)
    glm::ivec3 pos(rand() % (x2-x1) + x1, 20,0);
    // make a tile
    Tile tile(CUBENUM,cubes,form,pos);

    // rotate the tile according to trans
    // ---------------------------
    for(int i=0;i<trans;i++)
        tile.RotateRight(transNum[form]);

    return tile;
}

void SetUpTexture(GLuint &texture, const char path[]){
    // Load and create a texture
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image(path,&width,&height,0,SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D,0);
}

void init(){
    // Create shader program
    // ---------------------------------
    tileShader = Shader( "shader/vshader.glsl", "shader/fshader.glsl" );
    gridShader = Shader("shader/vshaderGrid.glsl","shader/fshaderGrid.glsl");
    robotShader = Shader("shader/robotVshader.glsl","shader/robotFshader.glsl");
    lightShader = Shader("shader/lightVshader.glsl","shader/lightFshader.glsl");
    
    //tileShader.Use();

    // Set up our vertex data
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
 
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
 
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
  
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };

    // Set up the grid data for the game window
    GLfloat gridVertices[] = {
        -0.5f,0.5f,0.5f,
        -0.5f,-0.5f,0.5f,
        0.5f,-0.5f,0.5f,
        0.5f,0.5f,0.5f,

        -0.5f,0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,0.5f,-0.5f        
    };

    // Grid indices for element array of gridVertives
    GLuint gridIndices[] = {
        0,1,
        1,2,
        2,3,
        3,0,

        4,5,
        5,6,
        6,7,
        7,4,

        0,4,
        1,5,
        2,6,
        3,7
    };


    // Set up vertex arrays and buffers for the tile.
    // ---------------------------------
    GLuint VBO;
    glGenVertexArrays(1, &tileVAO);
    glGenBuffers(1, &VBO);
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray(tileVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint position = glGetAttribLocation(tileShader.Program, "position" ); 
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(position);

    GLuint texCoord = glGetAttribLocation(tileShader.Program, "texCoord" ); 
    glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(texCoord); 

    // Unbind tileVAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    glBindVertexArray(0); 


    // Set up robot VAO
    glGenVertexArrays(1,&robotVAO);
    glBindVertexArray(robotVAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    position = glGetAttribLocation(robotShader.Program,"position");
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(position);

    GLuint normal = glGetAttribLocation(robotShader.Program,"normal");
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*) (5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normal);

    glBindVertexArray(0);

    // Set up light VAO
    glGenVertexArrays(1,&lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    position = glGetAttribLocation(robotShader.Program,"position");
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(position);

    glBindVertexArray(0);

    // Set up grid VAO
    // ---------------------------------
    GLuint EBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gridIndices), gridIndices, GL_STATIC_DRAW);

    position = glGetAttribLocation(gridShader.Program, "position" ); 
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(position);

    // Unbind tileVAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    glBindVertexArray(0); 

    // Set up textures
    // ---------------------------------
    SetUpTexture(texture[0], "./texture/grape.png");
    SetUpTexture(texture[1], "./texture/apple.png");
    SetUpTexture(texture[2], "./texture/banana.png");
    SetUpTexture(texture[3], "./texture/pear.png");
    SetUpTexture(texture[4], "./texture/orange.png");
    //SetUpTexture(texture[2], "./texture/grape.png");
    //SetUpTexture(texture[3], "./texture/apple.png");
    //SetUpTexture(texture[4], "./texture/grape.png");
}

void gameInit(){
    myWindow = Window();
    freeTiles = NULL;
    hasFreeTile = false;

    robot = Robot(-5,12,12);
    myTile = newTile(2,myWindow.width - 1 - 2);
    myTile.SetPos(robot.tipPosDiscrete);
    tileOnTip = true;
    hasTile = false;
}

void restartGame(){
    myWindow = Window();
    for(TileLink* l=freeTiles;l!=NULL;){
        TileLink* tmp = l->next;
        delete l;
        l = tmp;
    }
    freeTiles = NULL;
    hasFreeTile = false;

    myTile = newTile(2,myWindow.width - 1 - 2);
    hasTile = true;

    interval = 0.0f;
    gameSuspend = gameEnd = gameRestart = false;
}

void keyboard( unsigned char key, int x, int y ){
    switch( key ) {
        case 033: // Escape Key
        case 'q': case 'Q':
            exit( EXIT_SUCCESS );
            break;
        case 'z': case 'Z':
            gameSuspend = !gameSuspend;
            break;
        case 'r': case 'R':
            gameRestart = true;
        case ' ':
            //if(hasTile) myTile.Shuffle();
        	if(tileOnTip && myWindow.CheckTile(myTile)){
        		tileOnTip = false;
        		hasTile = true;
        	}
        	else if(hasTile){
        		//printf("Space detected\n");
        		myTile.Shuffle();
        		/*
        		int controlKey = glutGetModifiers();
        		if( controlKey & GLUT_ACTIVE_CTRL){
        			myTile.Shuffle();
        			printf("CTRL detected\n");
        		}
        		*/
        	}
        	break;
        default: keys[key] = true;
    }
}

void keyboardUp ( unsigned char key, int x, int y ){

    keys[key] = false;
}

void specialKey ( int key, int x, int y){
    //cout << "specialKey" << endl;

	int controlKey = glutGetModifiers();

	if( controlKey & GLUT_ACTIVE_CTRL){
		if(key == GLUT_KEY_UP) camUp = true;
		if(key == GLUT_KEY_DOWN) camDown = true;
		if(key == GLUT_KEY_LEFT) camLeft = true;
		if(key == GLUT_KEY_RIGHT) camRight = true;
	}
	else{
		camUp = camDown = camLeft = camRight = false;

	    if(! hasTile) return;

	    /*
	    if(key == GLUT_KEY_LEFT){
	        myTile.Left();
	        if(! myWindow.CheckTile(myTile)) myTile.Right();
	    }
	    */
	    /*
	    if(key == GLUT_KEY_RIGHT){
	        myTile.Right();
	        if(! myWindow.CheckTile(myTile)) myTile.Left();
	    }
	    */
	    if(key == GLUT_KEY_DOWN){
	        myTile.Down();
	        if(! myWindow.CheckTile(myTile)) myTile.Up();
	    }
	    if(key == GLUT_KEY_UP){
	        int totTrans = transNum[myTile.form];
	        if(totTrans == 4){ 
	            myTile.RotateRight(totTrans);
	            if(! myWindow.CheckTile(myTile)) myTile.RotateLeft(totTrans);
	        }
	        else{
	            if(myTile.trans == totTrans-1){
	                myTile.RotateLeft(totTrans);
	                if(! myWindow.CheckTile(myTile)) myTile.RotateRight(totTrans);
	            }
	            else{
	                myTile.RotateRight(totTrans);
	                if(! myWindow.CheckTile(myTile)) myTile.RotateLeft(totTrans);
	            }
	        }
	    }
	}
}

void specialKeyUp (int key, int x,int y){
	if(key == GLUT_KEY_UP) camUp = false;
	if(key == GLUT_KEY_DOWN) camDown = false;
	if(key == GLUT_KEY_LEFT) camLeft = false;
	if(key == GLUT_KEY_RIGHT) camRight = false;
}

void Do_Movement(GLfloat deltaTime){ // Update camera position
    //if(keys['w']) myCamera.Move(FORWARD,deltaTime,1); // Move on the Ground
    if(camUp) myCamera.Move(UP,deltaTime);
    //if(keys['s']) myCamera.Move(BACKWARD,deltaTime,1);    // Move on the Ground
    if(camDown) myCamera.Move(DOWN,deltaTime);
    if(camLeft) myCamera.Move(LEFT,deltaTime);
    if(camRight) myCamera.Move(RIGHT,deltaTime);

    // Move Robot Arm
    bool changePos=false;
    if(keys['a']) changePos = robot.Left(deltaTime);
    if(keys['d']) changePos = robot.Right(deltaTime);
    if(keys['w']) changePos = robot.Up(deltaTime);
    if(keys['s']) changePos = robot.Down(deltaTime);

    if(tileOnTip && changePos) myTile.SetPos(robot.tipPosDiscrete);
}

void mouse(int button,int state,int x,int y){   // Update camera aspect
    GLfloat sensitivity = 5.0;

    if( button == 3 || button == 4){
        if( state == GLUT_DOWN){
            if(button == 3) myCamera.Zoom(-sensitivity);
            if(button == 4) myCamera.Zoom(sensitivity);
        }
    }
}

void reshape( int width, int height ){ // Update viewport
    screenWidth = width;
    screenHeight = height;

    glViewport( 0, 0, width, height );
}

void display(void){ // Render

    // Clear the color buffer and depth buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update camera position
    GLfloat currentFrame = (float) glutGet(GLUT_ELAPSED_TIME) /1000;
    GLfloat deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    Do_Movement(deltaTime);

    if(gameRestart) restartGame();

    // Update tile position
    // --------------------

    if(! gameSuspend && ! gameEnd) interval += deltaTime;
    // every interval update tile state
    if(interval >= 0.6 && ! tileOnTip) {
        if(hasTile){ // tile exists, so try to drop down 1 slot, and detect if collides
            myTile.Down();
            if(! myWindow.CheckTile(myTile)){
                myTile.Up();
                hasTile = false;

                myWindow.AddTile(myTile);
                myWindow.Detect();
                myWindow.Eliminate();
                freeTiles = myWindow.CheckFreeTile(freeTiles);
                if(freeTiles != NULL) hasFreeTile = true;
                else hasFreeTile = false;

                if(! myWindow.CheckEnd()){
                    gameEnd = true;
                    printf("Game Ends: Over Stack!\n");
                }

                #ifdef DEBUG
                if(freeTiles != NULL){
                    printf("---FreeTiles---\n");
                    for(TileLink* l=freeTiles; l!=NULL; l=l->next){
                        for(int i=0;i<l->tile.num;i++){
                            glm::ivec3 pos = l->tile.GetPos(i);
                            printf("  (%d,%d)",pos.x,pos.y);
                        }
                        printf("\n");
                    }
                    printf("---------------\n");
                }
                else printf(":: No free tiles\n");
                #endif
            }
        }
        else if(! hasFreeTile){	// no tile, and no free tile, then bear a new player tile
            myTile = newTile(2,myWindow.width -1 - 2);
            myTile.SetPos(robot.tipPosDiscrete);
            tileOnTip = true;

            #ifdef DEBUG
            printf("New Tile : ");
            for(int i=0;i<myTile.num;i++){
                glm::ivec3 pos = myTile.GetPos(i);
                printf("(%d,%d) ",pos.x,pos.y);
            }
            printf("\n");   
            #endif

            /*
            hasTile = true;
            if(! myWindow.CheckTile(myTile)){
                myTile.Up();
                gameEnd = true;
                printf("Game Ends: No place to generate new tile!\n");
            }
            */
        }
        else{	// has free tiles, then drop free tiles and detect collides
            // Try to move all free tiles down 1 position
            // and remove tile no longer free
            TileLink* pre = NULL;
            for(TileLink* l=freeTiles; l!=NULL;){
                l->tile.Down();
                if(! myWindow.CheckTile(l->tile)){
                    l->tile.Up();
                    myWindow.AddTile(l->tile);
                    if(pre != NULL){
                        pre->next = l->next;
                        delete l;
                        l = pre->next;
                    }
                    else{
                        l = l->next;

                        TileLink* tmp = freeTiles;
                        freeTiles = freeTiles->next;
                        delete tmp;
                    }
                }
                else{
                    pre = l;
                    l = l->next;
                }
            }

            // Add new free tiles
            myWindow.Detect();
            myWindow.Eliminate();
            freeTiles = myWindow.CheckFreeTile(freeTiles);
            if(freeTiles != NULL) hasFreeTile = true;
            else hasFreeTile = false;

            #ifdef DEBUG
            if(freeTiles != NULL){
                printf("---FreeTiles---\n");
                for(TileLink* l=freeTiles; l!=NULL; l=l->next){
                    for(int i=0;i<l->tile.num;i++){
                        glm::ivec3 pos = l->tile.GetPos(i);
                        printf("  (%d,%d)",pos.x,pos.y);
                    }
                    printf("\n");
                }
                printf("---------------\n");
            }
            else printf(":: No free tiles\n");
            #endif
        }

        // renew interval
        interval = 0.0f;
    }

    // Set the transform matrices
    glm::mat4 model;

    glm::mat4 view;
    view = myCamera.GetViewMatrix();

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(myCamera.Aspect), (float)screenWidth / screenHeight,0.1f,1000.0f);

    glm::vec3 color;
    int type;

    // Draw the tile 
    // -----------------------------------

    // Use shader program
    tileShader.Use();

    // Active and bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    glUniform1i(glGetUniformLocation(tileShader.Program,"ourTexture0"),0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glUniform1i(glGetUniformLocation(tileShader.Program,"ourTexture1"),1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glUniform1i(glGetUniformLocation(tileShader.Program,"ourTexture2"),2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    glUniform1i(glGetUniformLocation(tileShader.Program,"ourTexture3"),3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,texture[4]);
    glUniform1i(glGetUniformLocation(tileShader.Program,"ourTexture4"),4);

    GLuint modelLoc = glGetUniformLocation(tileShader.Program,"model");
    // model matrix will be calculated later

    GLuint viewLoc = glGetUniformLocation(tileShader.Program,"view");
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));

    GLuint projectionLoc = glGetUniformLocation(tileShader.Program,"projection");
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));

    GLuint colorLoc = glGetUniformLocation(tileShader.Program,"myColor");
    GLuint typeLoc = glGetUniformLocation(tileShader.Program,"type");

    // Draw the tile
    glBindVertexArray(tileVAO);

    //bool inWindow = 0;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw the game window tiles with the same shader program and VAO
    for(int i=0;i<myWindow.width;i++)
        for(int j=0;j<myWindow.height;j++) if(myWindow.bitmap[i][j]){
            
            type = myWindow.type[i][j];
            glUniform1i(typeLoc,type);

            color = colors[myWindow.type[i][j]];
            glUniform3fv(colorLoc,1,glm::value_ptr(color));

            model = glm::mat4();
            model = glm::translate(model,glm::vec3(i,j,0));
            model = glm::scale(model,glm::vec3(0.95f,0.95f,0.95f));

            glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES,0,36);
            //glDrawElements(GL_LINES,24, GL_UNSIGNED_INT, 0);
        }

    // Draw the tile
    if(hasTile || tileOnTip){
        for(int i=0;i<myTile.num; i++){
            glm::vec3 pos = myTile.GetPos(i);
            if(pos.x < 0 || pos.x >= myWindow.width || pos.y < 0 || pos.y >= myWindow.height)
                continue;

            type = myTile[i].type;
            glUniform1i(typeLoc,type);

            color = colors[myTile[i].type];
            if(myWindow.bitmap[int(pos.x)][int(pos.y)]) {
            	color = glm::vec3(0.75f,0.75f,0.75f);
            	//printf("collision detected\n");
            }
            glUniform3fv(colorLoc,1,glm::value_ptr(color));

            model = glm::mat4();
            model = glm::translate(model,pos);
            model = glm::scale(model,glm::vec3(0.96f,0.96f,0.96f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);   
        }
    }
    
    // Draw the free tiles
    for(TileLink* l=freeTiles; l!=NULL; l=l->next ){
        for(int i=0;i<l->tile.num;i++){
            glm::vec3 pos = l->tile.GetPos(i);
            if(pos.x < 0 || pos.x >= myWindow.width || pos.y < 0 || pos.y >= myWindow.height)
                continue;

            type = l->tile[i].type;
            glUniform1i(typeLoc,type);

            color = colors[l->tile[i].type];
            glUniform3fv(colorLoc,1,glm::value_ptr(color));

            model = glm::mat4();
            model = glm::translate(model,pos);
            model = glm::scale(model,glm::vec3(0.95f,0.95f,0.95f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);  
        }
    }

    glBindVertexArray(0);

    // ---------------------------------------------------------------


    // Draw the grid
    // --------------------------------------------------------------
    gridShader.Use();
    // Get location
    modelLoc = glGetUniformLocation(gridShader.Program,"model");
    viewLoc = glGetUniformLocation(gridShader.Program,"view");
    projectionLoc = glGetUniformLocation(gridShader.Program,"projection");
    // Set matrices
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));


    glBindVertexArray(gridVAO);

    for(int i=0;i<myWindow.width;i++)
        for(int j=0;j<myWindow.height;j++){
            model = glm::mat4();
            model = glm::translate(model,glm::vec3(i,j,0));
            glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //glDrawArrays(GL_TRIANGLES,0,36);
            glDrawElements(GL_LINES,24, GL_UNSIGNED_INT, 0);
        }

    glBindVertexArray(0);


    // Draw the robot arm
    // -----------------------------------------------------

    robotShader.Use();

    // vertex shader matrices
    modelLoc = glGetUniformLocation(robotShader.Program,"model");

    viewLoc = glGetUniformLocation(robotShader.Program,"view");
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
    
    projectionLoc = glGetUniformLocation(robotShader.Program,"projection");
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));
    
    // fragment shader matrices
    GLint objectColorLoc = glGetUniformLocation(robotShader.Program, "objectColor");

    GLint lightColorLoc = glGetUniformLocation(robotShader.Program, "lightColor");
    glUniform3fv(lightColorLoc,1, glm::value_ptr(lightColor));

    GLint lightPosLoc = glGetUniformLocation(robotShader.Program,"lightPos");  
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(5.0f,10.0f,5.0f));
    model = glm::rotate(model, glm::radians((float) glutGet(GLUT_ELAPSED_TIME) /1000 * 25.0f), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::translate(model, glm::vec3(14.0f,0.0f,0.0f));
    lightPos = glm::vec3(model * glm::vec4(0.0f,0.0f,0.0f,1.0f));
    glUniform3fv(lightPosLoc,1, glm::value_ptr(lightPos));

    GLuint viewPosLoc = glGetUniformLocation(robotShader.Program,"viewPos");
    glm::vec3 viewPos = myCamera.Position;
    glUniform3fv(viewPosLoc,1,glm::value_ptr(viewPos));

    // bind VAO
    glBindVertexArray(robotVAO);

    // Draw the base
    glm::vec3 objectColor = glm::vec3(0.0f,1.0f,1.0f);
    glUniform3fv(objectColorLoc,1, glm::value_ptr(objectColor));

    model = glm::mat4();
    model = glm::translate(model,robot.basePos);
    model = glm::scale(model,robot.baseScale);
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES,0,36);

    // Draw the low arm
    objectColor = glm::vec3(1.0f,0.0f,1.0f);
    glUniform3fv(objectColorLoc,1,glm::value_ptr(objectColor));

    model = glm::mat4();
    model = glm::translate(model,robot.lowArmMid);
    model = glm::rotate(model,glm::radians(-robot.lowArmAngle),glm::vec3(0,0,1));
    model = glm::scale(model,robot.lowArmScale);
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES,0,36);

    // Draw the high arm
    objectColor = glm::vec3(1.0f,1.0f,0.0f);
    glUniform3fv(objectColorLoc,1,glm::value_ptr(objectColor));

    model = glm::mat4();
    model = glm::translate(model,robot.highArmMid);
    model = glm::rotate(model,glm::radians(-robot.highArmAngle),glm::vec3(0,0,1));
    model = glm::scale(model,robot.highArmScale);
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES,0,36);

    glBindVertexArray(0);

    // ----------------------------------------------

    // draw light
    // -----------------------------------

    lightShader.Use();

    modelLoc = glGetUniformLocation(lightShader.Program,"model");

    viewLoc = glGetUniformLocation(lightShader.Program,"view");
    glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));

    projectionLoc = glGetUniformLocation(lightShader.Program,"projection");
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE,glm::value_ptr(projection));

    glBindVertexArray(lightVAO);
 
    model = glm::mat4();
    model = glm::translate(model,lightPos);
    model = glm::scale(model,lightScale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);           

    glBindVertexArray(0);

    // ------------------------------------------

    // Update screen
    glutPostRedisplay();
    glutSwapBuffers();
}


// ---------------------------------------------------------

// The MAIN function, from here we start our application and run our Program/Game loop
int main(int argc, char **argv ){
    // Create window
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( screenWidth = 1300, screenHeight = 1000 );
    glutCreateWindow( "FruitTetris" );

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit(); 

    // Set OpenGL environment
    glEnable(GL_DEPTH_TEST);
    glutSetCursor(GLUT_CURSOR_NONE);

    // Initialize and calculate graphics data
    init();
    gameInit();

    // Bind GLUT callback functions
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutKeyboardUpFunc( keyboardUp);
    glutSpecialFunc( specialKey );
    glutSpecialUpFunc( specialKeyUp);
    glutMouseFunc( mouse );
    glutReshapeFunc( reshape );

    // Start main loop
    glutMainLoop();

    return 0;
}
