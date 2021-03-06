#include "cameras.hpp"
#include "glfw.hpp"

#include <iostream>

// Good reference here to map camera movements to lookAt calls
// http://learnwebgl.brown37.net/07_cameras/camera_movement.html

using namespace glm;

struct ViewFrame
{
  vec3 left;
  vec3 up;
  vec3 front;
  vec3 eye;

  ViewFrame(vec3 l, vec3 u, vec3 f, vec3 e) : left(l), up(u), front(f), eye(e)
  {
  }
};

ViewFrame fromViewToWorldMatrix(const mat4 &viewToWorldMatrix)
{
  return ViewFrame{-vec3(viewToWorldMatrix[0]), vec3(viewToWorldMatrix[1]),
      -vec3(viewToWorldMatrix[2]), vec3(viewToWorldMatrix[3])};
}

bool FirstPersonCameraController::update(float elapsedTime)
{
  if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_LEFT) &&
      !m_LeftButtonPressed) {
    m_LeftButtonPressed = true;
    glfwGetCursorPos(
        m_pWindow, &m_LastCursorPosition.x, &m_LastCursorPosition.y);
  } else if (!glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_LEFT) &&
             m_LeftButtonPressed) {
    m_LeftButtonPressed = false;
  }

  const auto cursorDelta = ([&]() {
    if (m_LeftButtonPressed) {
      dvec2 cursorPosition;
      glfwGetCursorPos(m_pWindow, &cursorPosition.x, &cursorPosition.y);
      const auto delta = cursorPosition - m_LastCursorPosition;
      m_LastCursorPosition = cursorPosition;
      return delta;
    }
    return dvec2(0);
  })();

  float truckLeft = 0.f;
  float pedestalUp = 0.f;
  float dollyIn = 0.f;
  float rollRightAngle = 0.f;


 // add speed up with Ctrl

  if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL)) {
    increaseSpeed(m_fSpeed*20.f);
  } else {
    m_fSpeed = m_fSpeed_loc;
  }


  if (glfwGetKey(m_pWindow, GLFW_KEY_W)) {
    dollyIn += m_fSpeed * elapsedTime;
  }

  // Truck left
  if (glfwGetKey(m_pWindow, GLFW_KEY_A)) {
    truckLeft += m_fSpeed * elapsedTime;
  }

  // Pedestal up
  if (glfwGetKey(m_pWindow, GLFW_KEY_UP)) {
    pedestalUp += m_fSpeed * elapsedTime;
  }

  // Dolly out
  if (glfwGetKey(m_pWindow, GLFW_KEY_S)) {
    dollyIn -= m_fSpeed * elapsedTime;
  }

  // Truck right
  if (glfwGetKey(m_pWindow, GLFW_KEY_D)) {
    truckLeft -= m_fSpeed * elapsedTime;
  }

  // Pedestal down
  if (glfwGetKey(m_pWindow, GLFW_KEY_DOWN)) {
    pedestalUp -= m_fSpeed * elapsedTime;
  }

  if (glfwGetKey(m_pWindow, GLFW_KEY_Q)) {
    rollRightAngle -= 0.001f;
  }
  if (glfwGetKey(m_pWindow, GLFW_KEY_E)) {
    rollRightAngle += 0.001f;
  }
  m_fSpeed = m_fSpeed_loc;


  // cursor going right, so minus because we want pan left angle:
  const float panLeftAngle = -0.01f * float(cursorDelta.x);
  const float tiltDownAngle = 0.01f * float(cursorDelta.y);

  const auto hasMoved = truckLeft || pedestalUp || dollyIn || panLeftAngle ||
                        tiltDownAngle || rollRightAngle;
  if (!hasMoved) {
    return false;
  }

  m_camera.moveLocal(truckLeft, pedestalUp, dollyIn);
  m_camera.rotateLocal(rollRightAngle, tiltDownAngle, 0.f);
  m_camera.rotateWorld(panLeftAngle, m_worldUpAxis);

  return true;
}

