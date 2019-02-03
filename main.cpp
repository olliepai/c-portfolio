// graphics stuff
#include "SDL2/SDL.h"
#include "OpenGL/gl3.h"

// standard stuff
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// custom stuff
#include "shader_compiler.h"
#include "graphics-math.h"

int main(int argc, char** argv) {
    int WIDTH = 500;
    int HEIGHT = 500;

    SDL_Window* window = SDL_CreateWindow("flyby", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    char* vertexShader = readFileToCharArray("vertex-shader.glsl");
    char* fragShader = readFileToCharArray("fragment-shader.glsl");

    unsigned int shader = compileShaderVF(vertexShader, fragShader);
    glUseProgram(shader);

    int positionId = glGetAttribLocation(shader, "position");

    float verts[] = {
      //top left
      -1, 0, -1,
      0, 0.3, -1,
      0, 0, 1,

      //top right
      1, 0, -1,
      0, 0.3, -1,
      0, 0, 1,

      //bottom left
      -1, 0, -1,
      0, -0.3, -1,
      0, 0, 1,

      //bottom right
      1, 0, -1,
      0, -.3, -1,
      0, 0, 1
    };

    unsigned short indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    // linking verts with vertex shaders
    unsigned int vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(positionId, 3, GL_FLOAT, false, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(positionId);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // setting up the perspectiveMatrix
    int perspectiveMatrixId = glGetUniformLocation(shader, "perspectiveMatrix");

    // setting up the modelMatrix
    int modelMatrixId = glGetUniformLocation(shader, "modelMatrix");
    mat4 modelMatrix;
    modelMatrix.setIdentity();
    glUniformMatrix4fv(modelMatrixId, 1, false, &modelMatrix.m[0][0]);

    // setting up the viewMatrix
    int viewMatrixId = glGetUniformLocation(shader, "viewMatrix");
    mat4 viewMatrix;
    viewMatrix.setIdentity();

    // setting up the camera
    Camera camera;
    camera.forward = vec3(0, 0, 1);
    camera.right = vec3(1, 0, 0);
    camera.up = vec3(1, 0, 0);
    camera.orientation = quat(0, 0, 0, 1);
    camera.setPerspectiveProjection(70.0, (float)WIDTH / (float)HEIGHT, 0.001, 1000);
    glUniformMatrix4fv(perspectiveMatrixId, 1, false, &camera.projection.m[0][0]);

    camera.position = vec3(0, 0, -10);
    viewMatrix.translate(camera.position);
    glUniformMatrix4fv(viewMatrixId, 1, false, viewMatrix.m[0]);

    glClearColor(0, 0.5, 1, 1);

    // setting up the clock
    bool isRunning = true;
    float p = 0;
    long startTime = std::clock();
    long endTime = 0;
    float deltaTime;

    // changes to modelMatrix (updates once at the beginning of the program)
    modelMatrix.translate(vec3(0, 2, 0));

    // camera movement constants
    bool moveForward = false;
    bool moveBack = false;
    bool moveUp = false;
    bool moveDown = false;
    bool moveRight = false;
    bool moveLeft = false;
    bool pitchUp = false;
    bool pitchDown = false;
    bool rollLeft = false;
    bool rollRight = false;
    bool yawLeft = false;
    bool yawRight = false;

    float camMoveSpeed = 0.01;
    float camRotateSpeed = 0.001;

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
          switch(event.type) {
            case SDL_QUIT: {
              isRunning = false;
              break;
            }
            case SDL_KEYDOWN: {
              if(event.key.keysym.sym == SDLK_ESCAPE){
                  isRunning = false;
              }else if(event.key.keysym.sym == SDLK_UP){
                  pitchUp = true;
              }else if(event.key.keysym.sym == SDLK_DOWN){
                  pitchDown = true;
              }else if(event.key.keysym.sym == SDLK_LEFT){
                  yawLeft = true;
              }else if(event.key.keysym.sym == SDLK_RIGHT){
                  yawRight = true;
              }else if(event.key.keysym.sym == SDLK_w){
                  moveForward = true;
              }else if(event.key.keysym.sym == SDLK_s){
                  moveBack = true;
              }else if(event.key.keysym.sym == SDLK_a){
                  moveLeft = true;
              }else if(event.key.keysym.sym == SDLK_d){
                  moveRight = true;
              }else if(event.key.keysym.sym == SDLK_q){
                  rollLeft = true;
              }else if(event.key.keysym.sym == SDLK_e){
                  rollRight = true;
              }else if(event.key.keysym.sym == SDLK_r){
                  moveDown = true;
              }else if(event.key.keysym.sym == SDLK_f){
                  moveUp = true;
              }

              break;
            }
            case SDL_KEYUP :{
              if(event.key.keysym.sym == SDLK_UP){
                  pitchUp = false;
              }else if(event.key.keysym.sym == SDLK_DOWN){
                  pitchDown = false;
              }else if(event.key.keysym.sym == SDLK_LEFT){
                  yawLeft = false;
              }else if(event.key.keysym.sym == SDLK_RIGHT){
                  yawRight = false;
              }else if(event.key.keysym.sym == SDLK_w){
                  moveForward = false;
              }else if(event.key.keysym.sym == SDLK_s){
                  moveBack = false;
              }else if(event.key.keysym.sym == SDLK_a){
                  moveLeft = false;
              }else if(event.key.keysym.sym == SDLK_d){
                  moveRight = false;
              }else if(event.key.keysym.sym == SDLK_q){
                  rollLeft = false;
              }else if(event.key.keysym.sym == SDLK_e){
                  rollRight = false;
              }else if(event.key.keysym.sym == SDLK_r){
                  moveDown = false;
              }else if(event.key.keysym.sym == SDLK_f){
                  moveUp = false;
              }
              break;
            }
            case SDL_MOUSEBUTTONDOWN :{
              int x, y;
              SDL_GetMouseState(&x, &y);
              break;
            }
          }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        camera.position -= vec3(camera.forward.x * camMoveSpeed * moveForward,
                                camera.forward.y * camMoveSpeed * moveForward,
                                camera.forward.z * camMoveSpeed * moveForward);

        camera.position += vec3(camera.forward.x * camMoveSpeed * moveBack,
                                camera.forward.y * camMoveSpeed * moveBack,
                                camera.forward.z * camMoveSpeed * moveBack);

        camera.position -= vec3(camera.right.x * camMoveSpeed * moveRight,
                                camera.right.y * camMoveSpeed * moveRight,
                                camera.right.z * camMoveSpeed * moveRight);

        camera.position += vec3(camera.right.x * camMoveSpeed * moveLeft,
                                camera.right.y * camMoveSpeed * moveLeft,
                                camera.right.z * camMoveSpeed * moveLeft);

        camera.position += vec3(camera.up.x * camMoveSpeed * moveUp,
                                camera.up.y * camMoveSpeed * moveUp,
                                camera.up.z * camMoveSpeed * moveUp);

        camera.position -= vec3(camera.up.x * camMoveSpeed * moveDown,
                                camera.up.y * camMoveSpeed * moveDown,
                                camera.up.z * camMoveSpeed * moveDown);

        camera.orientation.rotate(camera.right, -camRotateSpeed * pitchUp);
        camera.orientation.rotate(camera.right, camRotateSpeed * pitchDown);
        camera.orientation.rotate(camera.up, -camRotateSpeed * yawLeft);
        camera.orientation.rotate(camera.up, camRotateSpeed * yawRight);
        camera.orientation.rotate(camera.forward, camRotateSpeed * rollLeft);
        camera.orientation.rotate(camera.forward, -camRotateSpeed * rollRight);

        camera.forward = getForwardVector(viewMatrix);
        camera.up = getUpVector(viewMatrix);
        camera.right = getRightVector(viewMatrix);

        viewMatrix.setIdentity();
        viewMatrix.translate(camera.position);
        viewMatrix = multiply(quatToMat4(camera.orientation), viewMatrix);
        glUniformMatrix4fv(viewMatrixId, 1, false, viewMatrix.m[0]);

        // changes to modelMatrix go here (updates every frame)
        modelMatrix.translate(vec3(0, 0, deltaTime));
        glUniformMatrix4fv(modelMatrixId, 1, false, &modelMatrix.m[0][0]);

        // rendering goes here
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);

        SDL_GL_SwapWindow(window);

        endTime = std::clock();
        deltaTime = ((float) endTime - (float) startTime) / 1000000.0;
        startTime = endTime;
    }

    printf("Hello\n");

    return 0;
}
