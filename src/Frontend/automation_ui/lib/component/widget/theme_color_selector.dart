import 'package:flutter_neumorphic/flutter_neumorphic.dart';
import '../../core/client_helper.dart';
import 'color_selector.dart';

class ThemeColorSelector extends StatefulWidget {
  final BuildContext customContext;

  const ThemeColorSelector({required this.customContext, Key? key})
      : super(key: key);

  @override
  ThemeColorSelectorState createState() => ThemeColorSelectorState();
}

class ThemeColorSelectorState extends State<ThemeColorSelector> {
  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(4),
      color: Colors.black,
      child: ColorSelector(
        color: CoreClientHelper.getClient().getUserSettings().getBaseColor(),
        onColorChanged: (color) {
          //TODO when this is Called the State is Change with wrong Color
          //ColorSelector show wrong Color do not work right

          CoreClientHelper.getClient().getUserSettings().setBaseColor(color);
          setState(() {});
          /*NeumorphicTheme.update(widget.customContext,
              (current) => current!.copyWith(baseColor: color));*/
        },
        onDialogClosed: _onDialogClosed,
      ),
    );
  }

  void _onDialogClosed() {
    NeumorphicTheme.update(
        widget.customContext,
        (current) => current!.copyWith(
            baseColor: CoreClientHelper.getClient()
                .getUserSettings()
                .getBaseColor()));
  }
}