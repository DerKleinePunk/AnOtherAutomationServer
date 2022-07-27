import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'bootstrap_controller.dart';

abstract class BootstrapStep {
  const BootstrapStep();
  Future<bool> stepRequired(SharedPreferences prefs);
  Widget buildStep(BootstrapController controller);
}