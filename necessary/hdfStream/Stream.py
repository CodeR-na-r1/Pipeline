import time
import zmq
import cv2
import h5py

import capnp

FILENAME = "video/1051 03jun.h5"
FREQUENCE = 50   # frames per second
port = 5558

sleepTime = 1 / FREQUENCE # millisecnods

f = h5py.File(FILENAME, "r")

# --- capnp code ---

array_schema = capnp.load("../../scheme/ndarray.capnp")

def array_to_capnp(arr):

    ndarray = array_schema.NDArray.new_message()
    ndarray.shape = arr.shape
    ndarray.data = arr.tobytes()

    print(f"sizeArr -> {arr.shape}")
    print(f"sizeData -> {len(ndarray.data)}")

    ndarray.dtype = getattr(array_schema.NDArray.DType, arr.dtype.name)

    print(ndarray.dtype)
    ndarray.timestamp = int(time.time() * 10**7)
    return ndarray.to_bytes()

# ---

# --- zmq code ---


context = zmq.Context()
socket = context.socket(zmq.PUB)
url = f"tcp://*:{port}"
print(f"Start server at {url}")
socket.bind(url)

# ---

cv2.namedWindow("window", cv2.WINDOW_GUI_NORMAL)

for frame in f["image"]:
    try:

        cv2.imshow("window", frame)
        cv2.waitKey(1)

        img = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        socket.send(array_to_capnp(img))

        time.sleep(sleepTime)

    except KeyboardInterrupt as e:
            
            print("Stop server loop...")
            break
    
socket.close()
context.term()
cv2.destroyAllWindows()