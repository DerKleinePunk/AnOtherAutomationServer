import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../../theme_provider.dart';
import 'theme_color_selector.dart';

//TODO https://github.com/Idean/Flutter-Neumorphic/blob/54ed5895d831d39943dc53ef0fcdc275bb49cf16/example/lib/lib/ThemeConfigurator.dart
//TODO Save as User settings

class SettingsForm extends StatelessWidget {
  const SettingsForm({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    String buttonText = "Dark Theme";
    final themeProvider = Provider.of<ThemeProvider>(context);
    if (themeProvider.isDarkMode) {
      buttonText = "Light Theme";
    }
    return Column(
      children: [
        ThemeColorSelector(
          customContext: context,
        ),
        OutlinedButton(
          onPressed: () {
            if (themeProvider.isDarkMode) {
              themeProvider.themeMode = ThemeMode.light;
            } else {
              themeProvider.themeMode = ThemeMode.dark;
            }
          },
          child: Text(buttonText),
        )
      ],
    );
  }
}
