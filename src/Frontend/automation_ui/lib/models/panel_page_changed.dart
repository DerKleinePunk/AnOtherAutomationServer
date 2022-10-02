import 'package:flutter/foundation.dart';

class PanelChangedMap extends ChangeNotifier {
  final Map<String, String> _panelStateMap = {};

  void addEntry(String name, String state) {
    _panelStateMap[name] = state;
    notifyListeners();
  }

  void updateEntry(String name, String state) {
    if(_panelStateMap.containsKey(name)) {
      _panelStateMap[name] = state;
      notifyListeners();
    }
  }

  String? getValue(String name) {
    return _panelStateMap[name];
  }

}