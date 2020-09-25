/* eslint-disable */
import { Writer, Reader } from 'protobufjs/minimal';


export interface RotaryEncoder {
  angle: number;
  lastAngleChange: number;
  buttonPressed: boolean;
  lastButtonDown: number;
  lastButtonUp: number;
}

export interface FrontendDisplaySetting {
  theme: ThemeVariant;
  unit: Unit;
}

export interface SystemSettingRequest {
  brightness: number;
  date: number;
}

const baseRotaryEncoder: object = {
  angle: 0,
  lastAngleChange: 0,
  buttonPressed: false,
  lastButtonDown: 0,
  lastButtonUp: 0,
};

const baseFrontendDisplaySetting: object = {
  theme: 0,
  unit: 0,
};

const baseSystemSettingRequest: object = {
  brightness: 0,
  date: 0,
};

export const Unit = {
  imperial: 0 as const,
  metric: 1 as const,
  UNRECOGNIZED: -1 as const,
  fromJSON(object: any): Unit {
    switch (object) {
      case 0:
      case "imperial":
        return Unit.imperial;
      case 1:
      case "metric":
        return Unit.metric;
      case -1:
      case "UNRECOGNIZED":
      default:
        return Unit.UNRECOGNIZED;
    }
  },
  toJSON(object: Unit): string {
    switch (object) {
      case Unit.imperial:
        return "imperial";
      case Unit.metric:
        return "metric";
      default:
        return "UNKNOWN";
    }
  },
}

export type Unit = 0 | 1 | -1;

export const ThemeVariant = {
  dark: 0 as const,
  light: 1 as const,
  UNRECOGNIZED: -1 as const,
  fromJSON(object: any): ThemeVariant {
    switch (object) {
      case 0:
      case "dark":
        return ThemeVariant.dark;
      case 1:
      case "light":
        return ThemeVariant.light;
      case -1:
      case "UNRECOGNIZED":
      default:
        return ThemeVariant.UNRECOGNIZED;
    }
  },
  toJSON(object: ThemeVariant): string {
    switch (object) {
      case ThemeVariant.dark:
        return "dark";
      case ThemeVariant.light:
        return "light";
      default:
        return "UNKNOWN";
    }
  },
}

export type ThemeVariant = 0 | 1 | -1;

export const RotaryEncoder = {
  encode(message: RotaryEncoder, writer: Writer = Writer.create()): Writer {
    writer.uint32(8).uint32(message.angle);
    writer.uint32(21).float(message.lastAngleChange);
    writer.uint32(24).bool(message.buttonPressed);
    writer.uint32(37).float(message.lastButtonDown);
    writer.uint32(45).float(message.lastButtonUp);
    return writer;
  },
  decode(input: Uint8Array | Reader, length?: number): RotaryEncoder {
    const reader = input instanceof Uint8Array ? new Reader(input) : input;
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = Object.create(baseRotaryEncoder) as RotaryEncoder;
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.angle = reader.uint32();
          break;
        case 2:
          message.lastAngleChange = reader.float();
          break;
        case 3:
          message.buttonPressed = reader.bool();
          break;
        case 4:
          message.lastButtonDown = reader.float();
          break;
        case 5:
          message.lastButtonUp = reader.float();
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },
  fromJSON(object: any): RotaryEncoder {
    const message = Object.create(baseRotaryEncoder) as RotaryEncoder;
    if (object.angle !== undefined && object.angle !== null) {
      message.angle = Number(object.angle);
    } else {
      message.angle = 0;
    }
    if (object.lastAngleChange !== undefined && object.lastAngleChange !== null) {
      message.lastAngleChange = Number(object.lastAngleChange);
    } else {
      message.lastAngleChange = 0;
    }
    if (object.buttonPressed !== undefined && object.buttonPressed !== null) {
      message.buttonPressed = Boolean(object.buttonPressed);
    } else {
      message.buttonPressed = false;
    }
    if (object.lastButtonDown !== undefined && object.lastButtonDown !== null) {
      message.lastButtonDown = Number(object.lastButtonDown);
    } else {
      message.lastButtonDown = 0;
    }
    if (object.lastButtonUp !== undefined && object.lastButtonUp !== null) {
      message.lastButtonUp = Number(object.lastButtonUp);
    } else {
      message.lastButtonUp = 0;
    }
    return message;
  },
  fromPartial(object: DeepPartial<RotaryEncoder>): RotaryEncoder {
    const message = Object.create(baseRotaryEncoder) as RotaryEncoder;
    if (object.angle !== undefined && object.angle !== null) {
      message.angle = object.angle;
    } else {
      message.angle = 0;
    }
    if (object.lastAngleChange !== undefined && object.lastAngleChange !== null) {
      message.lastAngleChange = object.lastAngleChange;
    } else {
      message.lastAngleChange = 0;
    }
    if (object.buttonPressed !== undefined && object.buttonPressed !== null) {
      message.buttonPressed = object.buttonPressed;
    } else {
      message.buttonPressed = false;
    }
    if (object.lastButtonDown !== undefined && object.lastButtonDown !== null) {
      message.lastButtonDown = object.lastButtonDown;
    } else {
      message.lastButtonDown = 0;
    }
    if (object.lastButtonUp !== undefined && object.lastButtonUp !== null) {
      message.lastButtonUp = object.lastButtonUp;
    } else {
      message.lastButtonUp = 0;
    }
    return message;
  },
  toJSON(message: RotaryEncoder): unknown {
    const obj: any = {};
    obj.angle = message.angle || 0;
    obj.lastAngleChange = message.lastAngleChange || 0;
    obj.buttonPressed = message.buttonPressed || false;
    obj.lastButtonDown = message.lastButtonDown || 0;
    obj.lastButtonUp = message.lastButtonUp || 0;
    return obj;
  },
};

