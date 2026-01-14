const Joi = require("joi");
const express = require("express");
const bodyParser = require("body-parser");
const { createPool } = require("mysql2");

const app = express();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

const pool = createPool({
  host: "mysql",
  user: "rest_user",
  password: "password123",
  database: "rest_db_js",
  connectionLimit: 10,
});

const endpoint = "/video/:id(\\d+)";

/* -----------------------
   GET all videos
------------------------ */
app.get("/videos", (req, res) => {
  pool.query("SELECT * FROM videos", (err, results) => {
    if (err) {
      console.error(err);
      return res.status(500).json({ error: "Database error" });
    }

    if (results.length === 0) {
      return res.status(404).json({ message: "No videos found" });
    }

    res.json(results);
  });
});

/* -----------------------
   GET video by ID
------------------------ */
app.get(endpoint, (req, res) => {
  pool.query(
    "SELECT * FROM videos WHERE id = ?",
    [req.params.id],
    (err, results) => {
      if (err) {
        console.error(err);
        return res.status(500).json({ error: "Database error" });
      }

      if (!results[0]) {
        return res.status(404).json({ message: "Video not found" });
      }

      res.json(results[0]);
    }
  );
});

/* -----------------------
   CREATE / REPLACE video
------------------------ */
app.post(endpoint, (req, res) => {
  const schema = Joi.object({
    name: Joi.string().required(),
    likes: Joi.number().required(),
    views: Joi.number().required(),
  });

  const { error, value } = schema.validate(req.body);
  if (error) {
    return res.status(400).json({ error: error.details[0].message });
  }

  pool.query(
    "REPLACE INTO videos (id, name, likes, views) VALUES (?, ?, ?, ?)",
    [req.params.id, value.name, value.likes, value.views],
    (err) => {
      if (err) {
        console.error(err);
        return res.status(500).json({ error: "Database error" });
      }

      res.status(200).json({ message: "Video created/updated" });
    }
  );
});

/* -----------------------
   UPDATE video (partial)
------------------------ */
app.put(endpoint, (req, res) => {
  const schema = Joi.object({
    name: Joi.string(),
    likes: Joi.number(),
    views: Joi.number(),
  }).min(1);

  const { error, value } = schema.validate(req.body);
  if (error) {
    return res.status(400).json({ error: error.details[0].message });
  }

  const fields = [];
  const params = [];

  for (const key in value) {
    fields.push(`${key} = ?`);
    params.push(value[key]);
  }

  params.push(req.params.id);

  const sql = `UPDATE videos SET ${fields.join(", ")} WHERE id = ?`;

  pool.query(sql, params, (err, results) => {
    if (err) {
      console.error(err);
      return res.status(500).json({ error: "Database error" });
    }

    if (results.affectedRows === 0) {
      return res.status(404).json({ message: "Video not found" });
    }

    res.json({ message: "Video updated" });
  });
});

/* -----------------------
   DELETE video
------------------------ */
app.delete(endpoint, (req, res) => {
  pool.query(
    "DELETE FROM videos WHERE id = ?",
    [req.params.id],
    (err, results) => {
      if (err) {
        console.error(err);
        return res.status(500).json({ error: "Database error" });
      }

      if (results.affectedRows === 0) {
        return res.status(404).json({ message: "Video not found" });
      }

      res.json({ message: "Video deleted" });
    }
  );
});

/* -----------------------
   Server
------------------------ */
const port = process.env.PORT || 5000;
app.listen(port, () => {
  console.log(`Listening on port ${port}`);
});

