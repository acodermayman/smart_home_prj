import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_spinkit/flutter_spinkit.dart';
import 'package:smarthome_app/temp_humi.dart';
import 'package:web_socket_channel/io.dart';
// import 'package:web_socket_channel/status.dart' as status;

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final String esp_url = 'ws://192.168.99.100:81';
  bool isConnected = false;
  bool isLoaded = false;
  TempHumi dht = TempHumi(temp: 0, humi: 0);
  late IOWebSocketChannel channel;
  String message = '';

  @override
  void initState() {
    super.initState();
    channel = IOWebSocketChannel.connect(esp_url); // Khởi tạo trong initState
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
            dht = TempHumi.fromJson(json);
            isLoaded = true;
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
        print(error.toString());
      },
    );
  }

  @override
  void dispose() {
    channel.sink.close();
    super.dispose();
  }

//Function
  bool isActive = false;
  bool isOpen = false;
  void toggleLed() {
    setState(() {
      isActive = !isActive;
    });
    if (isActive) {
      channel.sink.add("LedOn");
    } else {
      channel.sink.add("LedOff");
    }
  }

  void toggleDoor() {
    setState(() {
      isOpen = !isOpen;
    });
  }

//Giao diện app
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.grey.shade100,
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Padding(
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
            SizedBox(
              height: 5,
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Text("Connect Status: "),
                Text(isConnected ? "Connected" : "Disconnected",
                    style: TextStyle(
                        fontWeight: FontWeight.bold,
                        color: isConnected ? Colors.greenAccent : Colors.grey)),
                SizedBox(
                  width: 5,
                ),
                !isConnected
                    ? SpinKitWave(
                        size: 12,
                        color: Colors.grey,
                      )
                    : Icon(
                        Icons.check_circle,
                        color: Colors.greenAccent,
                        size: 12,
                      )
              ],
            ),
            SizedBox(
              height: 15,
            ),
            Container(
              padding: const EdgeInsets.symmetric(vertical: 20),
              margin: const EdgeInsets.symmetric(horizontal: 20, vertical: 5),
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
                    mainAxisAlignment: MainAxisAlignment.start,
                    children: [
                      Image.asset(
                        'assets/icons/temperature_icon.png',
                        height: 30,
                      ),
                      Row(
                        children: [
                          Text("Temperature: "),
                          Text(
                            "${dht.temp} °C",
                            style: TextStyle(color: Colors.redAccent),
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
                            Text("Humidity: "),
                            Text(
                              "${dht.humi} %",
                              style: TextStyle(color: Colors.blueAccent),
                            ),
                          ],
                        )
                      ],
                    ),
                  ),
                ],
              ),
            ),
            SizedBox(
              height: 10,
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                ElevatedButton(
                  style: ElevatedButton.styleFrom(
                    padding: EdgeInsets.all(16),
                    backgroundColor: Colors.white,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(8),
                    ),
                  ),
                  onPressed: toggleLed,
                  child: Icon(
                    Icons.lightbulb_sharp,
                    color: isActive ? Colors.yellow : Colors.grey.shade400,
                    size: 55.0,
                  ),
                ),
                SizedBox(
                  width: 20,
                ),
                Container(
                  padding:
                      const EdgeInsets.symmetric(vertical: 10, horizontal: 20),
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
                        children: [
                          Text("Door Status: "),
                          Text(
                            isOpen ? "Open" : "Close",
                            style: TextStyle(
                                fontWeight: FontWeight.bold,
                                color: isOpen
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
                              child: Text(
                                "Open",
                                style: TextStyle(color: Colors.white),
                              )),
                          SizedBox(
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
                              child: Text(
                                "Close",
                                style: TextStyle(color: Colors.white),
                              )),
                        ],
                      )
                    ],
                  ),
                )
              ],
            )
          ],
        ),
      ),
    );
  }
}
