var express = require ('express');
var bodyParser = require ('body-parser');
var app =express();
app.listen(3000);

var urlencodedParser = bodyParser.urlencoded({ extended: false })
//cau hinh EJS
app.set("view engine","ejs");
app.set("views", "./views");
app.get("/test_ejs",function(req,res){
  res.render("test_ejs");
})


//-------

app.get("/login",function(req,res){
  res.send("<h1>Login</h1>");
});


app.post("/login",urlencodedParser,function(req,res){
  var user = req.body.username;
  var pass = req.body.password;
  res.send("<h1>login with user : </h1>"+user+" password :  "+pass);
});

app.get("/getparam/:id",function(req,res){
  var i = req.params.id;
  res.send("Parameter id "+i);
});
