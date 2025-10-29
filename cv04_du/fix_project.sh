# 1) nutné include pro kameru
sed -i '1i #include "gfx/Camera.h"' src/app/main.cpp

# 2) vlož do main loopu (přibližně před render s->render):
#    (Použij ed/awk pokud chceš přesné místo – zde jen block k vložení)

cat <<'CPP'

    // --- Camera state (mimo smyčku si dej staticky / nebo globálně)
    static Camera cam({0.f,1.6f,4.f}, -90.f, 0.f);
    static bool rmbDown=false;
    static double lastX=0.0, lastY=0.0;

    // --- Per-frame input ---
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
        double x,y; glfwGetCursorPos(w,&x,&y);
        if(!rmbDown){ rmbDown=true; lastX=x; lastY=y; }
        double dx = x - lastX, dy = y - lastY;
        lastX = x; lastY = y;
        cam.processMouse((float)dx, (float)-dy, true);
    } else {
        rmbDown=false;
    }

    float dt = 1.0f/60.0f; // zatím fix
    bool fwd = glfwGetKey(w, GLFW_KEY_W)==GLFW_PRESS;
    bool back= glfwGetKey(w, GLFW_KEY_S)==GLFW_PRESS;
    bool left= glfwGetKey(w, GLFW_KEY_A)==GLFW_PRESS;
    bool right=glfwGetKey(w, GLFW_KEY_D)==GLFW_PRESS;
    cam.processKeyboard(fwd,back,left,right,dt);

    int wpx,hpx; glfwGetFramebufferSize(w,&wpx,&hpx);
    float aspect = (float)wpx/(float)hpx;
    glm::mat4 V = cam.view();
    glm::mat4 P = cam.proj(60.f, aspect, 0.1f, 200.f);

CPP
