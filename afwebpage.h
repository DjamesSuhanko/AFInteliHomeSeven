const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
    <style>
input[type=text], select {
  width: 100%;
  padding: 12px 20px;
  margin: 8px 0;
  display: inline-block;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
}

input[type=text]:focus {
  background-color: lightblue;
}

input[type=submit] {
  width: 100%;
  background-color: #4CAF50;
  color: white;
  padding: 14px 20px;
  margin: 8px 0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

input[type=submit]:hover {
  background-color: #45a049;
}

div {
  border-radius: 5px;
  background-color: #f2f2f2;
  padding: 20px;
  box-shadow: 10px 10px 5px grey;
}

p {
  text-indent: 10px;
  text-align: justify;
  letter-spacing: 3px;
}

h1 {
  color: white;
  text-shadow: 2px 2px 4px #000000;
}

a {
  text-decoration: none;
  color: #008CBA;
}

</style>

  <title>Configurador</title>
  <meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">
  </head><body>
  <form action="/setup">
  <h1>AFInteliHome</h1>
  <p><a href="https://www.afeletronica.com.br">Visite o site</a> para mais produtos</p>
  <div>
    <label for="fname">Sua rede (SSID):</label> <input type="text" name="input_ssid"><br>
    <label for="fname">Senha da sua rede:</label> <input type="text" name="input_passwd"><br>
    </div>
    <br><br>
    <div>
    <label for="fname">Nome para rede de configuracao:</label><input type="text" name="input_apssid"><br>
    <label for="fname">Senha para a rede de configuracao:</label><input type="text" name="input_appasswd"><br>
    </div>
    <br><br>
    <div>
    <label for="fname">Usuario administrativo</label> <input type="text" name="input_admin_u"><br>
    <label for="fname">Senha administrativa</label> <input type="text" name="input_admin_p"><br>
    </div>
    <br><br>
    <div>
    <label for="fname">Chamada para Alexa (primeiro acionador):</label> <input type="text" name="input_command_one"><br>
    <label for="fname">Chamada para Alexa (segundo acionador):</label> <input type="text" name="input_command_two"><br>
    <label for="fname">Chamada para Alexa (terceiro acionador):</label> <input type="text" name="input_command_three"><br>
    <label for="fname">Chamada para Alexa (quarto acionador):</label> <input type="text" name="input_command_four"><br>
    <label for="fname">Chamada para Alexa (quinto acionador):</label> <input type="text" name="input_command_five"><br>
    <label for="fname">Chamada para Alexa (sexto acionador):</label> <input type="text" name="input_command_six"><br>
    <label for="fname">Chamada para Alexa (setimo acionador):</label> <input type="text" name="input_command_seven"><br>
    </div>
    
    
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";
