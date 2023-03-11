const char* html = R"=====(
<html>
  <head>
    <title>Esp32cam-ambilight</title>
    <style>
      fieldset {
        background-color: #eeeeee;
      }
      
      legend {
        background-color: gray;
        color: white;
        padding: 5px 10px;
      }
      
      input {
        margin: 5px;
      }

      .switch {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
      }
      .switch input {
        opacity: 0;
        width: 0;
        height: 0;
      }
      .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #ccc;
        -webkit-transition: .4s;
        transition: .4s;
      }
      .slider:before {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        -webkit-transition: .4s;
        transition: .4s;
      }
      input:checked + .slider {
        background-color: #2196f3;
      }
      input:focus + .slider {
        box-shadow: 0 0 1px #2196f3;
      }
      input:checked + .slider:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(26px);
      }
      /* Rounded sliders */
      .slider.round {
        border-radius: 34px;
      }
      .slider.round:before {
        border-radius: 50%;
      }
      
      datalist {
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      writing-mode: vertical-lr;
      width: 200px;
      }   
      option {
        padding: 0;
      }

      input[type="range"] {
        width: 200px;
        margin: 0;
      }
     
    </style> 
  </head>
  <body>
    <fieldset>
      <legend>Frame Preview:</legend>
      <div>
        <canvas id="canvas" width="600" height="400"></canvas>
      </div>
      <input type="button" id="Image-reload" onclick="imageReload()" value="Reload" />
    </fieldset>
    <br>

    <fieldset>
      <legend>Keystone Settings:</legend>
      <label>Enable Keystone: </label> 

      <label class="switch">
        <input type="checkbox" onchange="keystoneEnabled(this)" id="keystone-Toggle">
          <span class="slider"></span>
      </label> 

      <div>
        <input type="radio" id="Point 1" name="Points" value="1"
               checked>
        <label for="X1"> Point 1 &emsp;&emsp;X:</label>
        <input type="text" id="X1" name="country" value="0" 
               minlength="1" maxlength="4" size="4" readonly>
        <label for="Y1"> Y:</label>
        <input type="text" id="Y1" name="country" value="0" 
              minlength="1" maxlength="4" size="4" readonly>
      </div>

      <div>
        <input type="radio" id="Point 2" name="Points" value="2">
        <label for="Point 2"> Point 2 &emsp;&emsp;X:</label>
        <input type="text" id="X2" name="country" value="0" 
               minlength="1" maxlength="4" size="4" readonly>
        <label for="Y2"> Y:</label>
        <input type="text" id="Y2" name="country" value="0" 
              minlength="1" maxlength="4" size="4" readonly>
      </div>

      <div>
        <input type="radio" id="Point 3" name="Points" value="3">
        <label for="Point 3"> Point 3 &emsp;&emsp;X:</label>
        <input type="text" id="X3" name="country" value="0" 
               minlength="1" maxlength="4" size="4" readonly>
        <label for="Y3"> Y:</label>
        <input type="text" id="Y3" name="country" value="0" 
              minlength="1" maxlength="4" size="4" readonly>
      </div>

      <div>
        <input type="radio" id="Point 4" name="Points" value="4">
        <label for="Point 4"> Point 4 &emsp;&emsp;X:</label>
        <input type="text" id="X4" name="country" value="0" 
               minlength="1" maxlength="4" size="4" readonly>
        <label for="Y1"> Y:</label>
        <input type="text" id="Y4" name="country" value="0" 
              minlength="1" maxlength="4" size="4" readonly>
      </div>
      <br>
      <input type="button" id="keystone-save" onclick="ButtonSave(this)" value="Save" disabled/>
      <input type="button" id="keystone-Reset" onclick="ButtonReset(this)" value="Reset" disabled/>
    </fieldset>
    <br>
    
    <fieldset>
      <legend>Image Settings:</legend>

      <label for="Image-resolution">Image resolution:</label>
      <select id="Image-resolution" onchange="ImageSettingsChange(this)">
        <option value="0">      96X96  </option>
        <option value="1">QQVGA 160x120</option>
        <option value="2">QCIF  176x144</option>
        <option value="3">HQVGA 240x176</option>
        <option value="4">      240X240</option>
        <option value="5">QVGA  320x240</option>
        <option value="6">CIF   400x296</option>
        <option value="7">HVGA  480x320</option>
        <option value="8">VGA   640x480</option>
      </select>
      <br><br>
      <div>
        <label for="Brightness">Brightness:</label>
        <input type="range" id="Brightness" name="Brightness" onchange="ImageSettingsChange(this)" list="2-0-2 marks"
               min="-2" max="2" value="0" step="1">
        <datalist id="2-0-2 marks">
          <option value="-2"></option>
          <option value="0" ></option>
          <option value="2" ></option>
        </datalist>
      </div>
      <div>
        <label for="Saturation">Saturation:</label>
        <input type="range" id="Saturation" name="Saturation" onchange="ImageSettingsChange(this)" list="2-0-2 marks"
               min="-2" max="2" value="0" step="1">
      </div>
      <br>
      <div>
        <label for="Contrast">Contrast :&ensp;</label>
        <input type="range" id="Contrast" name="Contrast" onchange="ImageSettingsChange(this)" list="2-0-2 marks"
               min="-2" max="2" value="0" step="1">
      </div>
      <br>
      <div>
        <label for="Mode">Image Mode:</label>
        <select id="Mode" onchange="ImageSettingsChange(this)">
          <option value="0">Auto</option>
          <option value="1">Sun</option>
          <option value="2">cloud</option>
          <option value="3">Indoors</option>
        </select>
      </div>
      <br>
      <div>
        <label for="ISO-Value">ISO-Value:</label>
        <input type="range" id="ISO-Value-slider" name="ISO-Value-slider" onchange="ImageSettingsChange(this)"
               min="0" max="1200" value="145" step="1">
        <input type="text" id="ISO-Value-txt" name="ISO-Value-txt" onchange="ImageSettingsChange(this)"
               required
               minlength="1" maxlength="4" size="5" pattern="[0-9]">
      </div>
      <br>
      <label>Clock 2x: </label> 
      <br><br>
      <label class="switch">
        <input type="checkbox" onchange="ImageSettingsChange(this)" id="Clock-2x">
          <span class="slider"></span>
      </label>
      <br><br> 
      <input type="button" id="Image-settings-save" onclick="ButtonSave(this)" value="Save" />
      <input type="button" id="Image-settings-Reset" onclick="ButtonReset(this)" value="Reset" />
    </fieldset>
    <br>
    <!-- <fieldset>
      <legend>Server Settings:</legend>
    </fieldset> -->
    <input type="button" id="Final-done" onclick="ButtonDone(this)" value="Done" />

    <script>
      //-global------------
      var canvas = document.getElementById("canvas");
      var ctx = canvas.getContext("2d");
      var image = new Image();
      var keystoneEnable = false;

      image.onload= function(){
        ctx.drawImage(image,0,0);
      }
