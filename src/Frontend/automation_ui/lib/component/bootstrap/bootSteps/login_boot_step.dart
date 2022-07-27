import '../../../core/client_helper.dart';
import '../../../screens/login.dart';
import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter/foundation.dart' show kIsWeb;
import '../bootstrap_controller.dart';
import '../bootstrap_step.dart';

class LoginBootStep extends BootstrapStep {
  const LoginBootStep();

  @override
  Widget buildStep(BootstrapController controller) {
    return Login(controller: controller);
  }

  @override
  Future<bool> stepRequired(SharedPreferences prefs) async {
    String? serverUrl;
    _AuthDetails? details;
    if ((prefs.getBool('storeAuth') ?? false) &&
        CoreClientHelper.initNeeded()) {
      //2 Ways to go:
      //CORS -> no way to read the cookie as its http only
      //App/NoneCors -> either read the cookie or get login details
      serverUrl = prefs.getString('serverUrl');
      if (kIsWeb) {
        if (/*document.cookie?.isEmpty ?? true*/ true) {
          //We are runnin in CORS mode need to read user and pass
          if (serverUrl != null) {
            final client = CoreClientHelper.initClient(serverUrl);
            if (await client.checkAuthentication()) {
              return false;
            }
          }
          details = _getAuthDetails(prefs);
        } else {
          //TODO implement none CORS mode, read cookie and check if still ok
        }
      } else {
        details = _getAuthDetails(prefs);
      }
      if (serverUrl != null && details.present) {
        CoreClientHelper.initClient(serverUrl,
            username: details.user, password: details.password);
      }
    }
    //Check if client was created and has configuration for login
    return CoreClientHelper.initNeeded();
  }

  _AuthDetails _getAuthDetails(SharedPreferences preferences) {
    final user = preferences.getString('username');
    final password = preferences.getString('password');
    return _AuthDetails(user, password);
  }
}

class _AuthDetails {
  final String? user;
  final String? password;
  bool get present => user != null;
  _AuthDetails(this.user, this.password);
}