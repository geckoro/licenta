const char RAW_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <style>
        :root {
            --arrow-size: 1.4em;
            --arrow-color: rgb(172, 172, 172);
            --background-color: rgb(43, 43, 43);
            --border-color: rgb(71, 71, 71);
            --button-color: rgb(28, 28, 28);
            --button-active-color: rgb(16, 16, 16);
            --button-height-factor: 1.7em;
            --button-padding: 0.8em 1.4em 0.8em 1.4em;
            --button-send-padding: 0.4em 1em 0.4em 1em;
            --button-send-size: 3em;
            --button-size: 3em;
            --disabled-color: rgb(37, 37, 37);
            --presets-dropdown-font-size: 1em;
            --presets-dropdown-height: 3vh;
            --presets-dropdown-width: 18em;
            --presets-name-font-size: 1em;
            --presets-name-height: 3vh;
            --presets-name-width: 17em;
        }

        @media only screen and (max-width: 800px) {
            :root {
                --arrow-size: 2.2em;
                --button-height-factor: 2.2em;
                --button-padding: 1.4em 2em 1.4em 2em;
                --button-send-padding: 0.6em 1.2em 0.6em 1.2em;
                --button-send-size: 4em;
                --button-size: 5.6em;
                --presets-dropdown-font-size: 2em;
                --presets-dropdown-height: 5vh;
                --presets-dropdown-width: 10em;
                --presets-name-font-size: 2em;
                --presets-name-height: 5vh;
                --presets-name-width: 9em;
            }
        }

        html {
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
            background-color: var(--background-color);
            min-width: 800px;
            overflow: hidden;
        }

        #container {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: row;
        }

        #remote {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
            margin-right: 2em;
        }

        #presets {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
            margin-left: 2em;
        }

        .arrow {
            pointer-events: none;
            margin: auto;
            width: 0;
            height: 0;
            display: inline-block;
        }

        .arrow-top {
            border-left: var(--arrow-size) solid transparent;
            border-right: var(--arrow-size) solid transparent;
            border-bottom: var(--arrow-size) solid var(--arrow-color);
        }

        .arrow-right {
            border-left: var(--arrow-size) solid var(--arrow-color);
            border-top: var(--arrow-size) solid transparent;
            border-bottom: var(--arrow-size) solid transparent;
        }

        .arrow-bottom {
            border-left: var(--arrow-size) solid transparent;
            border-right: var(--arrow-size) solid transparent;
            border-top: var(--arrow-size) solid var(--arrow-color);
        }

        .arrow-left {
            border-top: var(--arrow-size) solid transparent;
            border-right: var(--arrow-size) solid var(--arrow-color);
            border-bottom: var(--arrow-size) solid transparent;
        }

        .circle {
            pointer-events: none;
            margin: auto;
            display: inline-block;
            width: var(--arrow-size);
            height: var(--arrow-size);
            background-color: rgb(209, 64, 64);
        }

        .toggle-recording {
            border-radius: 50%;
            background-color: red;
        }

        .button {
            width: var(--button-size);
            background-color: var(--button-color);
            vertical-align: middle;
            text-align: center;
            display: table-cell;
            padding: var(--button-padding);
            margin: calc(var(--button-size) / 10);
            border-radius: 0.5em;
            cursor: pointer;
        }

        .button:active {
            background-color: var(--button-active-color);
        }

        .disabled {
            background-color: var(--disabled-color);
            cursor: default;
        }

        .disabled:active {
            background-color: var(--disabled-color);
        }

        #presets-name {
            width: var(--presets-name-width);
            height: var(--presets-name-height);
            padding-inline: 0.5em;
            background-color: rgb(52, 52, 52);
            color: var(--arrow-color);
            font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
            font-size: var(--presets-name-font-size);
            font-weight: bold;
            margin-bottom: 1em;
            border: 1px solid var(--border-color);
        }

        #presets-name:disabled {
            opacity: 0.7;
        }

        #presets-dropdown {
            width: var(--presets-dropdown-width);
            height: var(--presets-dropdown-height);
            padding-inline: 0.5em;
            background-color: rgb(52, 52, 52);
            color: var(--arrow-color);
            font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
            font-size: var(--presets-dropdown-font-size);
            font-weight: bold;
            border: 1px solid var(--border-color);
        }

        #button-send {
            width: var(--button-send-size);
            color: var(--arrow-color);
            font-size: var(--arrow-size);
            font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
            font-weight: bold;
            padding: var(--button-send-padding);
            margin-top: 1em;
        }

        .direction-buttons {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: row;
        }

        .function-buttons {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: row;
            margin-top: 1em;
        }

        #alert {
            animation-name: obstacleAlert;
            animation-duration: 0.4s;
            animation-iteration-count: infinite;
            animation-direction: alternate;
            width: 100%;
            display: flex;
            visibility: hidden;
            justify-content: space-evenly;
            align-items: center;
        }

        @keyframes obstacleAlert {
            0% {
                opacity: 20%;
            }

            100% {
                opacity: 100%;
            }
        }
    </style>
</head>

