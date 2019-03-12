    requestData(); // get intial data straight away
	var StatusData;

	var testData = {
		"numRXs":3,
		"Band":{
			"1":0,
			"2":1,
			"3":2
		},
		"Channel":{
			"1":0,
			"2":1,
			"3":2
		}
	}
	//updateBandChannel(testData)
    function requestData() {

      var xhr = new XMLHttpRequest();
      xhr.open('GET', 'StaticVars');

      xhr.onload = function() {
        if (xhr.status === 200) {

          if (xhr.responseText) { // if the returned data is not null, update the values
            console.log("xhr.responseText")
            console.log(xhr.responseText)
            StatusData = JSON.parse(JSON.stringify(xhr.responseText));
            console.log("StatusData: ");
            console.log(StatusData);
			var data = JSON.parse(StatusData); //yeah not sure why I need to do this twice, but otherwise it doesn't work....
            console.log("data");
            console.log(data);

            document.getElementById("NumRXs").selectedIndex = parseInt(data.NumRXs);
			document.getElementById("ADCVBATmode").selectedIndex = parseInt(data.ADCVBATmode);
            document.getElementById("RXFilter").selectedIndex = parseInt(data.RXFilter);
			document.getElementById('ADCcalibValue').value = parseFloat(data.ADCcalibValue);
            createBandChannel(data.NumRXs)
            updateBandChannel(data)
          }
        }else{requestData() }
      };

      xhr.send();
    }

    function createBandChannel(numRXs) {
        numRXs = numRXs +1;
        for(var i=1;i<=numRXs;i++){ // GENERATE HTML
            $("#bandChannel").append('<fieldset class="bandChannel" id="RX'+i+'">');
            $("#RX"+i).append('<legend id=legenditem'+i+'>RX '+i+'</legend>');
            $("#RX"+i).append('<table class="tg" id="table'+i+'">');
            $("#table"+i).append('<tr>');
            $("#table"+i).append('<td class="table-cell-text">Band:</td>');
            $("#table"+i).append('<td class="table-cell">')
            $("#table"+i).append('<select name="band'+i+'" id="band'+i+'">')
            document.getElementById("band"+i).style.width = "80%";
            $("#band"+i).append('<option selected value="0">R</option>')
            $("#band"+i).append('<option value="1">A</option>')
            $("#band"+i).append('<option value="2">B</option>')
            $("#band"+i).append('<option value="3">E</option>')
            $("#band"+i).append('<option value="4">F</option>')
            $("#band"+i).append('<option value="5">D</option>')
            $("#band"+i).append('<option value="6">Connex</option>')
            $("#band"+i).append('<option value="7">Connex2</option>')
            $("#table"+i).append('</select></td>')
            $("#table"+i).append('<td class="table-cell-text"> Channel:</td>');
            $("#table"+i).append('<select name="channel'+i+'" id="channel'+i+'">')
            document.getElementById("channel"+i).style.width = "100%";
            $("#channel"+i).append('<option value="0">1</option>')
            $("#channel"+i).append('<option value="1">2</option>')
            $("#channel"+i).append('<option value="2">3</option>')
            $("#channel"+i).append('<option value="3">4</option>')
            $("#channel"+i).append('<option value="4">5</option>')
            $("#channel"+i).append('<option value="5">6</option>')
            $("#channel"+i).append('<option value="6">7</option>')
            $("#channel"+i).append('<option value="7">8</option>')
            $("#table"+i).append('</select></td>')
            $("#table"+i).append('</tr>');
            $("#bandChannel").append('</fieldset>');
        }
    }
    function updateBandChannel(data){
        console.log("****updateBandChannel****")
        console.log(data)
        for(var i=1;i<=numRXs;i++){ // FILL BAND AND CHANNEL FOR EACH ONE
            document.getElementById('band'+i).selectedIndex=data.Band[i];
            document.getElementById('channel'+i).selectedIndex=data.Channel[i];
        }
    }
