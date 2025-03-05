// Test script for the scoreboard functionality

// Create a global object to mimic the window object in a browser
global.window = { gameState: "test" };

// Import the core logic (unfortunately we'll have to reimplement parts 
// since we can't directly use the module in Node.js)

// Difficulty ranking for sorting (higher value = higher rank)
const difficultyRank = { easy: 1, medium: 2, hard: 3, infinity: 4 };

// Function to test the leaderboard processing logic
function testLeaderboardProcessing(rawScores) {
  console.log("Testing leaderboard processing with sample data...");
  
  // Logic from scoreboard.js
  const playerScores = {};

  rawScores.forEach(entry => {
    if (!entry.username || !entry.difficulty || typeof entry.score !== 'number') {
      return;
    }

    const username = entry.username;
    const difficulty = entry.difficulty.toLowerCase();
    const score = entry.score;

    // Initialize player record if not seen before
    if (!playerScores[username]) {
      playerScores[username] = {
        username: username,
        scores: { easy: 0, medium: 0, hard: 0, infinity: 0 }
      };
    }

    // Update score if it's higher
    if (score > playerScores[username].scores[difficulty]) {
      playerScores[username].scores[difficulty] = score;
    }
  });

  // Convert to array for sorting
  let playersArray = Object.values(playerScores);

  // Sort players by highest difficulty rank, then by score
  playersArray.sort((a, b) => {
    // Find highest difficulty for player a
    let aMaxDiff = 'easy';
    if (a.scores.infinity > 0) aMaxDiff = 'infinity';
    else if (a.scores.hard > 0) aMaxDiff = 'hard';
    else if (a.scores.medium > 0) aMaxDiff = 'medium';
    else if (a.scores.easy > 0) aMaxDiff = 'easy';

    // Find highest difficulty for player b
    let bMaxDiff = 'easy';
    if (b.scores.infinity > 0) bMaxDiff = 'infinity';
    else if (b.scores.hard > 0) bMaxDiff = 'hard';
    else if (b.scores.medium > 0) bMaxDiff = 'medium';
    else if (b.scores.easy > 0) bMaxDiff = 'easy';

    // Compare difficulty rank first
    const diffCompare = difficultyRank[bMaxDiff] - difficultyRank[aMaxDiff];
    if (diffCompare !== 0) {
      return diffCompare;
    }

    // If same highest difficulty, sort by score in that difficulty
    return b.scores[bMaxDiff] - a.scores[aMaxDiff];
  });

  // Limit to top 10
  return playersArray.slice(0, 10);
}

// Sample data to test with
const sampleScores = [
  { username: "Player1", difficulty: "easy", score: 200 },
  { username: "Player1", difficulty: "medium", score: 400 },
  { username: "Player2", difficulty: "easy", score: 300 },
  { username: "Player2", difficulty: "medium", score: 350 },
  { username: "Player3", difficulty: "hard", score: 600 },
  { username: "Player3", difficulty: "medium", score: 450 },
  { username: "Player4", difficulty: "infinity", score: 1000 },
  { username: "Player5", difficulty: "hard", score: 550 },
  { username: "Player5", difficulty: "infinity", score: 950 },
  { username: "Player1", difficulty: "easy", score: 250 }, // Higher score for same player/difficulty
  { username: "Player6", difficulty: "infinity", score: 1200 },
  { username: "Player7", difficulty: "medium", score: 500 },
  { username: "Player8", difficulty: "easy", score: 350 },
  { username: "Player9", difficulty: "medium", score: 520 },
  { username: "Player10", difficulty: "hard", score: 700 },
  { username: "Player11", difficulty: "infinity", score: 1100 },
  { username: "Player12", difficulty: "hard", score: 650 }
];

// Process and display results
const leaderboard = testLeaderboardProcessing(sampleScores);
console.log("\nTop 10 Players:");
console.log("==============");

leaderboard.forEach((player, index) => {
  // Find the player's highest difficulty score
  let highestDiff = 'easy';
  let highestScore = player.scores.easy;
  
  if (player.scores.medium > 0 && 
      (highestDiff === 'easy' || player.scores.medium > highestScore)) {
    highestDiff = 'medium';
    highestScore = player.scores.medium;
  }
  
  if (player.scores.hard > 0 && 
      (highestDiff === 'easy' || highestDiff === 'medium' || player.scores.hard > highestScore)) {
    highestDiff = 'hard';
    highestScore = player.scores.hard;
  }
  
  if (player.scores.infinity > 0 && 
      (highestDiff === 'easy' || highestDiff === 'medium' || highestDiff === 'hard' || player.scores.infinity > highestScore)) {
    highestDiff = 'infinity';
    highestScore = player.scores.infinity;
  }
  
  console.log(`${index + 1}. ${player.username}: ${highestScore} points (${highestDiff})`);
  
  // Show all scores
  console.log(`   All scores: Easy: ${player.scores.easy}, Medium: ${player.scores.medium}, Hard: ${player.scores.hard}, Infinity: ${player.scores.infinity}`);
});

// Try to fetch real data from the server if it's running
const http = require('http');

function fetchScores() {
  console.log("\nAttempting to fetch real scores from server...");
  
  const options = {
    hostname: 'localhost',
    port: 3000,
    path: '/scores',
    method: 'GET'
  };
  
  const req = http.request(options, (res) => {
    let data = '';
    
    res.on('data', (chunk) => {
      data += chunk;
    });
    
    res.on('end', () => {
      if (res.statusCode === 200) {
        try {
          const scores = JSON.parse(data);
          console.log(`Successfully fetched ${scores.length} scores from server`);
          
          if (scores.length > 0) {
            console.log("\nReal Leaderboard:");
            console.log("================");
            
            const realLeaderboard = testLeaderboardProcessing(scores);
            
            realLeaderboard.forEach((player, index) => {
              // Find the player's highest difficulty score
              let highestDiff = 'easy';
              let highestScore = player.scores.easy;
              
              if (player.scores.medium > 0 && 
                  (highestDiff === 'easy' || player.scores.medium > highestScore)) {
                highestDiff = 'medium';
                highestScore = player.scores.medium;
              }
              
              if (player.scores.hard > 0 && 
                  (highestDiff === 'easy' || highestDiff === 'medium' || player.scores.hard > highestScore)) {
                highestDiff = 'hard';
                highestScore = player.scores.hard;
              }
              
              if (player.scores.infinity > 0 && 
                  (highestDiff === 'easy' || highestDiff === 'medium' || highestDiff === 'hard' || player.scores.infinity > highestScore)) {
                highestDiff = 'infinity';
                highestScore = player.scores.infinity;
              }
              
              console.log(`${index + 1}. ${player.username}: ${highestScore} points (${highestDiff})`);
            });
          }
        } catch (e) {
          console.error("Error processing server response:", e);
          console.log("Raw response:", data);
        }
      } else {
        console.error(`Server returned status code ${res.statusCode}`);
      }
    });
  });
  
  req.on('error', (error) => {
    console.error("Error connecting to server:", error.message);
    console.log("Is the server running? Start it with 'node server.js'");
  });
  
  req.end();
}

// Check real server data
fetchScores();