export const FrontendDisplaySetting = {
  encode(message: FrontendDisplaySetting, writer: Writer = Writer.create()): Writer {
    writer.uint32(8).int32(message.theme);
    writer.uint32(16).int32(message.unit);
    return writer;
  },
  decode(input: Uint8Array | Reader, length?: number): FrontendDisplaySetting {
    const reader = input instanceof Uint8Array ? new Reader(input) : input;
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = Object.create(baseFrontendDisplaySetting) as FrontendDisplaySetting;
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.theme = reader.int32() as any;
          break;
        case 2:
          message.unit = reader.int32() as any;
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },
  fromJSON(object: any): FrontendDisplaySetting {
    const message = Object.create(baseFrontendDisplaySetting) as FrontendDisplaySetting;
    if (object.theme !== undefined && object.theme !== null) {
      message.theme = ThemeVariant.fromJSON(object.theme);
    } else {
      message.theme = 0;
    }
    if (object.unit !== undefined && object.unit !== null) {
      message.unit = Unit.fromJSON(object.unit);
    } else {
      message.unit = 0;
    }
    return message;
  },
  fromPartial(object: DeepPartial<FrontendDisplaySetting>): FrontendDisplaySetting {
    const message = Object.create(baseFrontendDisplaySetting) as FrontendDisplaySetting;
    if (object.theme !== undefined && object.theme !== null) {
      message.theme = object.theme;
    } else {
      message.theme = 0;
    }
    if (object.unit !== undefined && object.unit !== null) {
      message.unit = object.unit;
    } else {
      message.unit = 0;
    }
    return message;
  },
  toJSON(message: FrontendDisplaySetting): unknown {
    const obj: any = {};
    obj.theme = ThemeVariant.toJSON(message.theme);
    obj.unit = Unit.toJSON(message.unit);
    return obj;
  },
};

export const SystemSettingRequest = {
  encode(message: SystemSettingRequest, writer: Writer = Writer.create()): Writer {
    writer.uint32(8).uint32(message.brightness);
    writer.uint32(16).uint32(message.date);
    return writer;
  },
  decode(input: Uint8Array | Reader, length?: number): SystemSettingRequest {
    const reader = input instanceof Uint8Array ? new Reader(input) : input;
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = Object.create(baseSystemSettingRequest) as SystemSettingRequest;
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.brightness = reader.uint32();
          break;
        case 2:
          message.date = reader.uint32();
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },
  fromJSON(object: any): SystemSettingRequest {
    const message = Object.create(baseSystemSettingRequest) as SystemSettingRequest;
    if (object.brightness !== undefined && object.brightness !== null) {
      message.brightness = Number(object.brightness);
    } else {
      message.brightness = 0;
    }
    if (object.date !== undefined && object.date !== null) {
      message.date = Number(object.date);
    } else {
      message.date = 0;
    }
    return message;
  },
  fromPartial(object: DeepPartial<SystemSettingRequest>): SystemSettingRequest {
    const message = Object.create(baseSystemSettingRequest) as SystemSettingRequest;
    if (object.brightness !== undefined && object.brightness !== null) {
      message.brightness = object.brightness;
    } else {
      message.brightness = 0;
    }
    if (object.date !== undefined && object.date !== null) {
      message.date = object.date;
    } else {
      message.date = 0;
    }
    return message;
  },
  toJSON(message: SystemSettingRequest): unknown {
    const obj: any = {};
    obj.brightness = message.brightness || 0;
    obj.date = message.date || 0;
    return obj;
  },
};

type Builtin = Date | Function | Uint8Array | string | number | undefined;
type DeepPartial<T> = T extends Builtin
  ? T
  : T extends Array<infer U>
  ? Array<DeepPartial<U>>
  : T extends ReadonlyArray<infer U>
  ? ReadonlyArray<DeepPartial<U>>
  : T extends {}
  ? { [K in keyof T]?: DeepPartial<T[K]> }
  : Partial<T>;