import '../core/router.dart';
import 'package:flutter/material.dart';

class CorsHelp extends StatelessWidget {
  const CorsHelp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    final route = ModalRoute.of(context);
    final firstRoute = route?.isFirst ?? true;
    final RoutingData? args = route?.settings.arguments as RoutingData;
    if (args != null) {
      debugPrint(args.fullRoute);
    }

    List<Widget>? actions = [];
    if (firstRoute) {
      actions.add(IconButton(
          icon: const Icon(Icons.login),
          onPressed: () {
            Navigator.of(context).pushReplacementNamed('dashboard');
          }));
    }

    return Scaffold(
      appBar: AppBar(
        title: const Text('How to setup CORS'),
        actions: actions,
      ),
      body: ListView(
        padding: const EdgeInsets.all(20),
        children: const [
          ListTile(
            title: Text('!NOTE!'),
            subtitle: Text(
                'CORS is needed if Homeapp is running on a different domain as Automation Server. If you use the same no changes are required'),
          ),
          ListTile(
            title: Text('1. Enable CORS'),
            subtitle: Text(
                'Login to Automation Server as an admin user and select the Config menu. Within the menu select CORS. Click the enable button'),
          ),
          ListTile(
            title: Text('2. Set Origin Domains'),
            subtitle: Text(
                'Please add the Homeapp domain to the white list. A value of * will NOT work!'),
          ),
        ],
      ),
    );
  }
}