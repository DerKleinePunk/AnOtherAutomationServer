import 'package:flutter/foundation.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:io';

import 'http_stub.dart'
    if (dart.library.io) 'http_app.dart'
    if (dart.library.html) 'http_web.dart';

import 'http_exceptions.dart';

class ApiHttpClient {
  final http.Client httpClient = getClient();

  Future<dynamic> get(Uri url) async {
    //Todo solved cookie/auth Problem
    Map<String, String> headers = {"Accept": "application/json"};
    dynamic responseJson;
    try {
      final response = await httpClient.get(url, headers: headers);
      responseJson = _returnResponse(response);
    } on SocketException catch(exp) {
      throw FetchDataException("Internet Problem " + exp.message);
    } on Exception catch(exp) {
      throw FetchDataException(exp.toString());
    }
    return responseJson;
  }

  Future<dynamic> post(Uri url, dynamic body) async {
    //Todo solved cookie/auth Problem
    Map<String, String> headers = {"Accept": "application/json"};
    dynamic responseJson;
    try {
      final response = await httpClient.post(url, headers: headers, body: body);
      responseJson = _returnResponse(response);
    } on SocketException catch(exp) {
      throw FetchDataException("Internet Problem " + exp.message);
    } on Exception catch(exp) {
      throw FetchDataException(exp.toString());
    }
    return responseJson;
  }

  dynamic _returnResponse(http.Response response) {
    switch (response.statusCode) {
      case 200:
        debugPrint(response.body);
        var responseJson = json.decode(response.body);
        return responseJson;
      case 201:
        debugPrint(response.body);
        var responseJson = json.decode(response.body);
        return responseJson;
      case 400:
        throw BadRequestException(response.body.toString());
      case 401:
      case 403:
        throw UnauthorisedException(response.body.toString());
      case 500:
      default:
        throw FetchDataException(
            'Error occured while Communication with Server with StatusCode : ${response.statusCode}');
    }
  }
}