//https://github.com/Dev-Owl/Caladrius/blob/main/lib/pillowdart/client.dart
import 'package:flutter/foundation.dart'; //kIsWeb true if Webapp
import 'package:http/http.dart' as http;
import '../component/widget/third_party/adaptive_scaffold.dart';
import 'cookie_save.dart';
import 'data/auto_destination.dart';
import 'dart:convert';
import 'package:flutter_neumorphic/flutter_neumorphic.dart';

import 'home_server_endpoints.dart';
import 'http/http_stub.dart'
    if (dart.library.io) 'http/http_app.dart'
    if (dart.library.html) 'http/http_web.dart';

import 'message_stream.dart';
import '../core/audioplayer.dart';
import 'http/api_http_client.dart';
import 'http/http_exceptions.dart';
import 'user_settings.dart';

class ServerClient {
  String serverUrl;
  String? username;
  String? password;
  bool autoLogin = true;
  DateTime? lastSessionRequest;
  int ensureSessionsEveryXMinutes = 5;
  CookieSave? cookieSave;
  ServerMessageClient? _messageClient;
  AudioPlayerService? _audioPlayerService;
  final UserSettings _userSettings = UserSettings();

  final http.Client httpClient = getClient();
  final ApiHttpClient apiHttpClient = ApiHttpClient();

  ServerClient(this.serverUrl, {this.username, this.password});

  Future<http.Response> request(String endpoint, HttpMethod method,
      {Map<String, String>? queryParameter,
      dynamic body,
      Map<String, String>? header}) async {
    final uri = HomeServerEndpoints.combine(serverUrl, endpoint);
    if (queryParameter != null) {
      uri.queryParameters.addAll(queryParameter);
    }
    final headerForRequest = <String, String>{};
    if (header != null) {
      headerForRequest.addAll(header);
    }
    //Todo Soleved Cookie Problem
    /*final cookieHeader = cookieJar?.header;
        if (cookieHeader != null) {
          headerForRequest.addAll(cookieHeader);
        }*/
    switch (method) {
      case HttpMethod.GET:
        return httpClient.get(uri, headers: headerForRequest);
      case HttpMethod.POST:
        return httpClient.post(uri, body: body, headers: headerForRequest);
      case HttpMethod.PUT:
        return httpClient.put(uri, body: body, headers: headerForRequest);
      case HttpMethod.DELETE:
        return httpClient.delete(uri, body: body, headers: headerForRequest);
      case HttpMethod.HEAD:
        return httpClient.head(uri, headers: headerForRequest);
    }
  }

  Future<bool> authenticate(String? username, String? password,
      {bool autoLogin = true}) async {
    if ((cookieSave?.cookieStillValid() ?? false) &&
        username == this.username &&
        password == this.password) return true;
    //If we have a set of auth details like the requested probe the server
    if (this.username == username && this.password == password) {
      if (await checkAuthentication()) return true;
    } else if (this.username != null && this.password != null) {
      //Kill the session if any (http-only cookie no way to read)
      await removeSessionIfExists();
    }

    var requestBody = {
      'name': username,
      'password': password,
    };

    var sendString = jsonEncode(requestBody);
    final response = await httpClient.post(
      HomeServerEndpoints.combine(serverUrl, HomeServerEndpoints.login),
      body: sendString,
    );
    if (autoLogin) {
      this.username = username;
      this.password = password;
    } else {
      this.autoLogin = false;
      this.username = null;
      this.password = null;
    }

    if (response.statusCode == 200) {
      if (response.headers.containsKey('set-cookie') &&
          response.headers['set-cookie'] != null) {
        cookieSave = CookieSave(
          response.headers['set-cookie'] ?? '',
          DateTime.now().add(
            const Duration(
              minutes: 9,
            ),
          ),
        );
      }
      _messageClient = ServerMessageClient(serverUrl);
      _audioPlayerService = AudioPlayerService(this);
      _audioPlayerService!.init();
      return true;
    }
    return false;
  }

