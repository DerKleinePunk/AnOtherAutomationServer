import 'package:flutter/widgets.dart';

extension ViewMode on Widget {
  bool renderMobileMode(BuildContext context) {
    return MediaQuery.of(context).size.width < 600.0;
  }

  bool isLargeScreen(BuildContext context) {
    return MediaQuery.of(context).size.width > 960.0;
  }

  bool isMediumScreen(BuildContext context) {
    return MediaQuery.of(context).size.width >= 600.0;
  }
}

bool renderMobileMode(BuildContext context) {
  return MediaQuery.of(context).size.width < 600.0;
}