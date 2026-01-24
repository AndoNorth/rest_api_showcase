import Joi from "joi";
import express, { Request, Response } from "express";
import { createPool, Pool, ResultSetHeader, RowDataPacket } from "mysql2";

const app = express();

app.use(express.json());
app.use(express.urlencoded({ extended: false }));

/* -----------------------
   Database
------------------------ */
const pool: Pool = createPool({
  host: "mysql",
  user: "rest_user",
  password: "password123",
  database: "rest_db_js",
  connectionLimit: 10,
});

/* -----------------------
   Types
------------------------ */
interface Video extends RowDataPacket {
  id: number;
  name: string;
  likes: number;
  views: number;
}

const endpoint = "/video/:id";

/* -----------------------
   GET all videos
------------------------ */
app.get("/videos", (req: Request, res: Response) => {
  pool.query<Video[]>("SELECT * FROM videos", (err, results) => {
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
app.get(endpoint, (req: Request, res: Response) => {
  pool.query<Video[]>(
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
app.post(endpoint, (req: Request, res: Response) => {
  const schema = Joi.object({
    name: Joi.string().required(),
    likes: Joi.number().required(),
    views: Joi.number().required(),
  });

  const { error, value } = schema.validate(req.body);
  if (error) {
    return res.status(400).json({ error: error.details[0].message });
  }

  pool.query<ResultSetHeader>(
    "REPLACE INTO videos (id, name, likes, views) VALUES (?, ?, ?, ?)",
    [req.params.id, value.name, value.likes, value.views],
    (err) => {
      if (err) {
        console.error(err);
        return res.status(500).json({ error: "Database error" });
      }

      res.json({ message: "Video created/updated" });
    }
  );
});

/* -----------------------
   UPDATE video (partial)
------------------------ */
app.put(endpoint, (req: Request, res: Response) => {
  const schema = Joi.object({
    name: Joi.string(),
    likes: Joi.number(),
    views: Joi.number(),
  }).min(1);

  const { error, value } = schema.validate(req.body);
  if (error) {
    return res.status(400).json({ error: error.details[0].message });
  }

  const fields: string[] = [];
  const params: (string | number)[] = [];

  for (const key of Object.keys(value)) {
    fields.push(`${key} = ?`);
    params.push(value[key]);
  }
  const id = Number(req.params.id);
  if (Number.isNaN(id)) {
    return res.status(400).json({error: "Invalid Id"});
  }
  params.push(id);

  const sql = `UPDATE videos SET ${fields.join(", ")} WHERE id = ?`;

  pool.query<ResultSetHeader>(sql, params, (err, results) => {
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
app.delete(endpoint, (req: Request, res: Response) => {
  pool.query<ResultSetHeader>(
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
const port = Number(process.env.PORT) || 5000;
app.listen(port, () => {
  console.log(`Listening on port ${port}`);
});

