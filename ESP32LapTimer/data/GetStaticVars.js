    requestData(); // get intial data straight away 
	var StatusData;
  
    // request data updates every 5000 milliseconds
    //setInterval(requestData, 200);
	

    function requestData() {

      var xhr = new XMLHttpRequest();
      xhr.open('GET', 'StaticVars');

      xhr.onload = function() {
        if (xhr.status === 200) {

          if (xhr.responseText) { // if the returned data is not null, update the values

            StatusData = JSON.parse(JSON.stringify(xhr.responseText));
			var data = JSON.parse(StatusData); //yeah not sure why I need to do this twice, but otherwise it doesn't work....
			
            document.getElementById("NumRXs").selectedIndex = parseInt(data.NumRXs);
			document.getElementById("ADCVBATmode").selectedIndex = parseInt(data.ADCVBATmode);
            document.getElementById("RXFilter").selectedIndex = parseInt(data.RXFilter);
			document.getElementById('ADCcalibValue').value = parseFloat(data.ADCcalibValue);
			
          }
        }else{requestData() }
      };
      
      xhr.send();
    }