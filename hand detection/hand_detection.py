import cv2
import time
import mediapipe as mp
import math
import socket
import threading
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
        self.interestPoints = [4,8]
        self.pTime = 0
        self.cTime = 0
        self.thumbFinger=()
        self.indexFinger=()
        self.middleFinger=()
        self.squareColor = (192, 192, 192)
        self.cubeColor = (0,255,0)
        self.squareThickness = 3
    def sendCoords(self,startFinger,endFinger):
        client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ("localhost", 5555)
        while True:
            data = f"{startFinger}, {endFinger}"
            client.sendto(data.encode(), server_address)
            time.sleep(0.1)
        
    def drawRect(self,frame,startFinger,endFinger):
        if startFinger and endFinger:
            cv2.line(frame,startFinger,endFinger,self.squareColor,self.squareThickness)
            cv2.line(frame,startFinger,(startFinger[0],endFinger[1]),self.squareColor,self.squareThickness)
            cv2.line(frame,startFinger,(endFinger[0],startFinger[1]),self.squareColor,self.squareThickness)
            cv2.line(frame,endFinger,(startFinger[0],endFinger[1]),self.squareColor,self.squareThickness)
            cv2.line(frame,endFinger,(endFinger[0],startFinger[1]),self.squareColor,self.squareThickness) 
    
    def drawCuboid(self,frame,startFinger,endFinger,color):
        if startFinger and endFinger:
            cubelen = int(math.dist(startFinger,endFinger)/1.73205080757)
            a = (startFinger[0]+cubelen,startFinger[1])
            d = (startFinger[0],startFinger[1]-cubelen)
            e = (startFinger[0]+cubelen,startFinger[1]-cubelen)
            
            f = (endFinger[0]-cubelen,endFinger[1])
            b = (endFinger[0],endFinger[1]+cubelen)
            c = (endFinger[0]-cubelen,endFinger[1]+cubelen)
            #cv2.line(frame,startFinger,endFinger,color,self.squareThickness)
            cv2.line(frame,startFinger,a,color,self.squareThickness)
            cv2.line(frame,startFinger,d,color,self.squareThickness)
            cv2.line(frame,d,e,color,self.squareThickness)
            cv2.line(frame,a,e,color,self.squareThickness)
            
            cv2.line(frame,endFinger,f,color,self.squareThickness)
            cv2.line(frame,endFinger,e,color,self.squareThickness)
            cv2.line(frame,endFinger,b,color,self.squareThickness)
            cv2.line(frame,f,c,color,self.squareThickness)
            cv2.line(frame,c,b,color,self.squareThickness)
            cv2.line(frame,d,f,color,self.squareThickness)
            cv2.line(frame,startFinger,c,color,self.squareThickness)
            cv2.line(frame,a,b,color,self.squareThickness)

    def processFrame(self,frame):
        frame = cv2.resize(frame,(self.width,self.height))
        frame = cv2.flip(frame,self.flipMetric)
        frameRGB = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        results = self.hands.process(frameRGB)
        
        self.thumbFinger = None
        self.indexFinger = None
        
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
                        #cv2.circle(frame, (cx,cy), 5, (0,255,0), cv2.FILLED)
                
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
            #self.drawRect(frame=processedFrame,startFinger=self.thumbFinger,endFinger=self.indexFinger)
            self.drawCuboid(frame=processedFrame,startFinger=self.thumbFinger,endFinger=self.indexFinger,color=self.cubeColor)
            #currentFPS = self.fpsCalc()
            #cv2.putText(processedFrame,f"FPS : {currentFPS}", (10,70), cv2.FONT_HERSHEY_PLAIN, 3, (255,0,255), 3)
            cv2.imshow("Chopped Cube" ,processedFrame)
            if cv2.waitKey(1) & 0xFF == 27:
                    break
        self.cap.release()
        cv2.destroyAllWindows()
if __name__ == "__main__":
    myHands = HandTracker()
    #myHands.run()
    captureThread = threading.Thread(target=myHands.run)
    #sendThread = threading.Thread(target=myHands.sendCoords, args=(myHands.thumbFinger, myHands.indexFinger))
    captureThread.start()
    #sendThread.start()
