import 'package:flutter/material.dart';
import 'bootstrap.dart';
import 'bootSteps/login_boot_step.dart';

class UiBootstrap extends StatelessWidget {
  final BootCompleted bootCompleted;

  const UiBootstrap(this.bootCompleted, {Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return BootStrap(
      const [
        LoginBootStep(),
      ],
      bootCompleted,
    );
  }
}