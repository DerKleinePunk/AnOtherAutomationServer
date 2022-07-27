import '../component/bootstrap/bootstrap_controller.dart';
import '../component/widget/login_form.dart';
import 'package:flutter/material.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

class Login extends StatelessWidget {
  final BootstrapController? controller;

  const Login({Key? key, this.controller}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    String title = UILocalizations.of(context)!.titleLoginScreen;
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
      ),
      body: LoginForm(controller: controller),
    );
  }
}