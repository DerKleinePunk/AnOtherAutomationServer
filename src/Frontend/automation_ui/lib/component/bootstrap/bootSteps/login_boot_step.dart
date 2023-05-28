import '../../../core/client_helper.dart';
import '../../../screens/login.dart';
import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter/foundation.dart';
import '../bootstrap_controller.dart';
import '../bootstrap_step.dart';
// import 'dart:html' as html;

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
      serverUrl = prefs.getString('serverUrl');
      if (kIsWeb) {
        //TODO check Cookie is set try login with Cookie
        // App Startup Logic ?
        if (/*html.document.cookie?.isEmpty ?? true*/true) {
          if (serverUrl != null) {
            final client = CoreClientHelper.initClient(serverUrl);
            if (await client.checkAuthentication()) {
              return false;
            }
          }
          details = _getAuthDetails(prefs);
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
  bool get present => user != null && password != null;
  _AuthDetails(this.user, this.password);
}