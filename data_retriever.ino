float getHf(String rpc, String pool, String address) {
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    https.begin(*client, rpc);
    https.setTimeout(5000);
    https.addHeader("Content-Type", "application/json");
    String payload = "{\"method\":\"eth_call\",\"params\":[{\"to\":\"POOL\",\"data\":\"0xbf92857c000000000000000000000000ADDRESS\"},\"latest\"],\"id\":42,\"jsonrpc\":\"2.0\"}";
    payload.replace("POOL", pool);
    payload.replace("ADDRESS", address);
    float result = 0.0;
    int attempt = 0;

    while (attempt < 5 && result == 0.0) {
      int httpResponseCode = https.POST(payload);
      if (httpResponseCode == 200) {
        String response = https.getString();
        String hexNumber = response.substring(response.length() - 20, response.length() - 2);

        String truncated = hexNumber.substring(0, hexNumber.length() - 10); // truncated by 16^10
        float f = 0.0000010995;
        result = strtoul(truncated.c_str(), 0, 16) * f;
      }
      else {
        Serial.println("Error code: ");
        Serial.println(httpResponseCode);
        delay(10000);
      }
      attempt++;
    }
    https.end();
    return result;
  }
}
