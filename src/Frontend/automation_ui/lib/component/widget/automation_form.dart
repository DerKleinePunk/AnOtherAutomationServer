import 'package:flutter/material.dart';

import '../../core/automation_panel_controller.dart';
import '../../core/extensions.dart';

class AutomationForm extends StatelessWidget {
  final AutomationPanelController panelController;
  final String pageName;

  const AutomationForm(this.panelController, this.pageName, {Key? key})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    panelController.loadPage(pageName);
    int count = renderMobileMode(context) ? 3 : 4;
    return GridView.count(
        crossAxisCount: count,
        childAspectRatio: 1.0,
        padding: const EdgeInsets.all(10.0),
        mainAxisSpacing: 10.0,
        crossAxisSpacing: 10.0,
        children: panelController.getPanels(pageName));
  }
}