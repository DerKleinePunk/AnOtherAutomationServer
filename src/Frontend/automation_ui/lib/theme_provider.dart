import 'package:flutter/material.dart';

//---This to switch theme from Switch button----
class ThemeProvider extends ChangeNotifier {
  //-----Store the theme of our app--
  ThemeMode themeMode = ThemeMode.dark;

  //----If theme mode is equal to dark then we return True----
  //-----isDarkMode--is the field we will use in our switch---
  bool get isDarkMode => themeMode == ThemeMode.dark;

  //---implement ToggleTheme function----
  void toggleTheme(bool isOn) {
    themeMode = isOn ? ThemeMode.dark : ThemeMode.light;

    //---notify material app to update UI----
    notifyListeners();
  }
}