bool TrackballCameraController::update(float elapsedTime) {

    if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_MIDDLE ) &&
          !m_MiddleButtonPressed) {
        m_MiddleButtonPressed = true;
        // on r�cupere la position de la souris quand le bouton millieu de la souris est appuyer
        glfwGetCursorPos(
            m_pWindow, &m_LastCursorPosition.x, &m_LastCursorPosition.y);
      } else if (!glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_MIDDLE ) &&
                 m_MiddleButtonPressed) {
        m_MiddleButtonPressed = false;
      }


      const auto cursorDelta = ([&]() {
        if (m_MiddleButtonPressed) {
          dvec2 cursorPosition;
          glfwGetCursorPos(m_pWindow, &cursorPosition.x, &cursorPosition.y);
          const auto delta = cursorPosition - m_LastCursorPosition;
          m_LastCursorPosition = cursorPosition;
          return delta;
        }
        return dvec2(0);
      })();
      /*float panShift =0;
      if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_SHIFT)) {
            panShift = 0.01f;
      }*/




      if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_SHIFT)) {
            // Pan
           // std::cout<< "truckleft" << std::endl;
            const float truckLeft = 0.01f * float(cursorDelta.x);
            const float pedestalUp = 0.01f * float(cursorDelta.y);
            const auto hasMoved = truckLeft || pedestalUp;
            if (!hasMoved) {
              return false;
            }
            m_camera.moveLocal(truckLeft, pedestalUp, 0.f);
            return true;
      }

      if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL)) {
            // Pan

            float mouseOffset  = 0.01f * float(cursorDelta.x);
            const auto hasMoved = mouseOffset ;
            if (!hasMoved) {
              return false;
            }
            const auto viewVector = m_camera.center() - m_camera.eye();
            if (mouseOffset > 0.f) {
              // We don't want to move more that the length of the view vector (cannot
              // go beyond target)
              mouseOffset = glm::min(mouseOffset, glm::length(viewVector) - 1e-4f);
            }
            const auto neweye = m_camera.eye() + glm::normalize(viewVector)*mouseOffset ;
            Camera newcam = Camera(neweye, m_camera.center(), m_worldUpAxis) ;
            setCamera(newcam);
           // m_camera.moveLocal(truckLeft, pedestalUp, 0.f);
            return true;
      }
    // Orbit move on trackball update :Rotate around the center of the camera, along local axes



      const float latitudeAngle  = -0.01f * float(cursorDelta.x) /*+ panShift*/;
      const float longitudeAngle = 0.01f * float(cursorDelta.y);

      const auto hasMoved =  longitudeAngle || latitudeAngle ;
      if (!hasMoved) {
        return false;
      }
      const auto rotationMatrix_longi = glm::rotate(glm::mat4(1), longitudeAngle, m_camera.left());
      const auto rotationMatrix_lati = glm::rotate(glm::mat4(1), latitudeAngle,m_worldUpAxis);
      const auto finalrotat = glm::vec3(rotationMatrix_lati * rotationMatrix_longi*glm::vec4(m_camera.eye()- m_camera.center() , 0)); // 0 car une direction n'a pas de composante homogene
     /* const auto depthAxis = m_camera.eye() - m_camera.center();*/
      const auto newEye = finalrotat +m_camera.center() ;
      m_camera = Camera(newEye, m_camera.center(), m_worldUpAxis);


          /////////////////////////////////////
    //
        return true;

    }
/*
if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT) == true) {

  glm::ivec2 lastmousePos;
  SDL_GetRelativeMouseState(&lastmousePos.x, &lastmousePos.y);
  glm::ivec2 getMpos = windowManager.getMousePosition();
  // std::cout << "1: "<<getMpos[0] << "/" << lastmousePos.x << " 2: " <<
  // getMpos[1] << "/" << lastmousePos.y << std::endl; std::cout << "1:
  // "<<getMpos[0]-lastmousePos.x << " 2: " << getMpos[1]-lastmousePos.y <<
  // std::endl;
  cam1.rotateLeft(lastmousePos.x);
  cam1.rotateUp(lastmousePos.y);
}

if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT) == true) {

  glm::ivec2 lastmousePos;
  SDL_GetRelativeMouseState(&lastmousePos.x, &lastmousePos.y);
  glm::ivec2 getMpos = windowManager.getMousePosition();
  // std::cout << "1: "<<getMpos[0] << "/" << lastmousePos.x << " 2: " <<
  // getMpos[1] << "/" << lastmousePos.y << std::endl; std::cout << "1:
  // "<<getMpos[0]-lastmousePos.x << " 2: " << getMpos[1]-lastmousePos.y <<
  // std::endl;
  cam1.rotateLeft(lastmousePos.x);
  cam1.rotateUp(lastmousePos.y);
}

if (windowManager.isKeyPressed(SDLK_w) == true) {
  cam1.moveFront(-0.1);
}
if (windowManager.isKeyPressed(SDLK_s) == true) {
  cam1.moveFront(0.1);
}
*/
