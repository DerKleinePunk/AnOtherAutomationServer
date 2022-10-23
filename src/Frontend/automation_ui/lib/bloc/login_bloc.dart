import 'dart:async';
import 'package:bloc/bloc.dart';
import '../core/client_helper.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter/foundation.dart';
import '../server/http/http_exceptions.dart';

part 'login_event.dart';
part 'login_state.dart';

class LoginBloc extends Bloc<LoginEvent, LoginState> {
  LoginBloc() : super(LoginInitial()) {
    on<LoginEvent>(mapEventToState);
  }

  Future<void> mapEventToState(
    LoginEvent event,
    Emitter<LoginState> emit,
  ) async {
    if (event is LoginRequest) {
      emit(LoginRunning());
      var state = await tryLogin(event);
      emit(state);
    }
  }

  void ensureClientIsReady(LoginRequest request) {
    if (CoreClientHelper.initNeeded()) {
      CoreClientHelper.initClient(request.serverUrl,
          username: request.username, password: request.password);
    }
    final pillowClient = CoreClientHelper.getClient();
    if (pillowClient.serverUrl != request.serverUrl) {
      CoreClientHelper.initClient(request.serverUrl,
          username: request.username, password: request.password);
    }
  }

  Future<LoginState> tryLogin(LoginRequest request) async {
    try {
      final prefs = await SharedPreferences.getInstance();
      if (request.storeAuth) {
        if (kIsWeb) {
          await prefs.setString('username', request.username);
          debugPrint("prefs save username ${request.username}");
        } else {
          await prefs.setString('username', request.username);
          await prefs.setString('password', request.password);
        }
        await prefs.setString('serverUrl', request.serverUrl);
        await prefs.setBool('storeAuth', true);
      } else {
        await prefs.remove('username');
        await prefs.remove('password');
        await prefs.remove('serverUrl');
        await prefs.remove('storeAuth');
      }
      await prefs.setBool('sendbasic', request.addBasicAuth);
      ensureClientIsReady(request);
      final loginResult = await CoreClientHelper.getClient()
          .authenticate(request.username, request.password);
      if (loginResult) {
        return LoginOk();
      } else {
        return LoginFailed();
      }
    } on HttpException catch (exp) {
      return LoginFailed(message: exp.toString());
    } on FormatException catch (exp) {
      return LoginFailed(message: exp.message);
    } on Exception catch (exp) {
      return LoginFailed(message: exp.toString());
    }
  }
}
