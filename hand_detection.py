import cv2
import time
import mediapipe as mp
mirrorCam = 1
class HandTracker:
    def __init__(self,staticImgMode = False,maxHands = 1,minDetConf = 0.5, minTrackConf = 0.5, scale = 0.3,flipVal = mirrorCam):
        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(static_image_mode=staticImgMode,
                                max_num_hands=maxHands,
                                min_detection_confidence=minDetConf,
                                min_tracking_confidence=minTrackConf)
        self.mpDraw = mp.solutions.drawing_utils
        
        self.cap = cv2.VideoCapture(0)
        self.width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH)*scale)
        self.height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT)*scale)
        self.flipMetric = flipVal
        self.interestPoints = [0,4,8]
        self.pTime = 0
        self.cTime = 0
        self.thumbFinger=()
        self.indexFinger=()
        self.squareColor = (0,0,255)
        self.squareThickness = 3
        
    def drawSquare(self,frame):
        if self.thumbFinger and self.indexFinger:
            cv2.line(frame,self.thumbFinger,self.indexFinger,self.squareColor,self.squareThickness)
            cv2.line(frame,self.thumbFinger,(self.thumbFinger[0],self.indexFinger[1]),self.squareColor,self.squareThickness)
            cv2.line(frame,self.thumbFinger,(self.indexFinger[0],self.thumbFinger[1]),self.squareColor,self.squareThickness)
            cv2.line(frame,self.indexFinger,(self.thumbFinger[0],self.indexFinger[1]),self.squareColor,self.squareThickness)
            cv2.line(frame,self.indexFinger,(self.indexFinger[0],self.thumbFinger[1]),self.squareColor,self.squareThickness) 

    def processFrame(self,frame):
        frame = cv2.resize(frame,(self.width,self.height))
        frame = cv2.flip(frame,self.flipMetric)
        frameRGB = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        results = self.hands.process(frameRGB)
        if results.multi_hand_landmarks:
            for handLms in results.multi_hand_landmarks:
                for id, lm in enumerate(handLms.landmark):
                    if id in self.interestPoints:
                        h, w, c = frame.shape
                        cx, cy = int(lm.x * w), int(lm.y * h)
                        #print(f"{id} : {cx} : {cy}")
                        if id == 4:
                            self.thumbFinger = (cx,cy)
                        if id == 8:
                            self.indexFinger = (cx,cy)
                        cv2.circle(frame, (cx,cy), 5, (0,255,0), cv2.FILLED)
                
            #self.mpDraw.draw_landmarks(frame, handLms, self.mpHands.HAND_CONNECTIONS)
        return frame

    def fpsCalc(self):
        self.cTime = time.time()
        fps = int(1/(self.cTime-self.pTime))
        self.pTime = self.cTime
        
        return fps

    def run(self):
        while True:
            success, frame = self.cap.read()
            if not success:
                break
            processedFrame = self.processFrame(frame=frame)
            #print(f"Thumb : {self.thumbFinger} Index : {self.indexFinger}")
            self.drawSquare(frame=processedFrame)
            #currentFPS = self.fpsCalc()
            #cv2.putText(processedFrame,f"FPS : {currentFPS}", (10,70), cv2.FONT_HERSHEY_PLAIN, 3, (255,0,255), 3)
            cv2.imshow("Hand Detection" ,processedFrame)
            if cv2.waitKey(1) & 0xFF == 27:
                    break
        self.cap.release()
        cv2.destroyAllWindows()
if __name__ == "__main__":
    myHands = HandTracker()
    myHands.run()
        