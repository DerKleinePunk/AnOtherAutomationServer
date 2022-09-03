import 'package:flutter/foundation.dart';

class PanelValueMap extends ChangeNotifier {
  final Map<String, PanelValueEntry> _panelValueMap = {};

  void addEntry(String varName, PanelValueEntry entry) {
    _panelValueMap[varName] = entry;
    notifyListeners();
  }

  void updateEntry(String varName, String value) {
    if(_panelValueMap.containsKey(varName)) {
      _panelValueMap[varName]!.value = value;
      notifyListeners();
    }
  }

  PanelValueEntry? getEntry(String varName) {
    return _panelValueMap[varName];
  }

  String getValue(String varName) {
    return _panelValueMap[varName]!.value;
  }
}

class PanelValueEntry {
  String value;
  final String valueTypeText;

  PanelValueEntry(this.value, this.valueTypeText);

}