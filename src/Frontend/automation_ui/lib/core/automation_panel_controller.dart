import 'package:flutter/material.dart';
import '../models/panel_value_model.dart';
import '../models/panel_page_changed.dart';
import 'client_helper.dart';
import '../server/data/auto_destination.dart';
import '../component/widget/panels.dart';
import 'dart:convert';
import 'package:provider/provider.dart';

class AutomationPanelController {
  final Map<String, List<Widget>> _panels = <String, List<Widget>>{};
  BuildContext? _context;

  AutomationPanelController();

  void init(BuildContext context) {
    _context = context;
    CoreClientHelper.getClient().addListener(_onWebSocketMessage);
  }

  void _onWebSocketMessage(String wath, String message) {
    debugPrint("AutomationPanelController Websocket $wath $message");
    if (wath == "data") {
      var jsondata = jsonDecode(message);
      if (jsondata["Event"] == "ValueChanged") {
        debugPrint("ValueChanged $message");
        var valueMapNotifier = Provider.of<PanelValueMap>(_context!, listen: false);
        valueMapNotifier.updateEntry(jsondata["name"], jsondata["value"]);
      }
    }
  }

  List<Widget> getPanels(String pageName) {
    if (_panels[pageName] == null || _panels[pageName]!.isEmpty) {
      var list = [
          const CircularProgressIndicator(color: Colors.blue)
      ];
      return list;
    }
    return _panels[pageName]!;
  }

  SwitchPanelValue switchPanelGet(String id) {
    var valueMapNotifier = Provider.of<PanelValueMap>(_context!, listen: false);
    var value = valueMapNotifier.getValue(id);
    if (value == "ON") {
      return SwitchPanelValue.on;
    } else if (value == "OFF") {
      return SwitchPanelValue.off;
    }
    return SwitchPanelValue.waiting;
  }

  void switchPanelSet(String id, bool value) {
    var valueMapNotifier = Provider.of<PanelValueMap>(_context!, listen: false);
    var oldValue = valueMapNotifier.getValue(id);
    if (value && oldValue != "WAIT") {
      CoreClientHelper.getClient().setValueForId(id, "ON");
      valueMapNotifier.updateEntry(id, "WAIT");
    } else if (oldValue != "WAIT") {
      CoreClientHelper.getClient().setValueForId(id, "OFF");
      valueMapNotifier.updateEntry(id, "WAIT");
    }
  }

  void loadPage(String pageName) async {
    if (_panels[pageName] != null && _panels[pageName]!.isNotEmpty) {
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

        debugPrint("new Page Config get ${value.name}");
        _panels[value.name] = List.empty(growable: true); 
        for (AutomationElement entry in value.elements) {
          if (entry.typeName == "ONOFFBUTTON") {
            var panel = Panels.getSwitchPanel(entry.description, entry.id,
                switchPanelGet, switchPanelSet, false);
            _panels[value.name]!.add(panel);
            var valueMapNotifier = Provider.of<PanelValueMap>(_context!, listen: false);
            valueMapNotifier.addEntry(entry.id, PanelValueEntry(entry.value, ""));
          }
        }
        
        var panelChangedNotifier = Provider.of<PanelChangedMap>(_context!, listen: false);
        panelChangedNotifier.addEntry(pageName, "LOADED");
      } catch (exp) {
        debugPrint("loadPage $exp");
      }
    }, onError: (error, stack) {
      debugPrint("loadPage Error $error");
    });
  }

  void dispose() {
    CoreClientHelper.getClient().removeListener(_onWebSocketMessage);
    _panels.clear();
  }
}
