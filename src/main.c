/*
 * Potatis Clicker: Remake
 *
 * Copyright (C) 2022-2025 Benjamin Helle
 * 
 * This program is free software; you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation; either version 2 of the License, or  
 * (at your option) any later version.  
 * 
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  
 * GNU General Public License for more details.  
 * 
 * You should have received a copy of the GNU General Public License  
 * along with this program; if not, write to the Free Software  
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "../include/potatis_clicker.h"

char VERSION_STR[] = "v1.2.0";
#define VERSION_NUM 210

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

double cursor_x = 0, cursor_y = 0;

long long potatoes = 0; // the amount of potatoes the player has

typedef struct {
  float x, y, width, height;
  GLuint texture;
  bool hovered;
  bool pressed;
} Button;

Button potatis;

// I know that the code looks horrible right now. I'll fix it later.

void handle_input(GLFWwindow *window, float deltaTime) { // process movement and other inputs
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, 1); // close the game
}

bool is_mouse_over(Button* btn, double mx, double my) {
  float halfW = btn->width / 2.0f; // see if the cursor is over the button
  float halfH = btn->height / 2.0f;
  return mx >= (btn->x - halfW) && mx <= (btn->x + halfW) && my >= (btn->y - halfH) && my <= (btn->y + halfH); // check bounds
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  ypos = SCREEN_HEIGHT - ypos;
  potatis.hovered = is_mouse_over(&potatis, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    my = SCREEN_HEIGHT - my;
    // if the cursor is over the button
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      if (action == GLFW_PRESS) {
        if (is_mouse_over(&potatis, mx, my)) potatis.pressed = true;
      } else if (action == GLFW_RELEASE) { // if pressed, increment the count
        if (potatis.pressed && is_mouse_over(&potatis, mx, my)) potatoes++;
        potatis.pressed = false;
      }
    }
}

void render_bitmap_str(const char* str, void* font, bool centered, float x, float y) {
  int width = glutBitmapLength(font, (const unsigned char*)str);

  if (centered) glRasterPos2f(x - width / 2.0f, y); // center horizontally
  else glRasterPos2f(x, y); // left align

  for (const char* c = str; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c); // render the character
  }
}

void render_button(Button *button) {
  glPushMatrix(); // Dunno if it's necessary

  glColor3f(1.0, 1.0, 1.0); // white

  glTranslatef(button->x, button->y, 0);

  glBindTexture(GL_TEXTURE_2D, button->texture); // set the texture

  glEnable(GL_TEXTURE_2D); // enable texturing
  glEnable(GL_BLEND); // enable transparency
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // make the texture not blurry

  float halfW = button->width / 2.0f; // this makes that object is rendered from the center
  float halfH = button->height / 2.0f;

  glBegin(GL_QUADS); // draw the button
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfW, -halfH);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( halfW, -halfH);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( halfW,  halfH);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfW,  halfH);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, 0); // disable stuff
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

GLuint load_texture(const char* filepath) { // copied from Sandboxia2D, edited a bit
  if (!filepath) printf("Invalid filepath: %s\n", filepath);
  int width, height, nrChannels;
  unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

  if (!data) { // if the texture failed to load
    printf("Failed to load texture: %s\n", filepath);
    return 0;
  }

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);
  return textureID;
}

int main(int argc, char **argv) {

  printf("Potatis Clicker: Remake %s\nCopyright (C) 2022-2025 Benjamin Helle\n", VERSION_STR);

  if (!glfwInit()) {
    perror("Failed to initialize GLFW"); // throw an error if GLFW couldn't be initialized
    printf("Error code: %d\n", glfwGetError(NULL));
    return -2;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // opengl 2.1
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // create the window
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Potatis Clicker", NULL, NULL);

  if (!window) {
    perror("Failed to create window"); // throw an error if the window couldn't be created
    printf("Error code: %d\n", glfwGetError(NULL));
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    perror("Failed to initialize GLAD"); // throw an error if GLAD couldn't be initialized
    printf("Error code: %d\n", glfwGetError(NULL));
    glfwTerminate();
    return -1;
  }

  // init glut
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  glClearColor(0.824f, 0.561f, 0.271f, 1.0f); // set the background color, sky like

  printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  // init variables
  float lastFrame, currentFrame, deltaTime = 0.0f;

  glfwSwapInterval(1); // enable vsync

  // assign some callbacks
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);

  // try to load the score
  FILE* game_score = fopen("score.dat", "rb+");
  if (!game_score) { // if the file doesn't exist, create it
    game_score = fopen("score.dat", "wb+");
    fwrite(&potatoes, sizeof(long long), 1, game_score);
    fclose(game_score);
    game_score = fopen("score.dat", "rb+");
  }
  
  // if the file exists, load the score
  if (game_score) fread(&potatoes, sizeof(long long), 1, game_score);

  // load the potato texture
  GLuint potato_texture = load_texture("assets/potatis.png");
  if (!potato_texture) perror("Failed to load the potato texture");

  // init the potatis
  potatis.texture = potato_texture;
  potatis.height = 180;
  potatis.width = 230;
  potatis.hovered = false;

  // create custom icon
  GLFWimage icon[1];
  icon[0].pixels = stbi_load("assets/potatis.png", &icon[0].width, &icon[0].height, 0, 4);
  glfwSetWindowIcon(window, 1, icon);
  stbi_image_free(icon[0].pixels);

  while (!glfwWindowShouldClose(window)) { // main game loop
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT); // clear the screen
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // set the projection for text rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // text color
    glColor3f(0, 0, 0); // black
    // display the count
    render_bitmap_str("Potatoes:", GLUT_BITMAP_TIMES_ROMAN_24, 1, potatis.x, potatis.y+150);
    char potatoes_str[20];
    snprintf(potatoes_str, 20, "%lld", potatoes);
    glColor3f(1.0, 1.0, 1.0); // white
    render_bitmap_str(potatoes_str, GLUT_BITMAP_TIMES_ROMAN_24, 1, potatis.x, potatis.y+100);

    // display the version
    glColor3f(0, 0, 0); // black
    render_bitmap_str(VERSION_STR, GLUT_BITMAP_TIMES_ROMAN_24, 0, 10, 10);
    // display the game name
    render_bitmap_str("Potatis Clicker: Remake", GLUT_BITMAP_TIMES_ROMAN_24, 0, SCREEN_WIDTH-240, SCREEN_HEIGHT-30);
    // render the button
    render_button(&potatis);

    // handle inputs
    handle_input(window, deltaTime);

    // update the screen
    potatis.x = SCREEN_WIDTH / 2;
    potatis.y = SCREEN_HEIGHT / 2;

    // set the viewport
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSwapBuffers(window);
  }

  if (game_score) { // save the score
    game_score = fopen("score.dat", "wb+");
    fwrite(&potatoes, sizeof(long long), 1, game_score);
    fclose(game_score);
  }

  glfwTerminate(); // close the window
  return 0; // exit successfully
}
