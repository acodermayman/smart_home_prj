// ignore_for_file: public_member_api_docs, sort_constructors_first
import 'dart:convert';

class DeviceState {
  final double temperature;
  final double humidity;
  bool livingRoomLedState;
  bool bedRoomLedState;
  bool doorState;
  // final bool buzzerState;
  final bool fireDetected;
  DeviceState(
      {required this.temperature,
      required this.humidity,
      required this.livingRoomLedState,
      required this.bedRoomLedState,
      required this.doorState,
      required this.fireDetected});

  factory DeviceState.fromJson(Map<String, dynamic> json) {
    return DeviceState(
        temperature: (json["temperature"] ?? 0.0) as double,
        humidity: (json["humidity"] ?? 0.0) as double,
        livingRoomLedState: (json["livingRoomLedState"] ?? false) as bool,
        bedRoomLedState: (json["bedRoomLedState"] ?? false) as bool,
        doorState: (json["doorState"] ?? false) as bool,
        fireDetected : (json["fireDetected"] ?? false) as bool);
  }
}
