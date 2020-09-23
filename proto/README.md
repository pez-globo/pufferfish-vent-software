# Protocol Buffers

We are using Google Protocol Buffers to standardize presentation-layer data
serialization in the communication protocols between the MCU firmware and the
backend server and between the GUI frontend and the backend server.

## Firmware/Backend

The message schemas for communication between the backend server and the  MCU
firmware are defined in `mcu_pb.proto`. To update, you will need to modify that
file and then regenerate the C++ classes for the MCU firmware and the Python
classes for the backend.

To update the Python classes at `backend/ventserver/protocols/protobuf/mcu_pb.py`,
first install the `betterproto` package using pip with the `[compiler]` feature flag:
```bash
$ pip install "betterproto[compiler]"
```
Then run, from this directory:

```bash
$ export $PROTOC=/path/to/local/bin/protoc  # replace the path with the one for your computer

$ # Generate Python Code
$ $PROTOC -I=. --python_betterproto_out=../backend/ventserver/protocols/protobuf mcu_pb.proto
```

Note that if you installed the `protobuf-compiler` package on Ubuntu, then you
should replace `$PROTOC` with `protoc`.

To update the C++ classes at
`firmware/ventilator-controller-stm32/Core/Inc/Pufferfish/Protocols/mcu_pb.pb.h` and
`firmware/ventilator-controller-stm32/Core/Src/Pufferfish/Protocols/mcu_pb.pb.c`,
the easiest way is to install nanopb with pip, and run, from this directory:

```bash
$ export $PROTOC=/path/to/local/bin/protoc

$ # Generate C++ Code
$ nanopb_generator -I=. --output-dir="../firmware/ventilator-controller-stm32/Core/Inc/Pufferfish/Protocols" mcu_pb.proto
```

Note that after generating these files, you will need to move
`firmware/ventilator-controller-stm32/Core/Inc/Pufferfish/Protocols/mcu_pb.pb.c`
to `firmware/ventilator-controller-stm32/Core/Src/Pufferfish/Protocols/mcu_pb.pb.c`.

## Frontend/Backend

The message schemas for communication between the backend demo and the frontend
demo are defined in both `mcu_pb.proto` and `frontend_pb.proto`. Currently these
are separated so that backend/frontend-specific protocol buffers don't get compiled
into the firmware.

To update the Python classes at `backend/ventserver/protocols/protobuf`,
first install the `betterproto` package using pip, then run:

```bash
$ export $PROTOC=/path/to/local/bin/protoc  # replace the path with the one for your computer

$ # Generate Python Code
$ $PROTOC -I=. --python_betterproto_out=../backend/ventserver/protocols/protobuf mcu_pb.proto
$ $PROTOC -I=. --python_betterproto_out=../backend/ventserver/protocols/protobuf frontend_pb.proto
```

Note that if you installed the `protobuf-compiler` package on Ubuntu, then you
should replace `$PROTOC` with `protoc`.

To update the TypeScript classes at `frontend/src/store/controller/proto`,
first install the `typescript`, `google-protobuf`, and `protoc-gen-ts` packages
globally using npm:

```bash
$ npm install -g typescript
$ npm install -g google-protobuf
$ npm install -g protoc-gen-ts
```

Then run, from this directory:

```bash
$ export $PROTOC=/path/to/local/bin/protoc  # replace the path with the one for your computer

$ # Generate TypeScript Code (make sure to run yarn install in frontend first)
$ $PROTOC -I=. --ts_proto_out=../frontend/src/store/controller/proto --plugin="../frontend/node_modules/ts-proto/protoc-gen-ts_proto" mcu_pb.proto
$ $PROTOC -I=. --ts_proto_out=../frontend/src/store/controller/proto --plugin="../frontend/node_modules/ts-proto/protoc-gen-ts_proto" frontend_pb.proto
```

Note that if you installed the `protobuf-compiler` package on Ubuntu, then you
should replace `$PROTOC` with `protoc`.
