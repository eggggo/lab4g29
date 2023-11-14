const char body[] PROGMEM = R"===(
  <!DOCTYPE html>
    <html>
    <body>
      <input type="range" min="0" max="100" value="20" id="p_slider">
      <span id="p_label">P: 2</span> <br>
      <input type="range" min="0" max="100" value="1" id="i_slider">
      <span id="i_label">I: 0.1 </span> <br>
      <input type="range" min="-100" max="0" value="-10" id="d_slider">
      <span id="d_label">D: -1</span> <br>
    </body>
    <script>
      p_slider.onchange = function() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("p_label").innerHTML = this.responseText;
          }
        };
        var str = "p?val=";
        var res = str.concat(this.value);
        xhttp.open("GET", res, true);
        xhttp.send();
      }

      i_slider.onchange = function() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("i_label").innerHTML = this.responseText;
          }
        };
        var str = "i?val=";
        var res = str.concat(this.value);
        xhttp.open("GET", res, true);
        xhttp.send();
      }

      d_slider.onchange = function() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("d_label").innerHTML = this.responseText;
          }
        };
        var str = "d?val=";
        var res = str.concat(this.value);
        xhttp.open("GET", res, true);
        xhttp.send();
      }
    </script>
    </html>
)===";