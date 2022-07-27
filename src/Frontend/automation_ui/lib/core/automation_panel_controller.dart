import 'package:flutter/material.dart';
import 'client_helper.dart';
import '../server/data/auto_destination.dart';
import '../component/widget/panels.dart';

class AutomationPanelController {
  final Map<String, List<Widget>> _panels = <String, List<Widget>>{};
  final Map<String, bool> _switchMap = <String, bool>{};

  AutomationPanelController();

  void init() {
    /*for (int i = 0; i < 16; i++) {
      _switchMap["panel" + i.toString()] = false;
      _panels.add(Panels.getSwitchPanel("Panel " + i.toString(),
          "panel" + i.toString(), switchPanelGet, switchPanelSet, false));
    }*/
  }

  List<Widget> getPanels(String pageName) {
    if (_panels[pageName] == null || _panels[pageName]!.isEmpty) {
      var list = [const CircularProgressIndicator(color: Colors.blue)];
      return list;
    }
    return _panels[pageName]!;
  }

  bool switchPanelGet(String id) {
    return _switchMap[id]!;
  }

  void switchPanelSet(String id, bool value) {
    _switchMap[id] = value;
  }

  void loadPage(String pageName) async {
    if (_panels[pageName] != null || _panels[pageName]!.isNotEmpty) {
      //TODO Know if Pages ar Updated;
      return;
    }
    CoreClientHelper.getClient().loadAutomationPageConfig(pageName).then(
        (value) {
      try {
        if (value == null) {
          debugPrint("new Page Config get failed");
          return;
        }

        debugPrint("new Page Config get " + value.name);
        _panels[value.name] = List.empty(growable: true);
        for (AutomationElement entry in value.elements) {
          if (entry.typeName == "ONOFFBUTTON") {
            var panel = Panels.getSwitchPanel(
                entry.id, entry.id, switchPanelGet, switchPanelSet, false);
            _panels[value.name]!.add(panel);
          }
        }
      } catch (exp) {
        debugPrint("loadPage " + exp.toString());
      }
    }, onError: (error, stack) {
      debugPrint("loadPage Error" + error.toString());
    });
  }

  void dispose() {
    _panels.clear();
    _switchMap.clear();
  }
}