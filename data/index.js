const fs = require("fs");
const Joi = require("joi");
const cors = require("cors");
const express = require("express");
const app = express();
app.use(express.json());
app.use(cors());

function validateData(dataObject) {
  const schema = {
    start: Joi.string().regex(/^([01]\d|2[0-3]):[0-5]\d$/).required(),
    stop: Joi.string().regex(/^([01]\d|2[0-3]):[0-5]\d$/).required(),
  };
  return Joi.validate(dataObject, schema);
}

function writeJSON(arrayObj, pathToFile) {
  let data = JSON.stringify(arrayObj, null, 2);
  try {
    fs.writeFileSync(pathToFile, data, { encoding: "utf8", flag: "w", mode: 0o666 });
    console.log(`Data was saved successfully to ${pathToFile}`);
  } catch(err) {
    console.log(`Data couldn't be saved! Error: ${err}`);
  }  
}

function readJSON(pathToFile) {
  try {
    let data = JSON.parse(fs.readFileSync(pathToFile, 'utf8'));
    console.log(`Data was read successfully from file ${pathToFile}`);
    return data;  
  } catch(err) {
    console.log(`Data couldn't be read! Error: ${err}`);
  }
}

app.get("/api/times", (req, res) => {
  res.send(readJSON("./times.json"));
});

app.put("/api/times", (req, res) => {
  let data = req.body;
  times = [];
  data.forEach(function(obj) {
    let { error } = validateData(obj);
    if (error) {
      res.status(400).send(error.details[0].message);
    } else {
      let time = {
        start: obj.start,
        stop: obj.stop,
      };
      times.push(time);
    }
  });
  writeJSON(times, 'times.json');
  res.send(readJSON("./times.json"));
});

app.listen(3000, () => console.log("Listening on port 3000..."));
