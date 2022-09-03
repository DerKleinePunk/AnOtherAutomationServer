import 'package:flutter/foundation.dart';

class ChatMessageList extends ChangeNotifier {
  final List<ChatMessage> _messageList = [];

  void addMessage(ChatMessage message) {
    _messageList.add(message);
    notifyListeners();
  }

  List<ChatMessage> get getMessages => _messageList;

}

@immutable
class ChatMessage {
  final bool isMe ;
  final String userId;
  final String mesageText;

  const ChatMessage(this.isMe, this.userId, this.mesageText);

}