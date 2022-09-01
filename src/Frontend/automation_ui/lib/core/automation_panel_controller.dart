import 'package:flutter/material.dart';
import 'client_helper.dart';
import '../server/data/auto_destination.dart';
import '../component/widget/panels.dart';
import 'dart:convert';

class ValueChangeNotification extends Notification {
  final String id;
  final String value;

  const ValueChangeNotification({required this.id, required this.value});
}

class AutomationPanelController {
  final Map<String, List<Widget>> _panels = <String, List<Widget>>{};
  final Map<String, String> _valueMap = <String, String>{};
  BuildContext? _context;

  AutomationPanelController();

  void init(BuildContext context) {
    /*for (int i = 0; i < 16; i++) {
      _valueMap["panel" + i.toString()] = false;
      _panels.add(Panels.getSwitchPanel("Panel " + i.toString(),
          "panel" + i.toString(), switchPanelGet, switchPanelSet, false));
    }*/
    _context = context;
    CoreClientHelper.getClient().addListener(_onWebSocketMessage);
  }

  void _onWebSocketMessage(String wath, String message) {
    debugPrint("AudioPlayerService Websocket $wath $message");
    if (wath == "data") {
      var jsondata = jsonDecode(message);
      if (jsondata["Event"] == "ValueChanged") {
        debugPrint("ValueChanged $message");
        const ValueChangeNotification(id: "Foo", value: "ON").dispatch(_context);
      }
    }
  }

  List<Widget> getPanels(String pageName) {
    if (_panels[pageName] == null || _panels[pageName]!.isEmpty) {
      var list = [const CircularProgressIndicator(color: Colors.blue)];
      return list;
    }
    return _panels[pageName]!;
  }

  SwitchPanelValue switchPanelGet(String id) {
    if(_valueMap[id]! == "ON") {
      return SwitchPanelValue.on;
    } else if(_valueMap[id]! == "OFF"){
      return SwitchPanelValue.off;
    }
    return SwitchPanelValue.waiting;
  }

  void switchPanelSet(String id, bool value) {
    if(value && _valueMap[id] != "WAIT")
    {
      CoreClientHelper.getClient().setValueForId(id, "ON");
      _valueMap[id] = "WAIT";
    }
    else if(_valueMap[id] != "WAIT")
    {
      CoreClientHelper.getClient().setValueForId(id, "OFF");
      _valueMap[id] = "WAIT";
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
          _valueMap[entry.id] = entry.value;
          if (entry.typeName == "ONOFFBUTTON") {
            var panel = Panels.getSwitchPanel(
                entry.description, entry.id, switchPanelGet, switchPanelSet, false);
            _panels[value.name]!.add(panel);
          }
        }
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
    _valueMap.clear();
  }
}