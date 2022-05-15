void displayWebPage(WiFiClient client) {
  if (client) { // If a new client connects,
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // turns the GPIOs on and off
            if (header.indexOf("GET /AvalancheV3") >= 0) {
              updateChain("AvalancheV3");
              updateHf();
            } else if (header.indexOf("GET /PolygonV3") >= 0) {
              updateChain("PolygonV3");
              updateHf();
            } else if (header.indexOf("GET /Avalanche") >= 0) {
              updateChain("Avalanche");
              updateHf();
            } else if (header.indexOf("GET /Polygon") >= 0) {
              updateChain("Polygon");
              updateHf();
            } else if (header.indexOf("GET /?address=") >= 0 && header.indexOf("0x") >= 0) {
              int firstIndex = header.indexOf("=") + 1;
              String address = header.substring(firstIndex);
              int lastIndex = address.indexOf(" ");
              char tmpa[42];
              address.substring(2, lastIndex).toCharArray(tmpa, 42);
              updateAddress(tmpa);
              updateHf();
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            // drop down styling
            client.println(".dropbtn {background-color: #04AA6D; min-width: 400px; color: white; padding: 16px; font-size: 20px; border: none; }");
            client.println(".dropdown { position: relative; display: inline-block; }");
            client.println(".dropdown-content { display: none; position: absolute; background-color: #f1f1f1; min-width: 400px; box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2); z-index: 1; }");
            client.println(".dropdown-content a { color: black; padding: 12px 16px; text-decoration: none; display: block; }");
            client.println(".dropdown-content a:hover {background-color: #ddd;}");
            client.println(".dropdown:hover .dropdown-content {display: block;}");
            client.println(".dropdown:hover .dropbtn {background-color: #3e8e41;}");
            // input text styling
            client.println("input[type=text] {   width: 400px;   padding: 12px 20px;   margin: 8px 0;   box-sizing: border-box;   border: 2px solid black;   border-radius: 4px; }");
            // button styling
            client.println(".button { background-color: #195B6A; width: 400px; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 22px; margin: 2px; cursor: pointer;}");
            client.println("</style></head>");

            // Web Page Heading
            client.println("<body><h1>AAVE indicator</h1>");
            String healthFactor = (hf < 4722) ? String(hf) : "&infin;";
            client.println("<h2>Health factor: " + healthFactor + "</h2>");
            // drop down menu
            client.println("<div class=\"dropdown\"> <button class=\"dropbtn\">" + String(settings.chain) + "</button> <div class=\"dropdown-content\">");
            client.println("<a href=\"Avalanche\">Avalanche</a> <a href=\"Polygon\">Polygon</a> <a href=\"AvalancheV3\">AvalancheV3</a> <a href=\"PolygonV3\">PolygonV3</a></div> </div>");
            // input and submit form
            client.println("<form action=\"/\" id=\"addressForm\"> <input type=\"text\" name=\"address\" placeholder=0x" + String(settings.address) + ">");
            client.println("<p><input type=\"submit\" class=\"button\" value=\"Submit\"> </p> </form>");
            // script to enter address
            client.println("<script> $(\"#addressForm\").submit(function(event) { event.preventDefault();");
            client.println("var $form = $(this), term = $form.find(\"input[name='address']\").val(), url = $form.attr(\"action\"); var posting = $.post(url, {address: term}); }); </script>");
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