//      image.src ="https://www.photocrati.com/wp-content/uploads/2009/07/ps_export_as_lab.jpg"
       image.src ="/image.jpg";
      
      var X1,Y1,X2,Y2,X3,Y3,X4,Y4;
      var depth;
      
      var ButKeySave=document.getElementById("keystone-save");
      var ButKeyRest=document.getElementById("keystone-Reset");
      var togKeySton=document.getElementById("keystone-Toggle");

      canvas.addEventListener("click", KeystoneClickPosition);
      
      function KeystoneClickPosition(event){
        if(keystoneEnable){
          ctx.drawImage(image, 0, 0);
          var rect = event.target.getBoundingClientRect();
          var x = Math.round(event.clientX - rect.left);
          var y = Math.round(event.clientY - rect.top);
          console.log("Clicked at position (" + x + ", " + y + ")");
          const radioButtons = document.querySelectorAll('input[name="Points"]');
          let selectedSize;
          for (const radioButton of radioButtons) {
              if (radioButton.checked) {
                  selectedSize = radioButton.value;
                  break;
              }
          }
          console.log("radio= "+selectedSize);
          let pointX = document.getElementById("X"+selectedSize);
          let pointY = document.getElementById("Y"+selectedSize);
          let xhr = new XMLHttpRequest();
          switch(selectedSize){
            case "1":
              xhr.open("GET", "/imageset?request=cordSettings"+"&Point="+1+"&X="+x+"&Y="+y, true);
              console.log("/imageset?request=cordSettings"+"&Point="+1+"&X="+x+"&Y="+y);
              pointX.value=X1=x;
              pointY.value=Y1=y;
              // pointX.value
              break;
            case "2":
              xhr.open("GET", "/imageset?request=cordSettings"+"&Point="+2+"&X="+x+"&Y="+y, true); 
              console.log("/imageset?request=cordSettings"+"&Point="+2+"&X="+x+"&Y="+y);
              pointX.value=X2=x;
              pointY.value=Y2=y; 
              break;
            case "3":
              xhr.open("GET", "/imageset?request=cordSettings"+"&Point="+3+"&X="+x+"&Y="+y, true); 
              console.log("/imageset?request=cordSettings"+"&Point="+3+"&X="+x+"&Y="+y);
              pointX.value=X3=x;
              pointY.value=Y3=y; 
              break;            
            case "4":
              xhr.open("GET", "/imageset?request=cordSettings"+"&Point="+4+"&X="+x+"&Y="+y, true);
              console.log("/imageset?request=cordSettings"+"&Point="+4+"&X="+x+"&Y="+y); 
              pointX.value=X4=x;
              pointY.value=Y4=y; 
              break;
            
          }
          xhr.send();
          drawKeystone();
        }

      }
      
      function keystoneEnabled(element){
        if(element.checked){
          keystoneEnable=true;
          ButKeySave.disabled=false;
          ButKeyRest.disabled=false;
          drawKeystone();
        }
        else{
          ButtonSave(document.getElementById("keystone-save"))
        }
      }

      function Load_variables(req_type){
        depth=8;
        let xhr = new XMLHttpRequest();
        xhr.onload = function(){
            if (xhr.status == 200) {
              let data = JSON.parse(xhr.responseText);
              if((document.getElementById("Image-resolution").value!==data.ImageResolution) || req_type==="/load-variables"){
                canvas.width  = Number(data.width);
                canvas.height = Number(data.height);
                document.getElementById("X1").value=X1=Number(data.X1);
                document.getElementById("Y1").value=Y1=Number(data.Y1); 
                document.getElementById("X2").value=X2=Number(data.X2);
                document.getElementById("Y2").value=Y2=Number(data.Y2);
                document.getElementById("X3").value=X3=Number(data.X3);
                document.getElementById("Y3").value=Y3=Number(data.Y3); 
                document.getElementById("X4").value=X4=Number(data.X4);
                document.getElementById("Y4").value=Y4=Number(data.Y4);
                ctx.drawImage(image,0,0);
              }
              document.getElementById("Image-resolution").value=data.ImageResolution;
              document.getElementById("Brightness").value      =data.Brightness;
              document.getElementById("Saturation").value      =data.Saturation;
              document.getElementById("Contrast").value        =data.Contrast;
              document.getElementById("Mode").value            =data.Mode;
              document.getElementById("ISO-Value-slider").value=data.ISOValue;
              document.getElementById("ISO-Value-txt").value   =data.ISOValue;
              document.getElementById("Clock-2x").checked      =Boolean(Number(data.Clock2x));
            }
            else {
              console.error("Error getting canvas size: " + xhr.statusText);
            }
          }
        
        xhr.open("GET",req_type);
        xhr.send();


      }
      
      function drawKeystone(){
        ctx.beginPath();
        ctx.lineWidth = depth;
        ctx.strokeStyle = 'rgba(225,225,225,0.75)';
        ctx.moveTo(X1+(depth/2), Y1+(depth/2));
        ctx.lineTo(X2-(depth/2), Y2+(depth/2));
        ctx.lineTo(X3-(depth/2), Y3-(depth/2));
        ctx.lineTo(X4+(depth/2), Y4-(depth/2));
        ctx.closePath();
        ctx.stroke(); 
      }


      function imageReload(){
        // send a reload command to esp32
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/image.jpg", true);
        xhr.send();
        image.src ="/image.jpg";
        image.onload= function(){
          ctx.drawImage(image,0,0);
        }
      }

      function ButtonDone(element){
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/imageset?request=allDone", true);
          xhr.send();
      }
      
      function ButtonSave(element){
        if(element.id==="keystone-save"){
          keystoneEnable=false;
          togKeySton.checked=false;
          ButKeySave.disabled=true;
          ButKeyRest.disabled=true;
          ctx.drawImage(image, 0, 0);
          //may be send a save signal
        }
        else if(element.id==="Image-settings-save"){
          //send save message
        }
        
      }

      function ButtonReset(element){
        if(element.id==="keystone-Reset"){
          //send reset command()
          //get value from esp32 
          console.log("keystone-Reset")
          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/imageset?request=cordReset", true);
          xhr.send();
          document.getElementById("X1").value=X1=0;
          document.getElementById("Y1").value=Y1=0; 
          document.getElementById("X2").value=X2=canvas.width;
          document.getElementById("Y2").value=Y2=0;
          document.getElementById("X3").value=X3=canvas.width;
          document.getElementById("Y3").value=Y3=canvas.height; 
          document.getElementById("X4").value=X4=0;
          document.getElementById("Y4").value=Y4=canvas.height;
          keystoneEnable=false;
          togKeySton.checked=false;
          ctx.drawImage(image,0,0);
          
        }
        else if(element.id==="Image-settings-Reset"){
          //get value from esp32
          console.log("Image-settings-Reset")
          Load_variables("/default-variables");
          
        }

      }

      function ImageSettingsChange(element){
        var xhr = new XMLHttpRequest();
        if(element.id==="ISO-Value-slider"){
          document.getElementById("ISO-Value-txt").value=element.value
        }
        else if(element.id==="ISO-Value-txt"){
          document.getElementById("ISO-Value-slider").value=element.value
        }
        if(element.id==="Clock-2x"){
          xhr.open("GET", "/imageset?request=imageSettings"+"&setting="+element.id+"&value="+Number(element.checked), true);
          console.log("/imageset?request=imageSettings"+"&setting="+element.id+"&value="+Number(element.checked));
          xhr.send();
        }
        else{
          xhr.open("GET", "/imageset?request=imageSettings"+"&setting="+element.id+"&value="+element.value, true);
          console.log("/imageset?request=imageSettings"+"&setting="+element.id+"&value="+element.value);
          xhr.send();
        }
        if(element.id==="Image-resolution")
          Load_variables("/load-variables");
          window.location.reload();
      }
     
    window.onload = function() {
      Load_variables("/load-variables");
    }


    </script>
  </body>
</html>
)=====";
