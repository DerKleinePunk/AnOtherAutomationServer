import 'package:flutter_neumorphic/flutter_neumorphic.dart';
import 'package:provider/provider.dart';

import '../../models/panel_value_model.dart';

enum SwitchPanelValue { on, off, waiting }

typedef GetStateCallback = SwitchPanelValue Function(String id);
typedef SetStateCallback = void Function(String id, bool value);

class Panels {
  static getSwitchPanel(String title, String id, GetStateCallback getcallback,
      SetStateCallback setCallback, bool isOn) {
    return SwitchPanel(title, id, getcallback, setCallback);
  }
}

class SwitchPanel extends StatefulWidget {
  final String _title;
  final String _id;
  final GetStateCallback _getcallback;
  final SetStateCallback _setcallback;

  const SwitchPanel(this._title, this._id, this._getcallback, this._setcallback,
      {Key? key})
      : super(key: key);

  @override
  SwitchPanelState createState() => SwitchPanelState();
}

class SwitchPanelState extends State<SwitchPanel> {
  @override
  Widget build(BuildContext context) {
    return Consumer<PanelValueMap>(builder: (context, value, child) {
      SwitchPanelValue switchValue = widget._getcallback(widget._id);
      bool isOn = true;
      if (switchValue == SwitchPanelValue.off) {
        isOn = false;
      }
      return InkWell(
        child: Neumorphic(
            style: NeumorphicStyle(
                shape: NeumorphicShape.concave,
                boxShape:
                    NeumorphicBoxShape.roundRect(BorderRadius.circular(12)),
                depth: !isOn ? 8 : -6,
                lightSource: LightSource.topLeft,
                shadowLightColorEmboss: const Color(0xFFEBF6FC),
                shadowDarkColorEmboss: const Color(0xFF0F9DEF)),
            child:
                Column(mainAxisAlignment: MainAxisAlignment.center, children: [
              NeumorphicText(widget._title,
                  style: const NeumorphicStyle(
                      depth: 0, //customize depth here
                      color: Colors.black),
                  textStyle: NeumorphicTextStyle(fontSize: 18)),
              Visibility(
                  visible: value.getValue(widget._id) == "WAIT",
                  child: const CircularProgressIndicator(color: Colors.blue))
              /*NeumorphicSwitch(
                      style: const NeumorphicSwitchStyle(
                          lightSource: LightSource.left),
                      value: isOn,
                      onChanged: (value) {
                        widget._setcallback(widget._id, value);
                        setState(() {});
                      },
                    ),*/
            ])),
        onTap: () {
          widget._setcallback(widget._id, !isOn);
        },
      );
    });
  }
}
