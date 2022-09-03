import 'dart:io';
import 'dart:convert';

import 'package:audioplayers/audioplayers.dart';
import '../server/client.dart';
import 'package:flutter/foundation.dart';

typedef OnPlayerStateCallback = void Function(PlayerState state);

class AudioPlayerService {
  final ServerClient _serverClient;
  late AudioPlayer _audioPlayer;
  late AudioPlayer _audioPlayerNotification;
  late AudioCache _audioCache;
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
    _serverClient.addListener(_onWebSocketMessage);
    _audioPlayer = AudioPlayer(
        mode: PlayerMode.MEDIA_PLAYER, playerId: 'mneHomeAppPLayer');
    _audioPlayerNotification = AudioPlayer(
        mode: PlayerMode.MEDIA_PLAYER,
        playerId: 'mneHomeAppPLayerNotification');
    _audioPlayer
        .setReleaseMode(ReleaseMode.STOP); // set release mode so that it never
    _audioCache = AudioCache(fixedPlayer: _audioPlayerNotification);
    if (!kIsWeb) {
      // Calls to Platform.isIOS fails on web
      if (Platform.isIOS) {
        _audioCache.fixedPlayer?.notificationService.startHeadlessService();
      }
    }
    Logger.changeLogLevel(LogLevel.INFO);

    _audioPlayer.onDurationChanged.listen((Duration d) {
      _maxduration = d.inMilliseconds;
      debugPrint("_maxduration $_maxduration");
    });

    _audioPlayer.onAudioPositionChanged.listen((Duration d) {
      _currentDuration = d.inMilliseconds;
      _updateDuration();
    });

    _audioPlayerNotification.onAudioPositionChanged.listen((Duration d) {
      _currentDurationNotification = d.inMilliseconds;
      _updateDurationNotification();
    });

    _audioPlayer.onPlayerStateChanged.listen((PlayerState s) {
      debugPrint('Current player state: $s');
      switch (s) {
        case PlayerState.STOPPED:
          _audioPlayerStopped();
          break;
        case PlayerState.PLAYING:
          _audioPlayerPlaying();
          break;
        case PlayerState.PAUSED:
          _audioPlayerPaused();
          break;
        case PlayerState.COMPLETED:
          // TODO: Handle this case.
          break;
      }
    });

    _audioPlayerNotification.onPlayerStateChanged.listen((PlayerState s) {
      debugPrint('Current player state: $s');
      switch (s) {
        case PlayerState.STOPPED:
          _audioPlayerNotificationStopped();
          break;
        case PlayerState.PLAYING:
          // TODO: Handle this case.
          break;
        case PlayerState.PAUSED:
          _audioPlayerNotificationPaused();
          break;
        case PlayerState.COMPLETED:
          // TODO: Handle this case.
          break;
      }
    });

    _audioPlayer.onPlayerCompletion.listen((event) {
      debugPrint("PlayerCompletion");
    });

    _audioPlayer.onPlayerError.listen((msg) {
      debugPrint('audioPlayer error : $msg');
    });

    _audioPlayerNotification.onPlayerError.listen((msg) {
      debugPrint('audioPlayerNotification error : $msg');
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
    final uri =
        await _audioCache.load("http://localhost:8000/rainforest-ambient.mp3");

    if (_audioPlayer.state == PlayerState.PLAYING) {
      _audioPlayer.setVolume(0.5);
    }

    _maxdurationNotification = 0;
    await _audioCache.play(uri.toString(), isNotification: true);

    debugPrint("playing sound notification");
  }

  void play(String audioFile) async {
    int result = await _audioPlayer.setUrl(audioFile);
    if (result == 1) {
      debugPrint("Sound loading successfull");
    } else {
      debugPrint("Error loading sound");
    }

    _maxduration = 0;
    _audioPlayer.setVolume(1);
    result = await _audioPlayer.resume();
    if (result == 1) {
      debugPrint("Sound playing successfull");
    } else {
      debugPrint("Error while playing sound");
    }
  }

  void dispose() {
    //_audioPlayer.stop();
    _audioPlayer.dispose();
    //_audioPlayerNotification.stop();
    _audioPlayerNotification.dispose();
  }

  void _updateDuration() async {
    if (_maxduration == 0) {
      _maxduration = await _audioPlayer.getDuration();
    }
    double ready = ((_currentDuration / _maxduration) * 100);
    debugPrint("played $ready");
    if (ready == 100) {
      await _audioPlayer.stop();
    }
  }

  void _updateDurationNotification() async {
    if (_maxdurationNotification == 0) {
      _maxdurationNotification = await _audioPlayerNotification.getDuration();
    }
    double ready =
        ((_currentDurationNotification / _maxdurationNotification) * 100);
    debugPrint("played Notification $ready");
    if (ready == 100) {
      await _audioPlayerNotification.stop();
      if (_audioPlayer.state == PlayerState.PLAYING) {
        _audioPlayer.setVolume(1);
      }
    }
  }

  void _audioPlayerNotificationStopped() async {
    _maxdurationNotification = 0;
    for (var callback in _listenersPlayerNotification) {
      callback(PlayerState.STOPPED);
    }
  }

  void _audioPlayerNotificationPaused() async {
    for (var callback in _listenersPlayerNotification) {
      callback(PlayerState.PAUSED);
    }
  }

  void _audioPlayerPlaying() async {
    for (var callback in _listenersPlayer) {
      callback(PlayerState.PLAYING);
    }
  }

  void _audioPlayerStopped() async {
    _maxduration = 0;
    for (var callback in _listenersPlayer) {
      callback(PlayerState.STOPPED);
    }
  }

  void _audioPlayerPaused() async {
    for (var callback in _listenersPlayer) {
      callback(PlayerState.PAUSED);
    }
  }

  void addListnerPlayerState(OnPlayerStateCallback callback) {
    _listenersPlayer.add(callback);
  }

  void removeListnerPlayerState(OnPlayerStateCallback callback) {
    _listenersPlayer.remove(callback);
  }

  void pause() async {
    int result = await _audioPlayer.pause();
    if (result != 0) {
      debugPrint("error pause audioPlayer");
    }
  }
}