  Future<bool> checkAuthentication() async {
    //If in web do session test only every x minutes, saves requests
    if (kIsWeb &&
        lastSessionRequest != null &&
        lastSessionRequest!.difference(DateTime.now()).inMinutes <
            ensureSessionsEveryXMinutes) return true;

    /*final authTest = await httpClient.get(
      HomeServerEndpoints.combine(serverUrl, HomeServerEndpoints.session),
    );

    if (authTest.statusCode == 200) {
      final authState = jsonDecode(authTest.body);
      if (authState != null) {
        if (authState['userCtx']['name'] != null) {
          lastSessionRequest = DateTime.now();
          username = authState['userCtx']['name'];
          return true;
        }
      }
    }*/

    try {
      final authState = await apiHttpClient.get(
          HomeServerEndpoints.combine(serverUrl, HomeServerEndpoints.login));
      if (authState != null) {
        if (authState['userCtx']['name'] != null) {
          lastSessionRequest = DateTime.now();
          username = authState['userCtx']['name'];
          return true;
        }
      }
    } on HttpException catch (exp) {
      debugPrint(exp.toString());
    }

    lastSessionRequest = null;
    return false;
  }

  Future removeSessionIfExists() async {
    try {
      await httpClient.delete(
        HomeServerEndpoints.combine(serverUrl, HomeServerEndpoints.login),
      );
    } on FormatException catch (exp) {
      debugPrint(exp.message);
    } catch (exp) {
      debugPrint(exp.toString());
    }
    _messageClient?.close();
    _audioPlayerService?.dispose();
    username = null;
    password = null;
  }

  addListener(OnMessageCallback callback) {
    if (_messageClient == null) {
      debugPrint("messageClient is null");
    } else {
      _messageClient?.addListener(callback);
    }
  }

  removeListener(OnMessageCallback callback) {
    if (_messageClient == null) {
      debugPrint("messageClient is null");
    } else {
      _messageClient?.removeListener(callback);
    }
  }

  sendMsg(String message) {
    if (_messageClient == null) {
      debugPrint("messageClient is null");
    } else {
      _messageClient?.sendMsg(message);
    }
  }

  play(String audioFile) {
    final uri = HomeServerEndpoints.combine(serverUrl, audioFile);
    if (_audioPlayerService == null) {
      debugPrint("_audioPlayerService is null");
    } else {
      _audioPlayerService?.play(uri.toString());
    }
  }

  void addListnerPlayerState(OnPlayerStateCallback callback) {
    if (_audioPlayerService == null) {
      debugPrint("_audioPlayerService is null");
    } else {
      _audioPlayerService?.addListnerPlayerState(callback);
    }
  }

  void removeListnerPlayerState(OnPlayerStateCallback callback) {
    if (_audioPlayerService == null) {
      debugPrint("_audioPlayerService is null");
    } else {
      _audioPlayerService?.removeListnerPlayerState(callback);
    }
  }

  void pause() {
    if (_audioPlayerService == null) {
      debugPrint("_audioPlayerService is null");
    } else {
      _audioPlayerService?.pause();
    }
  }

  UserSettings getUserSettings() {
    return _userSettings;
  }

  Future<List<AdaptiveScaffoldDestination>> loadDestinations() async {
    final uri = HomeServerEndpoints.combine(
        serverUrl, HomeServerEndpoints.automation,
        path3: "pages");

    List<AdaptiveScaffoldDestination> resultList = [];

    try {
      var result = await apiHttpClient.get(uri);
      var autoDestinations = AutoDestinations.fromJson(result);
      for (var element in autoDestinations.autoDestinations) {
        var entry = AdaptiveScaffoldDestination(
            title: element.description,
            icon: Icons.data_array,
            name: element.name);
        resultList.add(entry);
      }
    } catch (exp) {
      debugPrint("loadDestinations $exp");
    }

    return resultList;
  }

  Future<AutomationPageConfig?> loadAutomationPageConfig(String pageName) async {
    final uri = HomeServerEndpoints.combine(
        serverUrl, HomeServerEndpoints.automation,
        path3: "page/$pageName");

    try {
      var result = await apiHttpClient.get(uri);
      var automationPageConfig = AutomationPageConfig.fromJson(result);
      return automationPageConfig;
    } catch (exp) {
      debugPrint("loadAutomationPageConfig " + exp.toString());
    }

    return null;
  }
}

// ignore: constant_identifier_names
enum HttpMethod { GET, POST, PUT, DELETE, HEAD }