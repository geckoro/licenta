const char RAW_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <style>
        :root {
            --arrow-size: 0.7em;
            --arrow-color: rgb(172, 172, 172);
            --background-color: rgb(43, 43, 43);
            --button-color: rgb(28, 28, 28);
            --disabled-color: rgb(37, 37, 37);
        }

        html {
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
            background-color: var(--background-color);
        }

        body {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
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
            width: 1.4em;
            height: calc(0.7em + var(--arrow-size));
            line-height: calc(0.7em + var(--arrow-size));
            background-color: var(--button-color);
            vertical-align: middle;
            text-align: center;
            display: table-cell;
            padding: 0.4em 1em 0.4em 1em;
            margin: 0.2em;
            border-radius: 0.5em;
            cursor: pointer;
        }

        .disabled {
            background-color: var(--disabled-color);
            cursor: default;
        }

        .button-directional {
            width: 2em;
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
    <div id="alert"
        style="width: 100%; display: flex; justify-content: space-evenly; align-items: center; ">
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
        <div class="button button-record" onclick="toggleRecording(this.children[0].classList)">
            <div class="circle toggle-recording"></div>
        </div>
    </div>

    <script>
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

        function toggleRecording(elementClassList) {
            if (elementClassList.value.includes("toggle-recording")) {
                alert("recording");
                recording = true;
            } else {
                alert("finished recording");
                recording = false;
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
                            document.getElementById("button-top").classList.add("disabled");
                        } else if (xhr.responseText == "true") {
                            document.getElementById("alert").style.visibility = "visible";
                            document.getElementById("button-top").classList.remove("disabled");
                        }
                    }
                }
            };

            xhr.send();
        }
    </script>
</body>

</html>
)rawliteral";