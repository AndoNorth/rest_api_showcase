const Joi = require("joi"); // npm i joi@13.1.0
const express = require("express"); // npm i express
const bodyParser = require("body-parser"); // npm i body-parser
const { createPool } = require("mysql"); // npm i mysql

const app = express();

const jsonParser = bodyParser.json();
app.use(jsonParser);
const urlencodedParser = bodyParser.urlencoded({ extended: false });
app.use(urlencodedParser);

const pool = createPool({
  host: "mysql",
  user: "rest_user",
  password: "password123",
  database: "rest_db_js",
  connectionLimit: 10,
});

const videos = [
  //   { id: 1, likes: 10, name: "ando's funny video", views: 121 },
  //   { id: 2, likes: 1420, name: "how to make paper planes", views: 42000 },
  //   { id: 3, likes: 23099, name: "awesome flip", views: 929299 },
];

const endPoint = "/videos/:id(\\d+)";

// get videos
app.get("/videos", (req, res) => {
  let sql = "select * from videos";
  pool.query(sql, (err, results) => {
    if (err) throw err;
    res.json(results);
  });
});

// get video at id
app.get(endPoint, (req, res) => {
  let sql = "select * from videos where id = ?";
  pool.query(sql, [req.params.id], (err, results) => {
    if (err) throw err;
    if (!results[0]) {
      res.json({ status: "Not Found!" });
    } else {
      res.json(results[0]);
    }
  });
});

// add video
app.post(endPoint, (req, res) => {
  const schema = {
    name: Joi.string().required(),
    likes: Joi.number().required(),
    views: Joi.number().required(),
  };

  const result = Joi.validate(req.body, schema);
  if (result.error) {
    return res.status(400).send(result.error.details[0].message); // can loop through all errors
  }

  const video = {
    id: parseInt(req.params.id),
    name: req.body.name,
    likes: parseInt(req.body.likes),
    views: parseInt(req.body.views),
  };

  let sql = "insert into videos (id, name, likes, views) values (?,?,?,?)";
  pool.query(
    sql,
    [req.params.id, req.body.name, req.body.likes, req.body.views],
    (err, results) => {
      if (err) throw err;
      res.json(results);
    }
  );
});

// update video - TODO: fix this
app.put(endPoint, (req, res) => {
  const video = videos.find((v) => v.id === parseInt(req.params.id));
  if (!video) {
    return res.status(404, "The video id was not found");
  }

  const schema = {
    name: Joi.string(),
    likes: Joi.number(),
    views: Joi.number(),
  };
  const result = Joi.validate(req.body, schema);

  // TODO: concatenate and loop through values
  if (result.value.name) {
    let sql = "update videos set name = ? where id = ?";
    pool.query(sql, [result.value.name, req.params.id], (err, results) => {
      if (err) throw err;
      console.log(results);
    });
  }
  if (result.value.likes) {
    let sql = "update videos set likes = ? where id = ?";
    pool.query(sql, [result.value.likes, req.params.id], (err, results) => {
      if (err) throw err;
      console.log(results);
    });
  }
  if (result.value.views) {
    let sql = "update videos set views = ? where id = ?";
    pool.query(sql, [result.value.views, req.params.id], (err, results) => {
      if (err) throw err;
      console.log(results);
    });
  }
});

// delete video
app.delete(endPoint, (req, res) => {
  let sql = "delete from videos where id = ?";
  pool.query(sql, [req.params.id], (err, results) => {
    if (err) throw err;
    res.json(results);
  });
});

// HTTP Server endpoint
const defaultPort = 5000;
const portNo = process.env.PORT || defaultPort;
app.listen(portNo, () => {
  console.log(`Listening on port ${portNo}`);
});
