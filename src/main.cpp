#include "pch.h"
#include <glad/gl.h>
#include "Core/Window.h"

bool shouldClose = false;

void KeyEvent(int keycode, int action){
    if(keycode == GLFW_KEY_E && action == GLFW_PRESS){
        printf("E pressed\n");
    }

    if(keycode == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        printf("LEFT MOUSE\n");
    }

}

void MouseEvent(float xpos, float ypos){

}

void ShutDownEvent(){
    shouldClose = true;
}



int main(){
    SOF::WindowProps windowprops{}; 
    windowprops.Title = "Shadow of the Forsaken";
    windowprops.KeyEvent = KeyEvent;
    windowprops.MouseEvent = MouseEvent;
    windowprops.ShutDownEvent = ShutDownEvent;
    SOF::Window window{windowprops};

    while(!shouldClose){
        glClearColor(1.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.OnUpdate();
    }

    

    return 0;
}
