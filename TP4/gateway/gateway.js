/*******************************************************************/
/*** Configuration ***/
/*******************************************************************/
const serialPortName = "COM51";
const thingSpeakAPIKey = "DVDTVSXTQFY8QZXT";
/*******************************************************************/


/*******************************************************************/
/*** Gateway code ***/
/*******************************************************************/
const SerialPort = require("serialport");
const https = require('https');

var discard = false;

const port = new SerialPort(serialPortName, {  
    baudRate: 1200
});

function tx(buff) {
    port.write(Buffer.from([0xaa, 0x55, 0xc3, 0x3c, buff.length]));
    port.write(buff);
}

function txResponse(code) {
    tx(Buffer.from([code]));
}

function handleRx(buff) {
    console.log("------------------------------------------");
    if (buff[0] == 0x01) {
        if (buff.length - 1 < 6) {
            console.log('Received command 0x01 (SendData), with wrong length');
            discard = false;
        } else {
            num1 = buff.readUInt16LE(1);
            num2 = buff.readUInt16LE(3);
            num3 = buff.readUInt16LE(5);
            console.log('Received command 0x01 (SendData), num1=' + num1, 'num2=' + num2, 'num3=' + num3);
            url = "https://api.thingspeak.com/update.json?api_key=" + thingSpeakAPIKey 
                + "&field1=" + num1
                + "&field2=" + num2
                + "&field3=" + num3;

            console.log('GET to ' + url);
            https.get(url, (resp) => {
                let data = '';
                resp.on('data', (chunk) => {
                    data += chunk;
                });

                resp.on('end', () => {
                    if (resp.statusCode != 200) {
                        console.log("statusCode: " + resp.statusCode);
                        txResponse(0xc1);
                        discard = false;
                    } else {
                        data = JSON.parse(data);
                        if (typeof data === 'object' && data !== null) {
                            console.log('Received', data);
                            txResponse(0x81);
                            discard = false;
                        } else {
                            console.log('Received', data, '(error, too fast?)');
                            txResponse(0xc1);
                            discard = false;
                        }
                    }
                });
            }).on("error", (err) => {
                console.log("Error: " + err.message);
                txResponse(0xc1);
                discard = false;
            });
        }
    } else if (buff[0] == 0x02) {
        txResponse(0x82);
        discard = false;
    } else {
        console.log('Received unknown command and data:', buff);
        discard = false;
    }
}

var state = 0;
var mode = 0;
var length;
var buff;

port.on('data', function (data) {
    for (var i = 0; i < data.length; i++) {
        if (discard == true) {
            console.log('Discard', data);
            break;
        }
        if (mode === 0) {
            switch (state) {
            case 0:
                if (data[i] == 0xAA) state++;
                break;
            case 1:
                if (data[i] == 0x55) state++;
                else state = 0;
                break;
            case 2:
                if (data[i] == 0xC3) state++;
                else state = 0;
                break;
            case 3:
                if (data[i] == 0x3C) {
                    state = 0;
                    mode = 1;
                } else state = 0;
                break;
            }
        } else if (mode == 1) {
            if (data[i]) {
                length = 0;
                buff = Buffer.alloc(data[i]);
                mode = 2;                
            } else {
                mode = 0;
            }
        } else {
            buff[length++] = data[i];
            if (length == buff.length) {
                discard = true;
                handleRx(buff);
                mode = 0;
            }
        }
    }
});

port.on('open', () => {
    console.log("Serial port open");
    
});

console.log("------------------------------------------");
console.log("22.99 2019 - Gateway v0.00");
console.log("Using API Key:", thingSpeakAPIKey);
console.log("Using COM port:", serialPortName);
console.log("Use CTRL+C to exit");
console.log("------------------------------------------");
