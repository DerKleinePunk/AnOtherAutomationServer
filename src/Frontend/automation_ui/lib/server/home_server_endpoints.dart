class HomeServerEndpoints {
  static const String websocket = '';
  static const String session = 'api/session';
  static const String automation = 'api/automation';

  static Uri combine(String path1, String path2, {String path3 = ""}) {
    if(path3.isNotEmpty)
    {
       return Uri.parse('$path1/$path2/$path3');
    }
    return Uri.parse('$path1/$path2');
  }
}