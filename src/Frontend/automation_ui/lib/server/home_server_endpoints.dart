class HomeServerEndpoints {
  static const String websocket = '';
  static const String login = 'api/auth/login';
  static const String automation = 'api/automation';

  static Uri combine(String path1, String path2, {String path3 = ""}) {
    if(path3.isNotEmpty)
    {
       return Uri.parse('$path1/$path2/$path3');
    }
    return Uri.parse('$path1/$path2');
  }
}