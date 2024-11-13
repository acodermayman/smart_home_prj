import 'package:flutter/material.dart';
import 'package:smarthome_app/models/account.dart';
import 'package:smarthome_app/views/home.dart';

class Authentication {
  Future<List<Account>> accounts = loadLocalAccount();

  Future<void> localLogin(
      BuildContext context, String userName, String passWord) async {
    try {
      List<Account> accountList = await accounts;
      bool isAuthenticated = false;

      for (Account account in accountList) {
        if (userName == account.userName && passWord == account.passWord) {
          isAuthenticated = true;
          Navigator.pushReplacement(
              context,
              MaterialPageRoute(
                  builder: (context) => HomePage(
                        account: account,
                      )));
          break;
        }
      }

      if (!isAuthenticated) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text("Username or password is incorrect"),
            backgroundColor: Colors.red.shade400,
            duration: const Duration(seconds: 3),
          ),
        );
      }
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(e.toString()),
          backgroundColor: Colors.red.shade400,
          duration: const Duration(seconds: 3),
        ),
      );
    }
  }
}
