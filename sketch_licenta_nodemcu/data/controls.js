// buttons have the following id's
//  - topleft       : 1
//  - top           : 2
//  - topright      : 3
//  - bottomleft    : 4
//  - bottom        : 5
//  - bottomright   : 6

var keysPressed = [];

// declaring elements
var presetName = document.getElementById('presets-name');
var select = document.getElementById('presets-dropdown');
var presetInputs = document.getElementById('presets-inputs');
var pressedInput = document.getElementById('pressed-input');
var buttonRecord = document.getElementById('button-record');
var buttonSend = document.getElementById('button-send');

var isMobile = /iPhone|iPad|iPod|Android/i.test(navigator.userAgent);
var isKeyPressed = false;
var isKeySequenceModified = false;

// global status
var status = "none";

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
    button,
    buttonId;

// declaring a timeout, so we can clear it when we don't want the button to be held pressed
var inputPressedTimeout;

// querying all the buttons
var buttons = document.querySelectorAll('.button');
var pressedInput = document.getElementById('pressed-input');

var sendButton = document.getElementById('button-send');
sendButton.addEventListener('click', (e) => {
    if (!isDisabled(sendButton)) {
        sendPreset(presetInputs.value);
    }
});

var event = isMobile ? 'touchstart' : 'mousedown';
// adding the event listener by looping
buttons.forEach(element => {
    element.addEventListener(event, () => {
        buttonPressedEvent(element);
    });
});
event = isMobile ? 'touchend' : 'mouseup';
// event on mouse up
document.addEventListener(event, (e) => {
    buttonReleasedEvent(e);
});

document.addEventListener('keydown', (e) => {
    var name = e.key;

    // if the key was not saved in the array, then save it
    if (!keysPressed.includes(name)) {
        keysPressed.push(name);
        isKeyPressed = false;
        // send release event because we need to send a pressed event for the new key
        buttonReleasedEvent(null);
    }

    // if there is already an event sent for that key, then exit the function
    if (isKeyPressed) {
        return;
    }

    isKeyPressed = true;

    // determine which button needs to be pressed and send the event
    determineButton();
    buttonPressedEvent(button);
}, false);

document.addEventListener('keyup', (e) => {
    var name = e.key;
    // remove the released key from the array
    keysPressed = keysPressed.filter(item => item !== name);
    isKeyPressed = false;
    buttonReleasedEvent(null);
    // if there is still a key pressed, then send the event for that key
    if (keysPressed.length === 1) {
        determineButton();
        if (keysPressed[0] !== name) {
            buttonPressedEvent(button);
        }
    }
}, false);

function determineButton() {
    if (keysPressed.length > 2 || keysPressed.length < 1) {
        return undefined;
    }
    if (keysPressed.length == 1) {
        if (keysPressed[0] == 'w') {
            button = document.getElementById('button-top');
        } else if (keysPressed[0] == 's') {
            button = document.getElementById('button-bottom');
        }
    } else {
        if (keysPressed.includes('w')) {
            if (keysPressed.includes('a')) {
                button = document.getElementById('button-topleft');
            }
            if (keysPressed.includes('d')) {
                button = document.getElementById('button-topright');
            }
        } else if (keysPressed.includes('s')) {
            if (keysPressed.includes('a')) {
                button = document.getElementById('button-bottomleft');
            }
            if (keysPressed.includes('d')) {
                button = document.getElementById('button-bottomright');
            }
        }
    }
}

function buttonPressedEvent(element) {
    if (!element) {
        return;
    }
    // reset the timer
    timer = 0;
    // remembering the button that has been pressed
    button = element;
    buttonId = element.id.split('-')[1];

    if (isNotFunctionalButton(buttonId) && !isDisabled(element)) {
        timerInterval = setInterval(function () {
            timer += 0.1;

            pressedInput.value = timer.toFixed(1) + "s";

        }, 100);

        sendInput();
    }
}

