// ignore_for_file: public_member_api_docs, sort_constructors_first
import 'dart:convert';

import 'package:flutter/services.dart';

class Account {
  final int id;
  final String userName;
  final String passWord;
  final String displayName;
  final String type;

  Account({required this.id, required this.userName, required this.passWord, required this.displayName, required this.type});

  factory Account.fromJson(Map<String, dynamic> json) {
    return Account(
      id: (json["id"] ?? 0) as int,
      userName: (json["userName"] ?? '') as String,
      passWord: (json["passWord"] ?? '') as String,
      displayName: (json["displayName"] ?? '') as String,
      type: (json["type"] ?? '') as String,
    );
  }

}

Future<List<Account>> loadLocalAccount() async {
  final response = await rootBundle.loadString('assets/data/account.json');
  var testList = jsonDecode(response) as List;
  List<Account> tests = testList.map((test) => Account.fromJson(test)).toList();
  return tests;
}