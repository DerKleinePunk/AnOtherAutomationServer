class CookieSave {
  final String cookie;
  final DateTime ttl;

  CookieSave(this.cookie, this.ttl);

  bool cookieStillValid() => ttl.isAfter(DateTime.now());

  Map<String, String> get header => {'Cookie': cookie.split(';').first};
}