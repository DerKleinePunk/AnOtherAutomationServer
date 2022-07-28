import 'core/router.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';
import 'package:package_info_plus/package_info_plus.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_neumorphic/flutter_neumorphic.dart';

late SharedPreferences preferences;

void main() async {
    WidgetsFlutterBinding.ensureInitialized();
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
    return NeumorphicApp(
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
    );
  }
}