import 'package:flutter/material.dart';

class UserSettings {
  Color _baseColor = Colors.amber;

  Color getBaseColor() {
    debugPrint("UserSesstings getBaseColor " + _baseColor.toString());
    return _baseColor;
  }

  void setBaseColor(Color color) {
    _baseColor = color;
    debugPrint("UserSesstings setBaseColor " + _baseColor.toString());
  }
}