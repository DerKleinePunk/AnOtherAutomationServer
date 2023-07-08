import 'dart:io';
import 'package:automation_ui/core/router.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';
import 'package:package_info_plus/package_info_plus.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_neumorphic/flutter_neumorphic.dart';
import 'package:window_size/window_size.dart';
import 'package:provider/provider.dart';

import 'models/chat_message_model.dart';
import 'models/panel_value_model.dart';
import 'models/panel_page_changed.dart';

late SharedPreferences preferences;

const double windowWidth = 400;
const double windowHeight = 800;

void setupWindow() {
  if (!kIsWeb && (Platform.isWindows || Platform.isLinux || Platform.isMacOS)) {
    setWindowTitle('Home Server');
    setWindowMinSize(const Size(windowWidth, windowHeight));
    getCurrentScreen().then((screen) {
      setWindowFrame(Rect.fromCenter(
        center: screen!.frame.center,
        width: windowWidth,
        height: windowHeight,
      ));
    });
  }
}

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  setupWindow();//Only used in Desktop App
  var packageInfo = await PackageInfo.fromPlatform();
  var version =
      "${packageInfo.packageName} ${packageInfo.version} (${packageInfo.buildNumber})";
  debugPrint = (String? message, {int? wrapWidth}) =>
      debugPrintSynchronouslyWithText(message, version, wrapWidth: wrapWidth);
  SharedPreferences.getInstance().then((instance) {
    preferences = instance;
    runApp(const MyApp());
  });
}

void debugPrintSynchronouslyWithText(String? message, String version,
    {int? wrapWidth}) {
  message = "[${DateTime.now()} - $version]: $message";
  debugPrintSynchronously(message, wrapWidth: wrapWidth);
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MultiProvider(
      providers: [
         ChangeNotifierProvider(create: (context) => ChatMessageList()),
         ChangeNotifierProvider(create: (context) => PanelValueMap()),
         ChangeNotifierProvider(create: (context) => PanelChangedMap())
      ],
      child: NeumorphicApp(
        onGenerateTitle: (context) {
          return UILocalizations.of(context)!.appTitle;
        },
        localizationsDelegates: const [
          UILocalizations.delegate,
          GlobalMaterialLocalizations.delegate,
          GlobalWidgetsLocalizations.delegate,
          GlobalCupertinoLocalizations.delegate,
        ],
        supportedLocales: UILocalizations.supportedLocales,
        initialRoute: 'dashboard',
        onGenerateRoute: AppRouter.generateRoute,
        themeMode: ThemeMode.dark,
        theme: const NeumorphicThemeData(
          baseColor: Color(0xFFFFFFFF),
          lightSource: LightSource.topLeft,
          depth: 10,
        ),
        darkTheme: const NeumorphicThemeData(
          baseColor: Color(0xFF3E3E3E),
          lightSource: LightSource.topLeft,
          depth: 6,
        ),
      ),
    );
  }
}
