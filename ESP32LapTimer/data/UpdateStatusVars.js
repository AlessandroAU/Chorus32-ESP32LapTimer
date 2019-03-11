    requestData(); // get intial data straight away
	var StatusData;

    // request data updates every 5000 milliseconds
    setInterval(requestData, 500);

    function requestData() {

      var xhr = new XMLHttpRequest();
      xhr.open('GET', 'StatusVars');

      xhr.onload = function() {
        if (xhr.status === 200) {

          if (xhr.responseText) { // if the returned data is not null, update the values

            StatusData = JSON.parse(JSON.stringify(xhr.responseText));
			var data = JSON.parse(StatusData);

            document.getElementById("Var_VBAT").innerText = data.Var_VBAT.toFixed(2);
            document.getElementById("Var_WifiClients").innerText = data.Var_WifiClients;
            document.getElementById("Var_CurrMode").innerText = data.Var_CurrMode;

          } else { // a problem occurred

            document.getElementById("Var_VBAT").innerText = "?";
            document.getElementById("Var_WifiClients").innerText = "?";
            document.getElementById("Var_CurrMode").innerText = "?";
          }
        } else {
          console.log('Request failed.  Returned status of ' + xhr.status);

          document.getElementById("Var_VBAT").innerText = "?";
          document.getElementById("Var_WifiClients").innerText = "?";
          document.getElementById("Var_CurrMode").innerText = "?";

        }
      };

      xhr.send();
    }
