import 'package:web_socket_channel/io.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

WebSocketChannel makeWsClient(String url) =>
    IOWebSocketChannel.connect(url);