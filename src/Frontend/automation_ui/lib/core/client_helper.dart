import '../main.dart';
import '../server/client.dart';
import 'package:flutter/foundation.dart';

class CoreClientHelper {
  static ServerClient? _pillowClient;

  static ServerClient getClient() {
    if (_pillowClient == null) {
      throw Exception('Client is not yet created');
    }
    //It can't be null here, to compile bang operator is needed
    return _pillowClient!;
  }

  static ServerClient initClient(String serverUrl,
      {String? username, String? password}) {
    _pillowClient =
        ServerClient(serverUrl, password: password, username: username);
    
    return getClient();
  }

  static bool initNeeded() {
    bool need = _pillowClient == null ||
      (_pillowClient?.username == null || _pillowClient?.password == null);
    debugPrint("initNeeded $need");
    return need;
  }

  static Future clearAuthStorage() async {
    var storeAuthDataInClient = preferences.getBool('storeAuth') ?? false;
    if(!storeAuthDataInClient) {
      await preferences.remove('username');
    }
    await preferences.remove('password');
  }
}