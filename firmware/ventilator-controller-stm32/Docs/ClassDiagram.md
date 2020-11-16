```mermaid
classDiagram
      class BackendNS
      <<namespace>> BackendNS
      BackendNS *-- Backend:Composition
      BackendNS o-- BackendMessage:Aggregation
      BackendNS : +Array<Message> message_descriptors

      class Backend
      Backend *-- BackendReceiver
      Backend *-- BackendSender
      Backend : +Array<Message> message_descriptors

      class BackendReceiver
      BackendReceiver --> FrameReceiver
      BackendReceiver --> BackendCRCReceiver
      BackendReceiver --> BackendDatagramReceiver
      BackendReceiver --> BackendMessageReceiver

      class BackendCRCReceiver
      CRCElementReceiver <|-- BackendCRCReceiver

      class BackendParsedCRC
      ParsedCRCElement <|-- BackendParsedCRC

      class BackendDatagramReceiver
      DatagramReceiver <|-- BackendDatagramReceiver
      BackendDatagramReceiver ..> BackendCRCReceiver

      class BackendParsedDatagram
      ParsedDatagram <|-- BackendParsedDatagram
      BackendParsedDatagram ..> BackendCRCReceiver

      class BackendMessageReceiver
      MessageReceiver <|-- BackendMessageReceiver
      BackendMessageReceiver ..> BackendMessage

      class BackendSender
      BackendSender --> FrameSender
      BackendSender --> BackendCRCSender
      BackendSender --> BackendDatagramSender
      BackendSender --> BackendMessageSender

      class BackendCRCSender
      CRCElementSender <|-- BackendCRCSender

      class BackendDatagramSender
      DatagramSender <|-- BackendDatagramSender

      class BackendMessageSender
      MessageSender <|-- BackendMessageSender

      BackendMessage <|-- Message

      class Message{
      }

      Message <|-- UnrecognizedMessage
      Message <|-- Alarms
      Message <|-- SensorMeasurements
      Message <|-- CycleMeasurements
      Message <|-- Parameters
      Message <|-- ParametersRequest
      Message <|-- Ping
      Message <|-- Announcement

      class UnrecognizedMessage {
      }

      class Alarms {
        +uint32 time
        +bool alarm_one
        +bool alarm_two
      }

```
