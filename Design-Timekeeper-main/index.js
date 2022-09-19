var mysql = require('mysql2');
var mqtt = require('mqtt');
var express = require('express');
var session = require('express-session');
var bodyParser = require('body-parser');
var path = require('path');
var app = express();
app.set('view engine', 'html');
// app.set('views', __dirname)
// app.engine('html', require('ejs').renderFile);
app.use(express.static(__dirname));
var count = 0;
var client = mqtt.connect("mqtt://broker.hivemq.com:1883")
var topic_list = ["home/timekeeper", "home/RTC"];
var server = app.listen(3000, () => {
    console.log("NguyenDaoconnect");
})

var connection = mysql.createConnection({
    host: '127.0.0.1',
    user: 'windows',
    password: 'nguyenthedao1999',
    database: 'timekeeper'
});

connection.connect(function(err) {
    if (err)
        throw err;
    console.log("connected databases");
    var sql = "DROP TABLE IF EXISTS RFID"; // xoa bo bang cu
    connection.query(sql, function(err, result) {
        if (err)
            throw err;
        console.log("ok");
    });
    sql = "CREATE TABLE RFID( id INT(10) PRIMARY KEY  auto_increment, Information varchar(30),RTC varchar(30), Timenow datetime )"
    connection.query(sql, function(err, result) {
        if (err)
            throw err;
        console.log("ok");
    });
});


var io = require('socket.io')(server); //Bind socket.io to our express server.
var Infor1;
var Time1;
var n_time; // now time

var cnt_check = 0;

client.on('message', function(topic, message, packet) {
    console.log("message is " + message);
    console.log("topic is " + topic);
    //message = JSON.parse(message);
    if (topic == topic_list[0]) {
        cnt_check++;
        // information
        Infor1 = message.toString();
    } else if (topic == topic_list[1]) {
        cnt_check++;
        // time 
        Time1 = message.toString();
    }

    // or: publish is not defined
    if (cnt_check == 2) {
        cnt_check = 0;
        console.log("Infor1, Time1");
        console.log("NguyenDao to save");
        var first_name = "timekeeper";

        var n = new Date()
        var month = n.getMonth() + 1
        var Date_Time = n.getFullYear() + "-" + month + "-" + n.getDate() + " " + n.getHours() + ":" + n.getMinutes() + ":" + n.getSeconds();
        //var Date_and_Time = new Date().toMysqlFormat(); 
        let query = "INSERT INTO `RFID` ( Information, RTC, Timenow) VALUES (  '" + Infor1 + "', '" + Time1 + "', '" + Date_Time.toString() + "')";
        connection.query(query, (err, result) => {
            if (err) {
                throw err;
            }
        });


        connection.query("SELECT * FROM rfid ORDER BY id DESC LIMIT 1", function(err, result, fields) {
            if (err) throw err;
            result.forEach(function(value) {
                n_time = value.Timenow.toString().slice(4, 24);
                console.log('RC522', { infor: value.Information, time: value.RTC, timenow: n_time });
                io.sockets.emit('RC522', { infor: value.Information, time: value.RTC, timenow: n_time });
                console.log("emit thanh cong");
            });

        });
    }

});

client.on("connect", function() {
    console.log("NguyenDao connected  " + client.connected);

});

//handle errors
client.on("error", function(error) {
    console.log("Can't connect" + error);
    process.exit(1)
});

var options = {
    retain: true,
    qos: 1
};

console.log("NguyenDao subscribing to topics");
client.subscribe(topic_list, { qos: 1 });
console.log("NguyenDao end of script");