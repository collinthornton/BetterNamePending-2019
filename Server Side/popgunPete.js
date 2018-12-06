
//Globals
// Variables for each of the environment variables (inputVars)
var cameraURL;
var cameraChange;
var cameraReset;
var defaultCameraUrl;

var testText;
var testNum;
var testCheck;
var testRange;
var testColor;
var testRad;
var objects;

var sonarDisplay;
var sonarCanvas;

var pete;
var isDown;
var peer;

//Init
window.onload = function init() {
    //setup camera url system
    initCamera();
    console.log(cameraURL);
    // testText = new InputVar("testText", testChange);
    // testNum = new InputVar("testNum", testChange);
    // testCheck = new InputVar("testCheck", testChange);
    // testRange = new InputVar("testRange", testChange);
    // testColor = new InputVar("testColor", testChange); https://www.w3schools.com/images/colorpicker.gif
    // objects = new InputVar("objects", testChange);
    pete = new Robot(document.getElementById("sonarDisplay"));
    //initCanvas();
    pete.display.tabIndex=100000;
    pete.draw();
    document.getElementById("sonarDisplay").addEventListener('keydown',controlKeyDown);
    document.getElementById("sonarDisplay").addEventListener('keyup',controlKeyUp);

    //peer = new Peer()//{key: 'lwjd5qra8257b9'});
    //peer.on('open', function(id){window.alert("it worked!");/*document.getElementById("myId").innerText= id;*/});

}

//Self-drawing Robot class
function Robot(display) {
    this.display = display;
    this.canvas = display.getContext("2d");
    this.centerX = this.display.width / 2;
    this.centerY = this.display.height / 2;
    this.width = this.display.width;
    this.height = this.display.height;
    this.robotRadius = 100;
    this.motors = {
        "frontLeft": 0, "frontRight": 0,
        "backLeft": 0, "backRight": 0
    };
    this.sonar=[
        5,600,
        600,5,
        5,400,
        400,5
    ];
    this.draw = function () {
        this.canvas.clearRect(0, 0, this.width, this.height);

        //draw border
        this.canvas.lineWidth = 2;
        this.canvas.strokeStyle = "white";
        this.canvas.strokeRect(4, 4, this.width - 4, this.height - 4);
        //draw box representing robot
        this.canvas.fillStyle = "#d95900";
        this.canvas.fillRect(this.centerX - this.robotRadius, this.centerY - this.robotRadius,
            2 * this.robotRadius, 2 * this.robotRadius);
        //draw wheels
        this.canvas.fillStyle = "#dddddd";
        this.canvas.fillRect(this.centerX - this.robotRadius,this.centerY -this.robotRadius+20,10+Math.abs(this.motors.frontLeft)*5,this.motors.frontLeft*10+Math.sign(this.motors.frontLeft)*15);
        this.canvas.fillRect(this.centerX + this.robotRadius-(10+Math.abs(this.motors.frontRight)*5),this.centerY -this.robotRadius+20,10+Math.abs(this.motors.frontRight)*5,this.motors.frontRight*10+Math.sign(this.motors.frontRight)*15);
        this.canvas.fillRect(this.centerX - this.robotRadius,this.centerY +this.robotRadius-20,10+Math.abs(this.motors.backLeft)*5,this.motors.backLeft*10+Math.sign(this.motors.backLeft)*15);
        this.canvas.fillRect(this.centerX + this.robotRadius-(10+Math.abs(this.motors.backRight)*5),this.centerY +this.robotRadius-20,10+Math.abs(this.motors.backRight)*5,(this.motors.backRight*10+Math.sign(this.motors.backRight)*15));
        this.canvas.fillStyle = "#444444";
        this.canvas.fillRect(this.centerX - this.robotRadius,this.centerY -this.robotRadius+20,10+Math.abs(this.motors.frontLeft)*5,2);
        this.canvas.fillRect(this.centerX + this.robotRadius-(10+Math.abs(this.motors.frontRight)*5),this.centerY -this.robotRadius+20,10+Math.abs(this.motors.frontRight)*5,2);
        this.canvas.fillRect(this.centerX - this.robotRadius,this.centerY +this.robotRadius-20,10+Math.abs(this.motors.backLeft)*5,2);
        this.canvas.fillRect(this.centerX + this.robotRadius-(10+Math.abs(this.motors.backRight)*5),this.centerY +this.robotRadius-20,10+Math.abs(this.motors.backRight)*5,2);
        //draw sonar
        this.canvas.font="28px Courier";
        this.canvas.fillStyle="#00FF00";
        var offset = Math.PI/180*180;
        var rotate = 15;
        var location = [
            [this.centerX-this.robotRadius-40, this.centerY-this.robotRadius],
            [this.centerX-this.robotRadius, this.centerY-this.robotRadius-20],
            [this.centerX+this.robotRadius-40, this.centerY-this.robotRadius-20],
            [this.centerX+this.robotRadius, this.centerY-this.robotRadius],
            [this.centerX+this.robotRadius, this.centerY+this.robotRadius+15],
            [this.centerX+this.robotRadius-40, this.centerY+this.robotRadius+40],
            [this.centerX-this.robotRadius, this.centerY+this.robotRadius+40],
            [this.centerX-this.robotRadius-40, this.centerY+this.robotRadius+15]
        ];
        for(var i = 0; i<this.sonar.length; i++){    
            this.canvas.fillText(this.sonar[i],location[i][0],location[i][1]);
        }

    }
}