<body>
    <div id="container">
        <div id="remote">
            <div id="alert">
                <h2 style="color: red;">Obstacle detected!</h2>
            </div>
            <div class="button" id="button-top">
                <div class="arrow arrow-top"></div>
            </div>
            <div class="direction-buttons">
                <div class="button button-directional" id="button-left">
                    <div class="arrow arrow-left"></div>
                </div>
                <div class="button button-directional" id="button-right">
                    <div class="arrow arrow-right"></div>
                </div>
            </div>
            <div class="button" id="button-bottom">
                <div class="arrow arrow-bottom"></div>
            </div>
            <div class="function-buttons">
                <div class="button" id="button-record" onclick="toggleRecording(this, this.children[0].classList)">
                    <div class="circle toggle-recording"></div>
                </div>
            </div>
        </div>
        <div id="presets">
            <input id="presets-name">
            </input>
            <select id="presets-dropdown">
            </select>
            <div class="button" id="button-send">
                SEND
            </div>
        </div>
    </div>

    <script>
        // collection of saved presets
        var presets = [{
            "text": "Select a preset...",
            "value": ""
        }];
        updatePresets();
        // collection of recorded inputs
        var inputs = [];
        // boolean to toggle recording of inputs
        var recording = false;

        var timer = 0,
            timerInterval,
            requestInterval,
            button;
        // querying all the buttons
        var buttons = document.querySelectorAll('.button');
        // adding the event listener by looping
        buttons.forEach(element => {
            element.addEventListener('mousedown', (e) => {
                // reset the timer
                timer = 0;
                // remembering the button that has been pressed
                button = e;
                timerInterval = setInterval(function () {
                    timer += 0.1;
                }, 100);
                requestInterval = setInterval(function () {
                    var buttonId = button.toElement.id.split('-')[1];
                    // check if it's not a functional button
                    if (buttonId !== undefined) {
                        switch (buttonId) {
                            case "top":
                                request(1, "inputPressed");
                                break;
                            case "right":
                                request(2, "inputPressed");
                                break;
                            case "bottom":
                                request(3, "inputPressed");
                                break;
                            case "left":
                                request(4, "inputPressed");
                                break;
                            default:
                                break;
                        }
                    }
                }, 1000);
            });
        });

        // event on mouse up
        document.addEventListener('mouseup', (e) => {
            // check if a button has been pressed
            if (button !== undefined) {
                clearInterval(timerInterval);
                clearInterval(requestInterval);
                // get the button id
                var buttonId = button.toElement.id.split('-')[1];
                // check if it's not a functional button
                if (buttonId !== undefined) {
                    switch (buttonId) {
                        case "top":
                            request(1, "inputReleased");
                            break;
                        case "right":
                            request(2, "inputReleased");
                            break;
                        case "bottom":
                            request(3, "inputReleased");
                            break;
                        case "left":
                            request(4, "inputReleased");
                            break;
                        default:
                            break;
                    }
                    var input = buttonId + "-" + timer.toFixed(1);
                    // if recording, store the id and how long it has been pressed
                    if (recording) {
                        inputs.push(input);
                    }
                }
                // reset the timer and unselect the button
                timer = 0;
                button = undefined;
            }
        });

        function request(button, endpoint = "input", time = 0) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", `/${endpoint}?button=${button}&time=${time}`, true);
            xhr.send();
        }

        function onArrowTop(time) {
            request(1, time);
        }

        function onArrowRight(time) {
            request(2, time);
        }

        function onArrowBottom(time) {
            request(3, time);
        }

        function onArrowLeft(time) {
            request(4, time);
        }

        function toggleRecording(element, elementClassList) {
            if (isDisabled(element)) {
                return;
            }

            if (elementClassList.value.includes("toggle-recording")) {
                togglePresetsFunctionality(true);
                recording = true;
            } else {
                var name = document.getElementById('presets-name').value;

                togglePresetsFunctionality(false);
                recording = false;
                addPreset(name, inputs);
                console.log(inputs);
                inputs = [];
            }
            elementClassList.toggle('toggle-recording');
        }

        function obstacleCheck() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/obstacleCheck", true);

            xhr.onreadystatechange = function () {
                if (xhr.readyState == XMLHttpRequest.DONE) { // XMLHttpRequest.DONE == 4
                    if (xhr.status == 200) {
                        console.log(xhr.responseText);

                        if (xhr.responseText == "false") {
                            document.getElementById("alert").style.visibility = "hidden";
                            document.getElementById("button-top").classList.remove("disabled");
                        } else if (xhr.responseText == "true") {
                            document.getElementById("alert").style.visibility = "visible";
                            document.getElementById("button-top").classList.add("disabled");
                        }
                    }
                }
            };

            xhr.send();
        }

        function isDisabled(button) {
            return button.classList.contains("disabled");
        }

        function addPreset(name, preset) {            
            var temp = {
                "text": name,
                "value": preset,
                "selected": true
            };
            presets.push(temp);
            updatePresets();
        }

        function updatePresets() {
            var select = document.getElementById('presets-dropdown');

            for (a in select.options) {
                select.options.remove(0);
            }

            for (var i = 0, l = presets.length; i < l; i++) {
                var option = presets[i];
                select.options.add(new Option(option.text, option.value, option.selected));
            }

            select.selectedIndex = presets.length - 1;
        }

        function updateInputFields() {
            var input = document.getElementById('presets-name');
            var buttonRecord = document.getElementById('button-record');

            if (!(input && input.value)) {
                buttonRecord.classList.add("disabled");
            } else {
                buttonRecord.classList.remove("disabled");
            }
        }

        function togglePresetsFunctionality(disable) {
            var input = document.getElementById('presets-name');
            var select = document.getElementById('presets-dropdown');
            var buttonSend = document.getElementById('button-send');
            if (disable) {
                input.disabled = true;
                select.disabled = true;
                buttonSend.classList.add("disabled");
            }
            else {
                input.disabled = false;
                select.disabled = false;
                buttonSend.classList.remove("disabled");
            }
        }

        document.addEventListener("DOMContentLoaded", function (event) {
            setInterval(obstacleCheck, 500);
            setInterval(updateInputFields, 100);
        });
    </script>
</body>

</html>
)rawliteral";
