part of 'login_bloc.dart';

@immutable
abstract class LoginState {}

class LoginInitial extends LoginState {}

class LoginRunning extends LoginState {}

class LoginFailed extends LoginState {
  final bool maybeCors;
  final String message;

  LoginFailed({this.maybeCors = false, this.message = ''});
}

class LoginOk extends LoginState {}