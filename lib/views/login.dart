import 'package:flutter/material.dart';
import 'package:smarthome_app/controllers/authentication.dart';
import 'package:smarthome_app/views/my_text-form-field.dart';

class LoginTab extends StatefulWidget {
  const LoginTab({super.key});

  @override
  State<LoginTab> createState() => _LoginTabState();
}

class _LoginTabState extends State<LoginTab> {
  Authentication _auth = Authentication();
  TextEditingController _userName = TextEditingController();
  TextEditingController _passWord = TextEditingController();
  TextEditingController _ipAddress = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: Padding(
        padding: EdgeInsets.symmetric(horizontal: 20),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            MyTextFormField(
                obscureText: false,
                hintText: "IpAddress",
                icon: const Icon(Icons.location_on),
                validate: "Please enter your ipaddress",
                controller: _ipAddress),
            MyTextFormField(
                obscureText: false,
                hintText: "Username",
                icon: const Icon(Icons.person),
                validate: "Please enter your username",
                controller: _userName),
            MyTextFormField(
                obscureText: true,
                hintText: "Password",
                icon: const Icon(Icons.lock),
                validate: "Please enter your password",
                controller: _passWord),
            SizedBox(
              height: 10,
            ),
            Container(
              margin: const EdgeInsets.symmetric(horizontal: 20),
              alignment: Alignment.center,
              width: double.infinity,
              // padding: const EdgeInsets.all(10),
              // margin: const EdgeInsets.symmetric(horizontal: 20),
              decoration: BoxDecoration(
                gradient: LinearGradient(
                  colors: [
                    Colors.blueAccent,
                    Colors.lightBlueAccent,
                  ],
                  begin: Alignment.centerLeft,
                  end: Alignment.centerRight,
                ),
                borderRadius: BorderRadius.circular(8.0), // Rounded corners
              ),
              child: MaterialButton(
                onPressed: () async {
                  await _auth.localLogin(
                      context, _userName.text, _passWord.text);
                },
                padding: const EdgeInsets.symmetric(
                    vertical: 12.0, horizontal: 24.0),
                textColor: Colors.white,
                child: const Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: <Widget>[
                    Text('Đăng Nhập'),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