//Environment Variable Functions

function initCamera() {
    cameraURL = document.getElementById("cameraURL");
    cameraChange = document.getElementById("cameraChange");
    cameraReset = document.getElementById("cameraReset");
    defaultCameraUrl = document.getElementById("cameraDisplay").src;
    cameraChange.onclick = function () {
        document.getElementById("cameraDisplay").src = cameraURL.value;
    }
    cameraReset.onclick = function () {
        document.getElementById("cameraDisplay").src = defaultCameraUrl;
    }
}

function onPeerOpen(id){
    document.getElementById("myId").innerText= id;
}

function controlKeyDown(e){
    e.preventDefault();
    if (isDown){
        return false;
    }
    isDown = true;
    var key = e.keyCode || e.which;
    var dir=null;
    //console.log(key+String.fromCharCode(key));
    switch (key){
        case 38: dir = "W"; break;
        case 37: dir="A"; break;
        case 39: dir="D"; break;
        case 40: dir="S"; break;
        case 16: dir="RL"; break;
        case 97: dir="RR"; break;
        case 87: dir = "W"; break;
        case 65: dir="A"; break;
        case 68: dir="D"; break;
        case 83: dir="S"; break;
        case 81: dir="RL"; break;
        case 69: dir="RR"; break;
        case 32: dir=" "; break;
        default: break;
    }
    if(dir!==null){console.log(dir);}
    command={"direction": dir};
    sendCommand(command);
    return false;
}

function sendCommand(command){
    //retranslate command
    switch(command.direction){
        case "W": pete.motors = {
            "frontLeft": 2, "frontRight": 2,
            "backLeft": 2, "backRight": 2
        };
        break;
        case "A": pete.motors = {
            "frontLeft": -2, "frontRight": 2,
            "backLeft": 2, "backRight": -2
        };
        break;
        case "D": pete.motors = {
            "frontLeft": 2, "frontRight": -2,
            "backLeft": -2, "backRight": 2
        };
        break;
        case "S": pete.motors = {
            "frontLeft": -2, "frontRight": -2,
            "backLeft": -2, "backRight": -2
        };
        break;
        case "RL": pete.motors = {
            "frontLeft": -2, "frontRight": 2,
            "backLeft": -2, "backRight": 2
        };
        break;
        case "RR": pete.motors = {
            "frontLeft": 2, "frontRight": -2,
            "backLeft": 2, "backRight": -2
        };
        break;
        case " ":pete.motors={
            "frontLeft": 0, "frontRight": 0,
            "backLeft": 0, "backRight": 0
        };
        default: console.log("unknown command"+command); break;
    }
    pete.draw()

}

function controlKeyUp(e){
    isDown = false;
}