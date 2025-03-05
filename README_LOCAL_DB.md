# Comet Splitter Game - Local SQLite Database Setup

This document explains how to use the local SQLite database version of the Comet Splitter game instead of Firebase.

## Overview

This version uses a simple Node.js server with SQLite to store game scores locally. The implementation includes:

1. A Node.js server (`server.js`) with Express and SQLite
2. A modified game frontend that communicates with the local server
3. Simple user authentication without requiring Firebase

## Setup Instructions

### 1. Install Dependencies

First, make sure Node.js is installed on your computer. Then install the required packages:

```bash
npm install express sqlite3 body-parser cors
```

### 2. Start the Local Server

Run the Node.js server:

```bash
node server.js
```

The server will start on port 3000 by default. You should see the message "Server listening on port 3000" in the console.

### 3. Launch the Game

Open `index_with_sqlite.html` in your browser to play the game with the local database.

## How It Works

### Server-Side (server.js)

- Creates a SQLite database file named `scoreboard.db`
- Provides two main API endpoints:
  - `GET /scores`: Returns all scores from the database
  - `POST /scores`: Adds or updates a score in the database

### Client-Side (scores.js)

- Modified to communicate with the local server
- Handles high score tracking and retrieval via HTTP requests
- Maintains compatibility with the original game code

### Leaderboard (scoreboard.js)

- Replaces Firebase's real-time listener with a polling mechanism
- Fetches and processes scores every 10 seconds
- Maintains the same ranking logic as the original:
  - Players are ranked by their highest difficulty level first
  - Within each difficulty, players are ranked by their score
- Provides the same API functions to ensure compatibility with the game

### Authentication

Simple local authentication is implemented:
- Users can enter a username to be identified
- Username is stored in localStorage
- No passwords are required for this simplified version

## Technical Details

### Database Schema

The `scores` table has the following schema:
```sql
CREATE TABLE IF NOT EXISTS scores (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL,
  difficulty TEXT NOT NULL,
  score INTEGER NOT NULL,
  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
  UNIQUE(username, difficulty)
);
```

### API Endpoints

#### GET /scores
- Returns all scores sorted by difficulty priority and score
- Example response:
  ```json
  [
    {
      "id": 1,
      "username": "Player1",
      "difficulty": "hard",
      "score": 800,
      "timestamp": "2025-03-05 12:34:56"
    },
    ...
  ]
  ```

#### POST /scores
- Accepts: `{ username, difficulty, score }`
- Updates scores if player already has an entry for the difficulty and the new score is higher

## Testing

You can test the implementation using the included test scripts:
- `test_api.js`: Tests basic API functionality
- `test_more_scores.js`: Adds multiple test scores to populate the database
- `test_scoreboard.js`: Tests the leaderboard sorting and display logic

Run these with:
```bash
node test_api.js
node test_more_scores.js
node test_scoreboard.js
```

## Comparison to Firebase Version

### Advantages
- Works offline
- No external dependencies
- Simpler setup and configuration
- Full control over the database

### Limitations
- Scores are only stored locally
- No real-time updates across devices
- Simplified authentication (no password protection)

## Troubleshooting

If you encounter issues:

1. Make sure the server is running (`node server.js`)
2. Check the console for error messages
3. Ensure port 3000 is not being used by another application
4. Check file permissions for the SQLite database