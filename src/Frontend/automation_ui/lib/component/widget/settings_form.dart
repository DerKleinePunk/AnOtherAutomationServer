import 'package:flutter_neumorphic/flutter_neumorphic.dart';
import 'theme_color_selector.dart';

//TODO https://github.com/Idean/Flutter-Neumorphic/blob/54ed5895d831d39943dc53ef0fcdc275bb49cf16/example/lib/lib/ThemeConfigurator.dart
//TODO Save as User settings

class SettingsForm extends StatelessWidget {
  const SettingsForm({ Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    String buttonText = "Dark Theme";
    var theme = NeumorphicTheme.of(context)!;
    if (theme.isUsingDark) {
      buttonText = "Light Theme";
    }
    return Column(
      children: [
        ThemeColorSelector(
          customContext: context,
        ),
        NeumorphicButton(
          onPressed: () {
            if (theme.isUsingDark) {
              theme.themeMode = ThemeMode.light;
            } else {
              theme.themeMode = ThemeMode.dark;
            }
          },
          child: Text(buttonText),
        )
      ],
    );
  }
}