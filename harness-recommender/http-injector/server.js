const express = require('express');
const morgan = require('morgan'); // Middleware de logging
const bodyParser = require("body-parser")

const app = express();
const jsonParser = bodyParser.json()

app.use(morgan('combined'));
app.use(bodyParser.json())

app.post('/users', function(req, res) {
  res.json([{ item: "123", score: 0 }, { item: '3123131', score: 0 } ]);
  console.log(req.body)
});

app.use(function(req, res, next){
  res.status(404).send('Page introuvable !');
});

app.listen(8080, function() {
  console.log('Example app listening on port 8080!');
});