// server.js - Simple Node.js API for a local scoreboard using SQLite

const express = require('express');
const bodyParser = require('body-parser');
const sqlite3 = require('sqlite3').verbose();
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware setup
app.use(bodyParser.json());
app.use(cors());

// Serve static files from the current directory
app.use(express.static(__dirname));

// Connect (or create) the SQLite database file
const dbFile = path.join(__dirname, 'scoreboard.db');
const db = new sqlite3.Database(dbFile, (err) => {
  if (err) {
    return console.error('Failed to connect to database:', err.message);
  }
  console.log('Connected to SQLite database.');
});

// Create the "scores" table if it does not exist
db.run(`
  CREATE TABLE IF NOT EXISTS scores (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL,
    difficulty TEXT NOT NULL,
    score INTEGER NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(username, difficulty)
  );
`, (err) => {
  if (err) {
    console.error('Failed to create table:', err.message);
  }
});

// Endpoint to get scores
app.get('/scores', (req, res) => {
  // If a limit parameter is provided, use it, otherwise return all scores
  const limit = req.query.limit ? parseInt(req.query.limit) : 0;
  
  let query = `
    SELECT * FROM scores
    ORDER BY 
      CASE difficulty
        WHEN 'infinity' THEN 4
        WHEN 'hard' THEN 3
        WHEN 'medium' THEN 2
        WHEN 'easy' THEN 1
        ELSE 0
      END DESC,
      score DESC
  `;
  
  // Add limit if requested
  if (limit > 0) {
    query += ` LIMIT ${limit}`;
  }
  
  db.all(query, [], (err, rows) => {
    if (err) {
      console.error('Error fetching scores:', err.message);
      return res.status(500).json({ error: 'Database error.' });
    }
    res.json(rows);
  });
});

// Endpoint to add or update a score
app.post('/scores', (req, res) => {
  const { username, difficulty, score } = req.body;
  if (!username || !difficulty || typeof score !== 'number') {
    return res.status(400).json({ error: 'Invalid input data.' });
  }

  // First, try to insert a new score record. If a record already exists for this user & difficulty,
  // use the "INSERT OR IGNORE" strategy and then update if necessary.
  const insertQuery = `
    INSERT OR IGNORE INTO scores (username, difficulty, score)
    VALUES (?, ?, ?);
  `;
  db.run(insertQuery, [username, difficulty.toLowerCase(), score], function(err) {
    if (err) {
      console.error('Error inserting score:', err.message);
      return res.status(500).json({ error: 'Database error.' });
    }

    // Now update if the new score is higher than the existing score.
    const updateQuery = `
      UPDATE scores 
      SET score = ?, timestamp = CURRENT_TIMESTAMP
      WHERE username = ? AND difficulty = ? AND score < ?;
    `;
    db.run(updateQuery, [score, username, difficulty.toLowerCase(), score], function(err) {
      if (err) {
        console.error('Error updating score:', err.message);
        return res.status(500).json({ error: 'Database error.' });
      }
      // Optionally, return the updated record or success message.
      res.json({ message: 'Score processed successfully.' });
    });
  });
});

// Default route to serve the game
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'index_with_sqlite.html'));
});

// Start the server
app.listen(PORT, () => {
  console.log(`Server listening on port ${PORT}`);
  console.log(`Open http://localhost:${PORT} in your browser to play the game`);
});