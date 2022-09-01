import 'package:audioplayers/audioplayers.dart';
import '../core/client_helper.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';
import '../component/widget/third_party/adaptive_scaffold.dart';
import '../component/widget/automation_form.dart';
import '../component/widget/settings_form.dart';
import '../core/automation_panel_controller.dart';
import 'package:flutter_neumorphic/flutter_neumorphic.dart';

class Dashboard extends StatefulWidget {
  const Dashboard({Key? key}) : super(key: key);

  @override
  DashboardState createState() => DashboardState();
}

class DashboardState extends State<Dashboard> {
  int _pageIndex = 0;
  final List<MessageData> _msglist = [];

  final TextEditingController _msgtext = TextEditingController();
  bool _isPlaying = false;
  late AutomationPanelController _panelController;
  List<AdaptiveScaffoldDestination> _listPages =
      List<AdaptiveScaffoldDestination>.empty();
  List<AdaptiveScaffoldDestination> _webPages =
      List<AdaptiveScaffoldDestination>.empty();
  @override
  void initState() {
    _msgtext.text = "";
    CoreClientHelper.getClient().addListener(_onWebSocketMessage);
    CoreClientHelper.getClient().addListnerPlayerState(_onPLayerState);
    _panelController = AutomationPanelController();
    _panelController.init(context);
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    if (_listPages.isNotEmpty) {
      return _mainPage(_webPages);
    }
    return FutureBuilder<List<AdaptiveScaffoldDestination>>(
      future: CoreClientHelper.getClient().loadDestinations(),
      builder: (BuildContext context,
          AsyncSnapshot<List<AdaptiveScaffoldDestination>> snapshot) {
        if (snapshot.connectionState == ConnectionState.done &&
            snapshot.hasData) {
          _webPages = snapshot.data!;
          return _mainPage(snapshot.data);
        } else if (snapshot.connectionState == ConnectionState.done &&
            snapshot.hasError) {
          return Center(
              child: Column(children: <Widget>[
            const Icon(
              Icons.error,
              color: Colors.red,
              size: 100,
            ),
            Padding(
              padding: const EdgeInsets.only(top: 30),
              child: Text(
                'Error: ${snapshot.error}',
                style: const TextStyle(fontSize: 20),
              ),
            )
          ]));
        }
        return Center(
            child: Column(children: const <Widget>[
          SizedBox(
            width: 80,
            height: 80,
            child: CircularProgressIndicator(
              color: Colors.blue,
            ),
            
          ),
          Padding(
            padding: EdgeInsets.only(top: 30),
            child: Text(
              'Retrieving Data',
              style: TextStyle(fontSize: 20),
            ),
          )
        ]));
      },
    );
  }

