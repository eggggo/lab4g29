const char body[] PROGMEM = R"===(
  <!DOCTYPE html>
    <html>
    <body>
    <!--TODO some ui feedback for button presses or something-->
    </body>
    <script>
      document.addEventListener('keydown', function(event) {
        var code = event.code;
        var xhttp = new XMLHttpRequest();
        if (code == 'KeyW' || code == 'KeyS') {
          var str = "straight?val=";
          var res = code == 'KeyW' ? str.concat('0') : str.concat('1');
          xhttp.open("GET", res, true);
        } else if (code == 'KeyA' || code == 'KeyD') {
          var str = "turn?val=";
          var res = code == 'KeyA' ? str.concat('0') : str.concat('1');
          xhttp.open("GET", res, true);
        }
        xhttp.send();
      })
    </script>
    </html>
)===";