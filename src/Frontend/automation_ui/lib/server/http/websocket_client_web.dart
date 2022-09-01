import 'package:web_socket_channel/html.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

WebSocketChannel makeWsClient(String url) {
  final HtmlWebSocketChannel channel = HtmlWebSocketChannel.connect(url, protocols: {"websocket"});
  ///channel.innerWebSocket.
  return channel;
}
    