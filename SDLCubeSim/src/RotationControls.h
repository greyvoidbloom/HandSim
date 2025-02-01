#ifndef ROTATION_H
#define ROTATION_H
#include <cmath>
#include<iostream>
#define FIXED_DELTA_TIME 0.016f 
class RotationHandler{
private:
    bool allowGestureRotation,allowKeyboardRotation;
    const float degreeNinety = M_PI_2; //pi/2
    float completedRotation;
    float angleX,angleY,relativeVelocityY,relativeVelocityX;
public:
    RotationHandler();
    float getXangle();
    float getYangle();
    bool isGestureRotationAllowed();
    bool isKeyboardRotationAllowed();
    void enableKeyboardRotation();
    void disableKeyboardRotation();
    void enableGestureRotation();
    void disableGestureRotation();
    void handleXvelocity(int* thumbFinger, int* indexFinger);
    void handleXDirection(int* thumbFinger, int* indexFinger);
    void gestureControlX(bool *dataReceived);
    void rotateLeft(float speed);

};
#endif
