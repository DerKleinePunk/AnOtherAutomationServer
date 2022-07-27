// Copyright 2020, the Flutter project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

import '../../../core/extensions.dart';
import 'package:flutter_neumorphic/flutter_neumorphic.dart';

/// See bottomNavigationBarItem or NavigationRailDestination
class AdaptiveScaffoldDestination {
  final String title;
  final IconData icon;
  final String name;

  const AdaptiveScaffoldDestination({
    required this.title,
    required this.icon,
    required this.name
  });
}

/// A widget that adapts to the current display size, displaying a [Drawer],
/// [NavigationRail], or [BottomNavigationBar]. Navigation destinations are
/// defined in the [destinations] parameter.
class AdaptiveScaffold extends StatefulWidget {
  final Widget? title;
  final List<Widget> actions;
  final Widget? body;
  final int currentIndex;
  final List<AdaptiveScaffoldDestination> destinations;
  final ValueChanged<int>? onNavigationIndexChange;
  final FloatingActionButton? floatingActionButton;

  const AdaptiveScaffold({
    this.title,
    this.body,
    this.actions = const [],
    required this.currentIndex,
    required this.destinations,
    this.onNavigationIndexChange,
    this.floatingActionButton,
    Key? key,
  }) : super(key: key);

  @override
  AdaptiveScaffoldState createState() => AdaptiveScaffoldState();
}

class AdaptiveScaffoldState extends State<AdaptiveScaffold> {
  @override
  Widget build(BuildContext context) {
    // Show a Drawer
    if (widget.isLargeScreen(context)) {
      return Row(
        children: [
          Drawer(
            backgroundColor: NeumorphicTheme.baseColor(context),
            child: Column(
              children: [
                DrawerHeader(
                  child: Center(
                    child: widget.title,
                  ),
                ),
                for (var d in widget.destinations)
                  ListTile(
                    leading: Icon(d.icon),
                    title: Text(d.title),
                    selected:
                        widget.destinations.indexOf(d) == widget.currentIndex,
                    onTap: () => _destinationTapped(d),
                  ),
              ],
            ),
          ),
          VerticalDivider(
            width: 1,
            thickness: 1,
            color: Colors.grey[300],
          ),
          Expanded(
            child: Scaffold(
              appBar: NeumorphicAppBar(
                title: widget.title,
                actions: widget.actions,
                color: NeumorphicTheme.baseColor(context),
              ),
              body: widget.body,
              floatingActionButton: widget.floatingActionButton,
            ),
          ),
        ],
      );
    }

    // Show a navigation rail
    if (widget.isMediumScreen(context)) {
      return Scaffold(
        appBar: NeumorphicAppBar(
            title: widget.title,
            actions: widget.actions,
            color: NeumorphicTheme.baseColor(context)),
        body: Row(
          children: [
            NavigationRail(
              leading: widget.floatingActionButton,
              destinations: [
                ...widget.destinations.map(
                  (d) => NavigationRailDestination(
                    icon: Icon(d.icon),
                    label: Text(d.title),
                  ),
                ),
              ],
              selectedIndex: widget.currentIndex,
              onDestinationSelected: widget.onNavigationIndexChange ?? (_) {},
              backgroundColor: NeumorphicTheme.baseColor(context),
            ),
            VerticalDivider(
              width: 1,
              thickness: 1,
              color: Colors.grey[300],
            ),
            Expanded(
              child: widget.body!,
            ),
          ],
        ),
      );
    }

    // Show a bottom app bar
    return Scaffold(
      body: widget.body,
      appBar: NeumorphicAppBar(
          title: widget.title,
          actions: widget.actions,
          color: NeumorphicTheme.baseColor(context)),
      bottomNavigationBar: BottomNavigationBar(
        backgroundColor: NeumorphicTheme.baseColor(context),
        items: [
          ...widget.destinations.map(
            (d) => BottomNavigationBarItem(
              icon: Icon(d.icon),
              label: d.title,
            ),
          ),
        ],
        currentIndex: widget.currentIndex,
        onTap: widget.onNavigationIndexChange,
      ),
      floatingActionButton: widget.floatingActionButton,
    );
  }

  void _destinationTapped(AdaptiveScaffoldDestination destination) {
    var idx = widget.destinations.indexOf(destination);
    if (idx != widget.currentIndex) {
      widget.onNavigationIndexChange!(idx);
    }
  }
}