import 'dart:convert';

import 'package:audioplayers/audioplayers.dart';
import '../server/client.dart';
import 'package:flutter/foundation.dart';

typedef OnPlayerStateCallback = void Function(PlayerState state);

class AudioPlayerService {
  final ServerClient _serverClient;
  late AudioPlayer _audioPlayer;
  late AudioPlayer _audioPlayerNotification;
  int _maxduration = 0;
  int _currentDuration = 0;
  int _maxdurationNotification = 0;
  int _currentDurationNotification = 0;
  final ObserverList<OnPlayerStateCallback> _listenersPlayer =
      ObserverList<OnPlayerStateCallback>();
  final ObserverList<OnPlayerStateCallback> _listenersPlayerNotification =
      ObserverList<OnPlayerStateCallback>();

  AudioPlayerService(this._serverClient);

  init() {
    AudioLogger.logLevel = AudioLogLevel.info;

    _serverClient.addListenerMessage(_onWebSocketMessage);
    _audioPlayer = AudioPlayer(playerId: 'mneHomeAppPLayer');
    _audioPlayerNotification = AudioPlayer(playerId: 'mneHomeAppPLayerNotification');
    _audioPlayer.setReleaseMode(ReleaseMode.stop); // set release mode so that it never

    _audioPlayer.onDurationChanged.listen((Duration d) {
      _maxduration = d.inMilliseconds;
      debugPrint("_maxduration $_maxduration");
    });

    _audioPlayer.onPositionChanged.listen((Duration d) {
      _currentDuration = d.inMilliseconds;
      _updateDuration();
    });

    _audioPlayerNotification.onPositionChanged.listen((Duration d) {
      _currentDurationNotification = d.inMilliseconds;
      _updateDurationNotification();
    });

    _audioPlayer.onPlayerStateChanged.listen((PlayerState s) {
      debugPrint('Current player state: $s');
      switch (s) {
        case PlayerState.stopped:
          _audioPlayerStopped();
          break;
        case PlayerState.playing:
          _audioPlayerPlaying();
          break;
        case PlayerState.paused:
          _audioPlayerPaused();
          break;
        case PlayerState.completed:
          // TODO: Handle this case.
          break;
        case PlayerState.disposed:
          // TODO: Handle this case.
          break;
      }
    });

    _audioPlayerNotification.onPlayerStateChanged.listen((PlayerState s) {
      debugPrint('Current player state: $s');
      switch (s) {
        case PlayerState.stopped:
          _audioPlayerNotificationStopped();
          break;
        case PlayerState.playing:
          // TODO: Handle this case.
          break;
        case PlayerState.paused:
          _audioPlayerNotificationPaused();
          break;
        case PlayerState.completed:
          // TODO: Handle this case.
          break;
        case PlayerState.disposed:
          // TODO: Handle this case.
          break;
      }
    });

    _audioPlayer.onPlayerComplete.listen((event) {
      debugPrint("PlayerCompletion");
    });

    _audioPlayer.onLog.listen((msg) {
      debugPrint('audioPlayer Log : $msg');
    });
 
    _audioPlayerNotification.onLog.listen((msg) {
      debugPrint('audioPlayerNotification log : $msg');
    });
  }

  void _onWebSocketMessage(String wath, String message) {
    debugPrint("AudioPlayerService Websocket $wath $message");
    if (wath == "data") {
      var jsondata = jsonDecode(message);
      if (jsondata["Event"] == "Action" && jsondata["what"] == "Notification") {
        _playNotification();
      }
    }
  }

  void _playNotification() async {
    debugPrint("_playNotification");
    
    await _audioPlayerNotification.play(UrlSource("http://localhost:8000/rainforest-ambient.mp3"));
    
    debugPrint("playing sound notification");
  }

  void play(String audioFile) async {
    try {
      await _audioPlayer.play(UrlSource(audioFile));
    } on AudioPlayerException catch (exp) {
      debugPrint(exp.toString());
      return;
    }
    
    _maxduration = 0;
    _audioPlayer.setVolume(1);
    await _audioPlayer.resume();
  }

  void dispose() {
    _serverClient.removeListenerMessage(_onWebSocketMessage);
    //_audioPlayer.stop();
    _audioPlayer.dispose();
    //_audioPlayerNotification.stop();
    _audioPlayerNotification.dispose();
  }

  void _updateDuration() async {
    if (_maxduration == 0) {
      var length = await _audioPlayer.getDuration();
      _maxduration = length!.inMilliseconds;
    }
    double ready = ((_currentDuration / _maxduration) * 100);
    debugPrint("played $ready");
    if (ready == 100) {
      await _audioPlayer.stop();
    }
  }

  void _updateDurationNotification() async {
    if (_maxdurationNotification == 0) {
      var length = await _audioPlayerNotification.getDuration();
      _maxdurationNotification = length!.inMilliseconds;
    }
    double ready =
        ((_currentDurationNotification / _maxdurationNotification) * 100);
    debugPrint("played Notification $ready");
    if (ready == 100) {
      await _audioPlayerNotification.stop();
      if (_audioPlayer.state == PlayerState.playing) {
        _audioPlayer.setVolume(1);
      }
    }
  }

  void _audioPlayerNotificationStopped() async {
    _maxdurationNotification = 0;
    for (var callback in _listenersPlayerNotification) {
      callback(PlayerState.stopped);
    }
  }

  void _audioPlayerNotificationPaused() async {
    for (var callback in _listenersPlayerNotification) {
      callback(PlayerState.paused);
    }
  }

  void _audioPlayerPlaying() async {
    for (var callback in _listenersPlayer) {
      callback(PlayerState.playing);
    }
  }

  void _audioPlayerStopped() async {
    _maxduration = 0;
    for (var callback in _listenersPlayer) {
      callback(PlayerState.stopped);
    }
  }

  void _audioPlayerPaused() async {
    for (var callback in _listenersPlayer) {
      callback(PlayerState.paused);
    }
  }

  void addListnerPlayerState(OnPlayerStateCallback callback) {
    _listenersPlayer.add(callback);
  }

  void removeListnerPlayerState(OnPlayerStateCallback callback) {
    _listenersPlayer.remove(callback);
  }

  void pause() async {
    await _audioPlayer.pause();
  }
}