import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:enum_to_string/enum_to_string.dart';

import '../../models/panel_value_model.dart';

enum SwitchPanelValue { on, off, waiting }

typedef GetStateCallback = SwitchPanelValue Function(String id);
typedef SetStateCallback = void Function(String idSet, String idGet, bool value);

class Panels {
  static getSwitchPanel(String title, String id, String setParameter, String valueParameter, GetStateCallback getcallback,
      SetStateCallback setCallback) {
    return SwitchPanel(title, getcallback, setCallback, setParameter, valueParameter);
  }
}

class SwitchPanel extends StatelessWidget {
  final String _title;
  final GetStateCallback _getcallback;
  final SetStateCallback _setcallback;
  final String _setParameter;
  final String _valueParameter;

  const SwitchPanel(this._title, this._getcallback, this._setcallback, this._setParameter, this._valueParameter,
      {Key? key})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Consumer<PanelValueMap>(builder: (context, value, child) {
      SwitchPanelValue switchValue = _getcallback(_valueParameter);
      bool isOn = true;
      if (switchValue == SwitchPanelValue.off) {
        isOn = false;
      }
      return InkWell(
        //Todo find new way
        /*child: Neumorphic(
            style: NeumorphicStyle(
                shape: NeumorphicShape.concave,
                boxShape:
                    NeumorphicBoxShape.roundRect(BorderRadius.circular(12)),
                depth: !isOn ? 8 : 0,
                lightSource: LightSource.topLeft,
                shadowLightColorEmboss: const Color(0xFFEBF6FC),
                shadowDarkColorEmboss: const Color(0xFF0F9DEF)),
            child:
                Column(mainAxisAlignment: MainAxisAlignment.center, children: [
              NeumorphicText("$_title\n${EnumToString.convertToString(switchValue)}",
                  style: NeumorphicStyle(
                      depth: 0, //customize depth here
                      color: isOn ? Colors.amber : Colors.white),
                  textStyle: NeumorphicTextStyle(fontSize: 18)),
              Visibility(
                  visible: value.getValue(_valueParameter) == "WAIT",
                  child: const CircularProgressIndicator(color: Colors.blue))
            ])),*/
        onTap: () {
          _setcallback(_setParameter, _valueParameter, !isOn);
        },
      );
    });
  }
}

