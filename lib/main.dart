import 'package:flutter/material.dart';
import 'package:smarthome_app/views/home.dart';
import 'package:smarthome_app/views/login.dart';
import 'package:smarthome_app/views/notification.dart';

void main() async{
  WidgetsFlutterBinding.ensureInitialized();
  await NotificationService.init();
  runApp(const MainApp());
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      debugShowCheckedModeBanner: false,
      home: LoginTab()
    );
  }
}
