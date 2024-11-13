import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_spinkit/flutter_spinkit.dart';
import 'package:smarthome_app/models/account.dart';
import 'package:smarthome_app/models/device_state.dart';
import 'package:smarthome_app/models/temp_humi.dart';
import 'package:smarthome_app/views/login.dart';
import 'package:smarthome_app/views/notification.dart';
import 'package:web_socket_channel/io.dart';
// import 'package:web_socket_channel/status.dart' as status;

class HomePage extends StatefulWidget {
  const HomePage({super.key, this.url, required this.account});
  final Account account;
  final String? url;
  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final String esp_url = 'ws://192.168.99.100:81';
  bool isConnected = false;
  bool isLoaded = false;
  // bool isLedLivingRoomActive = false;
  // bool isLedBedRoomActive = false;
  // bool isDoorOpen = false;
  // bool isFireDetect = false;
  // TempHumi dht = TempHumi(temp: 0, humi: 0);
  DeviceState deviceState = DeviceState(
      temperature: 0,
      humidity: 0,
      livingRoomLedState: false,
      bedRoomLedState: false,
      doorState: false,
      fireDetected: false);
  late IOWebSocketChannel channel;
  String message = '';

  //Function
  void toggleLivingRoomLed() {
    setState(() {
      deviceState.livingRoomLedState = !deviceState.livingRoomLedState;
    });
    if (deviceState.livingRoomLedState) {
      channel.sink.add("LivingRoomLedOn");
    } else {
      channel.sink.add("LivingRoomLedOff");
    }
  }

  void toggleBedRoomLed() {
    setState(() {
      deviceState.bedRoomLedState = !deviceState.bedRoomLedState;
    });
    if (deviceState.bedRoomLedState) {
      channel.sink.add("BedRoomLedOn");
    } else {
      channel.sink.add("BedRoomLedOff");
    }
  }

  void toggleDoor() {
    setState(() {
      deviceState.doorState = !deviceState.doorState;
      if (deviceState.doorState) {
        channel.sink.add("DoorOn");
      } else {
        channel.sink.add("DoorOff");
      }
    });
  }

  @override
  void initState() {
    super.initState();
    channel = IOWebSocketChannel.connect(widget.url ?? esp_url);
    channel.stream.listen(
      (message) {
        channel.sink.add('Flutter received $message');
        if (message == "connected") {
          print('Received from MCU: $message');
          setState(() {
            isConnected = true;
            message = message;
          });
        } else {
          print('Received from MCU: $message');
          Map<String, dynamic> json = jsonDecode(message);
          setState(() {
            deviceState = DeviceState.fromJson(json);
            isLoaded = true;
            if (deviceState.fireDetected == true) {
              NotificationService.showInstantNotification(
                  "Cảnh báo cháy", "Rất tiêc nhà bạn đã bị cháy :((");
            }
          });
        }
        //channel.sink.close(status.goingAway);
      },
      onDone: () {
        //if WebSocket is disconnected
        print("Web socket is closed");
        setState(() {
          message = 'disconnected';
          isLoaded = false;
          isConnected = false;
        });
      },
      onError: (error) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text(error.toString()),
            backgroundColor: Colors
                .red.shade400, //a Snackbar with a custom background color.
            duration: const Duration(
                seconds: 3), //a Snackbar that disappears after 3 seconds.
          ),
        );
        // print(error.toString());
      },
    );
  }

  @override
  void dispose() {
    channel.sink.close();
    super.dispose();
  }

