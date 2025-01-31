import pygame
import numpy as np
import socket
import re
import time
import threading
class cube3D:
    def __init__(self,width=800,height=600,fps=60,cubeScaling=100,heading="Chopped Cube",fov=400,host='localhost',port=5555):
        self.screenWidth = width
        self.screenHeight = height
        self.cubeScale = cubeScaling
        self.heading = heading
        self.fps = fps
        self.fov=fov
        self.cubeColor = (255,255,255) #white
        self.lineColor = (0,225,0)
        self.cubeWidth = 1
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server_address = (host, port)
        self.sock.bind(self.server_address)
        self.lastReceivedTime = time.time()
        self.rotationTimeout = 0.2
        self.rotationAngleX,self.rotationAngleY,self.rotationAngleZ = 0,0,0
        self.rotationIncrementX,self.rotationIncrementY,self.rotationIncrementZ = 0,0,0
        self.scaleFactor = 1
        
        self.vertices = np.array([
            [-1,-1,-1],# front bottom left
            [-1,1,-1],# front top left
            [1,1,-1], #front top right
            [1,-1,-1], #front bottom right
            [-1,-1,1],# back bottom left
            [-1,1,1],# back top left
            [1,1,1], #back top right
            [1,-1,1], #back bottom right
        ])
        self.vertices = self.vertices * self.cubeScale
        
        self.edges = [(0,1),(1,2),(2,3),(3,0),
                    (4,5),(5,6),(6,7),(7,4),
                    (0,4),(1,5),(2,6),(3,7),
                    ]
        self.innerLines=[(0,2),(2,7),(7,5),(5,0),]
        pygame.init()
        self.screen = pygame.display.set_mode((self.screenWidth,self.screenHeight))
        pygame.display.set_caption(self.heading)
        self.clock = pygame.time.Clock()
        
    def receiveData(self):
        print(f"Receiver listening on {self.server_address}")
        try:
            while True:
                data, _ = self.sock.recvfrom(4096)
                message = data.decode()
                self.lastReceivedTime = time.time()
                #print(message)
                try:
                    result = re.findall(r"\((\d+),\s*(\d+)\)", message)
                    if len(result) == 2:
                        (fin1X, fin1Y), (fin2X, fin2Y) = map(int,  result[0]), map(int, result[1])
                        deltaY = fin2Y - fin1Y
                        deltaX = fin2X - fin1X
                        self.rotationIncrementX = 0.02 if deltaX > 0 else -0.02 if deltaX < 0 else 0
                        self.rotationIncrementY = 0.02 if deltaY > 0 else -0.02 if deltaY < 0 else 0
                        
                        #print(f"1st finger: {fin1X,fin1Y} \n 2nd finger: {fin2X,fin2Y}")
                        #print(f"{self.rotationAngleX,self.rotationAngleY}")
                        
                except Exception as e:
                    print(f"Error processing message: {e}")
            
        except KeyboardInterrupt:
            print("Receiver shutting down")
        finally:
            self.sock.close()
    def checkRotationStop(self):
        if time.time() - self.lastReceivedTime > self.rotationTimeout:
            self.rotationIncrementX, self.rotationIncrementY, self.rotationIncrementZ = 0,0,0
            
    def projectVertices(self,vertices):
        vertices = np.array(vertices)
        x, y, z = vertices[:, 0], vertices[:, 1], vertices[:, 2]
        factor = self.fov/(self.fov+z)
        x_proj = x * factor + self.screenWidth/2
        y_proj = -y * factor + self.screenHeight/2
        #print(f"x_proj : {x_proj} \n y_proj: {y_proj} \n z: {z}")
        projected_points = list(zip(x_proj, y_proj))
        return projected_points
    def drawCube(self,screen,vertices):
        for edge in self.edges:
            start,end = edge
            pygame.draw.line(screen,self.cubeColor,vertices[start],vertices[end],self.cubeWidth)
    def drawInsides(self,screen,vertices):
        for line in self.innerLines:
            start,end = line
            pygame.draw.line(screen,self.lineColor,vertices[start],vertices[end],self.cubeWidth) 
    def rotateX(self,vertices,angle):
        rotationMatrix = np.array([
            [1,0,0],
            [0,np.cos(angle),-np.sin(angle)],
            [0,np.sin(angle),np.cos(angle)]
        ])
        return np.dot(vertices,rotationMatrix.T)
    def rotateY(self,vertices,angle):
        rotationMatrix = np.array([
            [np.cos(angle),0, np.sin(angle)],
            [0,1,0],
            [-np.sin(angle),0,np.cos(angle)]
        ])
        return np.dot(vertices,rotationMatrix.T)
    def rotateZ(self,vertices,angle):
        pass
    def scaleCube(self,scale):
        homogeneousVertices = np.hstack([self.vertices, np.ones((self.vertices.shape[0], 1))])
        scalingMatrix = np.array([
            [scale,0,0,0],
            [0,scale,0,0],
            [0,0,scale,0],
            [0,0,0,1]
        ])
        scaledVertices = np.dot(homogeneousVertices,scalingMatrix.T)
        return scaledVertices[:, :3]
        
    def run(self):
        receiverThread = threading.Thread(target=self.receiveData)
        receiverThread.daemon = True
        receiverThread.start()
        
        self.running = True
        while self.running:
            self.screen.fill((0,0,0))#black
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        self.running = False
            self.rotationAngleX +=self.rotationIncrementX
            self.rotationAngleY +=self.rotationIncrementY
            self.checkRotationStop()
            rotatedVertices = self.rotateY(vertices=self.vertices,angle=self.rotationAngleY)
            rotatedVertices = self.rotateX(vertices=rotatedVertices,angle=self.rotationAngleX)
            projectedPoints = self.projectVertices(vertices=rotatedVertices)
            self.drawCube(screen=self.screen,vertices=projectedPoints)
            self.drawInsides(screen=self.screen,vertices=projectedPoints)
            pygame.display.flip()
            self.clock.tick(self.fps)
        pygame.quit()
            
if __name__ == "__main__":
    testCube = cube3D()
    testCube.run()