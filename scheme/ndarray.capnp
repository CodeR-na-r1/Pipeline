#ndarray.capnp
@0x934efeaaa017fff0;

struct NDArray {
    shape @0 :List(UInt32);
    dtype @1 :DType;
    data @2 :Data;
    timestamp @3 :UInt64;

    enum DType {
	bool @0;
	uint8 @1;
	uint16 @2;
	uint32 @3;
	uint64 @4;
	int8 @5;
	int16 @6;
	int32 @7;
	int64 @8;
	float32 @9;
	float64 @10;
	float128 @11;
    }
}