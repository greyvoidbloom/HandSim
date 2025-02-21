#include "RotationControls.h"
    RotationHandler::RotationHandler(): allowGestureRotation(false),allowKeyboardRotation(false),angleX(0.0f),angleY(0.0f),relativeVelocityY(3.0f),relativeVelocityX(3.0f),completedRotation(0.0f){}
    float RotationHandler::getXangle(){return angleX;}
    float RotationHandler::getYangle(){return angleY;}
    bool RotationHandler::isGestureRotationAllowed(){return allowGestureRotation;}
    bool RotationHandler::isKeyboardRotationAllowed(){return allowKeyboardRotation;}
    void RotationHandler::enableKeyboardRotation(){
        allowKeyboardRotation = true;
        completedRotation=0.0f;
    }
    void RotationHandler::disableKeyboardRotation(){allowKeyboardRotation = false;}
    void RotationHandler::enableGestureRotation(){allowGestureRotation = true;}
    void RotationHandler::disableGestureRotation(){allowGestureRotation = false;}
    void RotationHandler::handleXvelocity(int* thumbFinger, int* indexFinger){
        float dist = (sqrt(pow((thumbFinger[0] - indexFinger[0]), 2) + pow((thumbFinger[1] - indexFinger[1]), 2)) / 10);
        relativeVelocityX = std::max(0.1f,dist);
    }
    void RotationHandler::handleYvelocity(int* thumbFinger, int* indexFinger){
        float dist = (sqrt(pow((thumbFinger[0] - indexFinger[0]), 2) + pow((thumbFinger[1] - indexFinger[1]), 2)) / 10);
        relativeVelocityY = std::max(0.1f,dist);

    }
    void RotationHandler::handleXDirection(int* thumbFinger, int* indexFinger){relativeVelocityX = ((thumbFinger[0] - indexFinger[0]) < 0) ? -relativeVelocityX : relativeVelocityX;}
    void RotationHandler::handleYDirection(int* thumbFinger, int* indexFinger){relativeVelocityY = ((thumbFinger[1] - indexFinger[1]) < 0) ? -relativeVelocityY : relativeVelocityY;}
    void RotationHandler::gestureControlY(bool *dataReceived){
        if (*dataReceived) {angleY = relativeVelocityY * FIXED_DELTA_TIME;} 
        else {
            angleY *= 0.9f;
            if (std::abs(angleY) < 0.01f) { 
                angleY = 0.0f;
            }
        }  
    }
    void RotationHandler::gestureControlX(bool *dataReceived){
        if (*dataReceived) {angleX = relativeVelocityX * FIXED_DELTA_TIME;} 
        else {
            angleX *= 0.9f;
            if (std::abs(angleX) < 0.01f) { 
                angleX = 0.0f;
            }
        } 
    }
    void RotationHandler::rotateHorizontal(float speed) {
        if (!isKeyboardRotationAllowed()) return;
        float step = -speed * FIXED_DELTA_TIME;
        float remainingRotation = degreeNinety - std::abs(completedRotation);
        if (std::abs(step) > remainingRotation) {
            step = -remainingRotation;
            disableKeyboardRotation();  
        }
        completedRotation += std::abs(step);
        if (completedRotation >= degreeNinety) {
            angleX = 0.0f;
        } else {
            angleX = step;
        }
        std::cout <<"step: "<<step<< " completed rotation: " << completedRotation << "/" << degreeNinety << std::endl;
    }