  Widget _mainPage(List<AdaptiveScaffoldDestination>? pages) {
    String title = _getPageTitel(_pageIndex);
    _mergePages(pages!);
    return AdaptiveScaffold(
        title: Text(title),
        actions: [
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: IconButton(
                icon: const Icon(Icons.logout),
                onPressed: () async {
                  await CoreClientHelper.getClient().removeSessionIfExists();
                  await CoreClientHelper.clearAuthStorage();
                  if (!mounted) return;
                  await Navigator.of(context).pushReplacementNamed('dashboard');
                }),
          ),
        ],
        currentIndex: _pageIndex,
        destinations: _listPages,
        body: _pageAtIndex(_pageIndex),
        onNavigationIndexChange: (newIndex) {
          setState(() {
            _pageIndex = newIndex;
          });
        },
        floatingActionButton: null
        //_hasFloatingActionButton ? _buildFab(context) : null,
        );
  }

  Widget _pageAtIndex(int index) {
    if (index == 0) {
      return dashboardBody();
    }

    if (index == 1) {
      //return const Settings();
      return const SettingsForm();
    }

    if (index < _listPages.length) {
      return AutomationForm(_panelController, _listPages[index].name);
    }
    return const Center(child: Text('Error'));
  }

  /*@override
  Widget build(BuildContext context) {
    final mobileMode = widget.renderMobileMode(context);
    return HomeServerBootstrap(() {
      String title = HomeServerLocalizations.of(context)!.titleDashboard;

      return Scaffold(
        appBar: AppBar(
          title: Text(title),
          actions: [
            IconButton(
                icon: const Icon(Icons.logout),
                onPressed: () async {
                  await CoreClientHelper.getClient().removeSessionIfExists();
                  await CoreClientHelper.clearAuthStorage();
                  await Navigator.of(context).pushReplacementNamed('dashboard');
                }),
          ],
        ),
        body: dashboardBody(),
        floatingActionButton: mobileMode
            ? FloatingActionButton(
                onPressed: () async {
                  /*final r = await showDatabaseAddDialog(context);
                  if (r != null) {
                    setState(() {
                      totalDbs?.add(r);
                    });
                  }*/
                },
                child: const Icon(Icons.add),
              )
            : null,
      );
    });
  }*/

  Widget dashboardBody() {
    return Stack(
      children: [
        Positioned(
            top: 0,
            bottom: 70,
            left: 0,
            right: 0,
            child: Container(
                padding: const EdgeInsets.all(15),
                child: SingleChildScrollView(
                    child: Column(
                  children: [
                    Column(
                      children: _msglist.map((onemsg) {
                        return Container(
                            margin: EdgeInsets.only(
                              //if is my message, then it has margin 40 at left
                              left: onemsg.isme ? 40 : 0,
                              right:
                                  onemsg.isme ? 0 : 40, //else margin at right
                            ),
                            child: Card(
                                color: onemsg.isme
                                    ? Colors.blue[100]
                                    : Colors.red[100],
                                //if its my message then, blue background else red background
                                child: Container(
                                  width: double.infinity,
                                  padding: const EdgeInsets.all(15),
                                  child: Column(
                                    crossAxisAlignment:
                                        CrossAxisAlignment.start,
                                    children: [
                                      Text(onemsg.isme
                                          ? "ID: ME"
                                          : "ID: ${onemsg.userid}"),
                                      Container(
                                        margin: const EdgeInsets.only(
                                            top: 10, bottom: 10),
                                        child: Text(
                                            "Message: ${onemsg.msgtext}", 
                                            style:
                                                const TextStyle(fontSize: 17)),
                                      ),
                                    ],
                                  ),
                                )));
                      }).toList(),
                    )
                  ],
                )))),
        Positioned(
          //position text field at bottom of screen

          bottom: 0, left: 0, right: 0,
          child: Container(
              color: Colors.black12,
              height: 70,
              child: Row(
                children: [
                  Expanded(
                      child: Container(
                    margin: const EdgeInsets.all(10),
                    child: TextField(
                      controller: _msgtext,
                      decoration:
                          const InputDecoration(hintText: "Enter your Message"),
                    ),
                  )),
                  Container(
                      margin: const EdgeInsets.all(10),
                      child: NeumorphicButton(
                          child: const Icon(Icons.send),
                          onPressed: () {
                            if (_msgtext.text != "") {
                              sendmsg(
                                  _msgtext.text); //send message with webspcket
                              _msgtext.text = "";
                            } else {
                              debugPrint("No Message no Send");
                            }
                          })),
                  Container(
                      margin: const EdgeInsets.all(10),
                      child: NeumorphicButton(
                          child: Icon(!_isPlaying
                              ? Icons.play_circle
                              : Icons.stop_circle),
                          onPressed: () {
                            !_isPlaying ? _playAudio() : _pauseAudio();
                          }))
                ],
              )),
        )
      ],
    );
  }

  _onWebSocketMessage(String wath, String message) {
    debugPrint("Dashboard Websocket " + wath + " " + message);
    if (wath == "Text") {
      _msglist.add(MessageData(message, "", false));
      setState(() {});
    }
  }

  @override
  void dispose() {
    CoreClientHelper.getClient().removeListener(_onWebSocketMessage);
    _panelController.dispose();
    super.dispose();
  }

  void sendmsg(String text) {
    CoreClientHelper.getClient().sendMsg(text);
    _msglist.add(MessageData(text, "", true));
  }

  void _playAudio() {
    CoreClientHelper.getClient().play("resources/musik.mp3");
  }

  void _pauseAudio() {
    CoreClientHelper.getClient().pause();
  }

  void _onPLayerState(PlayerState state) {
    if (state == PlayerState.STOPPED) {
      _isPlaying = false;
      setState(() {});
    } else if (state == PlayerState.PLAYING) {
      _isPlaying = true;
      setState(() {});
    }
  }

  String _getPageTitel(int pageIndex) {
    switch (pageIndex) {
      case 0:
        return UILocalizations.of(context)!.titleDashboard0;
      case 1:
        return UILocalizations.of(context)!.titleDashboard1;
    }

    if (pageIndex < _listPages.length) {
      return _listPages[pageIndex].title;
    }

    return "Missing Tile for Index " + pageIndex.toString();
  }

  //TODO Later us this to Update Screen when Pages Updatet in DB (Message Inform Us)
  void _loadDestinations() async {
    CoreClientHelper.getClient().loadDestinations().then((resultWeb) => () {
          _mergePages(resultWeb);
          //setState(() {});
        });
  }

  void _mergePages(List<AdaptiveScaffoldDestination> pagesWeb) {
    var newPagesList = [
      AdaptiveScaffoldDestination(
          title: _getPageTitel(0), icon: Icons.home, name: "internal"),
      AdaptiveScaffoldDestination(
          title: _getPageTitel(1), icon: Icons.settings, name: "internal"),
    ];

    debugPrint("Add ${pagesWeb.length} pages to dashboard");

    for (var entry in pagesWeb) {
      newPagesList.add(entry);
    }
    _listPages = newPagesList;
  }
} // Class

class MessageData {
  //message data model
  String msgtext, userid;
  bool isme;
  MessageData(this.msgtext, this.userid, this.isme);
}