"""Sans-I/O protocols for all protocol logic for the Pufferfish backend server.

Modules:
    exceptions: exceptions raised by modules in this sub-package.
    events: interface for events passed between communication protocol layers.
    server: protocol for all pure-logic functionalities of the backend server.
    backend: protocol for handling MCU and frontend filesystem events.
    frontend: protocol for converting between bytestrings and frontend events.
    mcu: protocol for converting between bytestream and MCU events.
    application: protocol logic for state synchronization.
    messages: protocol logic for data serialization/deserialization.
    datagrams: protocol logic for data integrity checking in frames.
    frames: protocol logic for data framing over a bytestream.
"""
