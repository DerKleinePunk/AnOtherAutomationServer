import '../../main.dart';
import 'bootstrap_controller.dart';
import 'package:flutter/material.dart';
import 'bootstrap.dart';

class BootStrapState extends State<BootStrap> implements BootstrapController {
  late int currentIndex;
  bool bootRunning = true;

  @override
  void initState() {
    super.initState();
    currentIndex = widget.currentIndex;
  }

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<Widget>(
        stream: work(),
        builder: (c, snap) {
          if (snap.hasData) {
            return snap.data!;
          }
          return const Scaffold(
            body: Center(
              child: CircularProgressIndicator(),
            ),
          );
        });
  }

  Stream<Widget> work() async* {
    while (bootRunning &&
        !(await widget.steps[currentIndex].stepRequired(preferences))) {
      if (currentIndex + 1 < widget.steps.length) {
        currentIndex++;
      } else {
        bootRunning = false;
      }
    }
    if (bootRunning) {
      yield widget.steps[currentIndex].buildStep(this);
    } else {
      yield widget.bootCompleted();
    }
  }

  @override
  void procced() {
    if (currentIndex + 1 < widget.steps.length) {
      setState(() {
        currentIndex++;
      });
    } else {
      setState(() {
        bootRunning = false;
      });
    }
  }

  @override
  void stepback() {
    if (currentIndex > 0) {
      setState(() {
        currentIndex--;
      });
    } else {
      setState(() {
        bootRunning = false;
      });
    }
  }
}