//*Giao diện app
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          //*Heading Section
          Container(
            margin: EdgeInsets.only(top: 40, left: 10, bottom: 30),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Row(
                  children: [
                    Container(
                      height: 50,
                      margin: EdgeInsets.only(right: 5),
                      child: ClipRRect(
                        borderRadius: BorderRadius.all(Radius.circular(10)),
                        child: AspectRatio(
                          aspectRatio: 1,
                          child: Image.asset('assets/images/chill.jpg',
                              fit: BoxFit.cover,
                              alignment: Alignment.bottomCenter),
                        ),
                      ),
                    ),
                    Text(
                      widget.account.displayName,
                      style: const TextStyle(
                          fontWeight: FontWeight.bold, fontSize: 25),
                    ),
                  ],
                ),
                IconButton(
                  onPressed: () {
                    Navigator.pushAndRemoveUntil(
                      context,
                      MaterialPageRoute(builder: (context) => LoginTab()),
                      (Route<dynamic> route) => false,
                    );
                  },
                  icon: Icon(Icons.logout),
                  iconSize: 30,
                )
              ],
            ),
          ),
          //*Icon Section
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 60.0),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Image.asset(
                  'assets/icons/cloud.png',
                  height: 60,
                ),
                Image.asset(
                  'assets/icons/wifi.png',
                  height: 60,
                ),
              ],
            ),
          ),
          Image.asset(
            'assets/icons/home-security.png',
            height: 120,
          ),
          const SizedBox(
            height: 5,
          ),
          //*Connect Status Section
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              const Text("Connect Status: "),
              Text(isConnected ? "Connected" : "Disconnected",
                  style: TextStyle(
                      fontWeight: FontWeight.bold,
                      color: isConnected ? Colors.greenAccent : Colors.grey)),
              const SizedBox(
                width: 5,
              ),
              !isConnected
                  ? const SpinKitWave(
                      size: 12,
                      color: Colors.grey,
                    )
                  : const Icon(
                      Icons.check_circle,
                      color: Colors.greenAccent,
                      size: 12,
                    )
            ],
          ),
          const SizedBox(
            height: 15,
          ),
          //*Temp Humi Section
          Container(
            padding: const EdgeInsets.symmetric(vertical: 20),
            margin: const EdgeInsets.symmetric(horizontal: 20, vertical: 5),
            decoration: BoxDecoration(
              color: Colors.white,
              borderRadius: const BorderRadius.all(Radius.circular(15)),
              boxShadow: [
                BoxShadow(
                  color: Colors.black.withAlpha(20),
                  blurRadius: 20,
                  spreadRadius: 10,
                ),
              ],
            ),
            child: Column(
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.start,
                  children: [
                    Image.asset(
                      'assets/icons/temperature_icon.png',
                      height: 30,
                    ),
                    Row(
                      children: [
                        const Text("Temperature: "),
                        Text(
                          "${deviceState.temperature} °C",
                          style: const TextStyle(color: Colors.redAccent),
                        ),
                      ],
                    )
                  ],
                ),
                Padding(
                  padding: const EdgeInsets.only(left: 6),
                  child: Row(
                    mainAxisAlignment: MainAxisAlignment.start,
                    children: [
                      Image.asset(
                        'assets/icons/humidity_icon.png',
                        height: 30,
                      ),
                      Row(
                        children: [
                          const Text("Humidity: "),
                          Text(
                            "${deviceState.humidity} %",
                            style: const TextStyle(color: Colors.blueAccent),
                          ),
                        ],
                      )
                    ],
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(
            height: 10,
          ),
          //*Led System Section
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Visibility(
                visible: widget.account.type == "chunha" ||
                    widget.account.type == "quangia",
                child: Container(
                  decoration: BoxDecoration(
                    borderRadius: const BorderRadius.all(Radius.circular(15)),
                    boxShadow: [
                      BoxShadow(
                          color: Colors.black.withAlpha(20),
                          blurRadius: 20,
                          spreadRadius: 5,
                          offset: const Offset(0, 5)),
                    ],
                  ),
                  width: 150,
                  child: ElevatedButton(
                    style: ElevatedButton.styleFrom(
                      padding: const EdgeInsets.all(16),
                      backgroundColor: Colors.white,
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(8),
                      ),
                    ),
                    onPressed: toggleLivingRoomLed,
                    child: Column(
                      children: [
                        Icon(
                          Icons.lightbulb_sharp,
                          color: deviceState.livingRoomLedState
                              ? Colors.yellow
                              : Colors.grey.shade400,
                          size: 55.0,
                        ),
                        Text(
                          "Phòng Khách",
                          style: TextStyle(color: Colors.grey.shade800),
                        )
                      ],
                    ),
                  ),
                ),
              ),
              const SizedBox(
                width: 20,
              ),
              Visibility(
                visible: widget.account.type == "chunha" ||
                    widget.account.type == "giupviec",
                child: Container(
                  decoration: BoxDecoration(
                    borderRadius: const BorderRadius.all(Radius.circular(15)),
                    boxShadow: [
                      BoxShadow(
                          color: Colors.black.withAlpha(20),
                          blurRadius: 20,
                          spreadRadius: 5,
                          offset: const Offset(0, 5)),
                    ],
                  ),
                  width: 150,
                  child: ElevatedButton(
                    style: ElevatedButton.styleFrom(
                      padding: const EdgeInsets.all(16),
                      backgroundColor: Colors.white,
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(8),
                      ),
                    ),
                    onPressed: toggleBedRoomLed,
                    child: Column(
                      children: [
                        Icon(
                          Icons.lightbulb_sharp,
                          color: deviceState.bedRoomLedState
                              ? Colors.yellow
                              : Colors.grey.shade400,
                          size: 55.0,
                        ),
                        Text(
                          "Phòng Ngủ",
                          style: TextStyle(color: Colors.grey.shade800),
                        )
                      ],
                    ),
                  ),
                ),
              ),
            ],
          ),
          const SizedBox(
            height: 20,
          ),
          //*Door Section
          Container(
            padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 20),
            margin: const EdgeInsets.symmetric(horizontal: 20),
            decoration: BoxDecoration(
              color: Colors.grey.shade100,
              borderRadius: const BorderRadius.all(Radius.circular(15)),
              boxShadow: [
                BoxShadow(
                  color: Colors.black.withAlpha(20),
                  blurRadius: 20,
                  spreadRadius: 10,
                ),
              ],
            ),
            child: Column(
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const Text("Door Status: "),
                    Text(
                      deviceState.doorState ? "Open" : "Close",
                      style: TextStyle(
                          fontWeight: FontWeight.bold,
                          color: deviceState.doorState
                              ? Colors.blueAccent
                              : Colors.redAccent),
                    )
                  ],
                ),
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    ElevatedButton(
                        onPressed: toggleDoor,
                        style: ElevatedButton.styleFrom(
                          backgroundColor: Colors.blueAccent,
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(8),
                          ),
                        ),
                        child: const Text(
                          "Open",
                          style: TextStyle(color: Colors.white),
                        )),
                    const SizedBox(
                      width: 10,
                    ),
                    ElevatedButton(
                        onPressed: toggleDoor,
                        style: ElevatedButton.styleFrom(
                          backgroundColor: Colors.redAccent,
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(8),
                          ),
                        ),
                        child: const Text(
                          "Close",
                          style: TextStyle(color: Colors.white),
                        )),
                  ],
                )
              ],
            ),
          ),
        ],
      ),
    );
  }
}