function buttonReleasedEvent(e) {
    // check if a button has been pressed
    if (button !== undefined) {
        clearInterval(timerInterval);
        clearTimeout(inputPressedTimeout);
        clearInterval(requestInterval);
        // get the button id
        buttonId = button.id.split('-')[1];
        if (isNotFunctionalButton(buttonId)) {
            switch (buttonId) {
                case "topleft":
                    xhrRequest(`/inputReleased?button=1&time=0`)
                    break;
                case "top":
                    xhrRequest(`/inputReleased?button=2&time=0`)
                    break;
                case "topright":
                    xhrRequest(`/inputReleased?button=3&time=0`)
                    break;
                case "bottomleft":
                    xhrRequest(`/inputReleased?button=4&time=0`)
                    break;
                case "bottom":
                    xhrRequest(`/inputReleased?button=5&time=0`)
                    break;
                case "bottomright":
                    xhrRequest(`/inputReleased?button=6&time=0`)
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
}

function sendPreset(preset) {
    status = "sent";
    var totalTime = calculateTotalTime(preset);
    toggleButtons(false);
    togglePresetsFunctionality(false);
    setTimeout(function () {
        xhrRequest(`/input?preset=${encodePreset(preset)}`);
        toggleButtons(true)
        togglePresetsFunctionality(true);
        status = "none";
    }, (totalTime + 1) * 1000);
}

function sendInput() {
    if (isNotFunctionalButton(buttonId)) {
        switch (buttonId) {
            case "topleft":
                xhrRequest(`/inputPressed?button=1&time=0`)
                break;
            case "top":
                xhrRequest(`/inputPressed?button=2&time=0`)
                break;
            case "topright":
                xhrRequest(`/inputPressed?button=3&time=0`)
                break;
            case "bottomleft":
                xhrRequest(`/inputPressed?button=4&time=0`)
                break;
            case "bottom":
                xhrRequest(`/inputPressed?button=5&time=0`)
                break;
            case "bottomright":
                xhrRequest(`/inputPressed?button=6&time=0`)
                break;
            default:
                break;
        }
    }
    inputPressedTimeout = setTimeout(sendInput, 1000);
}

function xhrRequest(path) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", path, true);
    xhr.send();
}

function calculateTotalTime(preset) {
    var times = preset.replaceAll('topleft', '')
        .replaceAll('top', '')
        .replaceAll('topright', '')
        .replaceAll('bottomleft', '')
        .replaceAll('bottom', '')
        .replaceAll('bottomright', '')
        .replaceAll('-', '')
        .split(' ');
    return times.reduce((a, b) => parseFloat(a) + parseFloat(b), 0).toFixed(1);
}

function encodePreset(preset) {
    return preset.replaceAll('topleft', '1')
        .replaceAll('top', '2')
        .replaceAll('topright', '3')
        .replaceAll('bottomleft', '4')
        .replaceAll('bottom', '5')
        .replaceAll('bottomright', '6')
        .replaceAll(' ', '_')
        .replaceAll('.', '');
}

function isDisabled(button) {
    return button.classList.contains("disabled");
}

function isNotFunctionalButton(buttonId) {
    return buttonId !== undefined && buttonId !== "record" && buttonId !== "send";
}

function toggleRecording(element, elementClassList) {
    if (isDisabled(element)) {
        return;
    }

    if (elementClassList.value.includes("toggle-recording")) {
        togglePresetsFunctionality(false);
        recording = true;
    } else {
        var name = presetName.value;
        togglePresetsFunctionality(true);
        recording = false;
        addPreset(name, inputs);
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
                if (xhr.responseText == "false") {
                    document.getElementById("alert").style.visibility = "hidden";
                    if (status != "sent") {
                        document.getElementById("button-topleft").classList.remove("disabled");
                        document.getElementById("button-top").classList.remove("disabled");
                        document.getElementById("button-topright").classList.remove("disabled");
                    }
                } else if (xhr.responseText == "true") {
                    document.getElementById("alert").style.visibility = "visible";
                    document.getElementById("button-topleft").classList.add("disabled");
                    document.getElementById("button-top").classList.add("disabled");
                    document.getElementById("button-topright").classList.add("disabled");
                }
            }
        }
    };

    xhr.send();
}

function addPreset(name, preset) {
    var temp = {
        "text": name,
        "value": preset.join(" "),
        "selected": true
    };
    presets.push(temp);
    updatePresets();
    onClickSelect();
}

function updatePresets() {
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
    if (!(presetName && presetName.value)) {
        buttonRecord.classList.add("disabled");
    } else {
        buttonRecord.classList.remove("disabled");
    }

    if (select.value == "") {
        buttonSend.classList.add("disabled");
    } else {
        buttonSend.classList.remove("disabled");
    }
}

function togglePresetsFunctionality(enable) {
    if (!enable) {
        pressedInput.value = "0.0s";
        presetName.disabled = true;
        select.disabled = true;
        buttonSend.classList.add("disabled");
    } else {
        pressedInput.value = "...";
        presetName.value = "";
        presetName.disabled = false;
        select.disabled = false;
        buttonSend.classList.remove("disabled");
    }
}

function toggleButtons(enable) {
    if (!enable) {
        buttons.forEach(element => {
            element.classList.add("disabled");
        });
    } else {
        buttons.forEach(element => {
            element.classList.remove("disabled");
        });
    }
}

function onClickSelect() {
    var text = select.value;
    if (text == "") {
        text = "...";
    }
    presetInputs.value = text;
}

function wait(ms) {
    var start = new Date().getTime();
    var end = start;
    while (end < start + ms) {
        end = new Date().getTime();
    }
}

document.addEventListener("DOMContentLoaded", function (event) {
    setInterval(updateInputFields, 100);
});