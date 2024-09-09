#pragma once
enum CCResponseType
{
    EffectRequest = 0x00,
    EffectStatus = 0x01,
    GenericEvent = 0x10,
    LoadEvent = 0x18,
    SaveEvent = 0x19,
    DataResponse = 0x20,
    RpcRequest = 0xD0,
    Login = 0xF0,
    LoginSuccess = 0xF1,
    GameUpdate = 0xFD,
    Disconnect = 0xFE,
    KeepAlive = 0xFF
};

enum CCRequestType {
    EffectTest = 0x00,
    EffectStart = 0x01,
    EffectStop = 0x02,
    RequestGenericEvent = 0x10,
    DataRequest = 0x20,
    RpcResponse = 0xD0,
    PlayerInfo = 0xE0,
    RequestLogin = 0xF0,
    RequestGameUpdate = 0xFD,
    RequestKeepAlive = 0xFF
};

enum CCEffectStatus {
    Success = 0x00,
    Failure = 0x01,
    Unavailable = 0x02,
    Retry = 0x03,
    Queue = 0x04,
    Running = 0x05,
    Paused = 0x06,
    Resumed = 0x07,
    Finished = 0x08
};