import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../../theme_provider.dart';
import 'theme_color_selector.dart';

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
              themeProvider.toggleTheme(false);
            } else {
              themeProvider.toggleTheme(true);
            }
          },
          child: Text(buttonText),
        )
      ],
    );
  }
}
