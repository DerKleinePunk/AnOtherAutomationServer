import 'package:flutter/foundation.dart';
import 'http/websocket_client_stup.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

typedef OnMessageCallback = void Function(String type, String message);
typedef OnCloseCallback = void Function();

class ServerMessageClient {
  WebSocketChannel? _channel; // =
  bool _connected = false;

  final ObserverList<OnMessageCallback> _listenersMessage =
      ObserverList<OnMessageCallback>();
  final ObserverList<OnCloseCallback> _listenersClose =
      ObserverList<OnCloseCallback>();

  ServerMessageClient(String server) {
    Uri serverUri = Uri.parse(server);
    String messageUri = "";
    if (serverUri.scheme == "https") {
      messageUri = "wss://";
    } else {
      messageUri = "ws://";
    }
    messageUri += serverUri.host;
    if (serverUri.port != 0) {
      messageUri += ":${serverUri.port.toString()}";
    }
    messageUri += "/messages";
    try {
      _channel = makeWsClient(messageUri);
      _channel!.stream.listen(
        (message) {
          if (message == "connected") {
            _connected = true;
            debugPrint("Connection establised.");
          } else if (message == "send:success") {
            debugPrint("Message send success");
          } else if (message == "send:error") {
            debugPrint("Message send error");
          } else if (message != null && message != "") {
            // && message.length() > 0
            if (message.substring(0, 1) == "{") {
              debugPrint("Message data $message");
              message = message.replaceAll(RegExp("'"), '"');
              //var jsondata = json.decode(message);
              for (var callback in _listenersMessage) {
                callback("data", message);
              }
            } else {
              debugPrint("Message Text $message");
              for (var callback in _listenersMessage) {
                callback("Text", message);
              }
            }
          } else {
            debugPrint("Message is empty");
          }
        },
        onDone: () {
          //if WebSocket is disconnected
          debugPrint("Web socket is closed");
          _connected = false;
          for (var callback in _listenersClose) {
            callback();
          }
        },
        onError: (error) {
          debugPrint(error.toString());
        },
      );
      debugPrint("ServerMessageClient try connect to $messageUri");
    } catch (exp) {
      debugPrint("ServerMessageClient $exp");
    }
  }

  void close() {
    _channel?.sink.close();
  }

  addListenerMessage(OnMessageCallback callback) {
    _listenersMessage.add(callback);
  }

  removeListenerMessage(OnMessageCallback callback) {
    _listenersMessage.remove(callback);
  }

  isConnected() {
    return _connected;
  }

  sendMsg(String message) {
    _channel?.sink.add(message);
  }
}
