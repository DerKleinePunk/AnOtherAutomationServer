import 'package:flutter/material.dart';
import 'bootstrap_step.dart';
import 'bootstrap_state.dart';

typedef BootCompleted = Widget Function();

class BootStrap extends StatefulWidget {
  final List<BootstrapStep> steps;
  final int currentIndex;
  final BootCompleted bootCompleted;

  const BootStrap(
    this.steps,
    this.bootCompleted, {
    Key? key,
    this.currentIndex = 0,
  }) : super(key: key);

  @override
  BootStrapState createState() => BootStrapState();
}





