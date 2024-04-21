import argparse
import time
from enum import Enum

import capnp
import numpy as np
import zmq

from functools import lru_cache
from itertools import cycle


class Dimensions(Enum):
    D1 = 1
    D2 = 2
    D3 = 3


array_schema = capnp.load("../../scheme/ndarray.capnp")


def array_to_capnp(arr):
    ndarray = array_schema.NDArray.new_message()
    ndarray.shape = arr.shape
    ndarray.data = arr.tobytes()
    print(f"sizeArr -> {arr.shape}")
    print(f"sizeData -> {len(ndarray.data)}")
    #print(arr[0][0][0])
    ndarray.dtype = getattr(array_schema.NDArray.DType, arr.dtype.name)
    print(ndarray.dtype)
    ndarray.timestamp = int(time.time() * 10**7)
    return ndarray.to_bytes()


def capnp_to_array(buffer):
    with array_schema.NDArray.from_bytes(buffer) as result:
        return (
            np.frombuffer(result.data, dtype=str(result.dtype)).reshape(
                list(result.shape)[::-1]
            ),
            result.timestamp,
        )


@lru_cache
def static(shape):
    return np.random.random_sample(shape)


freqs = cycle(np.linspace(1, 5, 30).tolist() + np.linspace(5, 1, 30).tolist())


def wave(shape):
    match len(shape):
        case 1:
            return np.sin(np.random.randint(1, 5) * np.linspace(0, 4 * np.pi, shape[0]))
        case 2:
            return np.repeat(wave(shape[:1]), shape[1]).reshape(*shape)
        case 3:
            return np.stack([wave(shape[1:])] * shape[0])


def make_array(cols, rows, depth, dimensions, type_):
    f = None
    match type_:
        case "random":
            f = np.random.random_sample
        case "static":
            f = static
        case "wave":
            f = wave
        case _:
            raise RuntimeError("Invalid array type")

    match dimensions:
        case 1:
            return f((cols))
        case 2:
            return f((cols, rows))
        case 3:
            return f((depth, cols, rows))
        case _:
            raise RuntimeError("Dimensions must be in {1, 2, 3}")


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", required=False, type=int, default=5555)
    parser.add_argument(
        "-d", "--dimensions", required=False, type=int, default=2, choices=[1, 2, 3]
    )
    parser.add_argument("-r", "--rows", required=False, type=int, default=5000)
    parser.add_argument("-c", "--cols", required=False, type=int, default=5000)
    parser.add_argument("-e", "--depth", required=False, type=int, default=1)
    parser.add_argument("-s", "--sleep", required=False, type=float, default=0)
    parser.add_argument(
        "-t",
        "--type",
        required=False,
        type=str,
        default="random",
        choices=["random", "static", "wave"],
    )

    args = parser.parse_args()

    context = zmq.Context()
    socket = context.socket(zmq.PUB)
    url = f"tcp://*:{args.port}"
    print(f"Start server at {url}")
    socket.bind(url)
    while True:
        try:
            array = make_array(
                args.cols, args.rows, args.depth, args.dimensions, args.type
            )
            print(array.shape)
            socket.send(array_to_capnp(array))
            # socket.send(bytes(time.time().__str__(), 'utf-8'))
            time.sleep(args.sleep)
        except KeyboardInterrupt as e:
            print("Stop server loop...")
            socket.close()
            context.term()
            break
