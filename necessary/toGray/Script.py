import cv2
import numpy

source = cv2.VideoCapture("h2r1080.mp4") 

fourcc = int(cv2.VideoWriter_fourcc(*'DIVX'))
width  = int(source.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(source.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = source.get(cv2.CAP_PROP_FPS)
out = cv2.VideoWriter("h2r1080Gray.mp4", fourcc, fps, (width, height), False)

while True: 

    ret, img = source.read() 
	
    if not ret:
        break

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) 

    out.write(gray)

    cv2.imshow("Live", gray) 

    key = cv2.waitKey(1) 
    if key == ord("q"): 
        break
	
cv2.destroyAllWindows() 
source.release()
out.release()