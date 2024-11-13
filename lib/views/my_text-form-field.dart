import 'package:flutter/material.dart';

class MyTextFormField extends StatefulWidget {
  const MyTextFormField(
      {super.key,
      required this.obscureText,
      required this.hintText,
      required this.icon,
      required this.validate,
      required this.controller});
  final bool obscureText;
  final String hintText;
  final Icon icon;
  final String validate;
  final TextEditingController controller;
  @override
  State<MyTextFormField> createState() => _MyTextFormFieldState();
}

class _MyTextFormFieldState extends State<MyTextFormField> {
  @override
  Widget build(BuildContext context) {
    return Container(
      padding: EdgeInsets.only(bottom: 10),
      decoration: BoxDecoration(boxShadow: [
        BoxShadow(
          color: Colors.black.withAlpha(20),
          blurRadius: 20,
          spreadRadius: 10,
        ),
      ]),
      child: TextFormField(
        controller: widget.controller,
        decoration: InputDecoration(
          contentPadding: EdgeInsets.symmetric(vertical: 20),
          // focusedBorder: OutlineInputBorder(
          //   borderRadius: BorderRadius.circular(10),
          //   borderSide: BorderSide(color: Colors.grey.shade400),
          // ),
          // enabledBorder: OutlineInputBorder(
          //   borderRadius: BorderRadius.circular(10),
          //   borderSide: BorderSide(color: Colors.grey.shade400),
          // ),
          border: OutlineInputBorder(
              borderSide: BorderSide.none,
              borderRadius: BorderRadius.circular(10)),
          hintText: widget.hintText,
          prefixIcon: widget.icon,
          fillColor: Colors.white,
          filled: true,
        ),
        obscureText: widget.obscureText, //hide the input
        validator: (value) {
          if (value == null || value.isEmpty) {
            return widget.validate;
          }
          return null;
        },
      ),
    );
  }
}
